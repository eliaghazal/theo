#pragma once
#include <QMainWindow>
#include <QListWidget>
#include <QTextEdit>
#include <QLineEdit>
#include <QLabel>
#include "GraphScene.h"
#include "algorithms/Hopcroft.h"

class MainWindow : public QMainWindow {
    Q_OBJECT
public:
    MainWindow(QWidget *parent = nullptr);

private slots:
    void onAddState();
    void onDelete();
    void onAddTransitionMode();
    void onSetStart();
    void onToggleAccept();
    void onUpdateAlphabet();
    void onImportJson();
    void onExportJson();
    void onValidate();
    void onRunHopcroft();
    void onStepSelected(int index);
    void onViewOriginal(bool checked);
    void onViewMinimized(bool checked);
    void onCompleteSink();

private:
    GraphScene* m_scene;
    QGraphicsView* m_view;
    
    // UI Elements
    QLineEdit* m_alphabetEdit;
    QLabel* m_validationLabel;
    
    // Hopcroft
    QListWidget* m_stepsList;
    QTextEdit* m_detailsText;
    std::vector<HopcroftStep> m_history;
    DFA m_originalDFA;
    DFA m_minimizedDFA;
};