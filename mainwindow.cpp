#include "mainwindow.h"
#include <QPushButton>
#include <QVBoxLayout>
#include <QMessageBox>
#include <QStackedWidget>
#include "hungrygame.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
{
    // 1. 设置主窗口大小
    this->setFixedSize(900, 600);
    this->setWindowTitle("植物大战僵尸 - 多模式版");

    // 2. 创建堆栈布局管理器（游戏机主体）
    stack = new QStackedWidget(this);
    setCentralWidget(stack);

    // 3. 实例化所有页面（把卡带准备好）
    menuPage = new QWidget();
    normalGame = new NormalGame();
    HungryGame *hungryGame = new HungryGame(); // 这里建议在.h里声明，或者保持局部变量

    // 4. 配置菜单页面布局
    QVBoxLayout *layout = new QVBoxLayout(menuPage);
    QPushButton *btnNormal = new QPushButton("经典模式 (塔防)", menuPage);
    QPushButton *btnHungry = new QPushButton("饥饿模式 (动作)", menuPage);

    btnNormal->setFixedSize(200, 60);
    btnHungry->setFixedSize(200, 60);

    layout->addWidget(btnNormal, 0, Qt::AlignCenter);
    layout->addWidget(btnHungry, 0, Qt::AlignCenter);

    // 5. 将所有页面装入堆栈
    stack->addWidget(menuPage);   // Index 0
    stack->addWidget(normalGame); // Index 1
    stack->addWidget(hungryGame); // Index 2

    // --- 6. 绑定按钮逻辑 (修改后的版本) ---

    // 点击经典模式
    connect(btnNormal, &QPushButton::clicked, [=]() {
        // 1. 如果此时正在玩饥饿模式，先停掉它的所有逻辑
        // 你需要去 HungryGame 实现 stop() 函数，停掉里面的 gameTimer 和 spawner
        // hungryGame->stop();

        // 2. 切换并启动经典模式
        stack->setCurrentIndex(1);
        normalGame->start(); // 启动 1.0 的僵尸生成计时器
        normalGame->setFocus();
    });

    // 点击饥饿模式的按钮
    connect(btnHungry, &QPushButton::clicked, [=]() {
        // 1. 彻底杀掉经典模式的后台逻辑
        // 这一步最关键！如果不调这个，1.0 的僵尸会源源不断产生并吃掉 2.0 的脑子
        normalGame->stop();

        // 2. 切换页面
        stack->setCurrentIndex(2);

        // 3. 启动饥饿模式的逻辑（计时器、刷脑子等）
        // 不要让 HungryGame 在构造函数里就运行，而是在这里手动 start
        hungryGame->start();
        hungryGame->setFocus();
    });
}

MainWindow::~MainWindow() {}