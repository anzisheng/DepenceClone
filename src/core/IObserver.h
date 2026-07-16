#pragma once

#include <QString>
#include <QVariant>
#include <QVector>

// 观察者接口
class IObserver {
public:
    virtual ~IObserver() = default;
    virtual void onNotify(const QString& event, const QVariant& data = {}) = 0;
    virtual QString getObserverId() const = 0;
};

// 被观察者接口（主题）
class ISubject {
public:
    virtual ~ISubject() = default;
    virtual void attach(IObserver* observer) = 0;
    virtual void detach(IObserver* observer) = 0;
    virtual void notify(const QString& event, const QVariant& data = {}) = 0;
};

// 简单主题实现
class SubjectBase : public ISubject {
protected:
    QVector<IObserver*> m_observers;

public:
    void attach(IObserver* observer) override {
        if (!m_observers.contains(observer)) {
            m_observers.append(observer);
        }
    }

    void detach(IObserver* observer) override {
        m_observers.removeAll(observer);
    }

    void notify(const QString& event, const QVariant& data = {}) override {
        for (auto* observer : m_observers) {
            observer->onNotify(event, data);
        }
    }
};