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
    // 3. 每一帧的逻辑（移动）
    void advance(int phase) override;

    // 受伤函数
    void hit(int damage);

    // --- 【关键修改】：将 private 改为 protected ---
protected:
    int hp = 100;       // 血量：现在子类可以直接修改这个值了
    qreal speed = 1.0;  // 速度
    QPixmap pixmap;     // 图片：子类可以换成路障僵尸的图
};

#endif // ZOMBIE_H