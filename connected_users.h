#pragma once

#include <QTcpSocket>
#include <QMutex>

#include "user.h"

class ConnectedPlayers {
public:
    void loginPlayer(QTcpSocket* playerCon, const QString& nickname, int rating);
    void changeNickname(QTcpSocket* playerCon, const QString& newNickname);
    QTcpSocket* disconnectPlayer(const QTcpSocket* playerCon);
    QTcpSocket* findGame(QTcpSocket* playerCon, int rating);
    void addConnection(QTcpSocket* playerCon);
    void stopSearching(const QSharedPointer<Player>& player);
    void logoutPlayer(QTcpSocket* playerCon);

    QTcpSocket* findConnection(const QString& nickname) const;
    QSharedPointer<Player> getPlayerInfo(const QTcpSocket* playerCon) const;

private:
    QMap<const QTcpSocket*, QSharedPointer<Player>> players_;
    mutable QReadWriteLock playerMutex_;

    QMap<const uint, QTcpSocket*> matchmakingPool_;
    QRecursiveMutex matchmakingMutex_;

    QMap<QString, QTcpSocket*> nicknames_;
    mutable QReadWriteLock nicknamesMutex_;
};
