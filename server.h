#pragma once
#include <QTcpServer>
#include <QTcpSocket>
#include <QMap>
#include <shared_mutex>
#include "database.h"
#include "query.h"
#include "thread_pool.h"
#include "connected_users.h"

class Server : public QTcpServer {
public:
    Server();

private:
    static void Write(const Query& message, QTcpSocket* con);
    static QList<Query> Read(QTcpSocket* con);

    void MeetUser();
    void ReceiveRequest();
    void DisconnectUser();
    void LoginUser(const Query& query, QTcpSocket* con);
    void RegisterUser(const Query& query, QTcpSocket* con);
    void FindGame(const Query& query, QTcpSocket* con);
    void ChangeNickname(const Query& query, QTcpSocket* con);
    void ChangePassword(const Query& query, QTcpSocket* con);

    ThreadPool threadPool_;
    Database database_;
    Users users_;
};
