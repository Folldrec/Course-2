#include "ContextDialog.h"
#include <QVBoxLayout>
#include <QFormLayout>
#include <QPushButton>
#include <QHBoxLayout>
#include <QMessageBox>

ContextDialog::ContextDialog(DataManager* dm, QWidget *parent)
    : QDialog(parent), dataManager(dm) {
    
    setupUI();
    setWindowTitle("Додавання контексту");
}

void ContextDialog::setupUI() {
    resize(400, 150);
    
    QVBoxLayout* mainLayout = new QVBoxLayout(this);
    
    QFormLayout* formLayout = new QFormLayout();
    
    nameEdit = new QLineEdit();
    nameEdit->setPlaceholderText("Введіть назву контексту");
    formLayout->addRow("Назва:*", nameEdit);
    
    parentCombo = new QComboBox();
    parentCombo->addItem("(Без батьківського)", -1);
    
    QList<Context*> contexts = dataManager->getAllContexts();
    for (Context* ctx : contexts) {
        parentCombo->addItem(
            dataManager->getContextPath(ctx->id),
            ctx->id
        );
    }
    
    formLayout->addRow("Батьківський:", parentCombo);
    
    mainLayout->addLayout(formLayout);
    
    QHBoxLayout* buttonLayout = new QHBoxLayout();
    
    QPushButton* okButton = new QPushButton("✅ OK");
    okButton->setDefault(true);
    connect(okButton, &QPushButton::clicked, this, &ContextDialog::onAccept);
    buttonLayout->addWidget(okButton);
    
    QPushButton* cancelButton = new QPushButton("❌ Скасувати");
    connect(cancelButton, &QPushButton::clicked, this, &QDialog::reject);
    buttonLayout->addWidget(cancelButton);
    
    mainLayout->addLayout(buttonLayout);
}

void ContextDialog::onAccept() {
    if (nameEdit->text().isEmpty()) {
        QMessageBox::warning(this, "Помилка", "Будь ласка, введіть назву контексту.");
        return;
    }
    
    accept();
}

Context ContextDialog::getContext() const {
    Context ctx;
    ctx.name = nameEdit->text();
    ctx.parentId = parentCombo->currentData().toInt();
    return ctx;
}