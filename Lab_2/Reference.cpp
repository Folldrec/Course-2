#include "Reference.h"
#include <QJsonArray>

Reference::Reference() 
    : id(0), type(ReferenceType::Book), year(QDateTime::currentDateTime().date().year()) {}

Reference::Reference(const QJsonObject& json) {
    id = json["id"].toInt();
    type = stringToType(json["type"].toString());
    title = json["title"].toString();
    authors = json["authors"].toString();
    year = json["year"].toInt();
    publisher = json["publisher"].toString();
    url = json["url"].toString();
    comments = json["comments"].toString();
    
    QJsonArray contextsArray = json["contexts"].toArray();
    for (const auto& ctx : contextsArray) {
        contextIds.append(ctx.toInt());
    }
    
    QJsonArray relatedArray = json["related"].toArray();
    for (const auto& rel : relatedArray) {
        relatedIds.append(rel.toInt());
    }
}

QJsonObject Reference::toJson() const {
    QJsonObject obj;
    obj["id"] = id;
    obj["type"] = typeToString();
    obj["title"] = title;
    obj["authors"] = authors;
    obj["year"] = year;
    obj["publisher"] = publisher;
    obj["url"] = url;
    obj["comments"] = comments;
    
    QJsonArray contextsArray;
    for (int ctxId : contextIds) {
        contextsArray.append(ctxId);
    }
    obj["contexts"] = contextsArray;
    
    QJsonArray relatedArray;
    for (int relId : relatedIds) {
        relatedArray.append(relId);
    }
    obj["related"] = relatedArray;
    
    return obj;
}

QString Reference::typeToString() const {
    switch (type) {
        case ReferenceType::Book: return "Book";
        case ReferenceType::WebResource: return "WebResource";
        case ReferenceType::Article: return "Article";
        case ReferenceType::Video: return "Video";
        case ReferenceType::Audio: return "Audio";
        default: return "Book";
    }
}

ReferenceType Reference::stringToType(const QString& str) {
    if (str == "WebResource") return ReferenceType::WebResource;
    if (str == "Article") return ReferenceType::Article;
    if (str == "Video") return ReferenceType::Video;
    if (str == "Audio") return ReferenceType::Audio;
    return ReferenceType::Book;
}

QString Reference::formatBibTeX() const {
    QString typeStr = typeToString().toLower();
    QStringList authorParts = authors.split(',');
    QString firstAuthorLastName = authorParts.isEmpty() ? "unknown" : 
        authorParts[0].trimmed().split(' ').last().toLower();
    QString key = QString("%1%2").arg(firstAuthorLastName).arg(year);
    
    QString result = QString("@%1{%2,\n").arg(typeStr, key);
    result += QString("  title = {%1},\n").arg(title);
    result += QString("  author = {%1},\n").arg(authors);
    result += QString("  year = {%1}").arg(year);
    if (!publisher.isEmpty()) result += QString(",\n  publisher = {%1}").arg(publisher);
    if (!url.isEmpty()) result += QString(",\n  url = {%1}").arg(url);
    result += "\n}\n";
    
    return result;
}

QString Reference::formatHarvard() const {
    QString result = QString("%1 (%2) %3").arg(authors).arg(year).arg(title);
    if (!publisher.isEmpty()) result += QString(". %1").arg(publisher);
    if (!url.isEmpty()) result += QString(". Available at: %1").arg(url);
    result += ".";
    return result;
}

QString Reference::formatDSTU() const {
    QString result = QString("%1 %2").arg(authors, title);
    if (!publisher.isEmpty()) result += QString(" / %1").arg(publisher);
    result += QString(", %1").arg(year);
    if (!url.isEmpty()) result += QString(". URL: %1").arg(url);
    result += ".";
    return result;
}