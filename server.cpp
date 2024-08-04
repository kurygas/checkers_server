#include "server.h"

Server::Server() {
    listen(QHostAddress::Any, 8080);
    connect(this, &QTcpServer::newConnection, this, &Server::MeetUser);
}

void Server::Write(const Query& message, QTcpSocket* con) {
    const auto& player = connectedUsers_.GetPlayerInfo(con);
    con->write(message.ToBytes());
}

QList<Query> Server::Read(QTcpSocket* con) {
    QByteArray buffer;
    QList<Query> result;
    const auto& player = connectedUsers_.GetPlayerInfo(con);
    const auto& data = con->readAll();


    for (const auto& byte : data) {
        if (Query::ToId(byte) == QueryId::Query) {
            if (!buffer.isEmpty()) {
                result.emplace_back(buffer);
                buffer.clear();
            }
        }
        else {
            buffer.push_back(byte);
        }
    }

    result.emplace_back(buffer);
    return result;
}

void Server::MeetUser() {
    while (hasPendingConnections()) {
        const auto* con = nextPendingConnection();
        connect(con, &QTcpSocket::readyRead, this, &Server::ReceiveRequest);
        connect(con, &QTcpSocket::disconnected, this, &Server::DisconnectUser);
        connectedUsers_.AddConnection(con);
    }
}

void Server::DisconnectUser() {
    auto con = reinterpret_cast<QTcpSocket*>(sender());
    auto* enemy = connectedUsers_.DisconnectUser(con);

    if (enemy) {
        const Query response(QueryId::EnemyDisconnected);
        Write(response, enemy);
    }
}

void Server::ReceiveRequest() {
    auto* con = reinterpret_cast<QTcpSocket*>(sender());
    auto queries = Read(con);

    for (auto& query : queries) {
        auto* handler = new QueryHandler(std::move(query), con, database_, connectedUsers_);
        connect(handler->GetCaller(), &Caller::Processed, this, &Server::Write);
        QThreadPool::globalInstance()->start(handler);
    }
}
