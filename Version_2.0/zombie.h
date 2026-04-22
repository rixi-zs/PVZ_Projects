#ifndef ZOMBIE_H
#define ZOMBIE_H

#include <QGraphicsItem>
#include <QPainter>
#include <QPixmap>

class Zombie : public QGraphicsItem
{
public:
    Zombie();

    // 1. 定义碰撞边界
    QRectF boundingRect() const override;
    // 2. 绘制僵尸
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget) override;
    // 3. 每一帧的逻辑
    void advance(int phase) override;

    // 受伤函数
    void hit(int damage);

    // --- 【新增：控制开关函数】 ---
    void setPlayerControl(bool manual) { isPlayer = manual; }
    int getHP() const { return hp; } // 方便以后 UI 显示血量
    void addHP(int amount) { hp += amount; if(hp > 100) hp = 100; } // 吃脑子回血用

protected:
    int hp = 100;
    qreal speed = 1.0;
    QPixmap pixmap;

    // --- 【新增属性】：记录是否为玩家控制 ---
    bool isPlayer = false;
};

#endif // ZOMBIE_H