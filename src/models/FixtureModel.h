#pragma once

#include "../core/IComponent.h"
#include "../core/IObserver.h"
#include <QMap>
#include <QColor>
#include <QVector3D>
#include <QVariantMap>
#include <QDebug>

class FixtureModel : public IComponent, public SubjectBase {
private:
    QString m_name;
    QString m_type;
    QMap<QString, QVariant> m_properties;
    bool m_isSelected = false;

public:
    FixtureModel(const QString& name = "Unnamed Fixture",
        const QString& type = "MovingHead")
        : m_name(name), m_type(type) {
        m_properties["position"] = QVector3D(0, 0, 0);
        m_properties["rotation"] = QVector3D(0, 0, 0);
        m_properties["color"] = QColor(255, 255, 255);
        m_properties["intensity"] = 100.0;
        m_properties["isOn"] = true;
    }

    QString getName() const override { return m_name; }

    void setName(const QString& name) override {
        m_name = name;
        notify("nameChanged", name);
    }

    QString getType() const override { return m_type; }

    void addChild(IComponent* child) override {
        Q_UNUSED(child);
        qDebug() << "灯具不能添加子节点";
    }

    void removeChild(IComponent* child) override {
        Q_UNUSED(child);
        qDebug() << "灯具不能移除子节点";
    }

    QVector<IComponent*> getChildren() const override { return {}; }
    bool isComposite() const override { return false; }

    void update() override {
        qDebug() << "更新灯具:" << m_name;
        notify("updated");
    }

    void render() override {
        qDebug() << "渲染灯具:" << m_name;
    }

    QVariant getProperty(const QString& key) const override {
        return m_properties.value(key);
    }

    void setProperty(const QString& key, const QVariant& value) override {
        if (m_properties[key] != value) {
            m_properties[key] = value;
            QVariantMap data;
            data["key"] = key;
            data["value"] = value;
            notify("propertyChanged", data);
        }
    }

    void togglePower() {
        bool current = m_properties["isOn"].toBool();
        setProperty("isOn", !current);
        notify("powerToggled", !current);
    }

    void setPosition(const QVector3D& pos) {
        setProperty("position", pos);
    }

    QVector3D getPosition() const {
        return m_properties["position"].value<QVector3D>();
    }

    bool isSelected() const { return m_isSelected; }

    void setSelected(bool selected) {
        if (m_isSelected != selected) {
            m_isSelected = selected;
            notify("selectionChanged", selected);
        }
    }
};