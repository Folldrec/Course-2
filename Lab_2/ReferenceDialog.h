#ifndef REFERENCEDIALOG_H
#define REFERENCEDIALOG_H

#include <QDialog>
#include <QLineEdit>
#include <QSpinBox>
#include <QComboBox>
#include <QTextEdit>
#include <QListWidget>
#include "DataManager.h"

class ReferenceDialog : public QDialog {
    Q_OBJECT
    
public:
    ReferenceDialog(DataManager* dm, QWidget *parent = nullptr, Reference* editRef = nullptr);
    Reference getReference() const;
    
private slots:
    void onAccept();
    void onSearchRelated(const QString& text);
    
private:
    void setupUI();
    
    DataManager* dataManager;
    Reference* originalRef;
    
    QComboBox* typeCombo;     
    QLineEdit* titleEdit;      
    QLineEdit* authorsEdit;    
    QSpinBox* yearSpin;         
    QLineEdit* publisherEdit;   
    QLineEdit* urlEdit;         
    QTextEdit* commentsEdit;    
    QListWidget* contextList;   
    QLineEdit* relatedSearch;   
    QListWidget* relatedList;   
};

#endif