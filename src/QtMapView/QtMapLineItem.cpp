#include <QtMapView/QtMapLineItem.hpp>
#include <QtMapView/QtMapView.hpp>
#include <QLinearGradient>
#include <QtMath>

StarLine::StarLine(QWidget *p):QWidget(p){
}

void StarLine::setLine(QPoint start, QPoint end){
    m_start = start;
    m_end = end;
    if(parentWidget()){
        setGeometry(parentWidget()->geometry());
    }
    update();
}

void StarLine::setWidth(int w)
{
    m_width = w;
}

int StarLine::getWidth()
{
    return m_width;
}

static void drawArc(QPainter *p, const QPointF& p1, const QPointF& p2, double radian){
    const auto dx = p2.x() - p1.x();
    const auto dy = p2.y() - p1.y();
    const auto u = cos(radian) - 1;
    const auto v = sin(radian);
    const auto a1 = atan2(u*dy - v*dx, u*dx + v*dy);
    const auto a2 = a1 + radian;
    const auto r = fabs(dx) < 1e-6 ? dy/ (sin(a2) - sin(a1)) : dx / (cos(a2) - cos(a1));
    const auto ox = p1.x() - r * cos(a1);
    const auto oy = p1.y() - r * sin(a1);
    const auto bb = QRectF(ox - r, oy - r, r*2, r*2);

    const auto start = -qRadiansToDegrees(a1) * 16;
    const auto span = -qRadiansToDegrees(radian) * 16;

    p->drawArc(bb,start,span);
}

void StarLine::paintEvent(QPaintEvent *event){
    event->accept();
    QPainter p(this);
    QLinearGradient gradient;
    gradient.setStart(m_start);
    gradient.setColorAt(0,Qt::red);
    gradient.setColorAt(0.5,Qt::yellow);
    gradient.setColorAt(1,Qt::green);
    gradient.setFinalStop(m_end);
    p.setPen(QPen(QBrush(gradient),m_width));
    /// 画直线
    //p.drawLine(m_start,m_end);

    /// 画弧线
    drawArc(&p,m_start,m_end,0.5);

    /// 画曲线
    //QPainterPath path;
    //QPointF c1 = QPointF((m_start.x() + m_end.x())/2,m_start.y());
    //QPointF c2 = QPointF((m_start.x() + m_end.x())/2,m_start.y());
    //path.moveTo(m_start);
    //path.cubicTo(c1,c2, m_end);
    //p.drawPath(path);
}

void StarLine::animationTick(){
}


QtMapLineItem::QtMapLineItem(){
    m_line = new StarLine();
    setView(m_line);
}

QtMapLineItem::QtMapLineItem(QPointF s, QPointF e)
{
    m_line = new StarLine();
    setView(m_line);
    setLine(s,e);
}

QtMapLineItem::~QtMapLineItem(){
}

void QtMapLineItem::setLine(QPointF s, QPointF e){
    m_geoStart = s;
    m_geoEnd = e;
    relayout();
}

void QtMapLineItem::relayout(){
    QtMapView* m = map();
    if(m == nullptr){
        return;
    }
    m_line->setLine(m->convertToViewPos(m_geoStart).toPoint(),m->convertToViewPos(m_geoEnd).toPoint());
}


