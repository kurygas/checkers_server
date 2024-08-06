#pragma once
#include <QList>
#include <QVariant>
#include <QDebug>

enum class QueryId : uint8_t {
    String = 0,
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
    CancelSearching,
    Logout,
    Same
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
    QString GetString(qsizetype index) const;
    uint GetUInt(qsizetype index) const;
    QueryId GetId(qsizetype index) const;

    QByteArray ToBytes() const;
    QueryId Type() const;

    template<typename T>
    static QueryId ToId(T data) {
        return static_cast<QueryId>(static_cast<uint8_t>(data));
    }

    template<typename T>
    static uint ToInt(T data) {
        return static_cast<uint>(static_cast<uint8_t>(data));
    }

    template<typename T>
    static char ToChar(T data) {
        return static_cast<char>(static_cast<uint8_t>(data));
    }


private:
    QueryId queryId_;
    QList<Data> queryData_;
};