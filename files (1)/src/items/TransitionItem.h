#pragma once
#include <QGraphicsPathItem>
#include "StateItem.h"

class TransitionItem : public QGraphicsPathItem {
public:
    TransitionItem(StateItem* from, StateItem* to, const QString& symbol);
    
    void updatePosition();
    void setSymbol(const QString& s);
    QString getSymbol() const { return m_symbol; }
    
    StateItem* getFrom() const { return m_from; }
    StateItem* getTo() const { return m_to; }

    void setHighlight(const QColor& color);
    void clearHighlight();

protected:
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget) override;

private:
    StateItem* m_from;
    StateItem* m_to;
    QString m_symbol;
    bool m_highlighted = false;
    QColor m_highlightColor;
};