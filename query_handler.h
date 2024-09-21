#pragma once

#include <QRunnable>
#include <QRandomGenerator>

#include "query.h"
#include "database.h"
#include "connected_users.h"

class Caller : public QObject {
    Q_OBJECT

public:
    signals:
    void disconnect(QTcpSocket* con);
    void processed(Query response, QTcpSocket* con);
};

class QueryHandler : public QRunnable {
public:
    QueryHandler(const Query& query, QTcpSocket* playerCon, Database& database, ConnectedPlayers& connectedPlayers);

    void run() override;
    const Caller* getCaller() const;

private:
    void loginUser();
    void registerUser();
    void findGame();
    void changeNickname();
    void changePassword();
    void cancelSearching();
    void logoutUser();
    void sendMove();
    void sendMatchResult();
    void enemyDisconnected(QTcpSocket* playerCon);

    const Query query_;
    QTcpSocket* playerCon_;
    Database& database_;
    ConnectedPlayers& connectedPlayers_;
    Caller caller_;
    QRandomGenerator generator_;
};
