#include "sunflower.h"
#include "sun.h"
#include "mainwindow.h" // 必须包含这个，才能识别 MainWindow 类型
#include <QGraphicsScene>
#include <QGraphicsView>

SunFlower::SunFlower() {
    // 1. 加载图片
    pixmap.load(":/images/SunFlower.gif");

    // 2. 初始化私人闹钟
    m_timer = new QTimer(this);

    // 3. 关联：闹钟响了，就执行产阳光函数
    connect(m_timer, &QTimer::timeout, this, &SunFlower::produceSun);

    // 4. 开启闹钟：每 10 秒产一个
    m_timer->start(10000);
}

QRectF SunFlower::boundingRect() const {
    return QRectF(0, 0, 70, 80);
}

void SunFlower::paint(QPainter *painter, const QStyleOptionGraphicsItem *, QWidget *) {
    // 画出向日葵图片
    painter->drawPixmap(0, 0, 70, 80, pixmap);
}

void SunFlower::produceSun() {
    // 只有当向日葵被放进场景且有视图时才产阳光
    if (scene() && !scene()->views().isEmpty()) {

        // 【关键改动】：通过场景找到视图，再通过视图找到主窗口
        // 这一步是为了把“大总管” MainWindow 找出来传给阳光
        MainWindow *mw = static_cast<MainWindow*>(scene()->views().at(0)->window());

        // 创建阳光时，把主窗口指针 mw 传进去
        Sun *s = new Sun(mw);

        s->setZValue(10);
        // 阳光出现在向日葵自己的坐标位置
        s->setPos(x() + 10, y() - 20);
        scene()->addItem(s);
    }
}