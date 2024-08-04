#pragma once
#include <QList>
#include <QVariant>
#include <QDebug>

enum class QueryId : uint8_t {
    Query = 0,
    String,
    Int,
    Login,
    Register,
    ChangeNickname,
    ChangePassword,
    FindGame,
    WrongPassword,
    NotExist,
    AlreadyExist,
    Ok,
    EnemyDisconnected,
    StartGame,
    CancelSearching
};

struct Data {
    enum class DataType {
        String = 0,
        Uint,
        Id
    };

    DataType type;
    QVariant var;
};

class Query {
public:
    explicit Query(QueryId queryId);
    explicit Query(const QByteArray& bytes);
    Query(const Query& other);

    void PushString(const QString& data);
    void PushUInt(uint data);
    void PushId(QueryId data);
    QByteArray ToBytes() const;
    QueryId GetId() const;

    template<typename T>
    T GetData(qsizetype index) const {
        return queryData_[index].var.value<T>();
    }

    static QueryId ToId(char data);

    template<typename T>
    static char ToChar(T data) {
        return static_cast<char>(static_cast<uint8_t>(data));
    }


private:
    QueryId queryId_;
    QList<Data> queryData_;
};
