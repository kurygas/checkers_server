#pragma once
#include <QString>
#include <QList>
#include <QTcpSocket>
#include <QSharedData>
#include <QReadWriteLock>

class User {
public:
    void SetNickname(const QString& nickname);
    void UpdateRating(int diff);
    void AddRatingForSearch(uint rating);
    void ClearRatings();
    void SetEnemy(QTcpSocket* enemy);

    QString GetNickname() const;
    uint GetRating() const;
    QList<uint> GetRatingsForSearch() const;
    QTcpSocket* GetEnemy() const;

private:
    QString nickname_;
    uint rating_;
    QList<uint> ratingsForSearch_;
    QTcpSocket* enemy_ = nullptr;
    mutable QReadWriteLock mutex_;
};
