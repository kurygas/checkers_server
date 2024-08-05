#include "query_handler.h"

QueryHandler::QueryHandler(const Query& query, QTcpSocket* con, Database& database, ConnectedUsers& connectedUsers)
: query_(query)
, con_(con)
, database_(database)
, connectedUsers_(connectedUsers) {}

void QueryHandler::run() {
    const auto id = query_.Type();

    if (id == QueryId::Login) {
        LoginUser();
    }
    else if (id == QueryId::Register) {
        RegisterUser();
    }
    else if (id == QueryId::ChangeNickname) {
        ChangeNickname();
    }
    else if (id == QueryId::ChangePassword) {
        ChangePassword();
    }
    else if (id == QueryId::FindGame) {
        FindGame();
    }
    else if (id == QueryId::CancelSearching) {
        CancelSearching();
    }
}

void QueryHandler::LoginUser() {
    auto nickname = query_.GetString(0);
    auto incomingPassword = query_.GetString(1);
    Query response(QueryId::Login);
    auto users = database_.GetUsers(nickname);

    if (users.next()) {
        const auto truePassword = users.value(1).toString();

        if (truePassword == incomingPassword) {
            response.PushId(QueryId::Ok);
            const auto rating = users.value(2).toUInt();
            response.PushUInt(rating);
            connectedUsers_.LoginUser(con_, nickname, rating);
        }
        else {
            response.PushId(QueryId::WrongPassword);
        }
    }
    else {
        response.PushId(QueryId::NotExist);
    }

    emit caller_.Processed(response, con_);
}

void QueryHandler::RegisterUser() {
    const auto nickname = query_.GetString(0);
    const auto password = query_.GetString(1);
    Query response(QueryId::Register);
    auto users = database_.GetUsers(nickname);

    if (!users.next()) {
        database_.AddUser(nickname, password);
        response.PushId(QueryId::Ok);
    }
    else {
        response.PushId(QueryId::AlreadyExist);
    }

    emit caller_.Processed(response, con_);
}

void QueryHandler::ChangeNickname() {
    const auto newNickname = query_.GetString(0);
    auto users = database_.GetUsers(newNickname);
    Query response(QueryId::ChangeNickname);

    if (!users.next()) {
        response.PushId(QueryId::Ok);
        database_.ChangeNickname(connectedUsers_.GetPlayerInfo(con_)->GetNickname(), newNickname);
        connectedUsers_.ChangeNickname(con_, newNickname);
    }
    else {
        response.PushId(QueryId::AlreadyExist);
    }

    emit caller_.Processed(response, con_);
}

void QueryHandler::ChangePassword() {
    const auto newPassword = query_.GetString(0);
    database_.ChangePassword(connectedUsers_.GetPlayerInfo(con_)->GetNickname(), newPassword);
}

void QueryHandler::FindGame() {
    const auto desiredRating = query_.GetUInt(0);
    auto* enemyCon = connectedUsers_.FindGame(con_, desiredRating);
    const auto player = connectedUsers_.GetPlayerInfo(con_);
    const auto enemy = connectedUsers_.GetPlayerInfo(enemyCon);

    if (enemyCon) {
        Query responseToEnemy(QueryId::StartGame);
        responseToEnemy.PushString(player->GetNickname());
        responseToEnemy.PushUInt(player->GetRating());
        emit caller_.Processed(responseToEnemy, enemyCon);

        Query responseToPlayer(QueryId::StartGame);
        responseToPlayer.PushString(enemy->GetNickname());
        responseToPlayer.PushUInt(enemy->GetRating());
        emit caller_.Processed(responseToPlayer, con_);
    }
}

void QueryHandler::CancelSearching() {
    const auto player = connectedUsers_.GetPlayerInfo(con_);
    connectedUsers_.StopSearching(player);

    if (!player->GetEnemy()) {
        Query response(QueryId::CancelSearching);
        emit caller_.Processed(response, con_);
    }
}

const Caller* QueryHandler::GetCaller() const {
    return &caller_;
}
