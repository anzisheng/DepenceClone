#pragma once

#include <QMainWindow>
#include <QDockWidget>
#include <QTreeWidget>
#include <QListWidget>
#include "core/IObserver.h"
#include "models/FixtureModel.h"

// 前向声明
class WaterCurtainWidget;

class MainWindow : public QMainWindow, public IObserver {
    Q_OBJECT

private:
    QTreeWidget* m_projectTree;
    QListWidget* m_propertyPanel;
    FixtureModel* m_selectedFixture;
    WaterCurtainWidget* m_glWidget;

    // 自适应相关
    struct ScreenScale {
        double widthScale = 1.0;
        double heightScale = 1.0;
        double fontSizeScale = 1.0;
    } m_scale;

    const int DESIGN_WIDTH = 1920;
    const int DESIGN_HEIGHT = 1080;
    const int BASE_FONT_SIZE = 9;

public:
    MainWindow(QWidget* parent = nullptr);
    ~MainWindow();

    void onNotify(const QString& event, const QVariant& data = {}) override;
    QString getObserverId() const override { return "MainWindow"; }

protected:
    void resizeEvent(QResizeEvent* event) override;
    void showEvent(QShowEvent* event) override;

private:
    void setupUI();
    void setupMenuBar();
    void setupDockWindows();
    void setupCentralWidget();
    void setupStatusBar();
    void applyScaling();
    void updateFontSizes();
    void traverseTree(QTreeWidgetItem* item, int depth);

private slots:
    void onProjectItemClicked(QTreeWidgetItem* item, int column);
    void onFixturePropertyChanged(const QString& key, const QVariant& value);
};