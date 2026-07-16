#pragma once

#include <QString>

template<typename T>
class Singleton {
protected:
    Singleton() = default;
    virtual ~Singleton() = default;
    Singleton(const Singleton&) = delete;
    Singleton& operator=(const Singleton&) = delete;

public:
    static T& getInstance() {
        static T instance;
        return instance;
    }
};

// 全局配置管理器
class AppConfig : public Singleton<AppConfig> {
    friend class Singleton<AppConfig>;
private:
    AppConfig() = default;

public:
    QString getTheme() const { return m_theme; }
    void setTheme(const QString& theme) { m_theme = theme; }

private:
    QString m_theme = "dark";
};