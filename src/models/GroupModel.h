#pragma once

#include "../core/IComponent.h"
#include <QVector>
#include <QDebug>

class GroupModel : public IComponent {
private:
    QString m_name;
    QVector<IComponent*> m_children;

public:
    GroupModel(const QString& name = "Group") : m_name(name) {}

    ~GroupModel() {
        qDeleteAll(m_children);
        m_children.clear();
    }

    QString getName() const override { return m_name; }
    void setName(const QString& name) override { m_name = name; }
    QString getType() const override { return "Group"; }

    void addChild(IComponent* child) override {
        if (child && !m_children.contains(child)) {
            m_children.append(child);
        }
    }

    void removeChild(IComponent* child) override {
        m_children.removeAll(child);
    }

    QVector<IComponent*> getChildren() const override { return m_children; }
    bool isComposite() const override { return true; }

    void update() override {
        qDebug() << "更新组:" << m_name << "包含" << m_children.size() << "个子组件";
        for (auto* child : m_children) {
            child->update();
        }
    }

    void render() override {
        qDebug() << "渲染组:" << m_name << "包含" << m_children.size() << "个子组件";
        for (auto* child : m_children) {
            child->render();
        }
    }

    QVariant getProperty(const QString& key) const override {
        QVariantList results;
        for (auto* child : m_children) {
            results.append(child->getProperty(key));
        }
        return results;
    }

    void setProperty(const QString& key, const QVariant& value) override {
        qDebug() << "组设置属性:" << key << "应用到所有子组件";
        for (auto* child : m_children) {
            child->setProperty(key, value);
        }
    }

    int childCount() const { return m_children.size(); }
    IComponent* childAt(int index) const {
        if (index >= 0 && index < m_children.size()) {
            return m_children[index];
        }
        return nullptr;
    }
    void clearChildren() {
        qDeleteAll(m_children);
        m_children.clear();
    }
};