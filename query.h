#pragma once
#include <QString>
#include <QList>
#include <variant>

enum class QueryId {
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
    StartGame
};

class Query {
public:
    explicit Query(QueryId queryId);
    explicit Query(const QByteArray& bytes);

    void PushData(const std::variant<QString, uint, QueryId>& data);

    template<typename T>
    [[nodiscard]] const T& GetData(size_t index) const;
    [[nodiscard]] QByteArray ToBytes() const;
    [[nodiscard]] size_t Size() const;
    [[nodiscard]] QueryId GetId() const;

    static uint8_t ToNum(char symbol);
    static QueryId ToType(char symbol);

    template<typename T>
    static char ToChar(T&& data);

private:
    QueryId queryId_;
    QList<std::variant<QString, uint, QueryId>> queryData_;
};
