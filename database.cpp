#include "database.h"

Database::Database()
: database_(QSqlDatabase::addDatabase("QSQLITE", "SQLITE")) {
    database_.setDatabaseName("/C:/Users/kuryga/CLionProjects/checkers_server/db.sqlite");
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
    return MakeQuery("SELECT nickname, password, rating FROM users WHERE nickname = '" + nickname + "';");
}

void Database::addPlayer(const QString &nickname, const QString& password) {
    MakeQuery("INSERT INTO users (nickname, password, rating) VALUES ('" + nickname + "', '" + password + "', 0);");
}

void Database::changeNickname(const QString& oldNickname, const QString& newNickname) {
    MakeQuery("UPDATE users SET nickname = '" + newNickname + "' WHERE nickname = '" + oldNickname + "';");
}

void Database::changePassword(const QString& nickname, const QString& newPassword) {
    MakeQuery("UPDATE users SET password = '" + newPassword + "' WHERE nickname = '" + nickname + "';");
}

void Database::updateRating(const QString& nickname, int newRating) {
    MakeQuery("UPDATE users SET rating = " + QString::number(newRating) + " WHERE nickname = '" + nickname + "';");
}
