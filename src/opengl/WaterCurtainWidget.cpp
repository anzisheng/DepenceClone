// src/opengl/WaterCurtainWidget.cpp
#include "WaterCurtainWidget.h"
#include <QMouseEvent>
#include <QMatrix4x4>
#include <cmath>
#include <random>

static std::random_device rd;
static std::mt19937 gen(rd());
static std::uniform_real_distribution<float> dis(0.0f, 1.0f);

WaterCurtainWidget::WaterCurtainWidget(QWidget* parent)
    : QOpenGLWidget(parent) {

    setFocusPolicy(Qt::StrongFocus);
    setMinimumSize(400, 300);

    connect(&m_animationTimer, &QTimer::timeout, this, &WaterCurtainWidget::animate);
    m_animationTimer.start(16);

    setupScene();
}

WaterCurtainWidget::~WaterCurtainWidget() {
    makeCurrent();
    delete m_shaderProgram;
    doneCurrent();
}

void WaterCurtainWidget::resetView() {
    m_rotationX = 30.0f;
    m_rotationY = -45.0f;
    update();
}

void WaterCurtainWidget::setupScene() {
    m_waterDrops.clear();

    int totalDrops = m_sceneParams.nozzleCount * 15;
    m_waterDrops.reserve(totalDrops);

    for (int i = 0; i < m_sceneParams.nozzleCount; ++i) {
        float x = -m_sceneParams.curtainWidth / 2.0f +
            (i + 0.5f) * m_sceneParams.nozzleSpacing;

        for (int j = 0; j < 15; ++j) {
            WaterDrop drop;
            drop.position = QVector3D(
                x + (dis(gen) - 0.5f) * 0.02f,
                m_sceneParams.curtainHeight / 2.0f - j * 0.2f + (dis(gen) - 0.5f) * 0.05f,
                0.0f
            );
            drop.velocity = QVector3D(
                (dis(gen) - 0.5f) * 0.005f,
                -0.3f - dis(gen) * 0.2f,
                0.0f
            );
            drop.size = 0.02f + dis(gen) * 0.02f;
            drop.opacity = 0.3f + dis(gen) * 0.7f;
            drop.life = dis(gen) * 2.0f;
            drop.maxLife = 2.0f + dis(gen) * 1.5f;
            drop.active = true;
            m_waterDrops.push_back(drop);
        }
    }
}

void WaterCurtainWidget::initializeGL() {
    initializeOpenGLFunctions();
    glClearColor(0.05f, 0.05f, 0.08f, 1.0f);
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    m_shaderProgram = new QOpenGLShaderProgram(this);

    const char* vertexShaderSource =
        "attribute vec3 aPosition;\n"
        "attribute vec4 aColor;\n"
        "uniform mat4 uModelView;\n"
        "uniform mat4 uProjection;\n"
        "varying vec4 vColor;\n"
        "void main() {\n"
        "    gl_Position = uProjection * uModelView * vec4(aPosition, 1.0);\n"
        "    vColor = aColor;\n"
        "}\n";

    const char* fragmentShaderSource =
        "varying vec4 vColor;\n"
        "void main() {\n"
        "    gl_FragColor = vColor;\n"
        "}\n";

    m_shaderProgram->addShaderFromSourceCode(QOpenGLShader::Vertex, vertexShaderSource);
    m_shaderProgram->addShaderFromSourceCode(QOpenGLShader::Fragment, fragmentShaderSource);
    m_shaderProgram->link();
    m_shaderProgram->bind();
}

void WaterCurtainWidget::paintGL() {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    QMatrix4x4 projection;
    projection.perspective(45.0f, (float)width() / height(), 0.1f, 20.0f);

    QMatrix4x4 view;
    view.translate(0.0f, 0.0f, -5.0f);
    view.rotate(m_rotationX, 1.0f, 0.0f, 0.0f);
    view.rotate(m_rotationY, 0.0f, 1.0f, 0.0f);

    m_shaderProgram->bind();
    m_shaderProgram->setUniformValue("uProjection", projection);
    m_shaderProgram->setUniformValue("uModelView", view);

    drawControlCard();
    drawBoardCard();
    drawWaterCurtain();

    // 绘制喷头
    for (int i = 0; i < m_sceneParams.nozzleCount; ++i) {
        float x = -m_sceneParams.curtainWidth / 2.0f +
            (i + 0.5f) * m_sceneParams.nozzleSpacing;
        drawNozzle(x, m_sceneParams.curtainHeight / 2.0f - 0.1f, 0.0f);
    }

    m_shaderProgram->release();
}

void WaterCurtainWidget::resizeGL(int w, int h) {
    glViewport(0, 0, w, h);
}

void WaterCurtainWidget::drawControlCard() {
    // 使用 glBegin/glEnd 需要链接 opengl32.lib
    // 或者使用 QOpenGLFunctions 的方法
    // 这里我们使用 QOpenGLFunctions 的方法

    float cx = 0.0f;
    float cy = m_sceneParams.curtainHeight / 2.0f + 0.3f;
    float cz = 0.0f;
    float w = 1.2f;
    float h = 0.4f;
    float d = 0.15f;

    // 前面
    glColor4f(0.1f, 0.3f, 0.6f, 1.0f);
    glBegin(GL_QUADS);
    glVertex3f(cx - w / 2, cy - h / 2, cz + d / 2);
    glVertex3f(cx + w / 2, cy - h / 2, cz + d / 2);
    glVertex3f(cx + w / 2, cy + h / 2, cz + d / 2);
    glVertex3f(cx - w / 2, cy + h / 2, cz + d / 2);
    glEnd();

    // 后面
    glColor4f(0.05f, 0.15f, 0.3f, 1.0f);
    glBegin(GL_QUADS);
    glVertex3f(cx - w / 2, cy - h / 2, cz - d / 2);
    glVertex3f(cx - w / 2, cy + h / 2, cz - d / 2);
    glVertex3f(cx + w / 2, cy + h / 2, cz - d / 2);
    glVertex3f(cx + w / 2, cy - h / 2, cz - d / 2);
    glEnd();

    // 顶面
    glColor4f(0.2f, 0.4f, 0.7f, 1.0f);
    glBegin(GL_QUADS);
    glVertex3f(cx - w / 2, cy + h / 2, cz - d / 2);
    glVertex3f(cx - w / 2, cy + h / 2, cz + d / 2);
    glVertex3f(cx + w / 2, cy + h / 2, cz + d / 2);
    glVertex3f(cx + w / 2, cy + h / 2, cz - d / 2);
    glEnd();

    // 底面
    glColor4f(0.08f, 0.2f, 0.4f, 1.0f);
    glBegin(GL_QUADS);
    glVertex3f(cx - w / 2, cy - h / 2, cz - d / 2);
    glVertex3f(cx + w / 2, cy - h / 2, cz - d / 2);
    glVertex3f(cx + w / 2, cy - h / 2, cz + d / 2);
    glVertex3f(cx - w / 2, cy - h / 2, cz + d / 2);
    glEnd();

    // 左面
    glColor4f(0.08f, 0.2f, 0.5f, 1.0f);
    glBegin(GL_QUADS);
    glVertex3f(cx - w / 2, cy - h / 2, cz - d / 2);
    glVertex3f(cx - w / 2, cy - h / 2, cz + d / 2);
    glVertex3f(cx - w / 2, cy + h / 2, cz + d / 2);
    glVertex3f(cx - w / 2, cy + h / 2, cz - d / 2);
    glEnd();

    // 右面
    glColor4f(0.08f, 0.2f, 0.5f, 1.0f);
    glBegin(GL_QUADS);
    glVertex3f(cx + w / 2, cy - h / 2, cz - d / 2);
    glVertex3f(cx + w / 2, cy + h / 2, cz - d / 2);
    glVertex3f(cx + w / 2, cy + h / 2, cz + d / 2);
    glVertex3f(cx + w / 2, cy - h / 2, cz + d / 2);
    glEnd();

    // 指示灯
    glColor4f(0.0f, 1.0f, 0.0f, 1.0f);
    glBegin(GL_QUADS);
    for (int i = 0; i < 5; ++i) {
        float lx = cx - 0.4f + i * 0.2f;
        float ly = cy + 0.05f;
        glVertex3f(lx - 0.02f, ly - 0.02f, cz + d / 2 + 0.001f);
        glVertex3f(lx + 0.02f, ly - 0.02f, cz + d / 2 + 0.001f);
        glVertex3f(lx + 0.02f, ly + 0.02f, cz + d / 2 + 0.001f);
        glVertex3f(lx - 0.02f, ly + 0.02f, cz + d / 2 + 0.001f);
    }
    glEnd();
}

void WaterCurtainWidget::drawBoardCard() {
    float cx = 0.0f;
    float cy = m_sceneParams.curtainHeight / 2.0f - 0.1f;
    float cz = 0.0f;
    float w = 1.6f;
    float h = 0.25f;
    float d = 0.08f;

    // 前面
    glColor4f(0.0f, 0.5f, 0.2f, 1.0f);
    glBegin(GL_QUADS);
    glVertex3f(cx - w / 2, cy - h / 2, cz + d / 2);
    glVertex3f(cx + w / 2, cy - h / 2, cz + d / 2);
    glVertex3f(cx + w / 2, cy + h / 2, cz + d / 2);
    glVertex3f(cx - w / 2, cy + h / 2, cz + d / 2);
    glEnd();

    // 后面
    glColor4f(0.0f, 0.25f, 0.1f, 1.0f);
    glBegin(GL_QUADS);
    glVertex3f(cx - w / 2, cy - h / 2, cz - d / 2);
    glVertex3f(cx - w / 2, cy + h / 2, cz - d / 2);
    glVertex3f(cx + w / 2, cy + h / 2, cz - d / 2);
    glVertex3f(cx + w / 2, cy - h / 2, cz - d / 2);
    glEnd();

    // 顶面
    glColor4f(0.1f, 0.6f, 0.3f, 1.0f);
    glBegin(GL_QUADS);
    glVertex3f(cx - w / 2, cy + h / 2, cz - d / 2);
    glVertex3f(cx - w / 2, cy + h / 2, cz + d / 2);
    glVertex3f(cx + w / 2, cy + h / 2, cz + d / 2);
    glVertex3f(cx + w / 2, cy + h / 2, cz - d / 2);
    glEnd();

    // 底面
    glColor4f(0.0f, 0.2f, 0.05f, 1.0f);
    glBegin(GL_QUADS);
    glVertex3f(cx - w / 2, cy - h / 2, cz - d / 2);
    glVertex3f(cx + w / 2, cy - h / 2, cz - d / 2);
    glVertex3f(cx + w / 2, cy - h / 2, cz + d / 2);
    glVertex3f(cx - w / 2, cy - h / 2, cz + d / 2);
    glEnd();

    // 左面
    glColor4f(0.0f, 0.3f, 0.15f, 1.0f);
    glBegin(GL_QUADS);
    glVertex3f(cx - w / 2, cy - h / 2, cz - d / 2);
    glVertex3f(cx - w / 2, cy - h / 2, cz + d / 2);
    glVertex3f(cx - w / 2, cy + h / 2, cz + d / 2);
    glVertex3f(cx - w / 2, cy + h / 2, cz - d / 2);
    glEnd();

    // 右面
    glColor4f(0.0f, 0.3f, 0.15f, 1.0f);
    glBegin(GL_QUADS);
    glVertex3f(cx + w / 2, cy - h / 2, cz - d / 2);
    glVertex3f(cx + w / 2, cy + h / 2, cz - d / 2);
    glVertex3f(cx + w / 2, cy + h / 2, cz + d / 2);
    glVertex3f(cx + w / 2, cy - h / 2, cz + d / 2);
    glEnd();

    // 芯片标记
    glColor4f(0.2f, 0.7f, 0.3f, 0.8f);
    glBegin(GL_QUADS);
    for (int i = -3; i <= 3; ++i) {
        if (i == 0) continue;
        float px = cx + i * 0.12f;
        glVertex3f(px - 0.03f, cy - 0.02f, cz + d / 2 + 0.001f);
        glVertex3f(px + 0.03f, cy - 0.02f, cz + d / 2 + 0.001f);
        glVertex3f(px + 0.03f, cy + 0.02f, cz + d / 2 + 0.001f);
        glVertex3f(px - 0.03f, cy + 0.02f, cz + d / 2 + 0.001f);
    }
    glEnd();
}

void WaterCurtainWidget::drawNozzle(float x, float y, float z) {
    float radius = 0.035f;
    float height = 0.08f;
    int segments = 12;

    glColor4f(0.7f, 0.7f, 0.8f, 1.0f);
    glBegin(GL_QUAD_STRIP);
    for (int i = 0; i <= segments; ++i) {
        float angle = 2.0f * M_PI * i / segments;
        float px = x + radius * cos(angle);
        float py = y + radius * sin(angle);
        glVertex3f(px, py, z + height);
        glVertex3f(px, py, z);
    }
    glEnd();

    glColor4f(0.8f, 0.8f, 0.9f, 1.0f);
    glBegin(GL_TRIANGLE_FAN);
    glVertex3f(x, y, z + height);
    for (int i = 0; i <= segments; ++i) {
        float angle = 2.0f * M_PI * i / segments;
        float px = x + radius * cos(angle);
        float py = y + radius * sin(angle);
        glVertex3f(px, py, z + height);
    }
    glEnd();

    glColor4f(0.5f, 0.5f, 0.6f, 1.0f);
    glBegin(GL_TRIANGLE_FAN);
    glVertex3f(x, y, z);
    for (int i = 0; i <= segments; ++i) {
        float angle = 2.0f * M_PI * i / segments;
        float px = x + radius * cos(angle);
        float py = y + radius * sin(angle);
        glVertex3f(px, py, z);
    }
    glEnd();

    // 喷口发光
    glColor4f(0.0f, 0.8f, 1.0f, 0.8f);
    float sx = 0.015f;
    glBegin(GL_QUADS);
    glVertex3f(x - sx, y - sx, z - 0.01f);
    glVertex3f(x + sx, y - sx, z - 0.01f);
    glVertex3f(x + sx, y + sx, z - 0.01f);
    glVertex3f(x - sx, y + sx, z - 0.01f);
    glEnd();
}

void WaterCurtainWidget::drawWaterCurtain() {
    // 水帘背景
    glColor4f(0.0f, 0.3f, 0.8f, 0.08f);
    float w = m_sceneParams.curtainWidth;
    float h = m_sceneParams.curtainHeight;
    float z = -0.1f;
    glBegin(GL_QUADS);
    glVertex3f(-w / 2, -h / 2, z);
    glVertex3f(w / 2, -h / 2, z);
    glVertex3f(w / 2, h / 2, z);
    glVertex3f(-w / 2, h / 2, z);
    glEnd();

    // 水帘线条
    glBegin(GL_LINES);
    for (int i = 0; i < m_sceneParams.nozzleCount; ++i) {
        float x = -m_sceneParams.curtainWidth / 2.0f +
            (i + 0.5f) * m_sceneParams.nozzleSpacing;
        for (int j = 0; j < 20; ++j) {
            float y = -m_sceneParams.curtainHeight / 2.0f +
                (j + 0.5f) * (m_sceneParams.curtainHeight / 20.0f);
            float alpha = 0.2f + 0.6f * (y / m_sceneParams.curtainHeight + 0.5f);
            float offset = 0.02f * sin(m_time * 2.0f + x * 5.0f + y * 3.0f);
            glColor4f(0.2f, 0.6f, 1.0f, alpha * 0.5f);
            glVertex3f(x + offset, y, z + 0.02f);
            glVertex3f(x + offset, y + 0.02f, z + 0.02f);
        }
    }
    glEnd();

    // 水滴
    for (const auto& drop : m_waterDrops) {
        if (drop.active) {
            drawWaterDrop(drop.position.x(), drop.position.y(),
                drop.position.z(), drop.size, drop.opacity);
        }
    }

    // 底部水花
    glPointSize(2.0f);
    glBegin(GL_POINTS);
    for (int i = 0; i < 50; ++i) {
        float x = -m_sceneParams.curtainWidth / 2.0f + dis(gen) * m_sceneParams.curtainWidth;
        float y = -m_sceneParams.curtainHeight / 2.0f + 0.05f;
        float alpha = 0.2f + 0.3f * (1.0f - abs(x) / (m_sceneParams.curtainWidth / 2.0f));
        glColor4f(0.3f, 0.7f, 1.0f, alpha);
        glVertex3f(x, y + dis(gen) * 0.02f, 0.02f);
    }
    glEnd();
}

void WaterCurtainWidget::drawWaterDrop(float x, float y, float z, float size, float opacity) {
    glColor4f(0.2f, 0.7f, 1.0f, opacity * 0.8f);
    glBegin(GL_TRIANGLE_FAN);
    glVertex3f(x, y, z);
    int segments = 8;
    for (int i = 0; i <= segments; ++i) {
        float angle = 2.0f * M_PI * i / segments;
        float px = x + size * cos(angle);
        float py = y + size * sin(angle);
        glColor4f(0.3f, 0.8f, 1.0f, opacity * 0.6f);
        glVertex3f(px, py, z);
    }
    glEnd();

    // 高光
    glColor4f(1.0f, 1.0f, 1.0f, opacity * 0.3f);
    glBegin(GL_TRIANGLE_FAN);
    glVertex3f(x + size * 0.3f, y + size * 0.3f, z + 0.005f);
    glVertex3f(x + size * 0.5f, y + size * 0.5f, z + 0.005f);
    glVertex3f(x + size * 0.2f, y + size * 0.6f, z + 0.005f);
    glEnd();
}

void WaterCurtainWidget::updateWaterDrops() {
    float dt = 0.016f;

    for (auto& drop : m_waterDrops) {
        if (!drop.active) continue;

        drop.position += drop.velocity * dt * 0.5f;

        float sway = 0.001f * sin(m_time * 3.0f + drop.position.y() * 2.0f);
        drop.position.setX(drop.position.x() + sway);

        drop.life += dt;

        if (drop.position.y() < -m_sceneParams.curtainHeight / 2.0f ||
            drop.life > drop.maxLife) {
            int nozzleIndex = (int)((drop.position.x() + m_sceneParams.curtainWidth / 2.0f) /
                m_sceneParams.nozzleSpacing);
            nozzleIndex = std::max(0, std::min(nozzleIndex, m_sceneParams.nozzleCount - 1));

            float x = -m_sceneParams.curtainWidth / 2.0f +
                (nozzleIndex + 0.5f) * m_sceneParams.nozzleSpacing;

            drop.position = QVector3D(
                x + (dis(gen) - 0.5f) * 0.02f,
                m_sceneParams.curtainHeight / 2.0f - 0.1f,
                0.0f
            );
            drop.velocity = QVector3D(
                (dis(gen) - 0.5f) * 0.005f,
                -0.3f - dis(gen) * 0.3f,
                0.0f
            );
            drop.life = 0.0f;
            drop.maxLife = 1.5f + dis(gen) * 1.5f;
            drop.size = 0.015f + dis(gen) * 0.025f;
            drop.opacity = 0.3f + dis(gen) * 0.7f;
        }
    }
}

void WaterCurtainWidget::animate() {
    m_time += 0.016f;
    updateWaterDrops();
    update();
}

void WaterCurtainWidget::mousePressEvent(QMouseEvent* event) {
    if (event->button() == Qt::LeftButton) {
        m_lastMousePos = event->pos();
    }
}

void WaterCurtainWidget::mouseMoveEvent(QMouseEvent* event) {
    if (event->buttons() & Qt::LeftButton) {
        QPoint delta = event->pos() - m_lastMousePos;
        m_rotationY += delta.x() * 0.5f;
        m_rotationX += delta.y() * 0.5f;
        m_rotationX = std::max(-90.0f, std::min(90.0f, m_rotationX));
        m_lastMousePos = event->pos();
        update();
    }
}

void WaterCurtainWidget::wheelEvent(QWheelEvent* event) {
    QOpenGLWidget::wheelEvent(event);
}