#include "connected_users.h"

void ConnectedPlayers::loginPlayer(QTcpSocket* playerCon, const QString& nickname, int rating) {
    const QReadLocker lock1(&playerMutex_);
    const QWriteLocker lock2(&nicknamesMutex_);

    if (players_.contains(playerCon)) {
        const auto& player = players_[playerCon];
        player->setNickname(nickname);
        player->setRating(rating);
        nicknames_[player->getNickname()] = playerCon;
    }
}

void ConnectedPlayers::stopSearching(const QSharedPointer<Player>& player) {
    const QMutexLocker<QRecursiveMutex> lock(&matchmakingMutex_);

    for (const auto& rating : player->getRatingsForSearch()) {
        if (matchmakingPool_.contains(rating)) {
            matchmakingPool_.remove(rating);
        }
    }

    player->clearRatingsForSearch();
}

QTcpSocket* ConnectedPlayers::findGame(QTcpSocket* playerCon, int rating) {
    const QReadLocker lock1(&playerMutex_);
    const QMutexLocker<QRecursiveMutex> lock2(&matchmakingMutex_);

    if (players_.contains(playerCon)) {
        const auto& player = players_[playerCon];
        player->addRatingForSearch(rating);
        QTcpSocket* enemyCon = nullptr;

        if (matchmakingPool_.contains(rating)) {
            enemyCon = matchmakingPool_[rating];
        }

        if (enemyCon != nullptr) {
            const auto& enemy = players_[enemyCon];

            if (enemy->getEnemyCon() == nullptr) {
                stopSearching(enemy);
                stopSearching(player);
                return enemyCon;
            }
        }

        matchmakingPool_[rating] = playerCon;
    }

    return nullptr;
}

void ConnectedPlayers::changeNickname(QTcpSocket* playerCon, const QString& newNickname) {
    const QReadLocker lock1(&playerMutex_);
    const QWriteLocker lock2(&nicknamesMutex_);

    if (players_.contains(playerCon)) {
        const auto& player = players_[playerCon];
        nicknames_.remove(player->getNickname());
        nicknames_[newNickname] = playerCon;
        players_[playerCon]->setNickname(newNickname);
    }
}

QTcpSocket* ConnectedPlayers::disconnectPlayer(const QTcpSocket* playerCon) {
    const QWriteLocker lock1(&playerMutex_);
    const QMutexLocker<QRecursiveMutex> lock2(&matchmakingMutex_);
    const QWriteLocker lock3(&nicknamesMutex_);

    if (players_.contains(playerCon)) {
        const auto player = players_[playerCon];
        stopSearching(player);
        auto* enemyCon = player->getEnemyCon();
        players_.remove(playerCon);
        nicknames_.remove(player->getNickname());

        if (enemyCon && players_.contains(enemyCon)) {
            const auto& enemy = players_[enemyCon];

            if (enemy->getEnemyCon() == playerCon) {
                enemy->setEnemy(nullptr);
                return enemyCon;
            }
        }
    }

    return nullptr;
}

QSharedPointer<Player> ConnectedPlayers::getPlayerInfo(const QTcpSocket* playerCon) const {
    const QReadLocker lock(&playerMutex_);
    return players_.contains(playerCon) ? players_[playerCon] : nullptr;
}

void ConnectedPlayers::addConnection(QTcpSocket* playerCon) {
    const QWriteLocker lock(&playerMutex_);
    players_[playerCon] = QSharedPointer<Player>::create(playerCon);
}

void ConnectedPlayers::logoutPlayer(QTcpSocket* playerCon) {
    const QWriteLocker lock(&playerMutex_);

    if (players_.contains(playerCon)) {
        const auto& player = players_[playerCon];
        nicknames_.remove(player->getNickname());
        players_[playerCon] = QSharedPointer<Player>::create(playerCon);
    }
}

QTcpSocket* ConnectedPlayers::findConnection(const QString& nickname) const {
    const QReadLocker lock1(&playerMutex_);
    const QReadLocker lock2(&nicknamesMutex_);
    return nicknames_.contains(nickname) ? nicknames_[nickname] : nullptr;
}
