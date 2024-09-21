#pragma once

#include <QList>
#include <QVariant>

enum class QueryId : uint8_t {
    String = 0,
    Short,
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
    Same,
    White,
    Black,
    Move,
    Win,
    Lose
};

struct Data {
    enum class DataType {
        String = 0,
        Short,
        Id,
        Int
    };

    DataType dataType;
    QVariant variant;
};

class Query {
public:
    explicit Query(QueryId queryId);
    explicit Query(const QByteArray& bytes);
    Query(const Query& other);

    void pushString(const QString& data);
    void pushInt(int data);
    void pushId(QueryId data);
    void pushShort(int data);

    QString getString(int index) const;
    int getInt(int index) const;
    QueryId getId(int index) const;
    QByteArray toBytes() const;
    QueryId getType() const;

    template<typename T>
    static QueryId toId(T data) {
        return static_cast<QueryId>(static_cast<uint8_t>(data));
    }

    template<typename T>
    static int toInt(T data) {
        return static_cast<int>(static_cast<uint8_t>(data));
    }

    template<typename T>
    static char toChar(T data) {
        return static_cast<char>(static_cast<uint8_t>(data));
    }

private:
    QueryId queryId_;
    QList<Data> queryData_;
};
