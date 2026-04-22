#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QGraphicsScene>
#include <QGraphicsPixmapItem>
#include <QGraphicsTextItem>
#include <QMouseEvent>
#include <QKeyEvent>
#include <QMessageBox>

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

    void addSunMoney(int amount);

protected:
    void mouseMoveEvent(QMouseEvent *event) override;
    void mousePressEvent(QMouseEvent *event) override;
    void keyPressEvent(QKeyEvent *event) override;

private:
    // --- 【核心逻辑函数声明】 ---
    void handleGameOver(); // 失败处理
    void handleGameWin();  // 胜利处理
    void resetGame();      // 重置游戏

    // --- 【新增：生成僵尸的辅助函数】 ---
    void spawnZombie(int row);
    void spawnConeZombie(int row);

    // --- 【基础组件】 ---
    QGraphicsScene *scene;
    QGraphicsPixmapItem *movingPlant = nullptr;
    int currentType = 0;

    // --- 【数值与状态系统】 ---
    int sunMoney = 50;
    QGraphicsTextItem *moneyText;

    int mapData[12][6];

    bool isGameOver = false;   // 游戏是否失败
    bool isGameWon = false;    // 游戏是否胜利
    int zombiesToSpawn = 8;    // 剩余待生成的僵尸名额
};

#endif