#include "MainWindow.h"
#include "models/FixtureModel.h"
#include "models/GroupModel.h"
#include "opengl/WaterCurtainWidget.h"
#include "utils/ScreenAdapter.h"
#include <QMenuBar>
#include <QStatusBar>
#include <QToolBar>
#include <QTreeWidget>
#include <QHeaderView>
#include <QScreen>
#include <QApplication>
#include <QFont>
#include <QLabel>
#include <QTimer>
#include <QMessageBox>
#include <QVBoxLayout>

MainWindow::MainWindow(QWidget* parent)
    : QMainWindow(parent), m_glWidget(nullptr) {

    // 1. 先设置窗口为全屏
    showFullScreen();

    // 2. 初始化 UI
    setupUI();

    // 3. 创建示例数据
    auto* root = new GroupModel("Stage Layout");
    auto* fixture1 = new FixtureModel("MAC Aura", "MovingHead");
    auto* fixture2 = new FixtureModel("Atomic 3000", "Strobe");
    auto* group1 = new GroupModel("Truss 1");

    group1->addChild(fixture1);
    group1->addChild(fixture2);
    root->addChild(group1);

    // 监听灯具变化
    fixture1->attach(this);
    m_selectedFixture = fixture1;

    // 4. 填充项目树
    auto* rootItem = new QTreeWidgetItem(m_projectTree);
    rootItem->setText(0, "📁 舞台布局");
    rootItem->setData(0, Qt::UserRole, "Group");

    auto* trussItem = new QTreeWidgetItem(rootItem);
    trussItem->setText(0, "🔲 Truss 1");
    trussItem->setData(0, Qt::UserRole, "Group");

    auto* f1Item = new QTreeWidgetItem(trussItem);
    f1Item->setText(0, "💡 MAC Aura");
    f1Item->setData(0, Qt::UserRole, "Fixture");

    auto* f2Item = new QTreeWidgetItem(trussItem);
    f2Item->setText(0, "⚡ Atomic 3000");
    f2Item->setData(0, Qt::UserRole, "Fixture");

    m_projectTree->expandAll();

    // 5. 应用自适应（延迟执行确保窗口已完全显示）
    QTimer::singleShot(100, this, &MainWindow::applyScaling);

    statusBar()->showMessage("程序已启动 | 全屏自适应模式 | 3D水帘系统");
}

MainWindow::~MainWindow() {
    // 清理观察者
    if (m_selectedFixture) {
        m_selectedFixture->detach(this);
    }
}

void MainWindow::setupUI() {
    setWindowTitle("Depence 4 Style Demo - Water Curtain System");
    setStyleSheet(R"(
        QMainWindow {
            background-color: #1e1e1e;
        }
        QDockWidget {
            titlebar-close-icon: url();
        }
        QDockWidget::title {
            background-color: #2d2d2d;
            color: #d4d4d4;
            padding: 4px;
            font-weight: bold;
        }
        QTreeWidget, QListWidget {
            background-color: #252526;
            color: #d4d4d4;
            border: 1px solid #3e3e42;
        }
        QTreeWidget::item:selected, QListWidget::item:selected {
            background-color: #264f78;
        }
        QMenuBar {
            background-color: #2d2d2d;
            color: #d4d4d4;
        }
        QMenuBar::item:selected {
            background-color: #3e3e42;
        }
        QStatusBar {
            background-color: #007acc;
            color: white;
        }
    )");

    setupMenuBar();
    setupDockWindows();
    setupCentralWidget();
    setupStatusBar();
}

void MainWindow::setupMenuBar() {
    // 文件菜单
    QMenu* fileMenu = menuBar()->addMenu("文件(&F)");
    fileMenu->addAction("新建项目(&N)", this, &QWidget::close, QKeySequence::New);
    fileMenu->addAction("打开项目(&O)...", this, &QWidget::close, QKeySequence::Open);
    fileMenu->addSeparator();
    fileMenu->addAction("导入(&I)", this, &QWidget::close);
    fileMenu->addAction("导出(&E)", this, &QWidget::close);
    fileMenu->addSeparator();
    fileMenu->addAction("退出(&X)", this, &QWidget::close, QKeySequence::Quit);

    // 视图菜单 - 控制面板显示
    QMenu* viewMenu = menuBar()->addMenu("视图(&V)");
    viewMenu->addAction("项目面板", this, [this]() {
        if (auto* dock = findChild<QDockWidget*>("projectDock")) {
            dock->setVisible(!dock->isVisible());
        }
        }, QKeySequence("Ctrl+1"));

    viewMenu->addAction("属性面板", this, [this]() {
        if (auto* dock = findChild<QDockWidget*>("propertyDock")) {
            dock->setVisible(!dock->isVisible());
        }
        }, QKeySequence("Ctrl+2"));

    viewMenu->addAction("全屏切换", this, &MainWindow::showFullScreen, QKeySequence("F11"));

    // 工具菜单 - 演示设计模式
    QMenu* toolsMenu = menuBar()->addMenu("工具(&T)");
    toolsMenu->addAction("测试观察者模式", this, [this]() {
        if (m_selectedFixture) {
            m_selectedFixture->togglePower();
            statusBar()->showMessage("灯具电源已切换 (观察者模式通知)", 2000);
        }
        });

    toolsMenu->addAction("测试组合模式", this, [this]() {
        qDebug() << "=== 组合模式遍历 ===";
        auto* tree = findChild<QTreeWidget*>("projectTree");
        if (tree && tree->topLevelItem(0)) {
            traverseTree(tree->topLevelItem(0), 0);
        }
        statusBar()->showMessage("组合模式遍历完成 (查看输出窗口)", 2000);
        });

    toolsMenu->addSeparator();
    toolsMenu->addAction("重置视角", this, [this]() {
        if (m_glWidget) {
            m_glWidget->resetView();
            statusBar()->showMessage("视角已重置", 1000);
        }
        });

    toolsMenu->addAction("关于", this, [this]() {
        QMessageBox::about(this, "关于",
            "水帘系统 3D 可视化\n\n"
            "硬件组件:\n"
            "• 控制卡 (Controller Card)\n"
            "• 板卡 (Board Card)\n"
            "• 喷头阵列 (Nozzle Array)\n"
            "• 水帘效果 (Water Curtain)\n\n"
            "设计模式:\n"
            "• 组合模式 (Composite)\n"
            "• 观察者模式 (Observer)\n"
            "• 单例模式 (Singleton)\n\n"
            "操作: 鼠标拖拽旋转视角\n"
            "Qt 6.8.3 | OpenGL");
        });
}

void MainWindow::setupDockWindows() {
    // 1. 项目面板（左侧）
    QDockWidget* projectDock = new QDockWidget("项目视图", this);
    projectDock->setObjectName("projectDock");
    projectDock->setFeatures(QDockWidget::DockWidgetClosable |
        QDockWidget::DockWidgetMovable |
        QDockWidget::DockWidgetFloatable);

    m_projectTree = new QTreeWidget(projectDock);
    m_projectTree->setObjectName("projectTree");
    m_projectTree->setHeaderLabel("项目结构");
    m_projectTree->setIndentation(20);
    projectDock->setWidget(m_projectTree);
    addDockWidget(Qt::LeftDockWidgetArea, projectDock);

    connect(m_projectTree, &QTreeWidget::itemClicked,
        this, &MainWindow::onProjectItemClicked);

    // 2. 属性面板（右侧）
    QDockWidget* propertyDock = new QDockWidget("属性", this);
    propertyDock->setObjectName("propertyDock");
    propertyDock->setFeatures(QDockWidget::DockWidgetClosable |
        QDockWidget::DockWidgetMovable |
        QDockWidget::DockWidgetFloatable);

    m_propertyPanel = new QListWidget(propertyDock);
    m_propertyPanel->addItem("系统状态: 运行中");
    m_propertyPanel->addItem("控制卡: 在线");
    m_propertyPanel->addItem("板卡: 在线");
    m_propertyPanel->addItem("喷头数量: 20");
    m_propertyPanel->addItem("水流速度: 1.0 m/s");
    m_propertyPanel->addItem("水帘高度: 3.0 m");
    propertyDock->setWidget(m_propertyPanel);
    addDockWidget(Qt::RightDockWidgetArea, propertyDock);
}

void MainWindow::setupCentralWidget() {
    // 使用 OpenGL 水帘窗口
    auto* container = new QWidget(this);
    auto* layout = new QVBoxLayout(container);
    layout->setContentsMargins(0, 0, 0, 0);

    m_glWidget = new WaterCurtainWidget(container);
    layout->addWidget(m_glWidget);

    setCentralWidget(container);
}

void MainWindow::setupStatusBar() {
    statusBar()->addPermanentWidget(new QLabel("📐 自适应: 开启"));
    statusBar()->addPermanentWidget(new QLabel("🔌 设计模式: 组合 | 观察者 | 单例"));
    statusBar()->addPermanentWidget(new QLabel("🎮 鼠标拖拽旋转"));
}

// ========== 自适应核心逻辑 ==========

void MainWindow::showEvent(QShowEvent* event) {
    QMainWindow::showEvent(event);
    QTimer::singleShot(50, this, &MainWindow::applyScaling);
}

void MainWindow::resizeEvent(QResizeEvent* event) {
    QMainWindow::resizeEvent(event);
    applyScaling();
}

void MainWindow::applyScaling() {
    if (!isVisible()) return;

    QRect windowRect = this->geometry();

    m_scale.widthScale = static_cast<double>(windowRect.width()) / DESIGN_WIDTH;
    m_scale.heightScale = static_cast<double>(windowRect.height()) / DESIGN_HEIGHT;
    m_scale.fontSizeScale = qMin(m_scale.widthScale, m_scale.heightScale);

    updateFontSizes();

    statusBar()->showMessage(QString("自适应: 宽%1% 高%2% | 窗口: %3x%4")
        .arg(m_scale.widthScale * 100, 0, 'f', 1)
        .arg(m_scale.heightScale * 100, 0, 'f', 1)
        .arg(windowRect.width())
        .arg(windowRect.height()), 3000);
}

void MainWindow::updateFontSizes() {
    int newSize = qMax(8, qRound(BASE_FONT_SIZE * m_scale.fontSizeScale));

    QFont defaultFont = QApplication::font();
    defaultFont.setPointSize(newSize);
    qApp->setFont(defaultFont);
}

// ========== 信号槽实现 ==========

void MainWindow::onProjectItemClicked(QTreeWidgetItem* item, int column) {
    Q_UNUSED(column);
    if (!item) return;

    QString type = item->data(0, Qt::UserRole).toString();
    QString name = item->text(0).remove(0, 2);

    statusBar()->showMessage(QString("选中: %1 (%2)").arg(name).arg(type), 2000);

    if (type == "Fixture") {
        m_propertyPanel->clear();
        m_propertyPanel->addItem(QString("名称: %1").arg(name));
        m_propertyPanel->addItem("类型: 灯具");
        m_propertyPanel->addItem("位置: (2.5, 0, 3.0)");
        m_propertyPanel->addItem("颜色: RGB(255, 0, 128)");
        m_propertyPanel->addItem("强度: 75%");
        m_propertyPanel->addItem("状态: 开启 ✓");
    }
}

void MainWindow::onFixturePropertyChanged(const QString& key, const QVariant& value) {
    QString message = QString("属性已变化: %1 = %2").arg(key).arg(value.toString());
    statusBar()->showMessage(message, 2000);

    if (key == "isOn") {
        bool isOn = value.toBool();
        if (m_propertyPanel && m_propertyPanel->count() > 4) {
            m_propertyPanel->item(4)->setText(QString("状态: %1").arg(isOn ? "开启 ✓" : "关闭 ✗"));
        }
    }
}

void MainWindow::onNotify(const QString& event, const QVariant& data) {
    QString message = QString("📢 收到通知: %1").arg(event);
    if (data.isValid()) {
        message += QString(" | 数据: %1").arg(data.toString());
    }

    statusBar()->showMessage(message, 2000);

    if (event == "powerToggled") {
        bool isOn = data.toBool();
        if (m_propertyPanel && m_propertyPanel->count() > 4) {
            m_propertyPanel->item(4)->setText(QString("状态: %1").arg(isOn ? "开启 ✓" : "关闭 ✗"));
        }
    }
    else if (event == "propertyChanged") {
        QVariantMap dataMap = data.toMap();
        QString key = dataMap["key"].toString();
        QVariant value = dataMap["value"];
        onFixturePropertyChanged(key, value);
    }
}

// ========== 辅助函数实现 ==========

void MainWindow::traverseTree(QTreeWidgetItem* item, int depth) {
    if (!item) return;

    QString indent = QString("  ").repeated(depth);
    qDebug() << indent << item->text(0);

    for (int i = 0; i < item->childCount(); ++i) {
        traverseTree(item->child(i), depth + 1);
    }
}