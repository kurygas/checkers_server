#include "database.h"

Database::Database()
: database_(QSqlDatabase::addDatabase("QSQLITE", "SQLITE")) {
    database_.setDatabaseName("/C:/players/kuryga/CLionProjects/checkers_server/db.sqlite");
    database_.open();
}

QSqlQuery Database::MakeQuery(const QString& queryString) {
    if (queryString.split(' ').front() == "SELECT") {
        mutex_.lockForRead();
    }
    else {
        mutex_.lockForWrite();
    }

    QSqlQuery query(queryString, database_);
    mutex_.unlock();
    return query;
}

QSqlQuery Database::getPlayers(const QString& nickname) {
    return MakeQuery("SELECT nickname, password, rating, friends FROM players WHERE nickname = '" + nickname + "';");
}

void Database::addPlayer(const QString &nickname, const QString& password) {
    MakeQuery("INSERT INTO players (nickname, password, rating, friends) "
              "VALUES ('" + nickname + "', '" + password + "', 0, '');");
}

void Database::changeNickname(const QString& oldNickname, const QString& newNickname) {
    MakeQuery("UPDATE players SET nickname = '" + newNickname + "' WHERE nickname = '" + oldNickname + "';");
}

void Database::changePassword(const QString& nickname, const QString& newPassword) {
    MakeQuery("UPDATE players SET password = '" + newPassword + "' WHERE nickname = '" + nickname + "';");
}

void Database::updateRating(const QString& nickname, int newRating) {
    MakeQuery("UPDATE players SET rating = " + QString::number(newRating) + " WHERE nickname = '" + nickname + "';");
}

void Database::updateFriends(const QString& nickname, const QList<QString>& friendList) {
    MakeQuery("UPDATE players SET friends = '" + friendList.join('$') + "' WHERE nickname = '" + nickname + "';");
}
