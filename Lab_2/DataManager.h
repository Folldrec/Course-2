#ifndef DATAMANAGER_H
#define DATAMANAGER_H

#include <QObject>
#include <QList>
#include "Reference.h"
#include "Context.h"

class DataManager : public QObject {
    Q_OBJECT
    
public:
    explicit DataManager(QObject *parent = nullptr);
    
    void addReference(const Reference& ref);
    void updateReference(const Reference& ref);
    void deleteReference(int id);
    Reference* getReference(int id);
    QList<Reference*> getAllReferences();
    QList<Reference*> searchReferences(const QString& query, int contextId = -1);
    
    void addContext(const Context& ctx);
    void updateContext(const Context& ctx);
    void deleteContext(int id);
    Context* getContext(int id);
    QList<Context*> getAllContexts();
    QList<Context*> getChildContexts(int parentId);
    QString getContextPath(int contextId);
    
    bool saveToFile(const QString& filename);
    bool loadFromFile(const QString& filename);
    bool exportReferences(const QString& filename, const QString& format, 
                         const QList<Reference*>& refs);
    bool importReferences(const QString& filename);
    
    int getNextReferenceId();
    int getNextContextId();
    
signals:
    void dataChanged();
    void referenceAdded(int id);
    void referenceUpdated(int id);
    void referenceDeleted(int id);
    void contextAdded(int id);
    void contextUpdated(int id);
    void contextDeleted(int id);
    
private:
    QList<Reference> references;
    QList<Context> contexts;
    int nextRefId;
    int nextCtxId;
};

#endif