#include "query.h"

Query::Query(QueryId queryId)
: queryId_(queryId) {}

Query::Query(const QByteArray& bytes)
: queryId_(ToId(bytes.front())) {
    for (uint i = 1; i < bytes.size(); ++i) {
        const auto id = ToId(bytes[i]);

        if (id == QueryId::String) {
            const auto stringSize = ToInt(bytes[i + 1]);
            queryData_.emplace_back(Data::DataType::String, bytes.sliced(i + 2, stringSize));
            i += stringSize + 1;
        }
        else if (id == QueryId::Int) {
            uint result = 0;

            for (int j = 0; j < 4; ++j) {
                const auto byte = ToInt(bytes[i + j + 1]) << (8 * j);
                result += byte;
            }

            queryData_.emplace_back(Data::DataType::Uint, result);
        }
        else {
            queryData_.emplace_back(Data::DataType::Id, ToInt(bytes[i]));
        }
    }
}

QueryId Query::Type() const {
    return queryId_;
}

QByteArray Query::ToBytes() const {
    QByteArray result;
    result.push_back(ToChar(queryId_));

    for (const auto& data : queryData_) {
        if (data.type == Data::DataType::Id) {
            result.push_back(ToChar(data.var.toUInt()));
        }
        else if (data.type == Data::DataType::Uint) {
            auto number = data.var.toUInt();
            result.push_back(ToChar(QueryId::Int));

            for (int i = 0; i < 4; ++i) {
                result.push_back(ToChar(number & 0xFF));
                number >>= 8;
            }
        }
        else {
            const auto& str = data.var.toString();
            result.push_back(ToChar(QueryId::String));
            result.push_back(ToChar(str.size()));
            result.push_back(str.toUtf8());
        }
    }

    result.push_front(ToChar(result.size()));
    return result;
}

void Query::PushString(const QString& data) {
    queryData_.emplace_back(Data::DataType::String, data);
}

void Query::PushUInt(const uint data) {
    queryData_.emplace_back(Data::DataType::Uint, data);
}

void Query::PushId(const QueryId data) {
    queryData_.emplace_back(Data::DataType::Id, static_cast<uint>(static_cast<uint8_t>(data)));
}

Query::Query(const Query& other)
: queryId_(other.queryId_) {
    for (const auto& data : other.queryData_) {
        if (data.type == Data::DataType::String) {
            queryData_.emplace_back(data.type, data.var.toString());
        }
        else {
            queryData_.emplace_back(data.type, data.var.toUInt());
        }
    }
}

QString Query::GetString(qsizetype index) const {
    return queryData_[index].var.toString();
}

uint Query::GetUInt(qsizetype index) const {
    return queryData_[index].var.toUInt();
}

QueryId Query::GetId(qsizetype index) const {
    return static_cast<QueryId>(static_cast<uint8_t>(queryData_[index].var.toUInt()));
}