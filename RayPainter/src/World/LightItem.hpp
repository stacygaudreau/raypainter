#pragma once

#include <QObject>
#include "WorldItem.hpp"


class LightItem : public WorldItem
{
public:
    explicit LightItem(QObject *parent = nullptr)
        :   WorldItem(parent) {
    }
    explicit LightItem(const QString& name, QObject *parent = nullptr)
        :   WorldItem(name, parent) {
    }

public:

signals:

private:
    Q_OBJECT
};
