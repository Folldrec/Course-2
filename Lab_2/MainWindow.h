#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QTreeWidget>
#include <QListWidget>
#include <QLineEdit>
#include <QPushButton>
#include <QComboBox>
#include <QTextEdit>
#include <QLabel>
#include "DataManager.h"

class MainWindow : public QMainWindow {
    Q_OBJECT
    
public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();
    
private slots:
    void onAddReference();
    void onEditReference();
    void onDeleteReference();
    void onAddContext();
    void onSearchChanged(const QString& text);
    void onContextSelectionChanged();
    void onReferenceSelectionChanged();
    void onExportReferences();
    void onImportReferences();
    void onSaveData();
    void onLoadData();
    void updateReferenceList();
    void updateContextTree();
    
private:
    void setupUI();
    void createActions();
    void createMenus();
    void buildContextTree(QTreeWidgetItem* parent, int parentId);
    void showReferenceDetails(Reference* ref);
    
    DataManager* dataManager;
    
    QTreeWidget* contextTree;    
    QListWidget* referenceList;  
    QLineEdit* searchEdit;       
    QPushButton* addRefButton;    
    QPushButton* editRefButton;   
    QPushButton* deleteRefButton;   
    QPushButton* addCtxButton;    
    QPushButton* exportButton;    
    QPushButton* importButton;  
    QTextEdit* detailsText;       
    QLabel* statsLabel;          
    
    QAction* saveAction;     
    QAction* loadAction;      
    QAction* exitAction;    
    QAction* aboutAction;    
    
    int selectedContextId;
};

#endif