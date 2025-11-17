#ifndef EXPORTDIALOG_H
#define EXPORTDIALOG_H

#include <QDialog>
#include <QComboBox>
#include <QLabel>
#include "DataManager.h"

class ExportDialog : public QDialog {
    Q_OBJECT
    
public:
    ExportDialog(DataManager* dm, int contextId, const QString& search, 
                 QWidget *parent = nullptr);
    
private slots:
    void onExport();
    
private:
    void setupUI();
    
    DataManager* dataManager;
    int selectedContextId;
    QString searchQuery;
    
    QComboBox* formatCombo;
    QLabel* infoLabel;
};

#endif