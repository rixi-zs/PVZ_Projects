#include "zombie.h"
#include <QGraphicsScene>
#include <QPainter>
#include <QStyleOptionGraphicsItem>

Zombie::Zombie()
{
    // 1. 初始位置
    setPos(900, 265);

    // 2. 加载图片
    pixmap.load(":/images/ZombieWalk1.gif");

    // 3. 初始属性
    hp = 100;
    speed = 1.0;
    isPlayer = false;
}

QRectF Zombie::boundingRect() const
{
    // 保持稍微扩大的边界，确保光环和红块显示完整
    return QRectF(-20, -20, 160, 160);
}

void Zombie::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
    // 获取当前的细节层次
    qreal lod = option->levelOfDetailFromTransform(painter->worldTransform());

    // --- 细节层次控制：主图画僵尸，小图画红块 ---
    if (lod < 0.2 && this->isPlayer) {
        painter->setBrush(Qt::red);
        painter->setPen(QPen(Qt::white, 20));
        painter->drawRect(0, 0, 120, 120);
        return;
    }

    // 正常绘制僵尸
    painter->drawPixmap(0, 0, 120, 120, pixmap);

    // 玩家特有的脚下光环
    if (this->isPlayer) {
        painter->setBrush(QColor(255, 255, 0, 80));
        painter->setPen(Qt::NoPen);
        painter->drawEllipse(10, 100, 100, 30);
    }
}

void Zombie::advance(int phase)
{
    if (!phase) return;

    // 只有非玩家控制的僵尸才自动向左走（经典模式）
    if (!isPlayer) {
        setPos(x() - speed, y());

        if (x() < -150) {
            if (scene()) {
                scene()->removeItem(this);
                delete this; // 修正点：改回 delete
            }
        }
    }
}

void Zombie::hit(int damage)
{
    hp -= damage;
    if (hp < 0) hp = 0;

    if (hp <= 0) {
        if (scene()) {
            scene()->removeItem(this);
            delete this; // 修正点：这里不再使用 deleteLater，直接删除对象
        }
    }
}