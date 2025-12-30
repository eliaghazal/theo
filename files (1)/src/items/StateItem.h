#pragma once
#include <QGraphicsEllipseItem>
#include <QBrush>
#include <QPen>
#include <QGraphicsScene>

class StateItem : public QGraphicsEllipseItem {
public:
    StateItem(const QString& id, double x, double y, bool isStart, bool isAccept);
    
    void setId(const QString& id);
    QString getId() const { return m_id; }
    
    void setStart(bool s);
    bool isStart() const { return m_isStart; }
    
    void setAccept(bool a);
    bool isAccept() const { return m_isAccept; }
    
    // Highlighting
    void setHighlight(const QColor& color);
    void clearHighlight();
    
protected:
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget) override;
    QVariant itemChange(GraphicsItemChange change, const QVariant &value) override;

private:
    QString m_id;
    bool m_isStart;
    bool m_isAccept;
    QColor m_highlightColor;
    bool m_highlighted;
};