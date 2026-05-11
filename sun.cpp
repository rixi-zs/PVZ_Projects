#include "sun.h"
#include "normalgame.h" // 包含新的游戏类头文件
#include <QGraphicsScene>
#include <QPainter>
#include <QGraphicsSceneMouseEvent>

// 构造函数：接收 NormalGame 指针
Sun::Sun(NormalGame *game) : m_game(game) {
    setZValue(20);
    pixmap.load(":/images/Sun.gif");
}

QRectF Sun::boundingRect() const {
    return QRectF(0, 0, 80, 80);
}

void Sun::paint(QPainter *painter, const QStyleOptionGraphicsItem *, QWidget *) {
    painter->drawPixmap(0, 0, 80, 80, pixmap);
}

void Sun::mousePressEvent(QGraphicsSceneMouseEvent *event) {
    // 指向 NormalGame 里的 addSunMoney
    if (m_game) {
        m_game->addSunMoney(50);
    }

    if (scene()) {
        scene()->removeItem(this);
        delete this;
    }
}