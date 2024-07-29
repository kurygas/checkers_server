#pragma once
#include <QString>
#include <QList>
#include <QTcpSocket>
#include <mutex>
#include <shared_mutex>

class User {
public:
    User(QString nickname, uint rating);

    void SetNickname(const QString& nickname);
    void SetRating(uint rating);
    void AddRatingForSearch(uint rating);
    void ClearRatings();
    void SetEnemy(QTcpSocket* enemy);

    const QString& GetNickname() const;
    uint GetRating() const;
    const QList<uint>& GetRatingsForSearch() const;
    QTcpSocket* GetEnemy() const;

private:
    QString nickname_;
    uint rating_;
    QList<uint> ratingsForSearch_;
    QTcpSocket* enemy_ = nullptr;
    mutable std::shared_mutex mutex_;
};