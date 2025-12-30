#pragma once
#include "../model/DFA.h"
#include <QJsonObject>
#include <QJsonArray>
#include <QJsonDocument>
#include <QString>

class DFAJson {
public:
    static QByteArray toJson(const DFA& dfa);
    static bool fromJson(const QByteArray& json, DFA& dfa, QString& error);
};