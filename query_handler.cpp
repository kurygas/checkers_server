#include "query_handler.h"

QueryHandler::QueryHandler(const Query& query, QTcpSocket* playerCon, Database& database, ConnectedPlayers& connectedPlayers)
: query_(query)
, playerCon_(playerCon)
, database_(database)
, connectedPlayers_(connectedPlayers) {}

const Caller* QueryHandler::getCaller() const {
    return &caller_;
}

void QueryHandler::run() {
    const auto id = query_.getType();

    if (id == QueryId::Login) {
        loginUser();
    }
    else if (id == QueryId::Register) {
        registerUser();
    }
    else if (id == QueryId::ChangeNickname) {
        changeNickname();
    }
    else if (id == QueryId::ChangePassword) {
        changePassword();
    }
    else if (id == QueryId::FindGame) {
        findGame();
    }
    else if (id == QueryId::CancelSearching) {
        cancelSearching();
    }
    else if (id == QueryId::Logout) {
        logoutUser();
    }
    else if (id == QueryId::Move) {
        sendMove();
    }
    else if (id == QueryId::Win) {
        sendMatchResult();
    }
    else if (id == QueryId::EnemyDisconnected) {
        enemyDisconnected(playerCon_);
    }
}

void QueryHandler::loginUser() {
    auto nickname = query_.getString(0);
    Query response(QueryId::Login);
    auto users = database_.getPlayers(nickname);

    if (users.next()) {
        auto* sameNicknameCon = connectedPlayers_.findConnection(nickname);

        if (sameNicknameCon) {
            emit caller_.disconnect(sameNicknameCon);
        }

        if (users.value(1).toString() == query_.getString(1)) {
            response.pushId(QueryId::Ok);
            const auto rating = users.value(2).toInt();
            response.pushInt(rating);
            auto friendList = users.value(3).toString().split('$');
            response.pushShort(static_cast<int>(friendList.size()));

            for (const auto& friendNickname : friendList) {
                response.pushString(friendNickname);
            }

            connectedPlayers_.loginPlayer(playerCon_, std::move(nickname), rating, std::move(friendList));
        }
        else {
            response.pushId(QueryId::WrongPassword);
        }
    }
    else {
        response.pushId(QueryId::NotExist);
    }

    emit caller_.processed(response, playerCon_);
}

void QueryHandler::registerUser() {
    const auto nickname = query_.getString(0);
    Query response(QueryId::Register);

    if (!database_.getPlayers(nickname).next()) {
        database_.addPlayer(nickname, query_.getString(1));
        response.pushId(QueryId::Ok);
    }
    else {
        response.pushId(QueryId::AlreadyExist);
    }

    emit caller_.processed(response, playerCon_);
}

void QueryHandler::changeNickname() {
    auto newNickname = query_.getString(0);
    Query response(QueryId::ChangeNickname);

    if (!database_.getPlayers(newNickname).next()) {
        response.pushId(QueryId::Ok);
        database_.changeNickname(connectedPlayers_.getPlayerInfo(playerCon_)->getNickname(), newNickname);
        connectedPlayers_.changeNickname(playerCon_, std::move(newNickname));
    }
    else {
        response.pushId(QueryId::AlreadyExist);
    }

    emit caller_.processed(response, playerCon_);
}

void QueryHandler::changePassword() {
    const auto nickname = connectedPlayers_.getPlayerInfo(playerCon_)->getNickname();
    const auto newPassword = query_.getString(0);
    auto users = database_.getPlayers(nickname);
    Query response(QueryId::ChangePassword);

    if (users.next()) {
        if (users.value(1).toString() == newPassword) {
            response.pushId(QueryId::Same);
        }
        else {
            response.pushId(QueryId::Ok);
            database_.changePassword(nickname, newPassword);
        }
    }

    emit caller_.processed(response, playerCon_);
}

void QueryHandler::findGame() {
    for (auto i = 1; i <= query_.getInt(0); ++i) {
        auto* enemyCon = connectedPlayers_.findGame(playerCon_, query_.getInt(i));

        if (!enemyCon) {
            continue;
        }

        const auto player = connectedPlayers_.getPlayerInfo(playerCon_);
        const auto enemy = connectedPlayers_.getPlayerInfo(enemyCon);

        if (player && enemy) {
            player->setEnemy(enemyCon);
            enemy->setEnemy(playerCon_);

            Query responseToEnemy(QueryId::StartGame);
            responseToEnemy.pushString(enemy->getNickname());
            responseToEnemy.pushInt(enemy->getRating());
            Query responseToPlayer(QueryId::StartGame);
            responseToPlayer.pushString(player->getNickname());
            responseToPlayer.pushInt(player->getRating());

            if (generator_.bounded(0, 2) == 0) {
                responseToEnemy.pushId(QueryId::White);
                responseToPlayer.pushId(QueryId::Black);
            }
            else {
                responseToEnemy.pushId(QueryId::Black);
                responseToPlayer.pushId(QueryId::White);
            }

            emit caller_.processed(responseToEnemy, enemyCon);
            emit caller_.processed(responseToPlayer, playerCon_);
        }
    }
}

void QueryHandler::cancelSearching() {
    const auto player = connectedPlayers_.getPlayerInfo(playerCon_);

    if (!player) {
        return;
    }

    connectedPlayers_.stopSearching(player);

    if (!player->getEnemyCon()) {
        Query response(QueryId::CancelSearching);
        emit caller_.processed(response, playerCon_);
    }
}

void QueryHandler::logoutUser() {
    connectedPlayers_.logoutPlayer(playerCon_);
}

void QueryHandler::sendMove() {
    emit caller_.processed(query_, connectedPlayers_.getPlayerInfo(playerCon_)->getEnemyCon());
}

void QueryHandler::sendMatchResult() {
    auto player = connectedPlayers_.getPlayerInfo(playerCon_);
    player->updateRating(50);
    database_.updateRating(player->getNickname(), player->getRating());
    auto enemy = connectedPlayers_.getPlayerInfo(player->getEnemyCon());
    enemy->updateRating(-50);
    database_.updateRating(enemy->getNickname(), enemy->getRating());
    emit caller_.processed(Query(QueryId::Lose), player->getEnemyCon());
}

void QueryHandler::enemyDisconnected(QTcpSocket* playerCon) {
    auto player = connectedPlayers_.getPlayerInfo(playerCon);
    auto* enemyCon = connectedPlayers_.disconnectPlayer(playerCon_);

    if (enemyCon) {
        player->updateRating(-50);
        database_.updateRating(player->getNickname(), player->getRating());
        auto enemy = connectedPlayers_.getPlayerInfo(enemyCon);
        enemy->updateRating(50);
        database_.updateRating(enemy->getNickname(), enemy->getRating());
        emit caller_.processed(Query(QueryId::EnemyDisconnected), enemyCon);
    }
}
