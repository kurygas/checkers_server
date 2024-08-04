#pragma once
#include <QTcpServer>
#include <QTcpSocket>
#include <QMap>
#include <QThreadPool>
#include "database.h"
#include "query.h"
#include "connected_users.h"
#include "query_handler.h"

class Server : public QTcpServer {
    Q_OBJECT
public:
    Server();

private:
    void Write(const Query& message, QTcpSocket* con);
    QList<Query> Read(QTcpSocket* con);
    void MeetUser();

    Database database_;
    ConnectedUsers connectedUsers_;

private slots:
    void DisconnectUser();
    void ReceiveRequest();
};
