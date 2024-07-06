#include "server.h"

Server::Server()
: database_(QSqlDatabase::addDatabase("QSQLITE", "SQLITE")) {
    listen(QHostAddress::Any, 8080);
    connect(this, &QTcpServer::newConnection, this, &Server::MeetUser);
    database_.setDatabaseName("/C:/Users/kuryga/CLionProjects/checkers_server/db.sqlite");
    database_.open();
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
    auto requestData = Split(con->readAll().toStdString());

    if (requestData.front() == "login") {
        LoginUser(requestData, con);
    }
    else if (requestData.front() == "register") {
        RegisterUser(requestData, con);
    }
}

void Server::LoginUser(const std::vector<std::string>& requestData, QTcpSocket* con) {
    std::string message;
    const auto& login = requestData[1];
    const auto& incomingPassword = requestData[2];

    QSqlQuery query(database_);
    auto queryString = "SELECT nickname, password FROM users WHERE nickname = '" + login + "'";
    query.exec(queryString.c_str());

    if (query.next()) {
        auto truePassword = query.value(1).toString().toStdString();
        if (truePassword == incomingPassword) {
            message = "Success";
        }
        else {
            message = "Wrong Password";
        }
    }
    else {
        message = "Login doesn't exist";
    }

    con->write(message.c_str());
}

void Server::RegisterUser(const std::vector<std::string>& requestData, QTcpSocket* con) {
    std::string message;
    const auto& login = requestData[1];
    const auto& password = requestData[2];

    if (login.contains('$')) {
        message = "Login can't contain symbol $";
    }

    QSqlQuery query(database_);
    auto queryString = "SELECT nickname, password FROM users WHERE nickname = '" + login + "'";
    query.exec(queryString.c_str());

    if (!query.next()) {
        queryString = "INSERT INTO users (nickname, password) VALUES ('" + login + "', '" + password + "');";
        query.exec(queryString.c_str());
        message = "Success";
    }
    else {
        message = "Login already existed";
    }

    con->write(message.c_str());
}

void Server::DisconnectUser() {
    delete reinterpret_cast<QTcpSocket*>(sender());
}
