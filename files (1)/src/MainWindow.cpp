#include "MainWindow.h"
#include "io/DFAJson.h"
#include <QToolBar>
#include <QDockWidget>
#include <QVBoxLayout>
#include <QPushButton>
#include <QFileDialog>
#include <QMessageBox>
#include <QGraphicsView>
#include <QSplitter>
#include <QGroupBox>
#include <QRadioButton>
#include <QButtonGroup>

MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent) {
    setWindowTitle("DFA Minimizer (Hopcroft)");
    resize(1200, 800);
    
    // Central View
    m_scene = new GraphScene(this);
    m_view = new QGraphicsView(m_scene);
    m_view->setRenderHint(QPainter::Antialiasing);
    m_view->setDragMode(QGraphicsView::RubberBandDrag);
    setCentralWidget(m_view);
    
    // --- Builder Dock (Left) ---
    QDockWidget* buildDock = new QDockWidget("Builder", this);
    buildDock->setAllowedAreas(Qt::LeftDockWidgetArea | Qt::RightDockWidgetArea);
    QWidget* buildWidget = new QWidget();
    QVBoxLayout* buildLayout = new QVBoxLayout(buildWidget);
    
    // Alphabet
    buildLayout->addWidget(new QLabel("Alphabet (comma sep):"));
    m_alphabetEdit = new QLineEdit("a,b");
    connect(m_alphabetEdit, &QLineEdit::editingFinished, this, &MainWindow::onUpdateAlphabet);
    buildLayout->addWidget(m_alphabetEdit);
    
    // Buttons
    QPushButton* btnAddState = new QPushButton("Add State Mode");
    QPushButton* btnDel = new QPushButton("Delete Selected");
    QPushButton* btnTrans = new QPushButton("Add Transition Mode");
    QPushButton* btnStart = new QPushButton("Set Start");
    QPushButton* btnAccept = new QPushButton("Toggle Accept");
    QPushButton* btnSink = new QPushButton("Complete w/ Sink");
    
    buildLayout->addWidget(btnAddState);
    buildLayout->addWidget(btnTrans);
    buildLayout->addWidget(btnDel);
    buildLayout->addWidget(btnStart);
    buildLayout->addWidget(btnAccept);
    buildLayout->addWidget(btnSink);
    
    // Validation
    QPushButton* btnVal = new QPushButton("Validate");
    m_validationLabel = new QLabel("Ready");
    m_validationLabel->setWordWrap(true);
    buildLayout->addWidget(btnVal);
    buildLayout->addWidget(m_validationLabel);
    
    // IO
    QPushButton* btnImport = new QPushButton("Import JSON");
    QPushButton* btnExport = new QPushButton("Export JSON");
    buildLayout->addWidget(btnImport);
    buildLayout->addWidget(btnExport);
    
    buildLayout->addStretch();
    buildDock->setWidget(buildWidget);
    addDockWidget(Qt::LeftDockWidgetArea, buildDock);
    
    // --- Hopcroft Dock (Right) ---
    QDockWidget* algoDock = new QDockWidget("Hopcroft Algorithm", this);
    algoDock->setAllowedAreas(Qt::RightDockWidgetArea);
    QWidget* algoWidget = new QWidget();
    QVBoxLayout* algoLayout = new QVBoxLayout(algoWidget);
    
    QPushButton* btnRun = new QPushButton("Run Minimization");
    algoLayout->addWidget(btnRun);
    
    // View Toggle
    QGroupBox* viewGroup = new QGroupBox("View");
    QHBoxLayout* viewLayout = new QHBoxLayout(viewGroup);
    QRadioButton* rbOriginal = new QRadioButton("Original");
    QRadioButton* rbMin = new QRadioButton("Minimized");
    rbOriginal->setChecked(true);
    viewLayout->addWidget(rbOriginal);
    viewLayout->addWidget(rbMin);
    algoLayout->addWidget(viewGroup);
    
    QButtonGroup* bg = new QButtonGroup(this);
    bg->addButton(rbOriginal);
    bg->addButton(rbMin);
    
    connect(rbOriginal, &QRadioButton::toggled, this, &MainWindow::onViewOriginal);
    connect(rbMin, &QRadioButton::toggled, this, &MainWindow::onViewMinimized);
    
    algoLayout->addWidget(new QLabel("Step Timeline:"));
    m_stepsList = new QListWidget();
    algoLayout->addWidget(m_stepsList);
    
    algoLayout->addWidget(new QLabel("Explanation:"));
    m_detailsText = new QTextEdit();
    m_detailsText->setReadOnly(true);
    m_detailsText->setMaximumHeight(100);
    algoLayout->addWidget(m_detailsText);
    
    algoDock->setWidget(algoWidget);
    addDockWidget(Qt::RightDockWidgetArea, algoDock);
    
    // Connects
    connect(btnAddState, &QPushButton::clicked, this, &MainWindow::onAddState);
    connect(btnTrans, &QPushButton::clicked, this, &MainWindow::onAddTransitionMode);
    connect(btnDel, &QPushButton::clicked, this, &MainWindow::onDelete);
    connect(btnStart, &QPushButton::clicked, this, &MainWindow::onSetStart);
    connect(btnAccept, &QPushButton::clicked, this, &MainWindow::onToggleAccept);
    connect(btnSink, &QPushButton::clicked, this, &MainWindow::onCompleteSink);
    connect(btnVal, &QPushButton::clicked, this, &MainWindow::onValidate);
    connect(btnImport, &QPushButton::clicked, this, &MainWindow::onImportJson);
    connect(btnExport, &QPushButton::clicked, this, &MainWindow::onExportJson);
    connect(btnRun, &QPushButton::clicked, this, &MainWindow::onRunHopcroft);
    connect(m_stepsList, &QListWidget::currentRowChanged, this, &MainWindow::onStepSelected);

    // Initial alphabet
    onUpdateAlphabet();
}

void MainWindow::onAddState() {
    m_scene->setMode(ToolMode::AddState);
    statusBar()->showMessage("Click on canvas to add state.");
}

void MainWindow::onAddTransitionMode() {
    m_scene->setMode(ToolMode::AddTransition);
    statusBar()->showMessage("Click Source then Target to add transition.");
}

void MainWindow::onDelete() {
    m_scene->deleteSelected();
}

void MainWindow::onSetStart() {
    m_scene->setStartSelected();
}

void MainWindow::onToggleAccept() {
    m_scene->toggleAcceptSelected();
}

void MainWindow::onUpdateAlphabet() {
    QString txt = m_alphabetEdit->text();
    QStringList parts = txt.split(',', Qt::SkipEmptyParts);
    std::vector<std::string> alpha;
    for (auto p : parts) alpha.push_back(p.trimmed().toStdString());
    m_scene->setAlphabet(alpha);
}

void MainWindow::onImportJson() {
    QString path = QFileDialog::getOpenFileName(this, "Import DFA", "", "JSON (*.json)");
    if (path.isEmpty()) return;
    
    QFile f(path);
    if (f.open(QIODevice::ReadOnly)) {
        DFA dfa;
        QString err;
        if (DFAJson::fromJson(f.readAll(), dfa, err)) {
            m_scene->loadFromDFA(dfa);
            
            // Update UI alphabet
            QStringList alphaList;
            for(auto s : dfa.alphabet) alphaList << QString::fromStdString(s);
            m_alphabetEdit->setText(alphaList.join(","));
            
            statusBar()->showMessage("Loaded DFA.");
        } else {
            QMessageBox::warning(this, "Error", "Failed to load: " + err);
        }
    }
}

void MainWindow::onExportJson() {
    DFA dfa = m_scene->exportToDFA();
    QByteArray data = DFAJson::toJson(dfa);
    
    QString path = QFileDialog::getSaveFileName(this, "Export DFA", "", "JSON (*.json)");
    if (path.isEmpty()) return;
    
    QFile f(path);
    if (f.open(QIODevice::WriteOnly)) {
        f.write(data);
        statusBar()->showMessage("Saved DFA.");
    }
}

void MainWindow::onValidate() {
    DFA dfa = m_scene->exportToDFA();
    auto missing = dfa.getMissingTransitions();
    bool deterministic = dfa.isDeterministic(); // Basic check
    // Advanced deterministic check: Ensure no state has >1 transition for same symbol
    // Our map model structure in DFA.h handles unique keys, but the Scene might produce
    // duplicate arrows visually. The exportToDFA combines them or overwrites.
    // Let's rely on Scene export correctness.
    
    QString status = "DFA is ";
    if (missing.empty()) status += "Complete. ";
    else status += "Partial (Missing " + QString::number(missing.size()) + " transitions). ";
    
    m_validationLabel->setText(status);
}

void MainWindow::onCompleteSink() {
    DFA dfa = m_scene->exportToDFA();
    dfa.completeWithSink();
    m_scene->loadFromDFA(dfa);
    statusBar()->showMessage("Completed with Sink State.");
}

void MainWindow::onRunHopcroft() {
    m_originalDFA = m_scene->exportToDFA();
    auto result = Hopcroft::minimize(m_originalDFA);
    m_minimizedDFA = result.first;
    m_history = result.second;
    
    m_stepsList->clear();
    for (size_t i=0; i<m_history.size(); ++i) {
        m_stepsList->addItem(QString("Step %1: Split by '%2'")
                             .arg(i+1)
                             .arg(QString::fromStdString(m_history[i].symbol)));
    }
    
    QMessageBox::information(this, "Hopcroft", "Minimization Complete! " + QString::number(m_minimizedDFA.states.size()) + " states.");
}

void MainWindow::onStepSelected(int index) {
    if (index < 0 || index >= (int)m_history.size()) return;
    
    const auto& step = m_history[index];
    m_detailsText->setText(QString::fromStdString(step.explanation));
    
    // Highlight states in scene
    m_scene->clearHighlights();
    
    // Highlight set A (splitter) in Cyan
    // Highlight set X (preimage) in Yellow
    // Highlight split blocks (Y) in Red
    
    // To visualize this on the *Original* graph (which the algo ran on), ensure we are in Original View
    // (Assuming user hasn't switched to Minimized view yet, or we force it)
    
    m_scene->highlightStates(std::vector<std::string>(step.A.begin(), step.A.end()), Qt::cyan);
    m_scene->highlightStates(std::vector<std::string>(step.X.begin(), step.X.end()), Qt::yellow);
    
    // Only highlight states involved in a split
    for (const auto& split : step.splits) {
         std::vector<std::string> Yvec(split.Y.begin(), split.Y.end());
         m_scene->highlightStates(Yvec, Qt::red);
    }
}

void MainWindow::onViewOriginal(bool checked) {
    if (checked) {
        m_scene->loadFromDFA(m_originalDFA);
    }
}

void MainWindow::onViewMinimized(bool checked) {
    if (checked) {
        if (m_minimizedDFA.states.empty()) {
             QMessageBox::warning(this, "Info", "Run Hopcroft first.");
             return;
        }
        m_scene->loadFromDFA(m_minimizedDFA);
    }
}