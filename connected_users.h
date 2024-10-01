#pragma once

#include <QTcpSocket>
#include <QMutex>

#include "player.h"

class ConnectedPlayers {
public:
    QTcpSocket* disconnectPlayer(const QTcpSocket* playerCon);
    QTcpSocket* findGame(QTcpSocket* playerCon, int rating);
    void addConnection(QTcpSocket* playerCon);
    void stopSearching(const QSharedPointer<Player>& player);
    void logoutPlayer(QTcpSocket* playerCon);

    QTcpSocket* findConnection(const QString& nickname) const;
    QSharedPointer<Player> getPlayerInfo(const QTcpSocket* playerCon) const;

    template<typename T, typename S>
    void loginPlayer(QTcpSocket* playerCon, T&& nickname, int rating, S&& friendList) {
        const QReadLocker lock1(&playerMutex_);
        const QWriteLocker lock2(&nicknamesMutex_);

        if (players_.contains(playerCon)) {
            const auto& player = players_[playerCon];
            player->setNickname(std::forward<T>(nickname));
            player->setRating(rating);
            player->setFriends(std::forward<S>(friendList));
            nicknames_[player->getNickname()] = playerCon;
        }
    }

    template<typename T>
    void changeNickname(QTcpSocket* playerCon, T&& newNickname) {
        const QReadLocker lock1(&playerMutex_);
        const QWriteLocker lock2(&nicknamesMutex_);

        if (players_.contains(playerCon)) {
            const auto& player = players_[playerCon];
            nicknames_.remove(player->getNickname());
            nicknames_[newNickname] = playerCon;
            players_[playerCon]->setNickname(std::forward<T>(newNickname));
        }
    }

private:
    QMap<const QTcpSocket*, QSharedPointer<Player>> players_;
    mutable QReadWriteLock playerMutex_;

    QMap<const uint, QTcpSocket*> matchmakingPool_;
    QRecursiveMutex matchmakingMutex_;

    QMap<QString, QTcpSocket*> nicknames_;
    mutable QReadWriteLock nicknamesMutex_;
};
