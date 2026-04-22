#include "sunflower.h"
#include "sun.h"
#include "normalgame.h" // 核心改动：包含 normalgame.h 而不是 mainwindow.h
#include <QGraphicsScene>
#include <QGraphicsView>
#include <QPainter>

SunFlower::SunFlower() {
    pixmap.load(":/images/SunFlower.gif");
    m_timer = new QTimer(this);
    connect(m_timer, &QTimer::timeout, this, &SunFlower::produceSun);
    m_timer->start(10000);
}

QRectF SunFlower::boundingRect() const {
    return QRectF(0, 0, 70, 80);
}

void SunFlower::paint(QPainter *painter, const QStyleOptionGraphicsItem *, QWidget *) {
    painter->drawPixmap(0, 0, 70, 80, pixmap);
}

void SunFlower::produceSun() {
    if (scene()) {
        // 【关键改动】：
        // 现在的向日葵是放在 NormalGame 的场景里的。
        // 由于 NormalGame 本身就是 scene 的 parent，或者是包含场景的父窗口，
        // 我们可以直接通过 scene()->parent() 或者通过视图转换得到 NormalGame 的指针。

        // 最稳妥的方法：因为 NormalGame 创建了 scene，我们可以直接获取
        NormalGame *game = qobject_cast<NormalGame*>(scene()->parent());

        if (game) {
            // 创建阳光，把 game (NormalGame类型) 传进去
            Sun *s = new Sun(game);
            s->setZValue(10);
            s->setPos(x() + 10, y() - 20);
            scene()->addItem(s);
        }
    }
}