#include "server.h"

Server::Server() {
    listen(QHostAddress::AnyIPv4, 1234);
    connect(this, &QTcpServer::newConnection, this, &Server::meetPlayer);
}

void Server::writeMessage(const Query& message, QTcpSocket* playerCon) {
    try {
        playerCon->write(message.toBytes());
    }
    catch (...) {
        playerCon->disconnectFromHost();
    }
}

QList<Query> Server::readMessage(QTcpSocket* playerCon) {
    QList<Query> result;
    QByteArray data;

    try {
        data = playerCon->readAll();
    }
    catch (...) {
        playerCon->disconnectFromHost();
    }

    for (auto i = 0; i < data.size(); i += 2) {
        QByteArray buffer;
        const auto querySize = (Query::toInt(data[i]) << 8) + Query::toInt(data[i + 1]);

        for (auto j = 0; j < querySize; ++j) {
            buffer.push_back(data[i + j + 2]);
        }

        result.emplace_back(buffer);
        i += querySize;
    }

    return result;
}

void Server::catchDisconnection() {
    startTask(Query(QueryId::EnemyDisconnected), reinterpret_cast<QTcpSocket*>(sender()));
}

void Server::meetPlayer() {
    while (hasPendingConnections()) {
        auto* con = nextPendingConnection();
        connect(con, &QTcpSocket::readyRead, this, &Server::receiveRequest);
        connect(con, &QTcpSocket::disconnected, this, &Server::catchDisconnection);
        connectedPlayers_.addConnection(con);
    }
}

void Server::receiveRequest() {
    auto* con = reinterpret_cast<QTcpSocket*>(sender());
    auto queries = readMessage(con);

    for (auto& query : queries) {
        startTask(query, con);
    }
}

void Server::startTask(const Query& query, QTcpSocket* playerCon) {
    auto* handler = new QueryHandler(query, playerCon, database_, connectedPlayers_);
    connect(handler->getCaller(), &Caller::processed, this, &Server::writeMessage);
    connect(handler->getCaller(), &Caller::disconnect, this, &Server::disconnectPlayer);
    QThreadPool::globalInstance()->start(handler);
}

void Server::disconnectPlayer(QTcpSocket* playerCon) {
    playerCon->disconnectFromHost();
}
