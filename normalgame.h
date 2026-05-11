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
#include <QMediaPlayer>   // 新增：播放器头文件
#include <QAudioOutput>    // 新增：音频输出头文件

class NormalGame : public QWidget
{
    Q_OBJECT
public:
    explicit NormalGame(QWidget *parent = nullptr);
    void addSunMoney(int amount);
    void start();
    void stop();

protected:
    void mouseMoveEvent(QMouseEvent *event) override;
    void mousePressEvent(QMouseEvent *event) override;
    void keyPressEvent(QKeyEvent *event) override;

private:
    void handleGameOver();
    void handleGameWin();
    void resetGame();
    void spawnZombie(int row);
    void spawnConeZombie(int row);

    QTimer *mainTimer;
    QTimer *skySunTimer;

    // --- 背景音乐组件 ---
    QMediaPlayer *m_bgmPlayer;    // 新增：背景音乐对象
    QAudioOutput *m_audioOutput;  // 新增：音量控制对象

    // --- 游戏逻辑变量 ---
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