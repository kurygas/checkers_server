#include "user.h"

QString User::GetNickname() const {
    const QReadLocker lock(&mutex_);
    return nickname_;
}

void User::SetNickname(const QString& nickname) {
    const QWriteLocker lock(&mutex_);
    nickname_ = nickname;
}

uint User::GetRating() const {
    const QReadLocker lock(&mutex_);
    return rating_;
}

void User::UpdateRating(const int diff) {
    const QWriteLocker lock(&mutex_);
    rating_ += diff;
}

QList<uint> User::GetRatingsForSearch() const {
    const QReadLocker lock(&mutex_);
    return ratingsForSearch_;
}

void User::AddRatingForSearch(const uint rating) {
    const QWriteLocker lock(&mutex_);
    ratingsForSearch_.push_back(rating);
}

void User::SetEnemy(QTcpSocket* enemy) {
    const QWriteLocker lock(&mutex_);
    enemy_ = enemy;
}

QTcpSocket* User::GetEnemy() const {
    const QReadLocker lock(&mutex_);
    return enemy_;
}

void User::ClearRatings() {
    const QWriteLocker lock(&mutex_);
    ratingsForSearch_.clear();
}
