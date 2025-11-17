#include "MainWindow.h"
#include "ReferenceDialog.h"
#include "ContextDialog.h"
#include "ExportDialog.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QSplitter>
#include <QMenuBar>
#include <QMessageBox>
#include <QFileDialog>
#include <QGroupBox>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent), selectedContextId(-1) {
    
    dataManager = new DataManager(this);
    
    setupUI();
    createActions();
    createMenus();
    
    updateContextTree();
    updateReferenceList();
    
    connect(dataManager, &DataManager::dataChanged, 
            this, &MainWindow::updateReferenceList);
}

MainWindow::~MainWindow() {
}

void MainWindow::setupUI() {
    setWindowTitle("Reference Manager - Менеджер посилань");
    resize(1200, 700);
    
    QWidget* centralWidget = new QWidget(this);
    setCentralWidget(centralWidget);
    
    QHBoxLayout* mainLayout = new QHBoxLayout(centralWidget);
    
    QWidget* leftPanel = new QWidget();
    QVBoxLayout* leftLayout = new QVBoxLayout(leftPanel);
    
    QLabel* contextLabel = new QLabel("Контексти:");
    contextLabel->setStyleSheet("font-weight: bold;");
    leftLayout->addWidget(contextLabel);
    
    contextTree = new QTreeWidget();
    contextTree->setHeaderLabel("Категорії");
    contextTree->setMinimumWidth(250);
    leftLayout->addWidget(contextTree);
    
    addCtxButton = new QPushButton("➕ Додати контекст"); 
    leftLayout->addWidget(addCtxButton);
    
    statsLabel = new QLabel(); 
    statsLabel->setStyleSheet("padding: 10px; background-color: #f0f0f0; border-radius: 5px;");
    leftLayout->addWidget(statsLabel);
    
    mainLayout->addWidget(leftPanel);
    
    QWidget* rightPanel = new QWidget();
    QVBoxLayout* rightLayout = new QVBoxLayout(rightPanel);
    
    QHBoxLayout* toolbarLayout = new QHBoxLayout();
    
    searchEdit = new QLineEdit(); 
    searchEdit->setPlaceholderText("🔍 Пошук за назвою або авторами...");
    toolbarLayout->addWidget(searchEdit);
    
    addRefButton = new QPushButton("➕ Додати посилання");
    addRefButton->setStyleSheet("background-color: #4CAF50; color: white; padding: 5px 15px;");
    toolbarLayout->addWidget(addRefButton);
    
    editRefButton = new QPushButton("✏️ Редагувати"); 
    editRefButton->setEnabled(false);
    toolbarLayout->addWidget(editRefButton);
    
    deleteRefButton = new QPushButton("🗑️ Видалити"); 
    deleteRefButton->setEnabled(false);
    deleteRefButton->setStyleSheet("background-color: #f44336; color: white;");
    toolbarLayout->addWidget(deleteRefButton);
    
    rightLayout->addLayout(toolbarLayout);
    
    QHBoxLayout* ioLayout = new QHBoxLayout();
    
    exportButton = new QPushButton("📥 Експорт"); 
    ioLayout->addWidget(exportButton);
    
    importButton = new QPushButton("📤 Імпорт"); 
    ioLayout->addWidget(importButton);
    
    ioLayout->addStretch();
    rightLayout->addLayout(ioLayout);
    
    QSplitter* splitter = new QSplitter(Qt::Vertical);
    
    referenceList = new QListWidget(); 
    splitter->addWidget(referenceList);
    
    QGroupBox* detailsBox = new QGroupBox("Деталі посилання");
    QVBoxLayout* detailsLayout = new QVBoxLayout(detailsBox);
    
    detailsText = new QTextEdit(); 
    detailsText->setReadOnly(true);
    detailsLayout->addWidget(detailsText);
    
    splitter->addWidget(detailsBox);
    splitter->setStretchFactor(0, 2);
    splitter->setStretchFactor(1, 1);
    
    rightLayout->addWidget(splitter);
    
    mainLayout->addWidget(rightPanel, 1);
    
    connect(addRefButton, &QPushButton::clicked, 
            this, &MainWindow::onAddReference);  
    connect(editRefButton, &QPushButton::clicked, 
            this, &MainWindow::onEditReference);  
    connect(deleteRefButton, &QPushButton::clicked, 
            this, &MainWindow::onDeleteReference); 
    connect(addCtxButton, &QPushButton::clicked, 
            this, &MainWindow::onAddContext); 
    connect(searchEdit, &QLineEdit::textChanged, 
            this, &MainWindow::onSearchChanged); 
    connect(contextTree, &QTreeWidget::itemSelectionChanged, 
            this, &MainWindow::onContextSelectionChanged); 
    connect(referenceList, &QListWidget::itemSelectionChanged, 
            this, &MainWindow::onReferenceSelectionChanged); 
    connect(exportButton, &QPushButton::clicked, 
            this, &MainWindow::onExportReferences); 
    connect(importButton, &QPushButton::clicked, 
            this, &MainWindow::onImportReferences); 
}

void MainWindow::createActions() {
    saveAction = new QAction("💾 Зберегти", this); 
    saveAction->setShortcut(QKeySequence::Save);
    connect(saveAction, &QAction::triggered, 
            this, &MainWindow::onSaveData); 
    
    loadAction = new QAction("📂 Завантажити", this); 
    loadAction->setShortcut(QKeySequence::Open);
    connect(loadAction, &QAction::triggered, 
            this, &MainWindow::onLoadData); 
    
    exitAction = new QAction("🚪 Вихід", this); 
    exitAction->setShortcut(QKeySequence::Quit);
    connect(exitAction, &QAction::triggered, 
            this, &QWidget::close); 
    
    aboutAction = new QAction("ℹ️ Про програму", this);
    connect(aboutAction, &QAction::triggered, this, [this]() {
        QMessageBox::about(this, "Про програму",
            "Reference Manager v1.0\n\n"
            "Менеджер бібліографічних посилань\n"
            "Лабораторна робота №2\n"
            "Парадигма ООП");
    });
}

void MainWindow::createMenus() {
    QMenuBar* menuBar = new QMenuBar(this);
    setMenuBar(menuBar);
    
    QMenu* fileMenu = menuBar->addMenu("📁 Файл");
    fileMenu->addAction(saveAction);
    fileMenu->addAction(loadAction);
    fileMenu->addSeparator();
    fileMenu->addAction(exitAction);
    
    QMenu* helpMenu = menuBar->addMenu("❓ Довідка");
    helpMenu->addAction(aboutAction);
}

void MainWindow::updateContextTree() {
    contextTree->clear();
    
    QTreeWidgetItem* allItem = new QTreeWidgetItem(contextTree);
    allItem->setText(0, "📚 Всі посилання");
    allItem->setData(0, Qt::UserRole, -1);
    
    buildContextTree(nullptr, -1);
    
    contextTree->expandAll();
    
    // Оновлення статистики
    int refCount = dataManager->getAllReferences().size();
    int ctxCount = dataManager->getAllContexts().size();
    statsLabel->setText(QString(
        "📊 Статистика:\n"
        "Посилань: %1\n"
        "Контекстів: %2"
    ).arg(refCount).arg(ctxCount));
}

void MainWindow::buildContextTree(QTreeWidgetItem* parent, int parentId) {
    QList<Context*> children = dataManager->getChildContexts(parentId);
    
    for (Context* ctx : children) {
        QTreeWidgetItem* item;
        if (parent) {
            item = new QTreeWidgetItem(parent);
        } else {
            item = new QTreeWidgetItem(contextTree);
        }
        
        item->setText(0, "📁 " + ctx->name);
        item->setData(0, Qt::UserRole, ctx->id);
        
        buildContextTree(item, ctx->id);
    }
}

void MainWindow::updateReferenceList() {
    QString search = searchEdit->text();
    QList<Reference*> refs = dataManager->searchReferences(search, selectedContextId);
    
    referenceList->clear();
    
    for (Reference* ref : refs) {
        QString icon;
        switch (ref->type) {
            case ReferenceType::Book: icon = "📖"; break;
            case ReferenceType::WebResource: icon = "🌐"; break;
            case ReferenceType::Article: icon = "📄"; break;
            case ReferenceType::Video: icon = "🎬"; break;
            case ReferenceType::Audio: icon = "🎵"; break;
        }
        
        QString itemText = QString("%1 %2 (%3, %4)")
            .arg(icon)
            .arg(ref->title)
            .arg(ref->authors.split(',').first())
            .arg(ref->year);
        
        QListWidgetItem* item = new QListWidgetItem(itemText);
        item->setData(Qt::UserRole, ref->id);
        referenceList->addItem(item);
    }
}

void MainWindow::showReferenceDetails(Reference* ref) {
    if (!ref) {
        detailsText->clear();
        return;
    }
    
    QString html = "<html><body style='font-family: Arial;'>";
    html += QString("<h2>%1</h2>").arg(ref->title);
    html += QString("<p><b>Автори:</b> %1</p>").arg(ref->authors);
    html += QString("<p><b>Рік:</b> %1</p>").arg(ref->year);
    html += QString("<p><b>Тип:</b> %1</p>").arg(ref->typeToString());
    
    if (!ref->publisher.isEmpty()) {
        html += QString("<p><b>Видавець:</b> %1</p>").arg(ref->publisher);
    }
    
    if (!ref->url.isEmpty()) {
        html += QString("<p><b>URL:</b> <a href='%1'>%1</a></p>").arg(ref->url);
    }
    
    if (!ref->contextIds.isEmpty()) {
        QStringList contexts;
        for (int ctxId : ref->contextIds) {
            contexts.append(dataManager->getContextPath(ctxId));
        }
        html += QString("<p><b>Контексти:</b> %1</p>").arg(contexts.join(", "));
    }
    
    if (!ref->relatedIds.isEmpty()) {
        QStringList related;
        for (int relId : ref->relatedIds) {
            Reference* relRef = dataManager->getReference(relId);
            if (relRef) {
                related.append(relRef->title);
            }
        }
        html += QString("<p><b>Зв'язані посилання:</b> %1</p>").arg(related.join(", "));
    }
    
    if (!ref->comments.isEmpty()) {
        html += QString("<p style='background-color: #ffffcc; padding: 10px;'>"
                       "<b>Коментарі:</b><br>%1</p>").arg(ref->comments);
    }
    
    html += "<hr><h3>Форматування:</h3>";
    html += "<p><b>BibTeX:</b></p><pre>" + ref->formatBibTeX() + "</pre>";
    html += "<p><b>Harvard:</b></p><pre>" + ref->formatHarvard() + "</pre>";
    html += "<p><b>ДСТУ:</b></p><pre>" + ref->formatDSTU() + "</pre>";
    
    html += "</body></html>";
    
    detailsText->setHtml(html);
}

void MainWindow::onAddReference() {
    ReferenceDialog dialog(dataManager, this);
    if (dialog.exec() == QDialog::Accepted) {
        Reference ref = dialog.getReference();
        dataManager->addReference(ref);
        updateReferenceList();
    }
}

void MainWindow::onEditReference() {
    QListWidgetItem* item = referenceList->currentItem();
    if (!item) return;
    
    int refId = item->data(Qt::UserRole).toInt();
    Reference* ref = dataManager->getReference(refId);
    if (!ref) return;
    
    ReferenceDialog dialog(dataManager, this, ref);
    if (dialog.exec() == QDialog::Accepted) {
        Reference updatedRef = dialog.getReference();
        updatedRef.id = refId;
        dataManager->updateReference(updatedRef);
        updateReferenceList();
    }
}

void MainWindow::onDeleteReference() {
    QListWidgetItem* item = referenceList->currentItem();
    if (!item) return;
    
    int refId = item->data(Qt::UserRole).toInt();
    
    QMessageBox::StandardButton reply = QMessageBox::question(
        this, "Підтвердження видалення",
        "Ви впевнені, що хочете видалити це посилання?",
        QMessageBox::Yes | QMessageBox::No
    );
    
    if (reply == QMessageBox::Yes) {
        dataManager->deleteReference(refId);
        updateReferenceList();
        detailsText->clear();
    }
}

void MainWindow::onAddContext() {
    ContextDialog dialog(dataManager, this);
    if (dialog.exec() == QDialog::Accepted) {
        Context ctx = dialog.getContext();
        dataManager->addContext(ctx);
        updateContextTree();
    }
}

void MainWindow::onSearchChanged(const QString& text) {
    updateReferenceList();
}

void MainWindow::onContextSelectionChanged() {
    QTreeWidgetItem* item = contextTree->currentItem();
    if (item) {
        selectedContextId = item->data(0, Qt::UserRole).toInt();
        updateReferenceList();
    }
}

void MainWindow::onReferenceSelectionChanged() {
    QListWidgetItem* item = referenceList->currentItem();
    bool hasSelection = (item != nullptr);
    
    editRefButton->setEnabled(hasSelection);
    deleteRefButton->setEnabled(hasSelection);
    
    if (hasSelection) {
        int refId = item->data(Qt::UserRole).toInt();
        Reference* ref = dataManager->getReference(refId);
        showReferenceDetails(ref);
    } else {
        detailsText->clear();
    }
}

void MainWindow::onExportReferences() {
    ExportDialog dialog(dataManager, selectedContextId, searchEdit->text(), this);
    dialog.exec();
}

void MainWindow::onImportReferences() {
    QString filename = QFileDialog::getOpenFileName(
        this, "Імпорт посилань", "", "JSON Files (*.json)"
    );
    
    if (filename.isEmpty()) return;
    
    if (dataManager->importReferences(filename)) {
        QMessageBox::information(this, "Успіх", "Посилання успішно імпортовано!");
        updateReferenceList();
        updateContextTree();
    } else {
        QMessageBox::warning(this, "Помилка", "Не вдалося імпортувати посилання.");
    }
}

void MainWindow::onSaveData() {
    QString filename = QFileDialog::getSaveFileName(
        this, "Зберегти дані", "", "JSON Files (*.json)"
    );
    
    if (filename.isEmpty()) return;
    
    if (dataManager->saveToFile(filename)) {
        QMessageBox::information(this, "Успіх", "Дані успішно збережено!");
    } else {
        QMessageBox::warning(this, "Помилка", "Не вдалося зберегти дані.");
    }
}

void MainWindow::onLoadData() {
    QString filename = QFileDialog::getOpenFileName(
        this, "Завантажити дані", "", "JSON Files (*.json)"
    );
    
    if (filename.isEmpty()) return;
    
    if (dataManager->loadFromFile(filename)) {
        QMessageBox::information(this, "Успіх", "Дані успішно завантажено!");
        updateContextTree();
        updateReferenceList();
        detailsText->clear();
    } else {
        QMessageBox::warning(this, "Помилка", "Не вдалося завантажити дані.");
    }
}