#include "server.h"

Server::Server() {
    listen(QHostAddress::Any, 8080);
    connect(this, &QTcpServer::newConnection, this, &Server::MeetUser);
}

void Server::Write(const Query& message, QTcpSocket* con) {
    try {
        con->write(message.ToBytes());
    }
    catch (...) {
        con->disconnectFromHost();
    }
}

QList<Query> Server::Read(QTcpSocket* con) {
    QList<Query> result;
    QByteArray data;

    try {
        data = con->readAll();
    }
    catch (...) {
        con->disconnectFromHost();
    }

    for (uint i = 0; i < data.size(); i += 2) {
        QByteArray buffer;
        const auto querySize = (Query::ToInt(data[i]) << 8) + Query::ToInt(data[i + 1]);

        for (uint j = 0; j < querySize; ++j) {
            buffer.push_back(data[i + j + 2]);
        }

        result.emplace_back(buffer);
        i += querySize;
    }

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
    const auto* con = reinterpret_cast<QTcpSocket*>(sender());
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
        auto* handler = new QueryHandler(query, con, database_, connectedUsers_);
        connect(handler->GetCaller(), &Caller::Processed, this, &Server::Write);
        QThreadPool::globalInstance()->start(handler);
    }
}
