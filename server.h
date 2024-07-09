#pragma once
#include <QTcpServer>
#include <QTcpSocket>
#include <QSet>
#include "utility_functions.h"
#include "database.h"

class Server : public QTcpServer {
public:
    Server();

private:
    static void Write(QTcpSocket* con, const QList<QString>& data);
    static QList<QList<QString>> Read(QTcpSocket* con);


    void MeetUser();
    void ReceiveRequest();
    void DisconnectUser();
    void LoginUser(const QList<QString>& request, QTcpSocket* con);
    void RegisterUser(const QList<QString>& request, QTcpSocket* con);

    Database database_;
};
