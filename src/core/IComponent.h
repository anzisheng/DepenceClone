#pragma once

#include <QString>
#include <QVector>
#include <QVariant>
#include <QDebug>

// 组件接口：所有可被添加到项目树的元素都实现此接口
class IComponent {
public:
    virtual ~IComponent() = default;

    // 核心方法
    virtual QString getName() const = 0;
    virtual void setName(const QString& name) = 0;
    virtual QString getType() const = 0;

    // 树形操作
    virtual void addChild(IComponent* child) {
        Q_UNUSED(child);
        qDebug() << "Leaf node cannot add children";
    }
    virtual void removeChild(IComponent* child) { Q_UNUSED(child); }
    virtual QVector<IComponent*> getChildren() const { return {}; }
    virtual bool isComposite() const { return false; }

    // 业务方法
    virtual void update() = 0;
    virtual void render() = 0;
    virtual QVariant getProperty(const QString& key) const = 0;
    virtual void setProperty(const QString& key, const QVariant& value) = 0;
};