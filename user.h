#pragma once

#include <QString>
#include <QList>
#include <QTcpSocket>
#include <QSharedData>
#include <QReadWriteLock>

class Player {
public:
    explicit Player(QTcpSocket* playerCon);

    void setNickname(const QString& nickname);
    void updateRating(int difference);
    void setRating(int rating);
    void addRatingForSearch(int rating);
    void clearRatingsForSearch();
    void setEnemy(QTcpSocket* enemy);
    QTcpSocket* getPlayerCon();

    QString getNickname() const;
    int getRating() const;
    QList<int> getRatingsForSearch() const;
    QTcpSocket* getEnemyCon() const;

private:
    QTcpSocket* con_;
    QString nickname_;
    int rating_ = 0;
    QList<int> ratingsForSearch_;
    QTcpSocket* enemy_ = nullptr;
    mutable QReadWriteLock mutex_;
};
