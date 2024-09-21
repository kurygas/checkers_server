#include "query.h"

Query::Query(const QueryId queryId)
: queryId_(queryId) {}

Query::Query(const QByteArray& bytes)
: queryId_(toId(bytes.front())) {
    for (auto i = 1; i < bytes.size(); ++i) {
        const auto id = toId(bytes[i]);

        if (id == QueryId::String) {
            const auto stringSize = toInt(bytes[i + 1]);
            queryData_.emplace_back(Data::DataType::String, bytes.sliced(i + 2, stringSize));
            i += stringSize + 1;
        }
        else if (id == QueryId::Int) {
            auto result = 0;

            for (auto j = 0; j < 4; ++j) {
                const auto byte = toInt(bytes[i + j + 1]) << (8 * j);
                result += byte;
            }

            queryData_.emplace_back(Data::DataType::Int, result);
            i += 4;
        }
        else if (id == QueryId::Short) {
            queryData_.emplace_back(Data::DataType::Short, toInt(bytes[i + 1]));
            ++i;
        }
        else {
            queryData_.emplace_back(Data::DataType::Id, toInt(bytes[i]));
        }
    }
}

QueryId Query::getType() const {
    return queryId_;
}

QByteArray Query::toBytes() const {
    QByteArray result;
    result.push_back(toChar(queryId_));

    for (const auto& data : queryData_) {
        if (data.dataType == Data::DataType::Id) {
            result.push_back(toChar(data.variant.toInt()));
        }
        else if (data.dataType == Data::DataType::Int) {
            auto number = data.variant.toInt();
            result.push_back(toChar(QueryId::Int));

            for (auto i = 0; i < 4; ++i) {
                result.push_back(toChar(number & 0xFF));
                number >>= 8;
            }
        }
        else if (data.dataType == Data::DataType::Short) {
            result.push_back(toChar(QueryId::Short));
            result.push_back(toChar(data.variant.toInt()));
        }
        else if (data.dataType == Data::DataType::String) {
            const auto& str = data.variant.toString();
            result.push_back(toChar(QueryId::String));
            result.push_back(toChar(str.size()));
            result.push_back(str.toUtf8());
        }
    }

    const auto querySize = result.size();
    result.push_front(toChar(querySize & 0xFF));
    result.push_front(toChar(querySize >> 8));
    return result;
}

void Query::pushString(const QString& data) {
    queryData_.emplace_back(Data::DataType::String, data);
}

void Query::pushInt(const int data) {
    queryData_.emplace_back(Data::DataType::Int, data);
}

void Query::pushId(const QueryId data) {
    queryData_.emplace_back(Data::DataType::Id, toInt(data));
}

void Query::pushShort(const int data) {
    queryData_.emplace_back(Data::DataType::Short, data);
}

Query::Query(const Query& other)
: queryId_(other.queryId_) {
    for (const auto& data : other.queryData_) {
        if (data.dataType == Data::DataType::String) {
            queryData_.emplace_back(data.dataType, data.variant.toString());
        }
        else {
            queryData_.emplace_back(data.dataType, data.variant.toInt());
        }
    }
}

int Query::getInt(const int index) const {
    return queryData_[index].variant.toInt();
}

QString Query::getString(const int index) const {
    return queryData_[index].variant.toString();
}

QueryId Query::getId(const int index) const {
    return toId(queryData_[index].variant.toInt());
}
