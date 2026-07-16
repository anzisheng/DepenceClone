#pragma once

#include <QOpenGLWidget>
#include <QOpenGLFunctions>
#include <QOpenGLShaderProgram>
#include <QTimer>
#include <QVector3D>
#include <vector>

class WaterCurtainWidget : public QOpenGLWidget, protected QOpenGLFunctions {
    Q_OBJECT

public:
    explicit WaterCurtainWidget(QWidget* parent = nullptr);
    ~WaterCurtainWidget();

    void resetView();

protected:
    void initializeGL() override;
    void paintGL() override;
    void resizeGL(int w, int h) override;

private:
    void setupScene();
    void drawControlCard();
    void drawBoardCard();
    void drawNozzle(float x, float y, float z);
    void drawWaterCurtain();
    void drawWaterDrop(float x, float y, float z, float size, float opacity);
    void updateWaterDrops();

private:
    struct {
        float cardWidth = 0.8f;
        float cardHeight = 0.5f;
        float cardDepth = 0.1f;
        float nozzleSpacing = 0.12f;
        int nozzleCount = 20;
        float curtainWidth = 2.4f;
        float curtainHeight = 3.0f;
    } m_sceneParams;

    struct WaterDrop {
        QVector3D position;
        QVector3D velocity;
        float size;
        float opacity;
        float life;
        float maxLife;
        bool active;
    };
    std::vector<WaterDrop> m_waterDrops;
    QTimer m_animationTimer;
    float m_time = 0.0f;

    float m_rotationX = 30.0f;
    float m_rotationY = -45.0f;
    QPoint m_lastMousePos;

    QOpenGLShaderProgram* m_shaderProgram = nullptr;

protected:
    void mousePressEvent(QMouseEvent* event) override;
    void mouseMoveEvent(QMouseEvent* event) override;
    void wheelEvent(QWheelEvent* event) override;

private slots:
    void animate();
};