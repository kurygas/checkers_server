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
    static void writeMessage(const Query& message, QTcpSocket* playerCon);
    static QList<Query> readMessage(QTcpSocket* playerCon);
    static void disconnectPlayer(QTcpSocket* playerCon);

    void meetPlayer();
    void catchDisconnection();
    void receiveRequest();

    void startTask(const Query& query, QTcpSocket* playerCon);

    Database database_;
    ConnectedPlayers connectedPlayers_;
};
