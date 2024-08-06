#pragma once
#include <QSqlDatabase>
#include <QSqlQuery>
#include <shared_mutex>
#include <mutex>

class Database {
public:
    Database();
    QSqlQuery GetUsers(const QString& nickname);
    void AddUser(const QString& nickname, const QString& password);
    void ChangeNickname(const QString& oldNickname, const QString& newNickname);
    void ChangePassword(const QString& nickname, const QString& newPassword);

private:
    QSqlQuery MakeQuery(const QString& queryString);

    QSqlDatabase database_;
    std::shared_mutex mutex_;
};