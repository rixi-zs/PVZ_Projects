#ifndef BULLET_H
#define BULLET_H

#include <QGraphicsObject>
#include <QPainter>

class Bullet : public QGraphicsObject {
    Q_OBJECT
public:
    Bullet();
    QRectF boundingRect() const override;
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget) override;

protected:
    // 关键：这个函数会被主计时器自动调用
    void advance(int phase) override;
};
#endif