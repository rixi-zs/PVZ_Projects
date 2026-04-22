#ifndef SUN_H
#define SUN_H

#include <QGraphicsObject>
#include <QPixmap> // 必须包含这个

class MainWindow;

class Sun : public QGraphicsObject
{
    Q_OBJECT
public:
    Sun(MainWindow *mainWin);

    QRectF boundingRect() const override;
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget) override;

protected:
    void mousePressEvent(QGraphicsSceneMouseEvent *event) override;

private:
    MainWindow *m_mainWindow;
    QPixmap pixmap; // --- 新增：存储阳光图片 ---
};

#endif