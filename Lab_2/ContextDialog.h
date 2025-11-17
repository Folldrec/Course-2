#ifndef CONTEXTDIALOG_H
#define CONTEXTDIALOG_H

#include <QDialog>
#include <QLineEdit>
#include <QComboBox>
#include "DataManager.h"

class ContextDialog : public QDialog {
    Q_OBJECT
    
public:
    ContextDialog(DataManager* dm, QWidget *parent = nullptr);
    Context getContext() const;
    
private slots:
    void onAccept();
    
private:
    void setupUI();
    
    DataManager* dataManager;
    QLineEdit* nameEdit;
    QComboBox* parentCombo;
};

#endif