#include "server.h"

Server::Server()
: QTcpServer()
, db_(QSqlDatabase::addDatabase("QSQLITE", "SQLITE")) {
    listen(QHostAddress::Any, 8080);
    connect(this, &QTcpServer::newConnection, this, &Server::MeetUser);
    db_.setDatabaseName("/C:/Users/kuryga/CLionProjects/checkers_server/db.sqlite");
    db_.open();
}

void Server::MeetUser() {
    while (hasPendingConnections()) {
        auto con = nextPendingConnection();
        connect(con, &QTcpSocket::readyRead, this, &Server::Request);
        connect(con, &QTcpSocket::disconnected, this, &Server::DisconnectUser);
    }
}

void Server::Request() {
    auto con = reinterpret_cast<QTcpSocket*>(sender());
    auto requestData = Split(con->readAll().toStdString(), '$');
    if (requestData.front() == "login") {
        LoginUser(requestData, con);
    }
    else if (requestData.front() == "register") {
        RegisterUser(requestData, con);
    }
}

void Server::LoginUser(const std::vector<std::string>& requestData, QTcpSocket* con) {
    const auto& login = requestData[1];
    const auto& incomingPassword = requestData[2];
    QSqlQuery query(db_);
    auto queryString = "SELECT nickname, password FROM users WHERE nickname = '" + login + "'";
    query.exec(queryString.c_str());
    std::string message = "Login doesn't existed";

    if (query.next()) {
        auto truePassword = query.value(1).toString().toStdString();
        if (truePassword == incomingPassword) {
            message = "Success";
        }
        else {
            message = "Wrong Password";
        }
    }

    con->write(message.c_str());
}

void Server::RegisterUser(const std::vector<std::string>& requestData, QTcpSocket* con) {
    const auto& login = requestData[1];
    const auto& password = requestData[2];
    QSqlQuery query(db_);
    auto queryString = "SELECT nickname, password FROM users WHERE nickname = '" + login + "'";
    query.exec(queryString.c_str());
    std::string message = "Login already existed";

    if (!query.next()) {
        queryString = "INSERT INTO users (nickname, password) VALUES ('" + login + "', '" + password + "');";
        query.exec(queryString.c_str());
        message = "Success";
    }

    con->write(message.c_str());
}

void Server::DisconnectUser() {
    reinterpret_cast<QTcpSocket*>(sender())->deleteLater();
}
