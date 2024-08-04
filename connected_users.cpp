#include "connected_users.h"

void ConnectedUsers::LoginUser(const QTcpSocket* con, const QString& nickname, const uint rating) {
    const QReadLocker lock(&playerMutex_);

    if (players_.contains(con)) {
        const auto& player = players_[con];
        player->SetNickname(nickname);
        player->SetRating(rating);
    }
}

void ConnectedUsers::StopSearching(const QSharedPointer<User>& user) {
    const QMutexLocker<QRecursiveMutex> lock(&matchmakingMutex_);

    for (const auto& rating : user->GetRatingsForSearch()) {
        if (matchmakingPool_.contains(rating)) {
            matchmakingPool_.remove(rating);
        }
    }

    user->ClearRatings();
}

QTcpSocket* ConnectedUsers::FindGame(QTcpSocket* con, const uint rating) {
    const QReadLocker lock1(&playerMutex_);
    const QMutexLocker<QRecursiveMutex> lock2(&matchmakingMutex_);

    if (players_.contains(con)) {
        const auto& playerInfo = players_[con];
        playerInfo->AddRatingForSearch(rating);
        QTcpSocket* enemyCon = nullptr;

        if (matchmakingPool_.contains(rating)) {
            enemyCon = matchmakingPool_[rating];
        }

        if (enemyCon != nullptr) {
            auto enemy = players_[enemyCon];

            if (enemy->GetEnemy() == nullptr) {
                StopSearching(enemy);
                StopSearching(playerInfo);
                return enemyCon;
            }
        }

        matchmakingPool_[rating] = con;
    }

    return nullptr;
}

void ConnectedUsers::ChangeNickname(const QTcpSocket* con, const QString& newNickname) {
    const QReadLocker lock(&playerMutex_);

    if (players_.contains(con)) {
        players_[con]->SetNickname(newNickname);
    }
}

QTcpSocket* ConnectedUsers::GetEnemy(const QTcpSocket* con) {
    const QReadLocker lock(&playerMutex_);
    return players_.contains(con) ? players_[con]->GetEnemy() : nullptr;
}

QTcpSocket* ConnectedUsers::DisconnectUser(const QTcpSocket* con) {
    const QWriteLocker lock(&playerMutex_);
    const QMutexLocker<QRecursiveMutex> lock2(&matchmakingMutex_);

    if (players_.contains(con)) {
        const auto& player = players_[con];
        StopSearching(player);
        players_.remove(con);
        const auto& enemyCon = player->GetEnemy();
        const auto& enemy = players_[enemyCon];

        if (enemy->GetEnemy() == con) {
            enemy->SetEnemy(nullptr);
            return enemyCon;
        }
    }

    return nullptr;
}

QSharedPointer<User> ConnectedUsers::GetPlayerInfo(const QTcpSocket* con) const {
    const QReadLocker lock(&playerMutex_);
    return players_.contains(con) ? players_[con] : nullptr;
}

void ConnectedUsers::AddConnection(const QTcpSocket* con) {
    const QWriteLocker lock(&playerMutex_);
    players_[con] = QSharedPointer<User>::create();
}
