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
    void Processed(Query response, QTcpSocket* con);
};

class QueryHandler : public QRunnable {
public:
    QueryHandler(const Query& query, QTcpSocket* con, Database& database, ConnectedUsers& connectedUsers);
    void run() override;
    const Caller* GetCaller() const;

private:
    void LoginUser();
    void RegisterUser();
    void FindGame();
    void ChangeNickname();
    void ChangePassword();
    void CancelSearching();
    void LogoutUser();

    const Query query_;
    QTcpSocket* con_;
    Database& database_;
    ConnectedUsers& connectedUsers_;
    Caller caller_;
    QRandomGenerator generator_;
};
