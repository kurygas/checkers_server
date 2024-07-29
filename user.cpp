#include "user.h"

User::User(QString nickname, const uint rating)
: nickname_(std::move(nickname))
, rating_(rating) {}

const QString& User::GetNickname() const {
    const std::shared_lock<std::shared_mutex> lock(mutex_);
    return nickname_;
}

void User::SetNickname(const QString& nickname) {
    const std::unique_lock<std::shared_mutex> lock(mutex_);
    nickname_ = nickname;
}

uint User::GetRating() const {
    const std::shared_lock<std::shared_mutex> lock(mutex_);
    return rating_;
}

void User::SetRating(const uint rating) {
    const std::unique_lock<std::shared_mutex> lock(mutex_);
    rating_ = rating;
}

const QList<uint>& User::GetRatingsForSearch() const {
    const std::shared_lock<std::shared_mutex> lock(mutex_);
    return ratingsForSearch_;
}

void User::AddRatingForSearch(const uint rating) {
    const std::unique_lock<std::shared_mutex> lock(mutex_);
    ratingsForSearch_.push_back(rating);
}

void User::SetEnemy(QTcpSocket* enemy) {
    const std::unique_lock<std::shared_mutex> lock(mutex_);
    enemy_ = enemy;
}

QTcpSocket* User::GetEnemy() const {
    const std::shared_lock<std::shared_mutex> lock(mutex_);
    return enemy_;
}

void User::ClearRatings() {
    ratingsForSearch_.clear();
}
