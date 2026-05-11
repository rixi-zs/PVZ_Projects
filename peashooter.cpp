#include "peashooter.h"
#include "bullet.h"
#include <QGraphicsScene>
#include "zombie.h" // 必须包含僵尸头文件，否则 dynamic_cast 无法识别
#include <QPainter>

Peashooter::Peashooter() {
    pixmap.load(":/images/Peashooter.gif");

    m_timer = new QTimer(this);
    connect(m_timer, &QTimer::timeout, this, &Peashooter::shoot);
    m_timer->start(2000); // 每 2 秒检查一次是否需要射击
}

QRectF Peashooter::boundingRect() const {
    return QRectF(0, 0, 70, 80);
}

void Peashooter::paint(QPainter *painter, const QStyleOptionGraphicsItem *, QWidget *) {
    painter->drawPixmap(0, 0, 70, 80, pixmap);
}

void Peashooter::shoot() {
    if (!scene()) return;

    // --- 【新增：空射检测逻辑】 ---
    bool hasZombie = false;

    // 获取场景中所有的物体
    QList<QGraphicsItem *> items = scene()->items();

    for (QGraphicsItem *item : items) {
        // 尝试看看这个物体是不是僵尸（包括普通僵尸和路障僵尸）
        Zombie *z = dynamic_cast<Zombie *>(item);

        if (z) {
            // 判断僵尸是否在这一行：
            // 1. qAbs(z->y() - this->y()) < 50 表示 Y 坐标接近，说明在同一行
            // 2. z->x() > this->x() 表示僵尸在植物右边
            if (qAbs(z->y() - this->y()) < 50 && z->x() > this->x()) {
                hasZombie = true;
                break; // 只要找到一个，就说明需要开火，跳出循环
            }
        }
    }

    // 如果这一行没僵尸，直接结束函数，不产生子弹
    if (!hasZombie) return;

    // --- 【发射子弹逻辑】 ---
    Bullet *b = new Bullet();
    // 设置子弹的初始位置（豌豆嘴巴的位置）
    b->setPos(this->x() + 50, this->y() + 10);
    scene()->addItem(b);
}