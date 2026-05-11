#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QStackedWidget>
#include "normalgame.h"  // 包含 1.0 模式
// #include "hungrygame.h" // 以后包含 2.0 模式

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private:
    QStackedWidget *stack;   // 核心：像幻灯片一样的堆栈窗口
    QWidget *menuPage;       // 菜单页面（选模式的地方）
    NormalGame *normalGame;  // 1.0 经典模式的实例
};

#endif