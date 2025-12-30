#include "GraphScene.h"
#include <QGraphicsSceneMouseEvent>
#include <QInputDialog>
#include <QMessageBox>

GraphScene::GraphScene(QObject* parent) 
    : QGraphicsScene(parent), m_mode(ToolMode::Select), m_stateCounter(0), 
      m_tempStartNode(nullptr), m_tempLine(nullptr)
{
    setSceneRect(0, 0, 2000, 2000);
    // Default alphabet
    m_alphabet = {"a", "b"};
}

void GraphScene::setMode(ToolMode mode) {
    m_mode = mode;
    if (m_tempLine) {
        removeItem(m_tempLine);
        delete m_tempLine;
        m_tempLine = nullptr;
    }
    m_tempStartNode = nullptr;
}

void GraphScene::setAlphabet(const std::vector<std::string>& alpha) {
    m_alphabet = alpha;
}

DFA GraphScene::exportToDFA() const {
    DFA dfa;
    dfa.alphabet = m_alphabet;
    
    for (auto* item : items()) {
        if (auto* s = dynamic_cast<StateItem*>(item)) {
            dfa.addState(s->getId().toStdString(), s->isStart(), s->isAccept(), s->x(), s->y());
        }
    }
    
    for (auto* item : items()) {
        if (auto* t = dynamic_cast<TransitionItem*>(item)) {
            QStringList symbols = t->getSymbol().split(',', Qt::SkipEmptyParts);
            for (const auto& sym : symbols) {
                 dfa.addTransition(t->getFrom()->getId().toStdString(), 
                                   sym.trimmed().toStdString(), 
                                   t->getTo()->getId().toStdString());
            }
        }
    }
    return dfa;
}

void GraphScene::loadFromDFA(const DFA& dfa) {
    clear();
    m_alphabet = dfa.alphabet;
    m_stateCounter = 0;
    
    std::map<std::string, StateItem*> nodeMap;
    
    // Load states
    for (const auto& s : dfa.states) {
        StateItem* item = new StateItem(QString::fromStdString(s.id), s.x, s.y, s.isStart, s.isAccept);
        addItem(item);
        nodeMap[s.id] = item;
        
        // Update counter heuristic
        if (s.id.rfind("q", 0) == 0) {
            try {
                int num = std::stoi(s.id.substr(1));
                if (num >= m_stateCounter) m_stateCounter = num + 1;
            } catch(...) {}
        }
    }
    
    // Load transitions
    // Need to group by from-to pair to combine symbols
    std::map<std::pair<std::string, std::string>, std::string> combinedTrans;
    for (const auto& kv : dfa.transitions) {
        std::string from = kv.first.first;
        std::string sym = kv.first.second;
        std::string to = kv.second;
        
        if (combinedTrans.count({from, to})) {
            combinedTrans[{from, to}] += "," + sym;
        } else {
            combinedTrans[{from, to}] = sym;
        }
    }
    
    for (const auto& kv : combinedTrans) {
        StateItem* src = nodeMap[kv.first.first];
        StateItem* dst = nodeMap[kv.first.second];
        if (src && dst) {
            TransitionItem* tItem = new TransitionItem(src, dst, QString::fromStdString(kv.second));
            addItem(tItem);
        }
    }
}

void GraphScene::deleteSelected() {
    for (auto* item : selectedItems()) {
        // If state, remove connected transitions first
        if (auto* s = dynamic_cast<StateItem*>(item)) {
            // Find transitions connected to this state
            // Copy list to avoid iterator invalidation
            QList<QGraphicsItem*> all = items(); 
            for (auto* other : all) {
                if (auto* t = dynamic_cast<TransitionItem*>(other)) {
                    if (t->getFrom() == s || t->getTo() == s) {
                        removeItem(t);
                        delete t;
                    }
                }
            }
        }
        removeItem(item);
        delete item;
    }
}

void GraphScene::setStartSelected() {
    auto sel = selectedItems();
    if (sel.size() != 1) return;
    if (auto* s = dynamic_cast<StateItem*>(sel.first())) {
        // Unset other starts
        for (auto* item : items()) {
            if (auto* other = dynamic_cast<StateItem*>(item)) {
                other->setStart(false);
            }
        }
        s->setStart(true);
    }
}

void GraphScene::toggleAcceptSelected() {
    for (auto* item : selectedItems()) {
        if (auto* s = dynamic_cast<StateItem*>(item)) {
            s->setAccept(!s->isAccept());
        }
    }
}

void GraphScene::highlightStates(const std::vector<std::string>& ids, QColor color) {
    for (auto* item : items()) {
        if (auto* s = dynamic_cast<StateItem*>(item)) {
            for (const auto& id : ids) {
                if (s->getId().toStdString() == id) {
                    s->setHighlight(color);
                }
            }
        }
    }
}

void GraphScene::clearHighlights() {
    for (auto* item : items()) {
        if (auto* s = dynamic_cast<StateItem*>(item)) s->clearHighlight();
        if (auto* t = dynamic_cast<TransitionItem*>(item)) t->clearHighlight();
    }
}

void GraphScene::mousePressEvent(QGraphicsSceneMouseEvent *event) {
    QPointF pos = event->scenePos();
    StateItem* item = itemAtPos(pos);
    
    if (m_mode == ToolMode::AddState) {
        if (!item) {
            QString id = QString("q%1").arg(m_stateCounter++);
            StateItem* newItem = new StateItem(id, pos.x(), pos.y(), false, false);
            addItem(newItem);
        }
    } else if (m_mode == ToolMode::AddTransition) {
        if (item) {
            if (!m_tempStartNode) {
                m_tempStartNode = item;
                m_tempLine = new QGraphicsLineItem(QLineF(pos, pos));
                addItem(m_tempLine);
            } else {
                createTransition(m_tempStartNode, item);
                removeItem(m_tempLine);
                delete m_tempLine;
                m_tempLine = nullptr;
                m_tempStartNode = nullptr;
            }
        } else {
            // Cancel transition
            if (m_tempStartNode) {
                removeItem(m_tempLine);
                delete m_tempLine;
                m_tempLine = nullptr;
                m_tempStartNode = nullptr;
            }
        }
    } else {
        QGraphicsScene::mousePressEvent(event);
    }
}

void GraphScene::mouseMoveEvent(QGraphicsSceneMouseEvent *event) {
    if (m_mode == ToolMode::AddTransition && m_tempLine) {
        QLineF line(m_tempStartNode->pos(), event->scenePos());
        m_tempLine->setLine(line);
    }
    
    // Update transitions for moving nodes
    if (m_mode == ToolMode::Select) {
         QGraphicsScene::mouseMoveEvent(event);
         for (auto* item : items()) {
             if (auto* t = dynamic_cast<TransitionItem*>(item)) {
                 t->updatePosition();
             }
         }
    }
}

void GraphScene::createTransition(StateItem* source, StateItem* target) {
    bool ok;
    QString text = QInputDialog::getText(nullptr, "Add Transition",
                                         "Symbols (comma separated):", QLineEdit::Normal,
                                         "", &ok);
    if (ok && !text.isEmpty()) {
        // Simple check if transition already exists
        // (In a real app, merging logic would be better)
        TransitionItem* t = new TransitionItem(source, target, text);
        addItem(t);
    }
}

StateItem* GraphScene::itemAtPos(const QPointF& pos) {
    for (auto* item : items()) {
        if (auto* s = dynamic_cast<StateItem*>(item)) {
            if (s->contains(s->mapFromScene(pos))) return s;
        }
    }
    return nullptr;
}