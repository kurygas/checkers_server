#pragma once
#include <QTcpServer>
#include <QTcpSocket>
#include <QSqlDatabase>
#include <QSqlQuery>
#include <QDebug>
#include <QByteArray>
#include <QThreadPool>
#include <memory>
#include "utility_functions.h"

class Server : public QTcpServer {
public:
    Server();

private:
    void MeetUser();
    void Request();
    void DisconnectUser();
    void LoginUser(std::vector<std::string>&& requestData, QTcpSocket* con);
    void RegisterUser(std::vector<std::string>&& requestData, QTcpSocket* con);

    QSqlDatabase db_;
};
