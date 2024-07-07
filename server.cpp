#include "server.h"

Server::Server() {
    listen(QHostAddress::Any, 8080);
    connect(this, &QTcpServer::newConnection, this, &Server::MeetUser);
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
    auto requestData = Split(con->readAll());

    if (requestData.front() == "login") {
        LoginUser(requestData, con);
    }
    else if (requestData.front() == "register") {
        RegisterUser(requestData, con);
    }
}

void Server::LoginUser(const QList<QString>& requestData, QTcpSocket* con) {
    QString message;
    const auto& login = requestData[1];
    const auto& incomingPassword = requestData[2];

    if (login.contains('$')) {
        message = "Login can't contain symbol $";
    }
    else {
        auto query = database_.GetUsers(login);

        if (query.next()) {
            auto truePassword = query.value(1).toString();
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
    }

    con->write(message.toUtf8());
}

void Server::RegisterUser(const QList<QString>& requestData, QTcpSocket* con) {
    QString message;
    const auto& login = requestData[1];
    const auto& password = requestData[2];

    if (login.contains('$')) {
        message = "Login can't contain symbol $";
    }
    else {
        auto query = database_.GetUsers(login);

        if (!query.next()) {
            database_.AddUser(login, password);
            message = "Success";
        }
        else {
            message = "Login already existed";
        }
    }

    con->write(message.toUtf8());
}

void Server::DisconnectUser() {
    delete reinterpret_cast<QTcpSocket*>(sender());
}
