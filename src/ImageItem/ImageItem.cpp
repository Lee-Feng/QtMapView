#include "ImageItem.h"
#include <QDebug>
#include <QPainter>

ImageItem::ImageItem(QQuickItem *parent):QQuickPaintedItem(parent)
{
#ifdef CPP_PROCESS_TOUCH_CLICK_IN_IMAGEITEM
    // 以下几行是C++处理鼠标事件的前提，否则所有(C++)鼠标事件直接忽略
    setAcceptHoverEvents(true);
    setAcceptedMouseButtons(Qt::AllButtons);
    acceptTouchEvents();
    setFlag(ItemAcceptsInputMethod, true);
#endif
    setFlag(QQuickItem::ItemHasContents,true);
}

QImage ImageItem::image() const
{
    return m_image;
}

void ImageItem::setImage(const QImage &image)
{
    bool size_changed = false;
    if(m_image.size() != image.size()){
        size_changed = true;
    }
    m_image = image;
    emit imageChanged();
    if(size_changed){
        emit imageSizeChanged();
    }
    update();
}

ImageItem::FillMode ImageItem::fillMode() const
{
    return  m_mode;
}

void ImageItem::setFillMode(ImageItem::FillMode mode)
{
    if(m_mode != mode){
        m_mode = mode;
        fillModeChanged();
    }
}

void ImageItem::paint(QPainter *painter)
{
    if(m_image.isNull()){
        painter->save();
        painter->fillRect(QRect(0,0,width(),height()),qRgb(0,0,0));
        painter->restore();
        return;
    }

    painter->save();
    painter->translate(getAbsTrans());
    qreal scale = getAbsScale();
    painter->scale(scale,scale);
    painter->drawImage(QPoint(0,0),m_image);
    if(!m_marks.isEmpty()){
        QPen pen = painter->pen();
        pen.setColor(m_mark_color);
        pen.setWidth(qMax(1.0/scale,1.0));
        painter->setPen(pen);
        foreach(const auto& p, m_marks){
            painter->drawRect(p);
        }
    }
    painter->restore();
}


QPoint ImageItem::convertToImagePos(QPoint localuipos)
{
    /// (imagePos)*scale + imageShift = localPos
    /// imagePos = (localPos-imageShift)/scale
    QPoint imageShift = getAbsTrans();
    qreal imageScale = getAbsScale();
    QPoint posInImage = (localuipos - imageShift)/imageScale;
    return posInImage;
}

int ImageItem::convertToImagePosX(int x)
{
    return convertToImagePos(QPoint(x,0)).x();
}

int ImageItem::convertToImagePosY(int y)
{
    return convertToImagePos(QPoint(0,y)).y();
}

QPointF ImageItem::convertToImageRefPos(QPoint localuipos)
{
    QPoint pos = convertToImagePos(localuipos);
    QSize imageSize = m_image.size();
    if(imageSize.width() <= 0){
        return QPointF(0,0);
    }
    if(imageSize.height() <= 0){
        return QPointF(0,0);
    }
    QPointF ret;
    ret.setX(((qreal)pos.x())/((qreal)imageSize.width()));
    ret.setY(((qreal)pos.y())/((qreal)imageSize.height()));
    return ret;
}

QPoint ImageItem::convertToUIPos(QPoint imagepos)
{
    /// (imagePos)*scale + imageShift = localPos
    /// imagePos = (localPos-imageShift)/scale
    QPoint imageShift = getAbsTrans();
    qreal imageScale = getAbsScale();
    QPoint posInUI = imagepos*imageScale + imageShift;
    return posInUI;
}

int ImageItem::convertToUIPosX(int x)
{
    return convertToUIPos(QPoint(x,0)).x();
}

int ImageItem::convertToUIPosY(int y)
{
    return convertToUIPos(QPoint(0,y)).y();
}

void ImageItem::setMarks(QVariantList marks)
{
    int old_count = m_marks.size();
    m_marks.clear();
    foreach(const auto& p,marks){
        QVariantMap rect = p.toMap();
        m_marks.append(QRect(rect["x"].toInt(),rect["y"].toInt(),rect["width"].toInt(),rect["height"].toInt()));
    }
    if(marks.size() != old_count){
        markCountChanged();
    }
}

int ImageItem::markCount()
{
    return m_marks.size();
}

int ImageItem::getImageWidth()
{
    return m_image.width();
}

int ImageItem::getImageHeight()
{
    return m_image.height();
}

#ifdef CPP_PROCESS_TOUCH_CLICK_IN_IMAGEITEM

void ImageItem::mousePressEvent(QMouseEvent *event)
{
    QQuickPaintedItem::mousePressEvent(event);
    qDebug() << __FUNCTION__ << event->pos() << "imagePos:" << convertImagePos(event->pos());
    clicked(event->pos());
}

void ImageItem::mouseMoveEvent(QMouseEvent *event)
{
    /// 这里不会被回调....（搞扯）
    QQuickPaintedItem::mouseMoveEvent(event);
    qDebug() << __FUNCTION__ << event->pos();
}

void ImageItem::hoverMoveEvent(QHoverEvent* event) {
    /// 这里鼠标在里面就回调
    QQuickPaintedItem::hoverMoveEvent(event);
    ///qDebug() << __FUNCTION__ << event->pos();
}

void ImageItem::mouseReleaseEvent(QMouseEvent *event)
{
    /// 这里双击才会被回调....（搞扯）
    QQuickPaintedItem::mouseReleaseEvent(event);
    qDebug() << __FUNCTION__ << event->pos();
}

void ImageItem::touchEvent(QTouchEvent *event)
{
    QQuickPaintedItem::touchEvent(event);
    qDebug() << __FUNCTION__ << event->touchPoints();
    Qt::TouchPointStates st = event->touchPointStates();
}
#endif

qreal ImageItem::getFitScale()
{
    qreal fit_scale = 1.0;
    if(!m_image.isNull()){

        qreal w_ratio = ((qreal)m_image.width())/(qreal)this->width();
        qreal h_ratio = ((qreal)m_image.height())/(qreal)this->height();

        switch (m_mode) {
        case Stretch:
            /// 不做任何事
            break;
        case PreserveAspectFit:
            fit_scale = 1.0 / std::max(w_ratio,h_ratio);
            break;
        case PreserveAspectCrop:
            fit_scale = 1.0 / std::min(w_ratio,h_ratio);
            break;
        case Tile:
        case TileVertically:
        case TileHorizontally:
        case Pad:
            break;
        default:
            break;
        }
    }
    return fit_scale;
}

qreal ImageItem::getAbsScale()
{
    return getFitScale();
}

QPoint ImageItem::getAbsTrans()
{
    return m_trans + getAlignShift();
}

QPoint ImageItem::getAlignShift()
{
//    QRectF bounding_rect = boundingRect();
//    QSize image_size = m_image.size();
//    qreal x_scale = ((qreal)image_size.width()) / ((qreal)(bounding_rect.width()));
//    qreal y_scale = ((qreal)image_size.height()) / ((qreal)(bounding_rect.height()));

    QPoint ret;
    int thiswidth = this->width();
    int thishight = this->height();
    qreal imgscale = getAbsScale();
    int imgwidth = (m_image.isNull() ? 0 : m_image.width() * imgscale);
    int imghight = (m_image.isNull() ? 0 : m_image.height() * imgscale);

    switch (m_mode) {
    case Stretch:
        ret.setX(0);
        ret.setY(0);
        break;
    case PreserveAspectFit:
        ret.setX((thiswidth - imgwidth)/2.0);
        ret.setY((thishight - imghight)/2.0);
        break;
    case PreserveAspectCrop:
        ret.setX((thiswidth - imgwidth)/2.0);
        ret.setY((thishight - imghight)/2.0);
        break;
    case Tile:
    case TileVertically:
    case TileHorizontally:
    case Pad:
    default:
        break;
    }
    return ret;
}
