#ifndef PlusItem_H
#define PlusItem_H

#include <QImage>
#include <QQuickPaintedItem>
#include <QColor>

class PlusItem : public QQuickPaintedItem
{
    Q_OBJECT
    Q_PROPERTY(int size READ size WRITE setSize NOTIFY sizeChanged)
    Q_PROPERTY(QColor color READ color WRITE setColor NOTIFY colorChanged)
public:
    PlusItem(QQuickItem *parent = nullptr);

    /**
     * @brief size
     * 获取当前加号的大小，也即是肥瘦值，宽度
     * @return
     */
    int size() const;

    /**
     * @brief setImage
     * 设置当前加号的大小，也即是肥瘦值，宽度
     * @param image
     */
    void setSize(const int &size);


    /**
     * @brief color
     * 获取+号的颜色
     * @return
     */
    QColor color() const;

    /**
     * @brief setColor
     * @param len
     */
    void setColor(QColor c);

private:
    void paint(QPainter *painter);
signals:
    void sizeChanged();
    void colorChanged();
private:
    int m_size = 2;
    QColor m_color = Qt::red;

};
#endif // PlusItem_H
