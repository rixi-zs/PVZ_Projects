#ifndef SUN_H
#define SUN_H

#include <QGraphicsItem>
#include <QPixmap>

// 前向声明，告诉编译器 NormalGame 是个类
class NormalGame;

class Sun : public QGraphicsItem {
public:
    // 构造函数参数改为 NormalGame
    Sun(NormalGame *game);

    QRectF boundingRect() const override;
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget) override;

protected:
    void mousePressEvent(QGraphicsSceneMouseEvent *event) override;

private:
    NormalGame *m_game; // 这里的类型改了
    QPixmap pixmap;
};

#endif