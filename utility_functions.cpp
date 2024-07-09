#include "utility_functions.h"

QList<QList<QString>> Parse(const QString& data) {
    QList<QList<QString>> result;

    for (const auto& symbol : data) {
        if (symbol == '#') {
            result.emplace_back(1, "");
        }
        else if (symbol == '$') {
            result.back().emplace_back();
        }
        else {
            result.back().back().push_back(symbol);
        }
    }

    return result;
}

QString Combine(const QList<QString>& data) {
    QString result = "#";

    for (const auto& str : data) {
        result.push_back(str);
        result.push_back('$');
    }

    result.chop(1);
    return result;
}
