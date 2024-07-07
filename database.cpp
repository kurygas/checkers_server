#include "database.h"

Database::Database()
: database_(QSqlDatabase::addDatabase("QSQLITE", "SQLITE")) {
    database_.setDatabaseName("/C:/Users/kuryga/CLionProjects/checkers_server/db.sqlite");
    database_.open();
}

QSqlQuery Database::MakeQuery(const QString& queryString) {
    QSqlQuery query(queryString, database_);
    query.exec();
    return query;
}

QSqlQuery Database::GetUsers(const QString &login) {
    return MakeQuery("SELECT nickname, password FROM users WHERE nickname = '" + login + "'");
}

void Database::AddUser(const QString &login, const QString &password) {
    MakeQuery("INSERT INTO users (nickname, password) VALUES ('" + login + "', '" + password + "');");
}