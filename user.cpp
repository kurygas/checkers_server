#include "user.h"

QString Player::getNickname() const {
    const QReadLocker lock(&mutex_);
    return nickname_;
}

void Player::setNickname(const QString& nickname) {
    const QWriteLocker lock(&mutex_);
    nickname_ = nickname;
}

int Player::getRating() const {
    const QReadLocker lock(&mutex_);
    return rating_;
}

void Player::updateRating(const int difference) {
    const QWriteLocker lock(&mutex_);

    if (difference < 0 && std::abs(difference) > rating_) {
        return;
    }

    rating_ += difference;
}

QList<int> Player::getRatingsForSearch() const {
    const QReadLocker lock(&mutex_);
    return ratingsForSearch_;
}

void Player::addRatingForSearch(int rating) {
    const QWriteLocker lock(&mutex_);
    ratingsForSearch_.push_back(rating);
}

void Player::setEnemy(QTcpSocket* enemy) {
    const QWriteLocker lock(&mutex_);
    enemy_ = enemy;
}

QTcpSocket* Player::getEnemyCon() const {
    const QReadLocker lock(&mutex_);
    return enemy_;
}

void Player::clearRatingsForSearch() {
    const QWriteLocker lock(&mutex_);
    ratingsForSearch_.clear();
}

void Player::setRating(int rating) {
    const QWriteLocker lock(&mutex_);
    rating_ = rating;
}

Player::Player(QTcpSocket* playerCon)
: con_(playerCon) {}

QTcpSocket* Player::getPlayerCon() {
    return con_;
}
