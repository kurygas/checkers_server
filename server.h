#pragma once
#include <QTcpServer>
#include <QTcpSocket>
#include "utility_functions.h"
#include "database.h"

class Server : public QTcpServer {
public:
    Server();

private:
    void MeetUser();
    void Request();
    void DisconnectUser();
    void LoginUser(const QList<QString>& requestData, QTcpSocket* con);
    void RegisterUser(const QList<QString>& requestData, QTcpSocket* con);

    Database database_;
};
