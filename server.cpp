#include "server.h"

Server::Server() {
    listen(QHostAddress::Any, 8080);
    connect(this, &QTcpServer::newConnection, this, &Server::MeetUser);
}

void Server::MeetUser() {
    while (hasPendingConnections()) {
        auto con = nextPendingConnection();
        connect(con, &QTcpSocket::readyRead, this, &Server::ReceiveRequest);
        connect(con, &QTcpSocket::disconnected, this, &Server::DisconnectUser);
    }
}

void Server::ReceiveRequest() {
    auto con = reinterpret_cast<QTcpSocket*>(sender());
    auto requests = Read(con);

    for (const auto& request : requests) {
        if (request.front() == "login") {
            LoginUser(request, con);
        }
        else if (request.front() == "reg") {
            RegisterUser(request, con);
        }
    }
}

void Server::LoginUser(const QList<QString>& request, QTcpSocket* con) {
    QList<QString> response(1, "login");
    const auto& login = request[1];
    const auto& incomingPassword = request[2];
    auto query = database_.GetUsers(login);

    if (query.next()) {
        auto truePassword = query.value(1).toString();

        if (truePassword == incomingPassword) {
            response.push_back("ok");
            response.push_back(query.value(2).toString());
        }
        else {
            response.push_back("wrong password");
        }
    }
    else {
        response.push_back("not exist");
    }

    Write(con, response);
}

void Server::RegisterUser(const QList<QString>& request, QTcpSocket* con) {
    QList<QString> response(1, "reg");
    const auto& login = request[1];
    const auto& password = request[2];
    auto query = database_.GetUsers(login);

    if (!query.next()) {
        database_.AddUser(login, password);
        response.push_back("ok");
    }
    else {
        response.push_back("already exist");
    }

    Write(con, response);
}

void Server::DisconnectUser() {
    delete reinterpret_cast<QTcpSocket*>(sender());
}

void Server::Write(QTcpSocket* con, const QList<QString>& data) {
    con->write(Combine(data).toUtf8());
}

QList<QList<QString>> Server::Read(QTcpSocket *con) {
    return Parse(con->readAll());
}
