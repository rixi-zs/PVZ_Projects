#include "hungrygame.h"
#include <QKeyEvent>
#include <QTimer>
#include <QRandomGenerator>
#include <QDebug>
#include <QtMath>

HungryGame::HungryGame(QWidget *parent) : QWidget(parent)
{
    // 1. 初始化场景
    scene = new QGraphicsScene(this);
    scene->setSceneRect(0, 0, 3000, 2000);

    // 2. 初始化视图
    view = new QGraphicsView(scene, this);
    view->setFixedSize(900, 600);

    QPixmap bgPixmap(":/images/final_land.jpg");
    QBrush bgBrush(bgPixmap);
    bgBrush.setStyle(Qt::TexturePattern);
    view->setBackgroundBrush(bgBrush);
    view->setFrameStyle(QFrame::NoFrame);
    view->setViewportUpdateMode(QGraphicsView::FullViewportUpdate);
    view->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    view->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);

    // --- 小地图视图配置 ---
    miniView = new QGraphicsView(scene, this);
    miniView->setGeometry(680, 430, 200, 150);
    miniView->setRenderHint(QPainter::Antialiasing);
    miniView->setFrameStyle(QFrame::Box);
    miniView->setStyleSheet("border: 2px solid white; background-color: rgba(0,0,0,120);");
    miniView->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    miniView->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    miniView->setInteractive(false);
    miniView->fitInView(scene->sceneRect(), Qt::KeepAspectRatio);

    // 3. 初始化 UI
    healthBar = new QProgressBar(this);
    healthBar->setGeometry(20, 20, 300, 30);
    healthBar->setRange(0, 100);
    healthBar->setValue(100);
    healthBar->setFormat("饥饿度: %v");
    healthBar->setStyleSheet(
        "QProgressBar { border: 2px solid #555; border-radius: 5px; background-color: rgba(0,0,0,150); text-align: center; color: white; font-weight: bold; }"
        "QProgressBar::chunk { background-color: #2ecc71; }"
        );

    // 4. 初始化计时器
    hungerTimer = new QTimer(this);
    connect(hungerTimer, &QTimer::timeout, [=]() {
        if (player && scene->items().contains(player)) player->hit(1);
    });

    gameTimer = new QTimer(this);
    connect(gameTimer, &QTimer::timeout, this, &HungryGame::updateGame);

    brainSpawner = new QTimer(this);
    connect(brainSpawner, &QTimer::timeout, [=]() {
        int rx = QRandomGenerator::global()->bounded(100, 2900);
        int ry = QRandomGenerator::global()->bounded(100, 1900);
        QGraphicsPixmapItem *brain = new QGraphicsPixmapItem();
        QPixmap brainPix(":/images/brain.png");
        brain->setPixmap(brainPix.scaled(60, 60, Qt::KeepAspectRatio, Qt::SmoothTransformation));
        brain->setPos(rx, ry);
        brain->setData(0, "brain");
        scene->addItem(brain);
    });

    // --- 【核心修改：菜问 5 个上限循环生成逻辑】 ---
    enemySpawner = new QTimer(this);
    connect(enemySpawner, &QTimer::timeout, [=]() {
        // 1. 寻找当前场上所有的菜问
        QList<QGraphicsItem *> currentBonks;
        QList<QGraphicsItem *> allItems = scene->items();
        for (QGraphicsItem *item : allItems) {
            if (item->data(0).toString() == "bonkchoy") {
                currentBonks.append(item);
            }
        }

        // 2. 如果达到或超过 5 个，删除最早的一个（列表最后一项）
        if (currentBonks.size() >= 5) {
            QGraphicsItem *oldest = currentBonks.last();
            scene->removeItem(oldest);
            delete oldest;
            qDebug() << "移除一个老菜问，准备在随机位置生成新威胁。";
        }

        // 3. 生成新的菜问
        int rx = QRandomGenerator::global()->bounded(100, 2900);
        int ry = QRandomGenerator::global()->bounded(100, 1900);
        QGraphicsPixmapItem *bonk = new QGraphicsPixmapItem();
        QPixmap bonkPix(":/images/BonkChoy.png");
        bonk->setPixmap(bonkPix.scaled(80, 80, Qt::KeepAspectRatio, Qt::SmoothTransformation));
        bonk->setPos(rx, ry);
        bonk->setData(0, "bonkchoy");
        scene->addItem(bonk);

        qDebug() << "菜问已循环刷新，当前总数:" << (currentBonks.size() < 5 ? currentBonks.size() + 1 : 5);
    });

    this->setFocusPolicy(Qt::StrongFocus);
    player = nullptr;
}

void HungryGame::start()
{
    scene->clear();
    createTerrain();

    player = new Zombie();
    player->setPlayerControl(true);
    player->setPos(1200, 800);
    scene->addItem(player);

    healthBar->setValue(100);
    gameTimer->start(16);
    brainSpawner->start(3000);
    hungerTimer->start(500);
    enemySpawner->start(8000);

    qDebug() << "饥饿模式启动：菜问已开启 5 个名额循环滚动机制。";
}

void HungryGame::stop()
{
    gameTimer->stop();
    brainSpawner->stop();
    hungerTimer->stop();
    enemySpawner->stop();
}

void HungryGame::updateGame()
{
    processMovement();

    // 菜问追踪 AI
    if (player && scene->items().contains(player)) {
        QList<QGraphicsItem *> allItems = scene->items();
        for (QGraphicsItem *item : allItems) {
            if (item->data(0).toString() == "bonkchoy") {
                qreal dx = player->x() - item->x();
                qreal dy = player->y() - item->y();
                qreal dist = qSqrt(dx * dx + dy * dy);

                if (dist > 0) {
                    qreal enemySpeed = 3.5;
                    item->moveBy((dx / dist) * enemySpeed, (dy / dist) * enemySpeed);

                    QList<QGraphicsItem*> hits = item->collidingItems();
                    for(auto *h : hits) {
                        if(h->data(0).toString() == "wall") {
                            item->moveBy(-(dx / dist) * enemySpeed, -(dy / dist) * enemySpeed);
                            break;
                        }
                    }
                }
            }
        }
    }

    scene->advance();

    if (player && scene->items().contains(player)) {
        view->centerOn(player);
        healthBar->setValue(player->getHP());
    }
    scene->update();
}

void HungryGame::processMovement()
{
    if (!player || !scene->items().contains(player)) return;

    int speed = 6;
    bool moved = false;

    if (keysPressed[Qt::Key_W]) { player->moveBy(0, -speed); moved = true; }
    if (keysPressed[Qt::Key_S]) { player->moveBy(0, speed);  moved = true; }
    if (keysPressed[Qt::Key_A]) { player->moveBy(-speed, 0); moved = true; }
    if (keysPressed[Qt::Key_D]) { player->moveBy(speed, 0);  moved = true; }

    if (moved) {
        if(player->x() < 0) player->setX(0);
        if(player->x() > 2880) player->setX(2880);
        if(player->y() < 0) player->setY(0);
        if(player->y() > 1880) player->setY(1880);

        QList<QGraphicsItem *> targets = player->collidingItems();
        for (QGraphicsItem *item : targets) {
            QString type = item->data(0).toString();

            if (type == "wall") {
                if (keysPressed[Qt::Key_W]) player->moveBy(0, speed);
                if (keysPressed[Qt::Key_S]) player->moveBy(0, -speed);
                if (keysPressed[Qt::Key_A]) player->moveBy(speed, 0);
                if (keysPressed[Qt::Key_D]) player->moveBy(-speed, 0);
                break;
            }

            if (type == "brain") {
                player->addHP(20);
                scene->removeItem(item);
                delete item;
            }
            else if (type == "bonkchoy") {
                player->hit(50);
                scene->removeItem(item);
                delete item;
            }
        }
    }
}

void HungryGame::createTerrain()
{
    int size = 50;
    int gap = 50;

    auto addWall = [this, size](int x, int y) {
        QGraphicsPixmapItem *wall = new QGraphicsPixmapItem();
        wall->setPixmap(QPixmap(":/images/WallNut.png").scaled(size, size, Qt::KeepAspectRatio, Qt::SmoothTransformation));
        wall->setPos(x, y);
        wall->setData(0, "wall");
        scene->addItem(wall);
    };

    for(int i = -10; i <= 10; ++i) {
        if(i == 0) continue;
        addWall(1500 + i * gap, 1000);
    }
    for(int i = -8; i <= 8; ++i) {
        if(i == 0) continue;
        addWall(1500, 1000 + i * gap);
    }

    int cornersX[] = {500, 2500, 500, 2500};
    int cornersY[] = {500, 500, 1500, 1500};

    for(int j = 0; j < 4; ++j) {
        for(int i = 0; i < 7; ++i) {
            int dirX = (j % 2 == 0) ? 1 : -1;
            int dirY = (j < 2) ? 1 : -1;
            addWall(cornersX[j] + i * gap * dirX, cornersY[j]);
            addWall(cornersX[j], cornersY[j] + i * gap * dirY);
        }
    }
}

void HungryGame::keyPressEvent(QKeyEvent *event) { if (!event->isAutoRepeat()) keysPressed[event->key()] = true; }
void HungryGame::keyReleaseEvent(QKeyEvent *event) { if (!event->isAutoRepeat()) keysPressed[event->key()] = false; }