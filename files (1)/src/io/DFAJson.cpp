#include "DFAJson.h"

QByteArray DFAJson::toJson(const DFA& dfa) {
    QJsonObject root;
    
    QJsonArray alphaArr;
    for(const auto& s : dfa.alphabet) alphaArr.append(QString::fromStdString(s));
    root["alphabet"] = alphaArr;
    
    QJsonArray stateArr;
    for(const auto& s : dfa.states) {
        QJsonObject sObj;
        sObj["id"] = QString::fromStdString(s.id);
        sObj["start"] = s.isStart;
        sObj["accept"] = s.isAccept;
        sObj["x"] = s.x;
        sObj["y"] = s.y;
        stateArr.append(sObj);
    }
    root["states"] = stateArr;
    
    QJsonArray transArr;
    for(const auto& kv : dfa.transitions) {
        QJsonObject tObj;
        tObj["from"] = QString::fromStdString(kv.first.first);
        tObj["symbol"] = QString::fromStdString(kv.first.second);
        tObj["to"] = QString::fromStdString(kv.second);
        transArr.append(tObj);
    }
    root["transitions"] = transArr;
    
    QJsonDocument doc(root);
    return doc.toJson(QJsonDocument::Indented);
}

bool DFAJson::fromJson(const QByteArray& json, DFA& dfa, QString& error) {
    QJsonDocument doc = QJsonDocument::fromJson(json);
    if (doc.isNull()) {
        error = "Invalid JSON";
        return false;
    }
    if (!doc.isObject()) {
        error = "JSON root must be an object";
        return false;
    }
    
    dfa.clear();
    QJsonObject root = doc.object();
    
    if (root.contains("alphabet") && root["alphabet"].isArray()) {
        QJsonArray arr = root["alphabet"].toArray();
        for(const auto& v : arr) dfa.alphabet.push_back(v.toString().toStdString());
    }
    
    if (root.contains("states") && root["states"].isArray()) {
        QJsonArray arr = root["states"].toArray();
        for(const auto& v : arr) {
            QJsonObject sObj = v.toObject();
            dfa.addState(
                sObj["id"].toString().toStdString(),
                sObj["start"].toBool(),
                sObj["accept"].toBool(),
                sObj["x"].toDouble(),
                sObj["y"].toDouble()
            );
        }
    }
    
    if (root.contains("transitions") && root["transitions"].isArray()) {
        QJsonArray arr = root["transitions"].toArray();
        for(const auto& v : arr) {
            QJsonObject tObj = v.toObject();
            dfa.addTransition(
                tObj["from"].toString().toStdString(),
                tObj["symbol"].toString().toStdString(),
                tObj["to"].toString().toStdString()
            );
        }
    }
    
    return true;
}