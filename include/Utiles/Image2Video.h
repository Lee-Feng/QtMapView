#pragma once

#include <QImage>
#include <opencv2/opencv.hpp>
#include <QFileInfo>
#include <QDir>
#include <QTimer>

namespace utiles {

/**
 * @brief The Image2Video class
 * 采用openCV合成
 *
 * 依赖的库：
win32 {
INCLUDEPATH += $$PTVAR_3rdLIB/opencv/include
LIBS += -L$$PTVAR_3rdLIB/opencv/x86/vc14/lib
VAR_3RDLIBS_DEBUG_END_WITH_D = opencv_core450 opencv_imgproc450 opencv_flann450 opencv_imgcodecs450 opencv_videoio450 \
 opencv_highgui450 opencv_ml450 opencv_video450 opencv_objdetect450 opencv_photo450 opencv_features2d450 opencv_calib3d450 opencv_stitching450

} else {
if(!contains(CC_PARTS,"arm")) {
    INCLUDEPATH += /usr/local/include/opencv4
    LIBS += -L/usr/local/lib
    VAR_3RDLIBS = opencv_core opencv_imgproc opencv_flann opencv_imgcodecs opencv_videoio \
     opencv_highgui opencv_ml opencv_video opencv_objdetect opencv_photo opencv_features2d opencv_calib3d opencv_stitching
}
}

 */
class Image2Video : public QObject{
    Q_OBJECT
public:
    Image2Video(){
        connect(&m_timer,SIGNAL(timeout()),this,SIGNAL(frameRequire()));
    }

    ~Image2Video(){
        stopRecord();
    }

public slots:
    /**
     * @brief startRecord
     * @param filepath  如:a/b/c/d/video.avi
     * @param fps       如24
     * @param width     如1024
     * @param height    如768
     * @return
     */
    bool startRecord(QString filepath,int fps,int width,int height){
        if(fps <= 0){
            fps = 24;
        }
        if(width < 0){
            width = 800;
        }
        if(height < 0){
            height = 600;
        }
        m_path = filepath;
        m_fps = fps;
        m_width = width;
        m_height = height;
        QFileInfo f(filepath);
        QDir d;
        d.mkpath(f.absolutePath());
        bool ret =  m_writer.open(qPrintable(filepath),
                                  //cv::VideoWriter::fourcc('I', '4', '2', '0'), /// 这个需要ffmpeg
                                  cv::VideoWriter::fourcc('M', 'J', 'P', 'G'),
                                  fps,
                                  cv::Size(width,height), true);
        if(ret){
            frameRequire();
            m_timer.setInterval(1000/fps);
            m_timer.setSingleShot(false);
            m_timer.start();
            recordingStateChanged();
        }
        return ret;
    }
    QString path() const{
        return m_path;
    }

    int fps() const{
        return m_fps;
    }

    void stopRecord(){
        if(m_timer.isActive()){
            m_timer.stop();
            m_writer.release();
            recordingStateChanged();
        }
    }

    bool isRecording() const{
        return m_timer.isActive();
    }

    /**
     * @brief pushFrame
     * 添加一帧,通常需要在frameRequire信号处理函数中放入帧数据
     * @param img
     * @return
     */
    bool pushFrame(QImage img,bool show = false){
        cv::Mat m;
        if(!QImageToMat(img,m)){
            return false;
        }
        cv::Mat resized;
        cv::resize(m,resized,cv::Size(m_width,m_height));
        cv::Mat frame_rgb;
        // 去除透明通道，因为QImageToMat的类型是4B，带透明通道，保存时会用错颜色
        cv::cvtColor(resized, frame_rgb, cv::COLOR_RGBA2RGB);
        if(show){
            cv::imshow("视频录制-当前帧",frame_rgb);
        }
        m_writer.write(frame_rgb);
        return true;
    }

signals:
    /// 请求帧，根据帧率时间控制
    void frameRequire();
    void recordingStateChanged();
protected:
    bool QImageToMat(QImage image,cv::Mat& dst)
    {
        bool suc = true;
        cv::Mat mat;
        switch (image.format())
        {
        case QImage::Format_ARGB32:
        case QImage::Format_RGB32:
        case QImage::Format_ARGB32_Premultiplied:
            mat = cv::Mat(image.height(), image.width(), CV_8UC4, (void*)image.constBits(), image.bytesPerLine());
            break;
        case QImage::Format_RGB888:
            mat = cv::Mat(image.height(), image.width(), CV_8UC3, (void*)image.constBits(), image.bytesPerLine());
            cv::cvtColor(mat, mat, cv::COLOR_BGR2RGB);
            break;
        case QImage::Format_Grayscale8:
            mat = cv::Mat(image.height(), image.width(), CV_8UC1, (void*)image.constBits(), image.bytesPerLine());
            break;
        default:
            suc = false;
            break;
        }
        dst = mat;
        return suc;
    }

protected:
    cv::VideoWriter m_writer;
    QString m_path = "imagevideo.avi";
    int m_fps = 24;
    int m_width = 800;
    int m_height = 600;
    QTimer m_timer;
};
}
