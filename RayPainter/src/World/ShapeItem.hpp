#pragma once

#include <QObject>
#include "WorldItem.hpp"


class ShapeItem : public WorldItem
{
public:
    explicit ShapeItem(QObject *parent = nullptr)
    :   WorldItem(parent) {
    }
    explicit ShapeItem(const QString& name, QObject *parent = nullptr)
    :   WorldItem(name, parent) {
    }

public:

signals:

private:
    Q_OBJECT
};
