#pragma once

#include <QScreen>
#include <QApplication>
#include <QRect>
#include <QSize>

class ScreenAdapter {
public:
    static QSize getScreenSize() {
        return QApplication::primaryScreen()->size();
    }

    static double getScaleFactor(int designWidth, int designHeight) {
        QSize screenSize = getScreenSize();
        double wScale = static_cast<double>(screenSize.width()) / designWidth;
        double hScale = static_cast<double>(screenSize.height()) / designHeight;
        return qMin(wScale, hScale);
    }

    static QSize scaleSize(const QSize& original, double scale) {
        return QSize(
            qRound(original.width() * scale),
            qRound(original.height() * scale)
        );
    }

    static QRect getAvailableGeometry() {
        return QApplication::primaryScreen()->availableGeometry();
    }
};