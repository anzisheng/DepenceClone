#pragma once

#include <QByteArray>
#include <QString>
#include <QVariant>

// 序列化策略接口
class ISerializationStrategy {
public:
    virtual ~ISerializationStrategy() = default;
    virtual QByteArray serialize(const QVariant& data) = 0;
    virtual QVariant deserialize(const QByteArray& data) = 0;
    virtual QString getFormatName() const = 0;
};

// 可序列化对象接口
class ISerializable {
public:
    virtual ~ISerializable() = default;
    virtual QVariant toVariant() const = 0;
    virtual void fromVariant(const QVariant& data) = 0;

    QByteArray serialize(ISerializationStrategy* strategy) {
        return strategy->serialize(toVariant());
    }

    void deserialize(const QByteArray& data, ISerializationStrategy* strategy) {
        fromVariant(strategy->deserialize(data));
    }
};