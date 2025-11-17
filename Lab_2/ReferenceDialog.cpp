#include "ReferenceDialog.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QFormLayout>
#include <QGroupBox>
#include <QPushButton>
#include <QLabel>
#include <QMessageBox>

ReferenceDialog::ReferenceDialog(DataManager* dm, QWidget *parent, Reference* editRef)
    : QDialog(parent), dataManager(dm), originalRef(editRef) {
    
    setupUI();
    
    if (editRef) {
        setWindowTitle("Редагування посилання");
        
        typeCombo->setCurrentIndex(static_cast<int>(editRef->type));
        titleEdit->setText(editRef->title);
        authorsEdit->setText(editRef->authors);
        yearSpin->setValue(editRef->year);
        publisherEdit->setText(editRef->publisher);
        urlEdit->setText(editRef->url);
        commentsEdit->setPlainText(editRef->comments);
        
        for (int i = 0; i < contextList->count(); i++) {
            QListWidgetItem* item = contextList->item(i);
            int ctxId = item->data(Qt::UserRole).toInt();
            if (editRef->contextIds.contains(ctxId)) {
                item->setCheckState(Qt::Checked);
            }
        }
        
        for (int i = 0; i < relatedList->count(); i++) {
            QListWidgetItem* item = relatedList->item(i);
            int refId = item->data(Qt::UserRole).toInt();
            if (editRef->relatedIds.contains(refId)) {
                item->setCheckState(Qt::Checked);
            }
        }
    } else {
        setWindowTitle("Додавання посилання");
    }
}

void ReferenceDialog::setupUI() {
    resize(600, 700);
    
    QVBoxLayout* mainLayout = new QVBoxLayout(this);
    
    QFormLayout* formLayout = new QFormLayout();
    
    typeCombo = new QComboBox(); 
    typeCombo->addItem("📖 Книга", static_cast<int>(ReferenceType::Book));
    typeCombo->addItem("🌐 Веб-ресурс", static_cast<int>(ReferenceType::WebResource));
    typeCombo->addItem("📄 Стаття", static_cast<int>(ReferenceType::Article));
    typeCombo->addItem("🎬 Відео", static_cast<int>(ReferenceType::Video));
    typeCombo->addItem("🎵 Аудіо", static_cast<int>(ReferenceType::Audio));
    formLayout->addRow("Тип:", typeCombo);
    
    titleEdit = new QLineEdit(); 
    titleEdit->setPlaceholderText("Введіть назву посилання");
    formLayout->addRow("Назва:*", titleEdit);
    
    authorsEdit = new QLineEdit(); 
    authorsEdit->setPlaceholderText("Прізвище І.Б., Прізвище2 І.Б.");
    formLayout->addRow("Автори:*", authorsEdit);
    
    yearSpin = new QSpinBox(); 
    yearSpin->setRange(1900, 2100);
    yearSpin->setValue(QDateTime::currentDateTime().date().year());
    formLayout->addRow("Рік:", yearSpin);
    
    publisherEdit = new QLineEdit();
    publisherEdit->setPlaceholderText("Назва видавництва");
    formLayout->addRow("Видавець:", publisherEdit);
    
    urlEdit = new QLineEdit();
    urlEdit->setPlaceholderText("https://example.com");
    formLayout->addRow("URL:", urlEdit);
    
    mainLayout->addLayout(formLayout);
    
    QGroupBox* contextBox = new QGroupBox("Контексти (можна обрати кілька)");
    QVBoxLayout* contextLayout = new QVBoxLayout(contextBox);
    
    contextList = new QListWidget();  
    contextList->setMaximumHeight(120);
    
    QList<Context*> allContexts = dataManager->getAllContexts();
    for (Context* ctx : allContexts) {
        QListWidgetItem* item = new QListWidgetItem(
            dataManager->getContextPath(ctx->id)
        );
        item->setData(Qt::UserRole, ctx->id);
        item->setCheckState(Qt::Unchecked);
        contextList->addItem(item);
    }
    
    contextLayout->addWidget(contextList);
    mainLayout->addWidget(contextBox);
    
    QGroupBox* relatedBox = new QGroupBox("Зв'язані посилання");
    QVBoxLayout* relatedLayout = new QVBoxLayout(relatedBox);
    
    relatedSearch = new QLineEdit(); 
    relatedSearch->setPlaceholderText("🔍 Пошук посилань");
    relatedLayout->addWidget(relatedSearch);
    connect(relatedSearch, &QLineEdit::textChanged, 
            this, &ReferenceDialog::onSearchRelated);
    
    relatedList = new QListWidget();
    relatedList->setMaximumHeight(120);
    
    QList<Reference*> allRefs = dataManager->getAllReferences();
    for (Reference* ref : allRefs) {
        if (originalRef && ref->id == originalRef->id) continue;
        
        QString itemText = QString("%1 (%2)")
            .arg(ref->title)
            .arg(ref->year);
        QListWidgetItem* item = new QListWidgetItem(itemText);
        item->setData(Qt::UserRole, ref->id);
        item->setCheckState(Qt::Unchecked);
        relatedList->addItem(item);
    }
    
    relatedLayout->addWidget(relatedList);
    mainLayout->addWidget(relatedBox);
    
    QLabel* commentsLabel = new QLabel("Коментарі:");
    mainLayout->addWidget(commentsLabel);
    
    commentsEdit = new QTextEdit(); 
    commentsEdit->setMaximumHeight(80);
    commentsEdit->setPlaceholderText("Додаткові нотатки про це посилання");
    mainLayout->addWidget(commentsEdit);
    
    QHBoxLayout* buttonLayout = new QHBoxLayout();
    
    QPushButton* okButton = new QPushButton("✅ OK");
    okButton->setDefault(true);
    connect(okButton, &QPushButton::clicked, this, &ReferenceDialog::onAccept);
    buttonLayout->addWidget(okButton);
    
    QPushButton* cancelButton = new QPushButton("❌ Скасувати");
    connect(cancelButton, &QPushButton::clicked, this, &QDialog::reject);
    buttonLayout->addWidget(cancelButton);
    
    mainLayout->addLayout(buttonLayout);
}

void ReferenceDialog::onAccept() {
    if (titleEdit->text().isEmpty() || authorsEdit->text().isEmpty()) {
        QMessageBox::warning(this, "Помилка", 
            "Будь ласка, заповніть обов'язкові поля: назву та авторів.");
        return;
    }
    
    accept();
}

void ReferenceDialog::onSearchRelated(const QString& text) {
    QString lowerText = text.toLower();
    
    for (int i = 0; i < relatedList->count(); i++) {
        QListWidgetItem* item = relatedList->item(i);
        bool matches = text.isEmpty() || 
            item->text().toLower().contains(lowerText);
        item->setHidden(!matches);
    }
}

Reference ReferenceDialog::getReference() const {
    Reference ref;
    
    ref.type = static_cast<ReferenceType>(
        typeCombo->currentData().toInt()
    );
    ref.title = titleEdit->text();
    ref.authors = authorsEdit->text();
    ref.year = yearSpin->value();
    ref.publisher = publisherEdit->text();
    ref.url = urlEdit->text();
    ref.comments = commentsEdit->toPlainText();
    
    for (int i = 0; i < contextList->count(); i++) {
        QListWidgetItem* item = contextList->item(i);
        if (item->checkState() == Qt::Checked) {
            ref.contextIds.append(item->data(Qt::UserRole).toInt());
        }
    }
    
    for (int i = 0; i < relatedList->count(); i++) {
        QListWidgetItem* item = relatedList->item(i);
        if (item->checkState() == Qt::Checked) {
            ref.relatedIds.append(item->data(Qt::UserRole).toInt());
        }
    }
    
    return ref;
}