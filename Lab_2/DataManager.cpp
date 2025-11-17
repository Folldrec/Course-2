#include "DataManager.h"
#include <QFile>
#include <QJsonDocument>
#include <QJsonArray>
#include <QJsonObject>
#include <QTextStream>

DataManager::DataManager(QObject *parent) 
    : QObject(parent), nextRefId(1), nextCtxId(1) {
        
}

void DataManager::addReference(const Reference& ref) {
    Reference newRef = ref;
    if (newRef.id == 0) {
        newRef.id = nextRefId++;
    }
    references.append(newRef);
    emit referenceAdded(newRef.id);
    emit dataChanged();
}

void DataManager::updateReference(const Reference& ref) {
    for (int i = 0; i < references.size(); i++) {
        if (references[i].id == ref.id) {
            references[i] = ref;
            emit referenceUpdated(ref.id);
            emit dataChanged();
            return;
        }
    }
}

void DataManager::deleteReference(int id) {
    for (int i = 0; i < references.size(); i++) {
        if (references[i].id == id) {
            references.removeAt(i);
            
            for (auto& ref : references) {
                ref.relatedIds.removeAll(id);
            }
            
            emit referenceDeleted(id);
            emit dataChanged();
            return;
        }
    }
}

Reference* DataManager::getReference(int id) {
    for (auto& ref : references) {
        if (ref.id == id) return &ref;
    }
    return nullptr;
}

QList<Reference*> DataManager::getAllReferences() {
    QList<Reference*> result;
    for (auto& ref : references) {
        result.append(&ref);
    }
    return result;
}

QList<Reference*> DataManager::searchReferences(const QString& query, int contextId) {
    QList<Reference*> result;
    QString lowerQuery = query.toLower();
    
    for (auto& ref : references) {
        bool matchesQuery = query.isEmpty() ||
            ref.title.toLower().contains(lowerQuery) ||
            ref.authors.toLower().contains(lowerQuery);
            
        bool matchesContext = (contextId == -1) || 
            ref.contextIds.contains(contextId);
        
        if (matchesQuery && matchesContext) {
            result.append(&ref);
        }
    }
    
    return result;
}

void DataManager::addContext(const Context& ctx) {
    Context newCtx = ctx;
    if (newCtx.id == 0) {
        newCtx.id = nextCtxId++;
    }
    contexts.append(newCtx);
    emit contextAdded(newCtx.id);
    emit dataChanged();
}

void DataManager::updateContext(const Context& ctx) {
    for (int i = 0; i < contexts.size(); i++) {
        if (contexts[i].id == ctx.id) {
            contexts[i] = ctx;
            emit contextUpdated(ctx.id);
            emit dataChanged();
            return;
        }
    }
}

void DataManager::deleteContext(int id) {
    for (int i = 0; i < contexts.size(); i++) {
        if (contexts[i].id == id) {
            contexts.removeAt(i);
            
            for (auto& ref : references) {
                ref.contextIds.removeAll(id);
            }
            
            emit contextDeleted(id);
            emit dataChanged();
            return;
        }
    }
}

Context* DataManager::getContext(int id) {
    for (auto& ctx : contexts) {
        if (ctx.id == id) return &ctx;
    }
    return nullptr;
}

QList<Context*> DataManager::getAllContexts() {
    QList<Context*> result;
    for (auto& ctx : contexts) {
        result.append(&ctx);
    }
    return result;
}

QList<Context*> DataManager::getChildContexts(int parentId) {
    QList<Context*> result;
    for (auto& ctx : contexts) {
        if (ctx.parentId == parentId) {
            result.append(&ctx);
        }
    }
    return result;
}

QString DataManager::getContextPath(int contextId) {
    QStringList path;
    Context* current = getContext(contextId);
    
    while (current) {
        path.prepend(current->name);
        if (current->parentId == -1) break;
        current = getContext(current->parentId);
    }
    
    return path.join(" / ");
}

bool DataManager::saveToFile(const QString& filename) {
    QJsonObject root;
    
    QJsonArray refsArray;
    for (const auto& ref : references) {
        refsArray.append(ref.toJson());
    }
    root["references"] = refsArray;
    
    QJsonArray ctxsArray;
    for (const auto& ctx : contexts) {
        ctxsArray.append(ctx.toJson());
    }
    root["contexts"] = ctxsArray;
    
    root["nextRefId"] = nextRefId;
    root["nextCtxId"] = nextCtxId;
    
    QJsonDocument doc(root);
    
    QFile file(filename);
    if (!file.open(QIODevice::WriteOnly)) {
        return false;
    }
    
    file.write(doc.toJson());
    file.close();
    return true;
}

bool DataManager::loadFromFile(const QString& filename) {
    QFile file(filename);
    if (!file.open(QIODevice::ReadOnly)) {
        return false;
    }
    
    QByteArray data = file.readAll();
    file.close();
    
    QJsonDocument doc = QJsonDocument::fromJson(data);
    QJsonObject root = doc.object();
    
    references.clear();
    contexts.clear();
    
    QJsonArray refsArray = root["references"].toArray();
    for (const auto& refVal : refsArray) {
        references.append(Reference(refVal.toObject()));
    }
    
    QJsonArray ctxsArray = root["contexts"].toArray();
    for (const auto& ctxVal : ctxsArray) {
        contexts.append(Context(ctxVal.toObject()));
    }
    
    nextRefId = root["nextRefId"].toInt(1);
    nextCtxId = root["nextCtxId"].toInt(1);
    
    emit dataChanged();
    return true;
}

bool DataManager::exportReferences(const QString& filename, const QString& format,
                                   const QList<Reference*>& refs) {
    QFile file(filename);
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text)) {
        return false;
    }
    
    QTextStream out(&file);
#if QT_VERSION < QT_VERSION_CHECK(6, 0, 0)
    out.setCodec("UTF-8");
#else
    out.setEncoding(QStringConverter::Utf8);
#endif
    
    int index = 1;
    for (const auto* ref : refs) {
        if (format == "bibtex") {
            out << ref->formatBibTeX() << "\n";
        } else if (format == "harvard") {
            out << ref->formatHarvard() << "\n\n";
        } else if (format == "dstu") {
            out << index << ". " << ref->formatDSTU() << "\n\n";
            index++;
        }
    }
    
    file.close();
    return true;
}

bool DataManager::importReferences(const QString& filename) {
    QFile file(filename);
    if (!file.open(QIODevice::ReadOnly)) {
        return false;
    }
    
    QByteArray data = file.readAll();
    file.close();
    
    QJsonDocument doc = QJsonDocument::fromJson(data);
    if (!doc.isArray()) {
        return false;
    }
    
    QJsonArray refsArray = doc.array();
    for (const auto& refVal : refsArray) {
        Reference ref(refVal.toObject());
        ref.id = 0; 
        addReference(ref);
    }
    
    return true;
}

int DataManager::getNextReferenceId() {
    return nextRefId;
}

int DataManager::getNextContextId() {
    return nextCtxId;
}