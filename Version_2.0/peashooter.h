#ifndef PEASHOOTER_H
#define PEASHOOTER_H

#include <QGraphicsObject>
#include <QTimer>
#include <QPainter>

class Peashooter : public QGraphicsObject {
    Q_OBJECT
public:
    Peashooter();
    QRectF boundingRect() const override;
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget) override;

private slots:
    void shoot(); // 发射子弹的动作

private:
    QPixmap pixmap;
    QTimer *m_timer; // 属于这棵豌豆的私人闹钟
};
#endif