#pragma once
#include <QSqlDatabase>
#include <QSqlQuery>
#include <QReadWriteLock>

class Database {
public:
    Database();
    QSqlQuery GetUsers(const QString& nickname);
    void AddUser(const QString& nickname, const QString& password);
    void ChangeNickname(const QString& oldNickname, const QString& newNickname);
    void ChangePassword(const QString& nickname, const QString& newPassword);
    void UpdateRating(const QString& nickname, uint newRating);

private:
    QSqlQuery MakeQuery(const QString& queryString);

    QSqlDatabase database_;
    QReadWriteLock mutex_;
};
