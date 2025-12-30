#include "StateItem.h"
#include <QPainter>
#include <QStyleOptionGraphicsItem>
#include <QFont>

const double RADIUS = 25.0;

StateItem::StateItem(const QString& id, double x, double y, bool isStart, bool isAccept)
    : QGraphicsEllipseItem(-RADIUS, -RADIUS, RADIUS*2, RADIUS*2),
      m_id(id), m_isStart(isStart), m_isAccept(isAccept), m_highlighted(false)
{
    setPos(x, y);
    setFlags(QGraphicsItem::ItemIsMovable | QGraphicsItem::ItemIsSelectable | QGraphicsItem::ItemSendsGeometryChanges);
    setBrush(Qt::white);
    setPen(QPen(Qt::black, 2));
    setZValue(1);
}

void StateItem::setId(const QString& id) {
    m_id = id;
    update();
}

void StateItem::setStart(bool s) {
    m_isStart = s;
    update();
}

void StateItem::setAccept(bool a) {
    m_isAccept = a;
    update();
}

void StateItem::setHighlight(const QColor& color) {
    m_highlightColor = color;
    m_highlighted = true;
    update();
}

void StateItem::clearHighlight() {
    m_highlighted = false;
    update();
}

void StateItem::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget) {
    Q_UNUSED(widget);
    
    // Shadow if selected
    if (option->state & QStyle::State_Selected) {
        painter->setBrush(Qt::NoBrush);
        painter->setPen(QPen(Qt::blue, 2, Qt::DashLine));
        painter->drawEllipse(boundingRect().adjusted(-2,-2,2,2));
    }

    // Fill
    painter->setBrush(m_highlighted ? m_highlightColor : Qt::white);
    painter->setPen(QPen(Qt::black, 2));
    painter->drawEllipse(rect());
    
    // Double circle for accept
    if (m_isAccept) {
        painter->drawEllipse(rect().adjusted(4,4,-4,-4));
    }
    
    // Start arrow (simple triangle to the left)
    if (m_isStart) {
        QPolygonF arrow;
        arrow << QPointF(-RADIUS-10, -10) << QPointF(-RADIUS, 0) << QPointF(-RADIUS-10, 10);
        painter->setBrush(Qt::black);
        painter->drawPolygon(arrow);
    }
    
    // Text
    painter->setPen(Qt::black);
    painter->setFont(QFont("Arial", 10, QFont::Bold));
    painter->drawText(rect(), Qt::AlignCenter, m_id);
}

QVariant StateItem::itemChange(GraphicsItemChange change, const QVariant &value) {
    if (change == ItemPositionHasChanged && scene()) {
        // Notify scene or transitions to update
        // We handle this in scene/transition logic usually, 
        // but QGraphicsScene detects this via signals if we hook them up,
        // or transitions can track nodes.
    }
    return QGraphicsEllipseItem::itemChange(change, value);
}