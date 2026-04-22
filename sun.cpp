#include "sun.h"
#include "mainwindow.h"
#include <QGraphicsScene>
#include <QPainter>
#include <QGraphicsSceneMouseEvent>

Sun::Sun(MainWindow *mainWin) : m_mainWindow(mainWin) {
    setZValue(20);

    // --- 【关键新增：加载图片】 ---
    // 请确保你的资源文件里有这个路径，并且图片文件确实叫 Sun.gif
    pixmap.load(":/images/Sun.gif");
}

QRectF Sun::boundingRect() const {
    // 调整碰撞箱大小以适应你的图片（比如 80x80）
    return QRectF(0, 0, 80, 80);
}

void Sun::paint(QPainter *painter, const QStyleOptionGraphicsItem *, QWidget *) {
    // --- 【关键修改：用贴图替代画圆】 ---
    // 删掉之前的 setBrush, setPen, drawEllipse...

    // 参数：(x, y, 宽度, 高度, 图片对象)
    // 这里的 80, 80 应该和你 boundingRect 保持一致
    painter->drawPixmap(0, 0, 80, 80, pixmap);
}

void Sun::mousePressEvent(QGraphicsSceneMouseEvent *event) {
    if (m_mainWindow) {
        m_mainWindow->addSunMoney(50);
    }

    if (scene()) {
        scene()->removeItem(this);
        deleteLater();
    }
}