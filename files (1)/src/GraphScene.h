#pragma once
#include <QGraphicsScene>
#include <map>
#include "items/StateItem.h"
#include "items/TransitionItem.h"
#include "model/DFA.h"

enum class ToolMode {
    Select,
    AddState,
    AddTransition
};

class GraphScene : public QGraphicsScene {
    Q_OBJECT
public:
    explicit GraphScene(QObject* parent = nullptr);
    
    void setMode(ToolMode mode);
    ToolMode getMode() const { return m_mode; }
    
    // Model sync
    DFA exportToDFA() const;
    void loadFromDFA(const DFA& dfa);
    
    void setAlphabet(const std::vector<std::string>& alpha);
    std::vector<std::string> getAlphabet() const { return m_alphabet; }

    // Graph manipulation
    void deleteSelected();
    void setStartSelected();
    void toggleAcceptSelected();
    
    // Highlights
    void clearHighlights();
    void highlightStates(const std::vector<std::string>& ids, QColor color);

signals:
    void message(const QString& msg);

protected:
    void mousePressEvent(QGraphicsSceneMouseEvent *event) override;
    void mouseMoveEvent(QGraphicsSceneMouseEvent *event) override;

private:
    ToolMode m_mode;
    std::vector<std::string> m_alphabet;
    int m_stateCounter;
    
    // Transition creation state
    StateItem* m_tempStartNode;
    QGraphicsLineItem* m_tempLine;
    
    void createTransition(StateItem* source, StateItem* target);
    StateItem* itemAtPos(const QPointF& pos);
};