#ifndef HUNGRYGAME_H
#define HUNGRYGAME_H

#include <QWidget>
#include <QGraphicsScene>
#include <QGraphicsView>
#include <QTimer>
#include <QMap>
#include <QProgressBar>
#include <QGraphicsRectItem>
#include "zombie.h"
#include <QtMath>

class HungryGame : public QWidget
{
    Q_OBJECT
public:
    explicit HungryGame(QWidget *parent = nullptr);

    // --- 【核心新增：手动控制接口】 ---
    void start(); // 启动游戏逻辑
    void stop();  // 停止游戏逻辑

protected:
    void keyPressEvent(QKeyEvent *event) override;
    void keyReleaseEvent(QKeyEvent *event) override;

private slots:
    void updateGame();

private:
    // --- 核心场景与视图 ---
    QGraphicsScene *scene;
    QGraphicsView *view;
    Zombie *player;

    // --- 小地图组件 ---
    QGraphicsView *miniView;

    // --- UI 组件 ---
    QProgressBar *healthBar;

    // --- 【修改：逻辑计时器声明为成员变量】 ---
    QTimer *gameTimer;      // 主循环计时器
    QTimer *brainSpawner;   // 脑子生成计时器
    QTimer *hungerTimer;    // 饥饿扣血计时器

    QMap<int, bool> keysPressed;
    QTimer *enemySpawner;
    void createTerrain();

    void processMovement();
};

#endif