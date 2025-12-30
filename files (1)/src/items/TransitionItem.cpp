#include "TransitionItem.h"
#include <QPainter>
#include <QtMath>

TransitionItem::TransitionItem(StateItem* from, StateItem* to, const QString& symbol)
    : m_from(from), m_to(to), m_symbol(symbol)
{
    setZValue(0);
    setPen(QPen(Qt::black, 2));
    updatePosition();
}

void TransitionItem::setSymbol(const QString& s) {
    m_symbol = s;
    update();
}

void TransitionItem::setHighlight(const QColor& color) {
    m_highlightColor = color;
    m_highlighted = true;
    update();
}

void TransitionItem::clearHighlight() {
    m_highlighted = false;
    update();
}

void TransitionItem::updatePosition() {
    if (!m_from || !m_to) return;
    
    QPointF p1 = m_from->pos();
    QPointF p2 = m_to->pos();
    
    QPainterPath path;
    
    if (m_from == m_to) {
        // Self loop
        QRectF r = m_from->rect();
        // Loop moves up-left and back
        path.moveTo(p1.x(), p1.y() - 25);
        path.cubicTo(p1.x() - 40, p1.y() - 80, 
                     p1.x() + 40, p1.y() - 80, 
                     p1.x(), p1.y() - 25);
    } else {
        // Line with slight curve if we wanted, but straight is fine for basic
        // Let's do straight for now, touching borders
        QLineF line(p1, p2);
        qreal length = line.length();
        // Adjust for radius
        qreal r = 25.0; 
        if (length > 2*r) {
            QPointF edge1 = line.pointAt(r / length);
            QPointF edge2 = line.pointAt(1.0 - r / length);
            path.moveTo(edge1);
            path.lineTo(edge2);
        } else {
            // Overlap
            path.moveTo(p1);
            path.lineTo(p2);
        }
    }
    
    setPath(path);
}

void TransitionItem::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget) {
    Q_UNUSED(option);
    Q_UNUSED(widget);
    
    painter->setPen(QPen(m_highlighted ? m_highlightColor : Qt::black, 2));
    painter->drawPath(path());
    
    // Draw arrow head
    QPainterPath p = path();
    if (p.elementCount() < 2) return;
    
    QPointF endPoint = p.pointAtPercent(1.0);
    // Approximate angle
    QPointF preEndPoint = p.pointAtPercent(0.95); // rough approximation
    if (m_from == m_to) preEndPoint = p.pointAtPercent(0.90);
    
    QLineF line(preEndPoint, endPoint);
    double angle = std::atan2(-line.dy(), line.dx());

    QPointF arrowP1 = endPoint - QPointF(sin(angle + M_PI / 3) * 10, cos(angle + M_PI / 3) * 10);
    QPointF arrowP2 = endPoint - QPointF(sin(angle + M_PI - M_PI / 3) * 10, cos(angle + M_PI - M_PI / 3) * 10);

    painter->setBrush(m_highlighted ? m_highlightColor : Qt::black);
    painter->drawPolygon(QPolygonF() << endPoint << arrowP1 << arrowP2);
    
    // Draw Text
    // Find midpoint
    QPointF mid;
    if (m_from == m_to) mid = QPointF(m_from->x(), m_from->y() - 75);
    else mid = (m_from->pos() + m_to->pos()) / 2;
    
    painter->setPen(Qt::blue);
    painter->setFont(QFont("Arial", 9));
    painter->drawText(QRectF(mid.x()-20, mid.y()-10, 40, 20), Qt::AlignCenter, m_symbol);
}