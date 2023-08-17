#ifndef IMAGEITEM_H
#define IMAGEITEM_H

#include <QImage>
#include <QQuickPaintedItem>


//#define CPP_PROCESS_TOUCH_CLICK_IN_IMAGEITEM

/**
 * @brief The ImageItem class
 * mark位置是以图片大小为准。0,0)是原始图片的左上角，横线是X, 纵向是Y
 */
class ImageItem : public QQuickPaintedItem
{
    Q_OBJECT
    Q_PROPERTY(QImage image READ image WRITE setImage NOTIFY imageChanged)
    Q_PROPERTY(FillMode fillMode READ fillMode WRITE setFillMode NOTIFY fillModeChanged)
    Q_PROPERTY(int markCount READ markCount NOTIFY markCountChanged)
    Q_PROPERTY(int imageWidth READ getImageWidth NOTIFY imageSizeChanged)
    Q_PROPERTY(int imageHeight READ getImageHeight NOTIFY imageSizeChanged)
public:
    enum FillMode
    {
        Stretch = 1,
        PreserveAspectFit = 2,
        PreserveAspectCrop = 3,
        Tile = 4,               /// 没实现
        TileVertically = 5,     /// 没实现
        TileHorizontally = 6,   /// 没实现
        Pad = 7                 /// 没实现
    };
    Q_ENUMS(FillMode)
public:
    ImageItem(QQuickItem *parent = nullptr);

    /**
     * @brief image
     * 获取当前图片
     * @return
     */
    QImage image() const;

    /**
     * @brief setImage
     * 设置图片
     * @param image
     */
    void setImage(const QImage &image);

    /**
     * @brief fillMode
     * 获取当前填充模式
     * @return
     */
    FillMode fillMode() const;
    /**
     * @brief setFillMode
     * 设置填充模式
     * @param mode
     */
    void setFillMode(FillMode mode);

    /**
     * @brief getFitScale
     * 获取当前自适应的缩放值
     * @return
     */
    qreal getFitScale();

    /**
     * @brief getAbsScale
     * 获取绝对缩放：fitscale * scale
     */
    Q_INVOKABLE qreal getAbsScale();

    /**
     * @brief getAbsTrans
     * 获取绝对平移
     * @return
     */
    Q_INVOKABLE QPoint getAbsTrans();

    /**
     * @brief getAlignShift
     * 获取当前自适应的偏移值
     * @return
     */
    QPoint getAlignShift();

    /**
     * @brief convertImagePos
     * 把当前Ui的局部坐标转换为图像的坐标
     *
     * Q_INVOKABLE:让QML可以直接调用。但是不能通过引用，指针等方式往外传值，只有用QJSValue
     *
     * @param localuipos
     */
    Q_INVOKABLE QPoint convertToImagePos(QPoint localuipos);
    Q_INVOKABLE int convertToImagePosX(int x);
    Q_INVOKABLE int convertToImagePosY(int y);
    /**
     * @brief convertToImageRefPos
     * 把当前Ui的局部坐标转换为图像的坐标，范围是0~1的比例
     * @param localuipos
     * @return
     */
    Q_INVOKABLE QPointF convertToImageRefPos(QPoint localuipos);

    /**
     * @brief convertToUIPos
     * 图片上的位置数据转换为UI显示位置
     * @param imagepos
     * @return
     */
    Q_INVOKABLE QPoint convertToUIPos(QPoint imagepos);
    Q_INVOKABLE int convertToUIPosX(int x);
    Q_INVOKABLE int convertToUIPosY(int y);

    /**
     * @brief setMarks
     * 设置显示矩形区域.
     * 每一个区域就是x,y,width,height
     * @param marks 区域，以图像坐标系为准
     */
    Q_INVOKABLE void setMarks(QVariantList marks);

    /**
     * @brief markCount
     * @return
     */
    Q_INVOKABLE int markCount();

    /**
     * @brief imageWidth
     * 图片宽度
     * @return
     */
    Q_INVOKABLE int getImageWidth();
    Q_INVOKABLE int getImageHeight();
private:
    void paint(QPainter *painter);

#ifdef CPP_PROCESS_TOUCH_CLICK_IN_IMAGEITEM
    virtual void mousePressEvent(QMouseEvent *event);
    virtual void mouseMoveEvent(QMouseEvent *event);
    virtual void hoverMoveEvent(QHoverEvent* event);
    virtual void mouseReleaseEvent(QMouseEvent *event);
    virtual void touchEvent(QTouchEvent *event);
#endif

signals:
    void imageChanged();
    void fillModeChanged();
    /// 窗口区域点击和触摸了
    void clicked(QPoint posInUI);
    ///窗口区域中的图像点击和触摸了
    void imageClicked(QPoint posInImage);
    /// 标记个数变了
    void markCountChanged();
    /// 图片大小变了
    void imageSizeChanged();
private:
    QImage m_image;
    FillMode m_mode = Stretch;
    /// 外界设置的缩放因子,最终的缩放因子就是fitscale*scale
    qreal m_scale;
    /// 平移值
    QPoint m_trans;

    QList<QRectF> m_marks;
    QColor m_mark_color = Qt::red;

};
#endif // IMAGEITEM_H
