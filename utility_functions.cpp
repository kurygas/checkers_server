#include "utility_functions.h"

QList<QString> Split(const QString& str) {
    QList<QString> result(1, "");

    for (const auto& symbol : str) {
        if (symbol == '$') {
            result.emplace_back();
        }
        else {
            result.back().push_back(symbol);
        }
    }

    return result;
}
