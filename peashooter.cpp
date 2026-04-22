#include "peashooter.h"
#include "bullet.h"
#include <QGraphicsScene>

Peashooter::Peashooter() {
    pixmap.load(":/images/Peashooter.gif");

    m_timer = new QTimer(this);
    connect(m_timer, &QTimer::timeout, this, &Peashooter::shoot);
    m_timer->start(2000); // 每 2 秒发射一颗子弹
}

QRectF Peashooter::boundingRect() const {
    return QRectF(0, 0, 70, 80);
}

void Peashooter::paint(QPainter *painter, const QStyleOptionGraphicsItem *, QWidget *) {
    painter->drawPixmap(0, 0, 70, 80, pixmap);
}

void Peashooter::shoot() {
    if (scene()) {
        Bullet *b = new Bullet();
        // 设置子弹的初始位置（豌豆嘴巴的位置）
        b->setPos(this->x() + 50, this->y() + 10);
        scene()->addItem(b);
    }
}