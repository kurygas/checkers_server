#include "connected_users.h"

void ConnectedUsers::LoginUser(const QTcpSocket* con, const QString& nickname, const uint rating) {
    const std::unique_lock<std::shared_mutex> lock(playersMutex_);
    players_[con] = std::make_shared<User>(nickname, rating);
}

void ConnectedUsers::StopSearching(const std::shared_ptr<User>& user) {
    for (const auto& rating : user->GetRatingsForSearch()) {
        if (matchmakingPool_.contains(rating)) {
            matchmakingPool_.remove(rating);
        }
    }
    user->ClearRatings();
}

QTcpSocket* ConnectedUsers::FindGame(QTcpSocket* con, const uint rating) {
    const std::shared_lock<std::shared_mutex> lock1(playersMutex_);
    const std::unique_lock<std::mutex> lock2(matchmakingMutex_);

    if (players_.contains(con)) {
        const auto& playerInfo = players_[con];
        playerInfo->AddRatingForSearch(rating);
        QTcpSocket *enemyCon = nullptr;

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
    const std::shared_lock<std::shared_mutex> lock(playersMutex_);

    if (players_.contains(con)) {
        players_[con]->SetNickname(newNickname);
    }
}

QTcpSocket* ConnectedUsers::GetEnemy(const QTcpSocket* con) {
    const std::shared_lock<std::shared_mutex> lock(playersMutex_);
    return players_.contains(con) ? players_[con]->GetEnemy() : nullptr;
}

QTcpSocket* ConnectedUsers::DisconnectUser(const QTcpSocket* con) {
    const std::unique_lock<std::shared_mutex> lock1(playersMutex_);
    const std::unique_lock<std::mutex> lock2(matchmakingMutex_);

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

std::shared_ptr<User> ConnectedUsers::GetPlayerInfo(const QTcpSocket* con) const {
    const std::shared_lock<std::shared_mutex> lock(playersMutex_);
    return players_.contains(con) ? players_[con] : nullptr;
}
