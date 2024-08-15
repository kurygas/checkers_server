#include "query_handler.h"

QueryHandler::QueryHandler(const Query& query, QTcpSocket* con, Database& database, ConnectedUsers& connectedUsers)
: query_(query)
, con_(con)
, database_(database)
, connectedUsers_(connectedUsers) {}

const Caller* QueryHandler::GetCaller() const {
    return &caller_;
}

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
    else if (id == QueryId::Logout) {
        LogoutUser();
    }
}

void QueryHandler::LoginUser() {
    auto nickname = query_.GetString(0);
    Query response(QueryId::Login);
    auto users = database_.GetUsers(nickname);

    if (users.next()) {
        if (users.value(1).toString() == query_.GetString(1)) {
            response.PushId(QueryId::Ok);
            const auto rating = users.value(2).toUInt();
            response.PushLong(rating);
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
    Query response(QueryId::Register);

    if (!database_.GetUsers(nickname).next()) {
        database_.AddUser(nickname, query_.GetString(1));
        response.PushId(QueryId::Ok);
    }
    else {
        response.PushId(QueryId::AlreadyExist);
    }

    emit caller_.Processed(response, con_);
}

void QueryHandler::ChangeNickname() {
    const auto newNickname = query_.GetString(0);
    Query response(QueryId::ChangeNickname);

    if (!database_.GetUsers(newNickname).next()) {
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
    const auto nickname = connectedUsers_.GetPlayerInfo(con_)->GetNickname();
    const auto newPassword = query_.GetString(0);
    auto users = database_.GetUsers(nickname);
    Query response(QueryId::ChangePassword);

    if (users.next()) {
        if (users.value(1).toString() == newPassword) {
            response.PushId(QueryId::Same);
        }
        else {
            response.PushId(QueryId::Ok);
            database_.ChangePassword(nickname, newPassword);
        }
    }

    emit caller_.Processed(response, con_);
}

void QueryHandler::FindGame() {
    for (uint i = 1; i <= query_.GetInt(0); ++i) {
        auto *enemyCon = connectedUsers_.FindGame(con_, query_.GetInt(i));

        if (!enemyCon) {
            continue;
        }

        const auto player = connectedUsers_.GetPlayerInfo(con_);
        const auto enemy = connectedUsers_.GetPlayerInfo(enemyCon);

        if (player && enemy) {
            Query responseToEnemy(QueryId::StartGame);
            responseToEnemy.PushString(player->GetNickname());
            responseToEnemy.PushLong(player->GetRating());
            Query responseToPlayer(QueryId::StartGame);
            responseToPlayer.PushString(enemy->GetNickname());
            responseToPlayer.PushLong(enemy->GetRating());

            if (generator_.bounded(0, 2) == 0) {
                responseToEnemy.PushId(QueryId::White);
                responseToPlayer.PushId(QueryId::Black);
            }
            else {
                responseToEnemy.PushId(QueryId::Black);
                responseToPlayer.PushId(QueryId::White);
            }

            emit caller_.Processed(responseToEnemy, enemyCon);
            emit caller_.Processed(responseToPlayer, con_);
        }
    }
}

void QueryHandler::CancelSearching() {
    const auto player = connectedUsers_.GetPlayerInfo(con_);

    if (!player) {
        return;
    }

    connectedUsers_.StopSearching(player);

    if (!player->GetEnemy()) {
        Query response(QueryId::CancelSearching);
        emit caller_.Processed(response, con_);
    }
}

void QueryHandler::LogoutUser() {
    connectedUsers_.LogoutUser(con_);
}
