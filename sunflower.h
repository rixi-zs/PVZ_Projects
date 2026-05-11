// sunflower.h
#ifndef SUNFLOWER_H
#define SUNFLOWER_H

#include <QGraphicsItem>
#include <QTimer>
#include <QPixmap>

class NormalGame; // 前向声明

class SunFlower : public QObject, public QGraphicsItem {
    Q_OBJECT
    Q_INTERFACES(QGraphicsItem)
public:
    SunFlower();
    QRectF boundingRect() const override;
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget) override;

public slots:
    void produceSun();

private:
    QPixmap pixmap;
    QTimer *m_timer;
};

#endif