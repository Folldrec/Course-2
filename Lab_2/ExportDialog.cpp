#include "ExportDialog.h"
#include <QVBoxLayout>
#include <QFormLayout>
#include <QPushButton>
#include <QFileDialog>
#include <QMessageBox>

ExportDialog::ExportDialog(DataManager* dm, int contextId, const QString& search, 
                          QWidget *parent)
    : QDialog(parent), dataManager(dm), selectedContextId(contextId), 
      searchQuery(search) {
    
    setupUI();
    setWindowTitle("Експорт посилань");
}

void ExportDialog::setupUI() {
    resize(450, 200);
    
    QVBoxLayout* mainLayout = new QVBoxLayout(this);
    
    mainLayout->addWidget(new QLabel("Оберіть формат експорту:"));
    
    QFormLayout* formLayout = new QFormLayout();
    
    formatCombo = new QComboBox();
    formatCombo->addItem("BibTeX", "bibtex");
    formatCombo->addItem("Harvard", "harvard");
    formatCombo->addItem("ДСТУ ГОСТ 7.1:2006", "dstu");
    formLayout->addRow("Формат:", formatCombo);
    
    mainLayout->addLayout(formLayout);
    
    QList<Reference*> refs = dataManager->searchReferences(searchQuery, selectedContextId);
    
    infoLabel = new QLabel();
    QString infoText = QString("Буде експортовано: <b>%1</b> посилань").arg(refs.size());
    if (selectedContextId != -1) {
        infoText += QString("<br>З контексту: <b>%1</b>")
            .arg(dataManager->getContextPath(selectedContextId));
    }
    if (!searchQuery.isEmpty()) {
        infoText += QString("<br>За запитом: <b>%1</b>").arg(searchQuery);
    }
    infoLabel->setText(infoText);
    infoLabel->setStyleSheet("padding: 10px; background-color: #e3f2fd; border-radius: 5px;");
    mainLayout->addWidget(infoLabel);
    
    mainLayout->addStretch();
    
    QPushButton* exportButton = new QPushButton("📥 Експортувати");
    exportButton->setStyleSheet("background-color: #4CAF50; color: white; padding: 8px;");
    connect(exportButton, &QPushButton::clicked, this, &ExportDialog::onExport);
    mainLayout->addWidget(exportButton);
}

void ExportDialog::onExport() {
    QString format = formatCombo->currentData().toString();
    QString ext = (format == "bibtex") ? "bib" : "txt";
    
    QString filename = QFileDialog::getSaveFileName(
        this, "Експорт посилань", 
        QString("references.%1").arg(ext),
        QString("Files (*.%1)").arg(ext)
    );
    
    if (filename.isEmpty()) return;
    
    QList<Reference*> refs = dataManager->searchReferences(searchQuery, selectedContextId);
    
    if (dataManager->exportReferences(filename, format, refs)) {
        QMessageBox::information(this, "Успіх", 
            QString("Експортовано %1 посилань у файл %2")
                .arg(refs.size())
                .arg(filename));
        accept();
    } else {
        QMessageBox::warning(this, "Помилка", "Не вдалося експортувати посилання.");
    }
}