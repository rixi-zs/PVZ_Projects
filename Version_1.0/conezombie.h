#ifndef CONEZOMBIE_H
#define CONEZOMBIE_H

#include "zombie.h"

class ConeZombie : public Zombie
{
public:
    ConeZombie();
    // 只需要重写画图，走路逻辑直接用父类的
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget) override;

private:
    QPixmap conePixmap; // 存储带路障的图片
};

#endif