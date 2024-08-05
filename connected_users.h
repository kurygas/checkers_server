#pragma once
#include <QString>
#include <QTcpSocket>
#include <QReadWriteLock>
#include <QMutex>
#include "user.h"

class ConnectedUsers {
public:
    void LoginUser(const QTcpSocket* con, const QString& nickname, uint rating);
    void ChangeNickname(const QTcpSocket* con, const QString& newNickname);
    QTcpSocket* GetEnemy(const QTcpSocket* con);
    QTcpSocket* DisconnectUser(const QTcpSocket* con);
    QSharedPointer<User> GetPlayerInfo(const QTcpSocket* con) const;
    QTcpSocket* FindGame(QTcpSocket* con, uint rating);
    void AddConnection(const QTcpSocket* con);
    void StopSearching(const QSharedPointer<User> &user);

private:
    QMap<const QTcpSocket*, QSharedPointer<User>> players_;
    mutable QReadWriteLock playerMutex_;

    QMap<const uint, QTcpSocket*> matchmakingPool_;
    QRecursiveMutex matchmakingMutex_;
};
