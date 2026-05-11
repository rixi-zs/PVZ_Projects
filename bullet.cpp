#include "bullet.h"
#include "zombie.h"    // 必须包含僵尸的头文件，否则 dynamic_cast 不认识它
#include <QGraphicsScene>
#include <QPainter>

Bullet::Bullet() {
    setZValue(15);
}

QRectF Bullet::boundingRect() const {
    return QRectF(0, 0, 20, 20);
}

void Bullet::paint(QPainter *painter, const QStyleOptionGraphicsItem *, QWidget *) {
    painter->setBrush(Qt::green);
    painter->setPen(QPen(Qt::darkGreen, 1));
    painter->drawEllipse(0, 0, 15, 15);
}

void Bullet::advance(int phase) {
    if (!phase) return;

    // 1. 每帧向右移动
    moveBy(7, 0);

    // --- 2. 【核心新增：碰撞检测】 ---
    // 检查子弹现在撞到了谁
    QList<QGraphicsItem *> items = collidingItems();
    for (QGraphicsItem *item : items) {
        // 尝试看看撞到的是不是僵尸
        Zombie *z = dynamic_cast<Zombie *>(item);
        if (z) {
            z->hit(20); // 调用僵尸的受伤函数，扣20血

            // 子弹击中目标后消失
            if (scene()) {
                scene()->removeItem(this);
                delete this; // 使用 delete 替代 deleteLater
            }
            return; // 撞到了就结束本次 advance，防止重复计算
        }
    }

    // 3. 飞出屏幕自毁
    if (this->x() > 900) {
        if (scene()) {
            scene()->removeItem(this);
            delete this; // 使用 delete 替代 deleteLater
        }
    }
}