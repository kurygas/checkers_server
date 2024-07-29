#pragma once
#include <QString>
#include <QTcpSocket>
#include <shared_mutex>
#include <mutex>
#include "user.h"

class ConnectedUsers {
public:
    void LoginUser(const QTcpSocket* con, const QString& nickname, uint rating);
    void ChangeNickname(const QTcpSocket* con, const QString& newNickname);
    QTcpSocket* GetEnemy(const QTcpSocket* con);
    QTcpSocket* DisconnectUser(const QTcpSocket* con);
    std::shared_ptr<User> GetPlayerInfo(const QTcpSocket* con) const;
    QTcpSocket* FindGame(QTcpSocket* con, uint rating);

private:
    void StopSearching(const std::shared_ptr<User>& user);

    QMap<const QTcpSocket*, std::shared_ptr<User>> players_;
    mutable std::shared_mutex playersMutex_;

    QMap<const uint, QTcpSocket*> matchmakingPool_;
    mutable std::mutex matchmakingMutex_;
};