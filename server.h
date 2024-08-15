#pragma once
#include <QTcpServer>
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
    static void Write(const Query& message, QTcpSocket* con);
    static QList<Query> Read(QTcpSocket* con);

    void MeetUser();
    void DisconnectUser();
    void ReceiveRequest();

    Database database_;
    ConnectedUsers connectedUsers_;
};
