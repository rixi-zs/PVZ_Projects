#include "zombie.h"
#include <QGraphicsScene>

Zombie::Zombie()
{
    // 1. 设置初始位置
    setPos(900, 265);

    // 2. 预先加载图片，不要在 paint 里面 load，否则电脑会卡
    pixmap.load(":/images/ZombieWalk1.gif");

    // 3. 初始血量
    hp = 100;
    speed = 1.0;
}

QRectF Zombie::boundingRect() const
{
    // 碰撞箱建议比图片稍微窄一点，这样子弹撞击感更真实
    return QRectF(20, 0, 80, 120);
}

void Zombie::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
    // 直接画出预加载好的图片
    painter->drawPixmap(0, 0, 120, 120, pixmap);

    // --- 调试小技巧：如果你想看僵尸的血量，可以临时取消下面两行的注释 ---
    // painter->setPen(Qt::red);
    // painter->drawText(0, 0, QString::number(hp));
}

void Zombie::advance(int phase)
{
    if (!phase) return;

    // 每一帧向左移动
    setPos(x() - speed, y());

    // 如果僵尸走出屏幕左侧，也需要清理掉
    if (x() < -150) {
        if (scene()) {
            scene()->removeItem(this);
            delete this;
        }
    }
}

// --- 【核心新增：受伤逻辑】 ---
void Zombie::hit(int damage)
{
    hp -= damage; // 扣除血量

    // 如果血量打没了
    if (hp <= 0) {
        if (scene()) {
            // 僵尸阵亡，从场景移除并销毁
            scene()->removeItem(this);
            delete this;
        }
    }
}