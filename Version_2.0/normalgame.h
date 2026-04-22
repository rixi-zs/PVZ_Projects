#ifndef NORMALGAME_H
#define NORMALGAME_H

#include <QWidget>
#include <QGraphicsScene>
#include <QGraphicsView>
#include <QGraphicsPixmapItem>
#include <QGraphicsTextItem>
#include <QMouseEvent>
#include <QKeyEvent>
#include <QTimer>

class NormalGame : public QWidget
{
    Q_OBJECT
public:
    explicit NormalGame(QWidget *parent = nullptr);
    void addSunMoney(int amount);
    void start();
    void stop();

protected:
    // 原有的鼠标和键盘逻辑搬到这里
    void mouseMoveEvent(QMouseEvent *event) override;
    void mousePressEvent(QMouseEvent *event) override;
    void keyPressEvent(QKeyEvent *event) override;

private:
    void handleGameOver();
    void handleGameWin();
    void resetGame();
    void spawnZombie(int row);
    void spawnConeZombie(int row);
    QTimer *mainTimer;    // 添加这一行
    QTimer *skySunTimer;  // 添加这一行

    // --- 之前 MainWindow 里的那些变量全部搬过来 ---
    QGraphicsScene *scene;
    QGraphicsView *view;
    QGraphicsPixmapItem *movingPlant = nullptr;
    int currentType = 0;
    int sunMoney = 50;
    QGraphicsTextItem *moneyText;
    int mapData[12][6];
    bool isGameOver = false;
    bool isGameWon = false;
    int zombiesToSpawn = 8;
};

#endif