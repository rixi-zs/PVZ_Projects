#include "conezombie.h"

ConeZombie::ConeZombie()
{
    // 1. 设置血量：路障僵尸比普通僵尸耐打得多
    hp = 280;

    // 2. 加载路障僵尸的素材
    conePixmap.load(":/images/ConeZombieWalk.gif");
}

void ConeZombie::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
    // 画出带路障的样子
    painter->drawPixmap(0, 0, 120, 120, conePixmap);
}