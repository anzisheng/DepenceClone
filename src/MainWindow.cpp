// MainWindow.cpp
#include "MainWindow.h"
#include <QMenuBar>
#include <QMenu>
#include <QStatusBar>
#include <QTextEdit>
#include <QListWidget>
#include <QDockWidget>
#include <QLabel>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
{
    // 设置窗口大小和标题
    resize(1024, 768);
    setWindowTitle("Depence 4 Style Demo");

    // 1. 设置菜单栏
    setupMenuBar();

    // 2. 设置停靠窗口
    setupDockWindows();

    // 3. 设置中央视口
    setupCentralWidget();

    // 4. 设置状态栏
    statusBar()->showMessage("就绪");
}

MainWindow::~MainWindow() {}

void MainWindow::setupMenuBar()
{
    // 创建 '文件' 菜单
    QMenu *fileMenu = menuBar()->addMenu("文件(&F)");
    fileMenu->addAction("新建(&N)", this, &QWidget::close, QKeySequence::New);
    fileMenu->addAction("打开(&O)...", this, &QWidget::close, QKeySequence::Open);
    fileMenu->addSeparator();
    fileMenu->addAction("退出(&X)", this, &QWidget::close, QKeySequence::Quit);

    // 创建 '视图' 菜单（用于显示/隐藏停靠窗口）
    QMenu *viewMenu = menuBar()->addMenu("视图(&V)");
    viewMenu->addAction("项目视图", this, [this](){
        // 通过 findChild 查找停靠窗口并切换可见性
        QDockWidget *dock = findChild<QDockWidget*>("projectDock");
        if (dock) dock->setVisible(!dock->isVisible());
    });
    viewMenu->addAction("库视图", this, [this](){
        QDockWidget *dock = findChild<QDockWidget*>("libraryDock");
        if (dock) dock->setVisible(!dock->isVisible());
    });
    // ... 可以继续添加更多
}

void MainWindow::setupDockWindows()
{
    // 1. 创建 "项目视图" 停靠窗口
    QDockWidget *projectDock = new QDockWidget("项目视图", this);
    projectDock->setObjectName("projectDock"); // 方便 findChild 查找
    m_projectDockList = new QListWidget(projectDock);
    m_projectDockList->addItems({"场景", "灯具", "摄像机"});
    projectDock->setWidget(m_projectDockList);
    addDockWidget(Qt::LeftDockWidgetArea, projectDock);

    // 2. 创建 "库视图" 停靠窗口
    QDockWidget *libraryDock = new QDockWidget("库视图", this);
    libraryDock->setObjectName("libraryDock");
    m_libraryDockList = new QListWidget(libraryDock);
    m_libraryDockList->addItems({"摇头灯", "激光器", "LED屏"});
    libraryDock->setWidget(m_libraryDockList);
    addDockWidget(Qt::LeftDockWidgetArea, libraryDock);

    // 3. 创建 "属性/设置" 停靠窗口 (放在右侧)
    QDockWidget *settingsDock = new QDockWidget("设置", this);
    QListWidget *settingsList = new QListWidget(settingsDock);
    settingsList->addItems({"位置", "旋转", "颜色"});
    settingsDock->setWidget(settingsList);
    addDockWidget(Qt::RightDockWidgetArea, settingsDock);
}

void MainWindow::setupCentralWidget()
{
    // 中央区域放置一个文本编辑器，模拟 3D 视口
    m_centralTextEdit = new QTextEdit(this);
    m_centralTextEdit->setPlainText("中央 3D 视口 (模拟 Depence 的渲染视图)");
    // 可以设置一些样式让它看起来更像一个视口
    m_centralTextEdit->setStyleSheet("QTextEdit { background-color: #2b2b2b; color: #d4d4d4; }");
    setCentralWidget(m_centralTextEdit);
}