#include <QtMapView/QtMapLineItem.hpp>
#include <QtMapView/QtMapView.hpp>
#include <QLinearGradient>
#include <QtMath>
#include <QDebug>

StarLine::StarLine(QWidget *p):QWidget(p){
    run(0);
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

QLinearGradient StarLine::genLinearGradient(qreal percent)
{
    //qreal len = 20.0 / qMax(width(),height());
    return genLinearGradient(QColor(0x00,0x99,0x00),QColor(0x99,0x99,0xFF),QColor(0xFF,0x99,0x66),QColor(0xFF,0x99,0xFF),percent,0.05);
}

void StarLine::reset(qreal percent)
{
    m_percent = percent;
}

qreal StarLine::percent()
{
    return m_percent;
}

void StarLine::run(qreal percent)
{
    reset(percent);
    m_time.restart();
    m_running = true;
}

void StarLine::stop()
{
    m_running = false;
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
    if(m_running){
        qreal step = ((m_time.elapsed() / 1000.0) * 0.5);
        m_percent  = m_percent +step;
        if(m_percent > 1){
            m_percent = 0;
        }
        m_time.restart();
    }
    QLinearGradient gradient = genLinearGradient(m_percent);
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

QLinearGradient StarLine::genLinearGradient(QColor backgroundStart, QColor backgroundEnd, QColor targetStart, QColor targetEnd, qreal targetPos, qreal targetLength)
{
    if(targetPos < 0){
        targetPos = 0;
    } else if(targetPos > 1){
        targetPos = 1;
    }
    if(targetLength < 0){
        targetLength = 0;
    } else if(targetLength > 1){
        targetLength = 1;
    }
    QLinearGradient gradient;
    gradient.setStart(m_start);
    gradient.setColorAt(0,backgroundStart);
    gradient.setColorAt(qMax(targetPos - 0.00001,0.0),backgroundEnd);
    gradient.setColorAt(targetPos,targetStart);
    gradient.setColorAt(qMin(targetPos+targetLength,1.0),targetEnd);
    gradient.setColorAt(qMin(targetPos+targetLength + 0.00001,1.0), backgroundEnd);
    gradient.setColorAt(1,backgroundStart);
    gradient.setFinalStop(m_end);
    return gradient;
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


