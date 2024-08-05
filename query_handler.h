#pragma once
#include <QRunnable>
#include <QTcpSocket>
#include "query.h"
#include "database.h"
#include "connected_users.h"

struct Caller : public QObject {
    Q_OBJECT
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

    const Query query_;
    QTcpSocket* con_;
    Database& database_;
    ConnectedUsers& connectedUsers_;
    Caller caller_;
};
