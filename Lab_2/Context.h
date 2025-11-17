#ifndef CONTEXT_H
#define CONTEXT_H

#include <QString>
#include <QJsonObject>

class Context {
public:
    Context();
    Context(const QJsonObject& json);
    
    int id;
    QString name;
    int parentId; 
    
    QJsonObject toJson() const;
};

#endif