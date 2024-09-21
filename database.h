#pragma once

#include <QSqlDatabase>
#include <QSqlQuery>
#include <QReadWriteLock>

class Database {
public:
    Database();

    QSqlQuery getPlayers(const QString& nickname);
    void addPlayer(const QString& nickname, const QString& password);
    void changeNickname(const QString& oldNickname, const QString& newNickname);
    void changePassword(const QString& nickname, const QString& newPassword);
    void updateRating(const QString& nickname, int newRating);

private:
    QSqlQuery MakeQuery(const QString& queryString);

    QSqlDatabase database_;
    QReadWriteLock mutex_;
};
