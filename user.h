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
    void SetEnemy(QTcpSocket* enemy);

    [[nodiscard]] const QString& GetNickname() const;
    [[nodiscard]] uint GetRating() const;
    [[nodiscard]] const QList<uint>& GetRatingsForSearch() const;
    [[nodiscard]] QTcpSocket* GetEnemy() const;

private:
    QString nickname_;
    uint rating_;
    QList<uint> ratingsForSearch_;
    QTcpSocket* enemy_ = nullptr;
    mutable std::shared_mutex mutex_;
};