#include "hungrygame.h"
#include <QKeyEvent>
#include <QTimer>
#include <QRandomGenerator>
#include <QDebug>
#include <QtMath>
#include <QMessageBox>
#include <QPushButton>

HungryGame::HungryGame(QWidget *parent) : QWidget(parent)
{
    // 1. 初始化场景 (3000x2000)
    scene = new QGraphicsScene(this);
    scene->setSceneRect(0, 0, 3000, 2000);

    // 2. 初始化主视图
    view = new QGraphicsView(scene, this);
    view->setFixedSize(900, 600);

    QPixmap bgPixmap(":/images/final_land.jpg");
    view->setBackgroundBrush(QBrush(bgPixmap));
    view->setFrameStyle(QFrame::NoFrame);
    view->setViewportUpdateMode(QGraphicsView::FullViewportUpdate);
    view->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    view->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);

    // --- 背景音乐初始化 ---
    m_audioOutput = new QAudioOutput(this);
    m_bgmPlayer = new QMediaPlayer(this);
    m_bgmPlayer->setAudioOutput(m_audioOutput);
    // 注意：请确保资源文件路径正确
    m_bgmPlayer->setSource(QUrl("qrc:/audio/bgm.mp3"));
    m_bgmPlayer->setLoops(QMediaPlayer::Infinite);
    m_audioOutput->setVolume(0.5); // 音量 50%

    // --- 小地图视图配置 ---
    miniView = new QGraphicsView(scene, this);
    miniView->setGeometry(680, 430, 200, 150);
    miniView->setRenderHint(QPainter::Antialiasing);
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
        brain->setPixmap(QPixmap(":/images/brain.png").scaled(60, 60, Qt::KeepAspectRatio, Qt::SmoothTransformation));
        brain->setPos(rx, ry);
        brain->setData(0, "brain");
        scene->addItem(brain);
    });

    enemySpawner = new QTimer(this);
    connect(enemySpawner, &QTimer::timeout, [=]() {
        QList<QGraphicsItem *> currentBonks;
        for (QGraphicsItem *item : scene->items()) {
            if (item->data(0).toString() == "bonkchoy") currentBonks.append(item);
        }
        if (currentBonks.size() >= 5) {
            QGraphicsItem *oldest = currentBonks.last();
            scene->removeItem(oldest);
            delete oldest;
        }
        QGraphicsPixmapItem *bonk = new QGraphicsPixmapItem();
        bonk->setPixmap(QPixmap(":/images/BonkChoy.png").scaled(80, 80, Qt::KeepAspectRatio, Qt::SmoothTransformation));
        bonk->setPos(QRandomGenerator::global()->bounded(100, 2900), QRandomGenerator::global()->bounded(100, 1900));
        bonk->setData(0, "bonkchoy");
        scene->addItem(bonk);
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

    m_bgmPlayer->play(); // 播放音乐
    qDebug() << "游戏开始，音乐已开启";
}

void HungryGame::stop()
{
    gameTimer->stop();
    brainSpawner->stop();
    hungerTimer->stop();
    enemySpawner->stop();
    m_bgmPlayer->stop(); // 停止音乐
}

void HungryGame::updateGame()
{
    processMovement();

    // 菜问追踪 AI
    if (player && scene->items().contains(player)) {
        for (QGraphicsItem *item : scene->items()) {
            if (item->data(0).toString() == "bonkchoy") {
                qreal dx = player->x() - item->x();
                qreal dy = player->y() - item->y();
                qreal dist = qSqrt(dx * dx + dy * dy);
                if (dist > 0) {
                    qreal enemySpeed = 3.5;
                    item->moveBy((dx / dist) * enemySpeed, (dy / dist) * enemySpeed);
                    // 撞墙处理
                    for(auto *h : item->collidingItems()) {
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
        int hp = player->getHP();
        healthBar->setValue(hp);

        if (hp <= 0) {
            stop();
            healthBar->setValue(0);
            QTimer::singleShot(50, this, &HungryGame::showGameOverDialog);
            return;
        }
    }
    scene->update();
}

void HungryGame::showGameOverDialog()
{
    m_bgmPlayer->pause(); // 弹窗时暂停音乐
    QMessageBox msgBox(QMessageBox::Question, "游戏结束",
                       "你饿晕了！菜问们守护了脑子...\n你要重新挑战吗？",
                       QMessageBox::Yes | QMessageBox::No, this);

    msgBox.setButtonText(QMessageBox::Yes, "再来一局");
    msgBox.setButtonText(QMessageBox::No, "不玩了");

    if (msgBox.exec() == QMessageBox::Yes) {
        start();
    } else {
        this->close();
    }
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
        // 边界限制
        if(player->x() < 0) player->setX(0);
        if(player->x() > 2880) player->setX(2880);
        if(player->y() < 0) player->setY(0);
        if(player->y() > 1880) player->setY(1880);

        for (QGraphicsItem *item : player->collidingItems()) {
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
            } else if (type == "bonkchoy") {
                player->hit(50);
                scene->removeItem(item);
                delete item;
            }
        }
    }
}

void HungryGame::createTerrain()
{
    int size = 50, gap = 50;
    auto addWall = [this, size](int x, int y) {
        QGraphicsPixmapItem *wall = new QGraphicsPixmapItem();
        wall->setPixmap(QPixmap(":/images/WallNut.png").scaled(size, size, Qt::KeepAspectRatio));
        wall->setPos(x, y);
        wall->setData(0, "wall");
        scene->addItem(wall);
    };

    // 十字长廊
    for(int i = -10; i <= 10; ++i) { if(i != 0) addWall(1500 + i * gap, 1000); }
    for(int i = -8; i <= 8; ++i) { if(i != 0) addWall(1500, 1000 + i * gap); }

    // 四角 L 型
    int cx[] = {500, 2500, 500, 2500}, cy[] = {500, 500, 1500, 1500};
    for(int j = 0; j < 4; ++j) {
        for(int i = 0; i < 7; ++i) {
            int dx = (j%2==0)?1:-1, dy = (j<2)?1:-1;
            addWall(cx[j] + i*gap*dx, cy[j]);
            addWall(cx[j], cy[j] + i*gap*dy);
        }
    }
}

void HungryGame::keyPressEvent(QKeyEvent *event) { if (!event->isAutoRepeat()) keysPressed[event->key()] = true; }
void HungryGame::keyReleaseEvent(QKeyEvent *event) { if (!event->isAutoRepeat()) keysPressed[event->key()] = false; }