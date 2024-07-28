#include "query.h"

uint8_t Query::ToNum(char symbol) {
    return static_cast<uint8_t>(symbol);
}

QueryId Query::ToType(char symbol) {
    return static_cast<QueryId>(ToNum(symbol));
}

template<typename T>
char Query::ToChar(T&& data) {
    return static_cast<char>(data);
}

Query::Query(QueryId queryId)
: queryId_(queryId) {}

Query::Query(const QByteArray& bytes)
: queryId_(ToType(bytes.front())) {
    for (int i = 1; i < bytes.size(); ++i) {
        if (ToType(bytes[i]) == QueryId::String) {
            const uint16_t stringSize = ToNum(bytes[i + 1]);
            queryData_.push_back(bytes.sliced(i + 2, stringSize));
            i += stringSize + 1;
        }
        else if (ToType(bytes[i]) == QueryId::Int) {
            uint result = 0;

            for (int j = 0; j < 4; ++j) {
                const uint byte = ToNum(bytes[i + j + 1]) << (8 * j);
                result += byte;
            }

            queryData_.push_back(result);
        }
        else {
            queryData_.push_back(ToType(bytes[i]));
        }
    }
}

void Query::PushData(const std::variant<QString, uint, QueryId>& data) {
    queryData_.push_back(data);
}

QueryId Query::GetId() const {
    return queryId_;
}

QByteArray Query::ToBytes() const {
    QByteArray result(1, ToChar(QueryId::Query));
    result.push_back(ToChar(queryId_));

    for (const auto& data : queryData_) {
        if (std::holds_alternative<QueryId>(data)) {
            result.push_back(ToChar(std::get<QueryId>(data)));
        }
        else if (std::holds_alternative<uint>(data)) {
            uint16_t number = std::get<uint>(data);

            for (int i = 0; i < 4; ++i) {
                result.push_back(ToChar(number & 0xFF));
                number >>= 8;
            }

            result.push_back(ToChar(number & 0xFF));
            result.push_back(ToChar(number >> 8));
        }
        else {
            const auto str = std::get<QString>(data);
            result.push_back(ToChar(QueryId::String));
            result.push_back(ToChar(str.size()));
            result.push_back(str.toUtf8());
        }
    }

    return result;
}

template<typename T>
const T& Query::GetData(size_t index) const {
    return std::get<T>(queryData_[index]);
}

size_t Query::Size() const {
    return queryData_.size();
}
