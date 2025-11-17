#ifndef REFERENCE_H
#define REFERENCE_H

#include <QString>
#include <QStringList>
#include <QDateTime>
#include <QJsonObject>

enum class ReferenceType {
    Book,
    WebResource,
    Article,
    Video,
    Audio
};

class Reference {
public:
    Reference();
    Reference(const QJsonObject& json);
    
    int id;
    ReferenceType type;
    QString title;
    QString authors;
    int year;
    QString publisher;
    QString url;
    QList<int> contextIds;
    QString comments;
    QList<int> relatedIds;
    
    QJsonObject toJson() const;
    QString typeToString() const;
    static ReferenceType stringToType(const QString& str);
    
    QString formatBibTeX() const;
    QString formatHarvard() const;
    QString formatDSTU() const;
};

#endif