#include "Context.h"

Context::Context() : id(0), parentId(-1) {}

Context::Context(const QJsonObject& json) {
    id = json["id"].toInt();
    name = json["name"].toString();
    parentId = json["parentId"].toInt(-1);
}

QJsonObject Context::toJson() const {
    QJsonObject obj;
    obj["id"] = id;
    obj["name"] = name;
    obj["parentId"] = parentId;
    return obj;
}