#pragma once

#include <QString>
#include <QList>
#include <QTcpSocket>
#include <QSharedData>
#include <QReadWriteLock>

class Player {
public:
    explicit Player(QTcpSocket* playerCon);

    void updateRating(int difference);
    void setRating(int rating);
    void addRatingForSearch(int rating);
    void clearRatingsForSearch();
    void setEnemy(QTcpSocket* enemy);
    QTcpSocket* getPlayerCon();

    const QList<QString>& getFriends() const;

    template<typename T>
    void setNickname(T&& nickname) {
        const QWriteLocker lock(&mutex_);
        nickname_ = std::forward<T>(nickname);
    }

    template<typename T>
    void setFriends(T&& friendList) {
        friendList_ = std::forward<T>(friendList);
    }

    template<typename T>
    void addFriend(T&& newFriend) {
        friendList_.emplace_back(std::forward<T>(newFriend));
    }

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
    QStringList friendList_;
    mutable QReadWriteLock mutex_;
};
