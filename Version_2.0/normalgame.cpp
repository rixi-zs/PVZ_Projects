#include "normalgame.h"
#include "zombie.h"
#include "conezombie.h"
#include "sun.h"
#include "sunflower.h"
#include "peashooter.h"
#include <QGraphicsView>
#include <QGraphicsPixmapItem>
#include <QTimer>
#include <QKeyEvent>
#include <QRandomGenerator>
#include <QMessageBox>
#include <QVBoxLayout>

NormalGame::NormalGame(QWidget *parent) : QWidget(parent)
{
    // 1. 初始化战场
    scene = new QGraphicsScene(this);
    scene->setSceneRect(0, 0, 900, 600);

    // 2. 设置视图
    view = new QGraphicsView(scene, this);
    view->setFixedSize(900, 600);
    view->setViewportUpdateMode(QGraphicsView::FullViewportUpdate);
    view->setFrameStyle(QFrame::NoFrame);

    this->setMouseTracking(true);
    view->setMouseTracking(true);
    view->viewport()->setMouseTracking(true);

    // --- 【修改点：计时器初始化，但不立刻 start】 ---

    // 主计时器（控制碰撞和胜负判定）
    mainTimer = new QTimer(this);
    connect(mainTimer, &QTimer::timeout, [=]() {
        if (isGameOver || isGameWon) return;
        scene->advance();

        int currentZombieCount = 0;
        QList<QGraphicsItem *> allItems = scene->items();
        for (QGraphicsItem *item : allItems) {
            Zombie *z = dynamic_cast<Zombie *>(item);
            if (z) {
                currentZombieCount++;
                if (z->x() < 20) { handleGameOver(); break; }
            }
        }
        if (zombiesToSpawn <= 0 && currentZombieCount == 0) { handleGameWin(); }
    });

    // 天降阳光计时器
    skySunTimer = new QTimer(this);
    connect(skySunTimer, &QTimer::timeout, [=]() {
        if (isGameOver || isGameWon) return;
        int randomX = QRandomGenerator::global()->bounded(100, 800);
        int randomY = QRandomGenerator::global()->bounded(50, 500);
        Sun *newSun = new Sun(this);
        newSun->setPos(randomX, randomY);
        scene->addItem(newSun);
    });

    // 默认进入时先显示一次初始界面
    resetGame();
}

// --- 【核心新增：控制函数】 ---

void NormalGame::start() {
    // 启动所有计时器
    mainTimer->start(33);
    skySunTimer->start(10000);

    // 关卡脚本重置并启动
    // 注意：QTimer::singleShot 是无法手动取消的，
    // 建议在 spawnZombie 里面判断 isRunning 状态。
    QTimer::singleShot(17000, [=]() { if(mainTimer->isActive()) spawnZombie(3); });
    QTimer::singleShot(35000, [=]() { if(mainTimer->isActive()) spawnZombie(2); });
    QTimer::singleShot(50000, [=]() { if(mainTimer->isActive()) spawnConeZombie(4); });
    QTimer::singleShot(100000, [=]() {
        if(mainTimer->isActive()){
            spawnZombie(2);
            spawnConeZombie(3);
            spawnZombie(4);
        }
    });
}

void NormalGame::stop() {
    // 停止所有计时器
    mainTimer->stop();
    skySunTimer->stop();
}

// --- 辅助函数 ---

void NormalGame::spawnZombie(int row) {
    if (isGameOver || isGameWon || !mainTimer->isActive()) return;
    Zombie *z = new Zombie();
    z->setPos(900, (row - 1) * 100 - 35);
    scene->addItem(z);
    zombiesToSpawn--;
}

void NormalGame::spawnConeZombie(int row) {
    if (isGameOver || isGameWon || !mainTimer->isActive()) return;
    ConeZombie *cz = new ConeZombie();
    cz->setPos(900, (row - 1) * 100 - 35);
    scene->addItem(cz);
    zombiesToSpawn--;
}

void NormalGame::addSunMoney(int amount) {
    sunMoney += amount;
    moneyText->setPlainText(QString("阳光: %1").arg(sunMoney));
}

void NormalGame::handleGameWin() {
    if (isGameWon) return;
    isGameWon = true;
    QMessageBox::information(this, "恭喜！", "你成功守住了你的脑子！");
    stop(); // 胜利后停止
    resetGame();
}

void NormalGame::mouseMoveEvent(QMouseEvent *event) {
    if (movingPlant != nullptr) {
        int grid_x = (event->pos().x() / 80) * 80;
        int grid_y = (event->pos().y() / 100) * 100;
        movingPlant->setPos(grid_x + 20, grid_y + 0);
    }
}

void NormalGame::keyPressEvent(QKeyEvent *event) {
    if (event->key() == Qt::Key_1) currentType = 1;
    else if (event->key() == Qt::Key_2) currentType = 2;
}

void NormalGame::mousePressEvent(QMouseEvent *event) {
    int col = event->pos().x() / 80;
    int row = event->pos().y() / 100;
    if (col < 0 || col >= 12 || row < 0 || row >= 6) return;

    if (movingPlant == nullptr) {
        if (mapData[col][row] == 1) return;
        if (currentType == 1 && sunMoney < 50) return;
        if (currentType == 2 && sunMoney < 100) return;
        if (currentType == 0) return;

        movingPlant = new QGraphicsPixmapItem();
        if (currentType == 2) movingPlant->setPixmap(QPixmap(":/images/Peashooter.gif").scaled(70, 80));
        else movingPlant->setPixmap(QPixmap(":/images/SunFlower.gif").scaled(70, 80));
        movingPlant->setOpacity(0.5);
        scene->addItem(movingPlant);
        movingPlant->setPos(col * 80 + 20, row * 100);
    } else {
        QPointF plantPos = movingPlant->pos();
        if (currentType == 1) {
            SunFlower *realFlower = new SunFlower();
            realFlower->setPos(plantPos);
            scene->addItem(realFlower);
            addSunMoney(-50);
            mapData[col][row] = 1;
        } else if (currentType == 2) {
            Peashooter *realPea = new Peashooter();
            realPea->setPos(plantPos);
            scene->addItem(realPea);
            addSunMoney(-100);
            mapData[col][row] = 1;
        }
        scene->removeItem(movingPlant);
        delete movingPlant;
        movingPlant = nullptr;
    }
}

void NormalGame::handleGameOver() {
    if (isGameOver) return;
    isGameOver = true;
    stop(); // 失败后停止计时器
    QMessageBox::StandardButton reply = QMessageBox::critical(this, "游戏结束", "脑子被吃了！再来一次？", QMessageBox::Retry | QMessageBox::Close);
    if (reply == QMessageBox::Retry) {
        resetGame();
        start();
    }
}

void NormalGame::resetGame() {
    scene->clear();
    isGameOver = false; isGameWon = false;
    sunMoney = 150; zombiesToSpawn = 20; // 调多点僵尸
    for(int i = 0; i < 12; ++i) for(int j = 0; j < 6; ++j) mapData[i][j] = 0;

    QGraphicsPixmapItem *bg = new QGraphicsPixmapItem();
    bg->setPixmap(QPixmap(":/images/background.png.jpg"));
    bg->setZValue(-1);
    scene->addItem(bg);

    moneyText = new QGraphicsTextItem();
    moneyText->setPlainText(QString("阳光: %1").arg(sunMoney));
    moneyText->setDefaultTextColor(Qt::yellow);
    moneyText->setFont(QFont("微软雅黑", 18, QFont::Bold));
    moneyText->setPos(20, 20);
    moneyText->setZValue(100);
    scene->addItem(moneyText);
}