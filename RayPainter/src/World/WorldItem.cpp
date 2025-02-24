#include "WorldItem.hpp"

WorldItem::WorldItem(QObject *parent)
:   QObject{parent}
{}

WorldItem::WorldItem(const QString &name, QObject *parent)
:   QObject{parent},
    _name(name)
{
}
