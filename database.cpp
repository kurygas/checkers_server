#include "database.h"

Database::Database()
: database_(QSqlDatabase::addDatabase("QSQLITE", "SQLITE")) {
    database_.setDatabaseName("/C:/Users/kuryga/CLionProjects/checkers_server/db.sqlite");
    database_.open();
}

QSqlQuery Database::MakeQuery(const QString& queryString) {
    const bool notChange = queryString.split(' ').front() == "SELECT";

    if (notChange) {
        mutex_.lock_shared();
    }
    else {
        mutex_.lock();
    }

    QSqlQuery query(queryString, database_);

    if (notChange) {
        mutex_.unlock_shared();
    }
    else {
        mutex_.unlock();
    }

    return query;
}

QSqlQuery Database::GetUsers(const QString& login) {
    return MakeQuery("SELECT nickname, password, rating FROM users WHERE nickname = '" + login + "';");
}

void Database::AddUser(const QString &login, const QString& password) {
    MakeQuery("INSERT INTO users (nickname, password, rating) VALUES ('" + login + "', '" + password + "', 0);");
}

void Database::ChangeNickname(const QString& oldNickname, const QString& newNickname) {
    MakeQuery("UPDATE users SET nickname = '" + newNickname + "' WHERE nickname = '" + oldNickname + "';");
}

void Database::ChangePassword(const QString& nickname, const QString& newPassword) {
    MakeQuery("UPDATE users SET password = '" + newPassword + "' WHERE nickname = '" + nickname + "';");
}