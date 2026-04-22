#ifndef SUNFLOWER_H
#define SUNFLOWER_H

#include <QGraphicsObject>
#include <QGraphicsPixmapItem>
#include <QTimer>
#include <QPainter>

class SunFlower : public QGraphicsObject
{
    Q_OBJECT
public:
    SunFlower();

    // 必须实现的两个函数
    QRectF boundingRect() const override;
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget) override;

private slots:
    void produceSun(); // 这个函数负责“吐”阳光

private:
    QPixmap pixmap;    // 存储向日葵的图片
    QTimer *m_timer;   // 属于这棵植物的私人闹钟
};

#endif