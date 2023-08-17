#pragma once

#include <windows.h>
#include <WinBase.h>
#include <wingdi.h>
#include <winuser.h>
#include <memory>
#include <cmath>
#include <string.h>
#include <assert.h>

#pragma  comment(lib, "gdi32.lib")
#pragma  comment(lib, "user32.lib")

namespace utiles {

/**
 * @brief The WindowCapture class
 * 目前只用于windows。
 * 使用方式：
 *
    方式一：
    utiles::WindowCapture wc(GetDesktopWindow());
    utiles::WindowCapture::CaptureData data = wc.refresh();
    /// 收到的数据一个像素时4个字节，所以用8888, bmp的RGB是GBRX
    QImage image(data.buff, data.width, data.height, QImage::Format_RGBX8888);
    l->setPixmap(QPixmap::fromImage(image.mirrored(false,true).rgbSwapped()));

    方式二：（需要QT += winextras）
    utiles::WindowCapture wc(GetDesktopWindow());
    utiles::WindowCapture::CaptureData data = wc.refresh();
    QImage img2 = QtWin::imageFromHBITMAP(data.captureBitmap);

    完整实例
    {
        QApplication app(argc, argv);
        utiles::WindowCapture wc(GetDesktopWindow());
        QLabel* l = new QLabel();
        l->setGeometry(0,0,200,200);
        l->show();
        QTimer t;
        t.setInterval(20);
        t.setSingleShot(false);
        QObject::connect(&t,&QTimer::timeout,[=,&wc](){
            utiles::WindowCapture::CaptureData data = wc.refresh();
            /// 收到的数据一个像素时4个字节，所以用8888, bmp的RGB是GBRX
            QImage image(data.buff, data.width, data.height, QImage::Format_RGBX8888);
            l->setPixmap(QPixmap::fromImage(image.mirrored(false,true).rgbSwapped()));
        });
        t.start();
        return app.exec();
    }

 */
class WindowCapture{
public:
    class CaptureData{
    public:
        unsigned char* buff = NULL;
        int size = 0;
        int width = 0;
        int height = 0;
        HBITMAP captureBitmap;
    };
public:
    WindowCapture(HWND wid){
        hWnd = wid;
        hWinDC = GetDC(hWnd);
        hMemDC = CreateCompatibleDC(hWinDC);
        createBitmap();
    }

    ~WindowCapture(){
        releaseBitmap();
        ReleaseDC(hWnd,hWinDC);
        DeleteDC(hMemDC);
    }

    HWND getWindId(){
        return hWnd;
    }

    /**
     * @brief refresh 当窗口更新了或者内容变化了都应该重新刷新
     */
    CaptureData refresh(){
        int x = 0;
        int y = 0;
        getWindowClientAreaSize(hWnd,x,y);
        if(x != m_lastWidth || y != m_lastHeight){
            releaseBitmap();
            createBitmap();
        }
        m_lastWidth = x;
        m_lastHeight = y;


        //StretchBlt函数，将拷贝整个屏幕的图像到hCaptureDC中，函数说明见MSDN，此处不详细解释
        StretchBlt(hMemDC, 0, 0, x, y, hWinDC, 0, 0, x, y, SRCCOPY);

        //第二次调用函数，并且第五个参数为有效的指针且可写入，
        //函数调用后，将会把图像的每个像素点的RGB值 以16进制值写入到bitmapBits中，
        //常见的一个像素是占4个字节，第一个字节是蓝色--B，第二个字节是绿色--G，第三个字节是红色--R，第四个不知道
        const unsigned char* before = bitmapBits;
        GetDIBits(hMemDC, hBitmap, 0, m_lastHeight, bitmapBits, &bitmapInfo, DIB_RGB_COLORS);
        const unsigned char* after = bitmapBits;
        assert(before == after);

        CaptureData ret;
        ret.buff = bitmapBits;
        ret.size = bitmapInfo.bmiHeader.biSizeImage;
        ret.width = m_lastWidth;
        ret.height = m_lastHeight;
        ret.captureBitmap = hBitmap;
        return ret;
    }

protected:
    void getWindowClientAreaSize(HWND hwnd, int &width, int& height) const{
        RECT rect;
        GetWindowRect(hwnd,&rect);
        width = (int)abs(rect.right - rect.left);
        height = (int)abs(rect.top - rect.bottom);
    }

    void releaseBitmap(){
        SelectObject(hMemDC, hOldBitmap);
        //然后是释放资源
        delete []bitmapBits;
        bitmapBits = NULL;
        DeleteObject(hBitmap);
        DeleteDC(hMemDC);
        DeleteDC(hWinDC);
    }
    void createBitmap(){
        assert(bitmapBits == NULL);

        int x = 0;
        int y = 0;
        getWindowClientAreaSize(hWnd,x,y);
        m_lastWidth = x;
        m_lastHeight = y;
        hWinDC = GetDC(hWnd);

        hMemDC = CreateCompatibleDC(hWinDC);
        //因为是获取整个窗口的图像，所以这里的x,y是事先获取到的屏幕分辨率，GetSystemMetrics可完成此功能
        //两个内存对象的HDC参数要使用同一个，刚开始我使用错了，造成一直无法取得数据
        hBitmap = CreateCompatibleBitmap(hWinDC, x, y);

        bitmapInfo = {0};
        //BITMAPINFO结构有两个成员变量，分别是BITMAPINFOHEADER结构和RGBQUAD结构，
        //BITMPINFOHEADER结构中包含了图像的一些基本信息，包括宽与高
        //我们要先将biSize属性初始为BITMAPINFOHEADER结构的大小，以便GetDIBits函数在调用的时候填充其他的属性变量
        bitmapInfo.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
        //将hCaptureBitmap选入到hCaptureDC中，通俗点说就是关联起来，对hCaptureDC的操作也就是对hCaptureBitmap操作，
        //比如在hCaptureDC中写入文字在hCaptureBitmap中也能体现，常见给图片打水印可以用这个方法实现
        hOldBitmap = (HBITMAP)SelectObject(hMemDC, hBitmap);
        //StretchBlt函数，将拷贝整个屏幕的图像到hCaptureDC中，函数说明见MSDN，此处不详细解释
        StretchBlt(hMemDC, 0, 0, x, y, hWinDC, 0, 0, x, y, SRCCOPY);
        //第一次调用 GetDIBits函数，并将参数五置为NULL，且bitmapInfo的BITMAPINFOHEADER结构的biSize已经初始化过，
        //函数将会把图像的宽，高还有整个图像所点的字节数填充到bitmapInfo结构中，以便接下来的使用
        //函数的详细解释网上很多。
        GetDIBits(hMemDC, hBitmap, 0, y, NULL, &bitmapInfo, DIB_RGB_COLORS);
        if(bitmapInfo.bmiHeader.biSizeImage<=0){
            bitmapInfo.bmiHeader.biSizeImage=bitmapInfo.bmiHeader.biWidth*
                    abs(bitmapInfo.bmiHeader.biHeight)*
                    (bitmapInfo.bmiHeader.biBitCount+7)/8;
        }
        /// 不写这句话会在GetDIBits函数获取图片内容是时导致堆栈破坏而崩溃！！！
        /// 即这里写上之后，第二次调用GetDIBits的地方就不会在退出时崩溃了。
        bitmapInfo.bmiHeader.biCompression=BI_RGB;

        //根据第一次调用 GetDIBits函数完成后，填充到bitmapInfo中的图像占字节数大小来动态分配内存
        //你也可以用图像的宽*高来得到图像所占字节数，但此方法有一点要说明 ，第一行的字节数必须是4的倍数，不够的用0补足。
        //例：宽为923像素的图像，923/4 = 230(取整数部分)，多出三个字节，这个时候我们就要补足四位，变成(230+1)*4 = 924 实际一行占的字节数
        bitmapBits = new unsigned char[bitmapInfo.bmiHeader.biSizeImage];
        memset(bitmapBits, 0, bitmapInfo.bmiHeader.biSizeImage);//初始为0
    }

private:
    void demo() const{
        int x, y;
        x = GetSystemMetrics(SM_CXSCREEN);//屏幕宽度
        y = GetSystemMetrics(SM_CYSCREEN);//屏幕高度

        HDC hWinDC = GetDC(GetDesktopWindow());

        HDC hMemDC = CreateCompatibleDC(hWinDC);
        //因为是获取整个窗口的图像，所以这里的x,y是事先获取到的屏幕分辨率，GetSystemMetrics可完成此功能
        //两个内存对象的HDC参数要使用同一个，刚开始我使用错了，造成一直无法取得数据
        HBITMAP hBitmap = CreateCompatibleBitmap(hWinDC, x, y);

        BITMAPINFO bitmapInfo;
        ZeroMemory(&bitmapInfo,sizeof(BITMAPINFO));

        //BITMAPINFO结构有两个成员变量，分别是BITMAPINFOHEADER结构和RGBQUAD结构，
        //BITMPINFOHEADER结构中包含了图像的一些基本信息，包括宽与高
        //我们要先将biSize属性初始为BITMAPINFOHEADER结构的大小，以便GetDIBits函数在调用的时候填充其他的属性变量
        bitmapInfo.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
        //将hBitmap选入到hMemDC中，通俗点说就是关联起来，对hMemDC的操作也就是对hBitmap操作，
        //比如在hMemDC中写入文字在hBitmap中也能体现，常见给图片打水印可以用这个方法实现
        HBITMAP hOldBitmap = (HBITMAP)SelectObject(hMemDC, hBitmap);
        //StretchBlt函数，将拷贝整个屏幕的图像到hMemDC中，函数说明见MSDN，此处不详细解释
        StretchBlt(hMemDC, 0, 0, x, y, hWinDC, 0, 0, x, y, SRCCOPY);
        //第一次调用 GetDIBits函数，并将参数五置为NULL，且bitmapInfo的BITMAPINFOHEADER结构的biSize已经初始化过，
        //函数将会把图像的宽，高还有整个图像所点的字节数填充到bitmapInfo结构中，以便接下来的使用
        //函数的详细解释网上很多。
        GetDIBits(hMemDC, hBitmap, 0, y, NULL, &bitmapInfo, DIB_RGB_COLORS);
        if(bitmapInfo.bmiHeader.biSizeImage<=0){
            bitmapInfo.bmiHeader.biSizeImage=bitmapInfo.bmiHeader.biWidth*
                    abs(bitmapInfo.bmiHeader.biHeight)*
                    (bitmapInfo.bmiHeader.biBitCount+7)/8;
        }
        /// 不写这句话会在GetDIBits函数获取图片内容是时导致堆栈破坏而崩溃！！！
        /// 即这里写上之后，第二次调用GetDIBits的地方就不会在退出时崩溃了。
        bitmapInfo.bmiHeader.biCompression=BI_RGB;

        //根据第一次调用 GetDIBits函数完成后，填充到bitmapInfo中的图像占字节数大小来动态分配内存
        //你也可以用图像的宽*高来得到图像所占字节数，但此方法有一点要说明 ，第一行的字节数必须是4的倍数，不够的用0补足。
        //例：宽为923像素的图像，923/4 = 230(取整数部分)，多出三个字节，这个时候我们就要补足四位，变成(230+1)*4 = 924 实际一行占的字节数
        unsigned char *bitmapBits = new unsigned char[bitmapInfo.bmiHeader.biSizeImage];
        memset(bitmapBits, 0, bitmapInfo.bmiHeader.biSizeImage);//初始为0


        //第二次调用函数，并且第五个参数为有效的指针且可写入，
        //函数调用后，将会把图像的每个像素点的RGB值 以16进制值写入到bitmapBits中，
        //常见的一个像素是占4个字节，第一个字节是蓝色--B，第二个字节是绿色--G，第三个字节是红色--R，第四个不知道
        /// 这里的第5个参数bitmapInfo，需要的其实是一个BITMAPINFO结构，而这时我们传入的其实只有BITMAPINFOHEADER。
        /// 如果在位图不小于16位时，这是可行的。但是在位图小于16位时，它还需要另外的内存空间来储存一个调色板数据，所以就会发生堆栈损坏的错误。
        /// 解决办法是：
        /// struct { BITMAPINFO info; RGBQUAD moreColors[255]; } fbi;
        /// BITMAPINFOHEADER &bi = fbi.info.bmiHeader;
        /// bi.biSize = sizeof(BITMAPINFOHEADER);
        /// ...
        /// GetDIBits(..., &fbi.info, ...);
        ///
        /// 或者：
        /// bitmapInfo.bmiHeader.biCompression=BI_RGB;
        const unsigned char* before = bitmapBits;
        GetDIBits(hMemDC, hBitmap, 0, y, bitmapBits, &bitmapInfo, DIB_RGB_COLORS);
        const unsigned char* after = bitmapBits;
        assert(before == after);

        SelectObject(hMemDC, hOldBitmap);
        //然后是释放资源
        delete []bitmapBits;
        DeleteObject(hBitmap);
        DeleteDC(hMemDC);
        DeleteDC(hWinDC);
    }

protected:

    int m_lastWidth = 0;
    int m_lastHeight = 0;

    HWND 	hWnd;
    HDC 	hWinDC ;
    HDC 	hMemDC;

    /// 根据窗口大小创建的
    HBITMAP 	hBitmap;
    HBITMAP 	hOldBitmap;
    BITMAPINFO	bitmapInfo;
    // 原始数据，每次刷新写入的数据就是这个值
    // 并且该值也会转换到write_buffer
    unsigned char* bitmapBits = NULL;
};
}
