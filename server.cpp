#include "server.h"

Server::Server() {
    listen(QHostAddress::Any, 8080);
    connect(this, &QTcpServer::newConnection, this, &Server::MeetUser);
}

void Server::Write(const Query& message, QTcpSocket* con) {
    con->write(message.ToBytes());
}

QList<Query> Server::Read(QTcpSocket* con) {
    QByteArray buffer;
    QList<Query> result;

    for (const auto& byte : con->readAll()) {
        if (Query::ToType(byte) == QueryId::Query) {
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
    }
}

void Server::ReceiveRequest() {
    auto* con = reinterpret_cast<QTcpSocket*>(sender());
    const auto queries = Read(con);

    for (const auto& query : queries) {
        if (query.GetId() == QueryId::Login) {
            threadPool_.PushTask([this, &query, con]{Server::LoginUser(query, con);});
        }
        else if (query.GetId() == QueryId::Register) {
            threadPool_.PushTask([this, &query, con]{Server::RegisterUser(query, con);});
        }
        else if (query.GetId() == QueryId::ChangeNickname) {
            threadPool_.PushTask([this, &query, con]{Server::ChangeNickname(query, con);});
        }
        else if (query.GetId() == QueryId::ChangePassword) {
            threadPool_.PushTask([this, &query, con]{Server::ChangePassword(query, con);});
        }
        else if (query.GetId() == QueryId::FindGame) {
            threadPool_.PushTask([this, &query, con]{Server::FindGame(query, con);});
        }
    }
}

void Server::LoginUser(const Query& query, QTcpSocket* con) {
    Query response(QueryId::Login);
    const auto& nickname = query.GetData<QString>(0);
    const auto& incomingPassword = query.GetData<QString>(1);
    auto users = database_.GetUsers(nickname);

    if (users.next()) {
        auto truePassword = users.value(1).toString();

        if (truePassword == incomingPassword) {
            response.PushData(QueryId::Ok);
            response.PushData(static_cast<uint16_t>(users.value(2).toUInt()));
        }
        else {
            response.PushData(QueryId::WrongPassword);
        }
    }
    else {
        response.PushData(QueryId::NotExist);
    }

    Write(response, con);
}

void Server::RegisterUser(const Query& query, QTcpSocket* con) {
    Query response(QueryId::Register);
    const auto& nickname = query.GetData<QString>(0);
    const auto& password = query.GetData<QString>(1);
    auto users = database_.GetUsers(nickname);

    if (!users.next()) {
        database_.AddUser(nickname, password);
        response.PushData(QueryId::Ok);
    }
    else {
        response.PushData(QueryId::AlreadyExist);
    }

    Write(response, con);
}

void Server::DisconnectUser() {
    auto con = reinterpret_cast<QTcpSocket*>(sender());

    if (connections_.contains(con)) {
        auto id = connections_[con];

        if (id.has_value()) {
            Query query(QueryId::EnemyDisconnected);
            query.push
        }
    }
}

void Server::ChangeNickname(const Query& query, QTcpSocket* con) {
    const auto& oldNickname = query.GetData<QString>(0);
    const auto& newNickname = query.GetData<QString>(1);
    auto users = database_.GetUsers(newNickname);
    Query response(QueryId::ChangeNickname);

    if (!users.next()) {
        response.PushData(QueryId::Ok);
        database_.ChangeNickname(oldNickname, newNickname);
    }
    else {
        response.PushData(QueryId::AlreadyExist);
    }

    Write(query, con);
}

void Server::ChangePassword(const Query &query, QTcpSocket *con) {
    const auto& nickname = query.GetData<QString>(0);
    const auto& newPassword = query.GetData<QString>(1);
    database_.ChangePassword(nickname, newPassword);
}

void Server::FindGame(const Query& query, QTcpSocket* con) {

}
