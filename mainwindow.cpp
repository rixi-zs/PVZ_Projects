#include "mainwindow.h"
#include "zombie.h"
#include <QGraphicsView>
#include <QGraphicsPixmapItem>
#include <QTimer>
#include <QKeyEvent>
#include "sun.h"
#include "sunflower.h"
#include "peashooter.h"
#include "bullet.h"
#include <QRandomGenerator>
#include <QMessageBox>
#include "conezombie.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
{
    // 1. 设置窗口大小
    this->setFixedSize(900, 600);
    this->setWindowTitle("植物大战僵尸 - 关卡挑战版");

    // 2. 初始化战场
    scene = new QGraphicsScene(this);
    scene->setSceneRect(0, 0, 900, 600);

    // 3. 设置视图
    QGraphicsView *view = new QGraphicsView(scene, this);
    view->setFixedSize(900, 600);
    view->setViewportUpdateMode(QGraphicsView::FullViewportUpdate);
    view->setFrameStyle(QFrame::NoFrame);
    view->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    view->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);

    this->setMouseTracking(true);
    view->setMouseTracking(true);
    view->viewport()->setMouseTracking(true);
    setCentralWidget(view);

    // 4. 加载背景
    QGraphicsPixmapItem *bg = new QGraphicsPixmapItem();
    bg->setPixmap(QPixmap(":/images/background.png.jpg"));
    bg->setZValue(-1);
    scene->addItem(bg);

    // --- 初始化变量与地图 ---
    sunMoney = 50;
    isGameOver = false;
    isGameWon = false;
    zombiesToSpawn = 8;

    for(int i = 0; i < 12; ++i) {
        for(int j = 0; j < 6; ++j) {
            mapData[i][j] = 0;
        }
    }

    // 5. 阳光余额显示
    moneyText = new QGraphicsTextItem();
    moneyText->setPlainText(QString("阳光: %1").arg(sunMoney));
    moneyText->setDefaultTextColor(Qt::yellow);
    moneyText->setFont(QFont("微软雅黑", 18, QFont::Bold));
    moneyText->setPos(20, 20);
    moneyText->setZValue(100);
    scene->addItem(moneyText);

    // --- 6. 【关卡脚本：按照设计好的时间出怪】 ---
    QTimer::singleShot(17000, [=]() { spawnZombie(3); });
    QTimer::singleShot(35000, [=]() { spawnZombie(2); });
    QTimer::singleShot(50000, [=]() { spawnConeZombie(4); });
    QTimer::singleShot(70000, [=]() { spawnZombie(3); });
    QTimer::singleShot(85000, [=]() { spawnZombie(2); });
    QTimer::singleShot(100000, [=]() {
        spawnZombie(2);
        spawnConeZombie(3);
        spawnZombie(4);
        this->setWindowTitle("！！！最后一波僵尸！！！");
    });

    // 7. 游戏主计时器
    QTimer *timer = new QTimer(this);
    connect(timer, &QTimer::timeout, [=]() {
        if (isGameOver || isGameWon) return;

        scene->advance();

        int currentZombieCount = 0;
        QList<QGraphicsItem *> allItems = scene->items();
        for (QGraphicsItem *item : allItems) {
            Zombie *z = dynamic_cast<Zombie *>(item);
            if (z) {
                currentZombieCount++;
                if (z->x() < 20) {
                    handleGameOver();
                    break;
                }
            }
        }

        if (zombiesToSpawn <= 0 && currentZombieCount == 0) {
            handleGameWin();
        }
    });
    timer->start(33);

    // 8. 自动天降阳光定时器 (恢复之前的随机生成逻辑)
    QTimer *skySunTimer = new QTimer(this);
    connect(skySunTimer, &QTimer::timeout, [=]() {
        if (isGameOver || isGameWon) return;

        // 随机生成坐标 (按照你之前的 randomX, randomY 逻辑)
        int randomX = QRandomGenerator::global()->bounded(100, 800);
        int randomY = QRandomGenerator::global()->bounded(50, 500);

        Sun *newSun = new Sun(this);
        newSun->setPos(randomX, randomY);
        scene->addItem(newSun);
    });
    skySunTimer->start(10000);
}

// --- 辅助生成函数 (修正高度对齐) ---
void MainWindow::spawnZombie(int row) {
    if (isGameOver || isGameWon) return;
    Zombie *z = new Zombie();
    // 这里的坐标计算确保僵尸出现在你之前的 265 附近轨道上
    z->setPos(900, (row - 1) * 100 - 35);
    scene->addItem(z);
    zombiesToSpawn--;
}

void MainWindow::spawnConeZombie(int row) {
    if (isGameOver || isGameWon) return;
    ConeZombie *cz = new ConeZombie();
    cz->setPos(900, (row - 1) * 100 - 35);
    scene->addItem(cz);
    zombiesToSpawn--;
}

void MainWindow::addSunMoney(int amount)
{
    sunMoney += amount;
    moneyText->setPlainText(QString("阳光: %1").arg(sunMoney));
}

void MainWindow::handleGameWin()
{
    if (isGameWon) return;
    isGameWon = true;
    QMessageBox::information(this, "恭喜！", "你成功守住了你的脑子！");
    resetGame();
}

void MainWindow::mouseMoveEvent(QMouseEvent *event)
{
    if (movingPlant != nullptr) {
        int grid_x = (event->pos().x() / 80) * 80;
        int grid_y = (event->pos().y() / 100) * 100;
        movingPlant->setPos(grid_x + 20, grid_y + 0);
    }
}

void MainWindow::keyPressEvent(QKeyEvent *event)
{
    if (event->key() == Qt::Key_1) {
        currentType = 1;
        this->setWindowTitle("当前选择：向日葵");
    }
    else if (event->key() == Qt::Key_2) {
        currentType = 2;
        this->setWindowTitle("当前选择：豌豆射手");
    }
}

void MainWindow::mousePressEvent(QMouseEvent *event)
{
    int col = event->pos().x() / 80;
    int row = event->pos().y() / 100;

    if (col < 0 || col >= 12 || row < 0 || row >= 6) return;

    if (movingPlant == nullptr) {
        if (mapData[col][row] == 1) return;
        if (currentType == 1 && sunMoney < 50) return;
        if (currentType == 2 && sunMoney < 100) return;
        if (currentType == 0) return;

        movingPlant = new QGraphicsPixmapItem();
        if (currentType == 2)
            movingPlant->setPixmap(QPixmap(":/images/Peashooter.gif").scaled(70, 80));
        else
            movingPlant->setPixmap(QPixmap(":/images/SunFlower.gif").scaled(70, 80));

        movingPlant->setOpacity(0.5);
        scene->addItem(movingPlant);
        movingPlant->setPos(col * 80 + 20, row * 100);
    }
    else {
        QPointF plantPos = movingPlant->pos();
        if (currentType == 1) {
            SunFlower *realFlower = new SunFlower();
            realFlower->setPos(plantPos);
            scene->addItem(realFlower);
            addSunMoney(-50);
            mapData[col][row] = 1;
        }
        else if (currentType == 2) {
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

void MainWindow::handleGameOver()
{
    if (isGameOver) return;
    isGameOver = true;

    QMessageBox::StandardButton reply;
    reply = QMessageBox::critical(this, "游戏结束", "僵尸吃掉了你的脑子！要再试一次吗？",
                                  QMessageBox::Retry | QMessageBox::Close);

    if (reply == QMessageBox::Retry) {
        resetGame();
    } else {
        this->close();
    }
}

void MainWindow::resetGame()
{
    scene->clear();
    isGameOver = false;
    isGameWon = false;
    currentType = 0;
    movingPlant = nullptr;
    sunMoney = 150;
    zombiesToSpawn = 8;

    for(int i = 0; i < 12; ++i)
        for(int j = 0; j < 6; ++j)
            mapData[i][j] = 0;

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

    // 重置后的关卡也同步更新
    QTimer::singleShot(17000, [=]() { spawnZombie(3); });
    QTimer::singleShot(35000, [=]() { spawnZombie(2); });
    QTimer::singleShot(50000, [=]() { spawnConeZombie(4); });

    this->setWindowTitle("植物大战僵尸 - 游戏已重启");
}

MainWindow::~MainWindow() {}