#pragma once
#include <QSqlDatabase>
#include <QSqlQuery>

class Database {
public:
    Database();
    QSqlQuery GetUsers(const QString& login);
    void AddUser(const QString& login, const QString& password);

private:
    QSqlQuery MakeQuery(const QString& queryString);

    QSqlDatabase database_;
};