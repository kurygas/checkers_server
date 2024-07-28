#include "connected_users.h"

void ConnectedUsers::LoginUser(const QTcpSocket* con, const QString& nickname, const uint rating) {
    const std::unique_lock<std::shared_mutex> lock(playersMutex_);
    players_[con] = std::make_shared<User>(nickname, rating);
}

QTcpSocket* ConnectedUsers::FindGame(QTcpSocket* con, const uint rating) {
    const std::shared_lock<std::shared_mutex> lock1(playersMutex_);
    const std::unique_lock<std::mutex> lock2(matchmakingMutex_);
    const auto playerInfo = players_[con];
    QTcpSocket* enemy = nullptr;

    if (matchmakingPool_.contains(rating)) {
        enemy = matchmakingPool_[rating];
    }

    if (enemy != nullptr)
}