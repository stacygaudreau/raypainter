#pragma once

#include <QObject>
#include <QAbstractItemModel>
#include <QMimeData>
#include <QByteArray>
#include <QDataStream>
#include <QIODevice>
#include <memory>
#include "WorldItem.hpp"
#include "LightItem.hpp"
#include "ShapeItem.hpp"


class WorldModel : public QAbstractItemModel
{
public:
    explicit WorldModel(QObject* parent = nullptr)
    :   QAbstractItemModel(parent) {
        // root node is just an item with nullptr as parent
        rootItem = std::make_unique<WorldItem>("World", nullptr);
        // lighting and shape nodes are attached to the primary root
        rootLight = std::make_unique<WorldItem>("Lights", rootItem.get());
        rootShape = std::make_unique<WorldItem>("Shapes", rootItem.get());
    }
    ~WorldModel() = default;


    QModelIndex index(int row, int column, const QModelIndex& parent = {}) const override {
        if (!hasIndex(row, column, parent))
            return { };
        auto parentItem = parent.isValid()
                              ? static_cast<WorldItem*>(parent.internalPointer()) : rootItem.get();
        auto childItem = dynamic_cast<WorldItem*>(parentItem->children().value(row));
        return childItem ? createIndex(row, column, childItem) : QModelIndex{ };
    }

    QModelIndex parent(const QModelIndex& index) const override {
        if (!index.isValid())
            return { };
        auto childItem = static_cast<WorldItem*>(index.internalPointer());
        auto parentItem = dynamic_cast<WorldItem*>(childItem->parent());
        return (parentItem == rootItem.get() || !parentItem)
                   ? QModelIndex()
                   : createIndex(parentItem->children().indexOf(childItem), 0, parentItem);
    }

    int rowCount(const QModelIndex& parent = { }) const override {
        auto parentItem = parent.isValid()
                              ? static_cast<WorldItem*>(parent.internalPointer()) : rootItem.get();
        return parentItem->children().size();
    }

    int columnCount(const QModelIndex& parent = { }) const override { return 1; }

    QVariant data(const QModelIndex& index, int role = Qt::DisplayRole) const override {
        if (!index.isValid())
            return { };

        auto item = static_cast<WorldItem*>(index.internalPointer());
        return (role == Qt::DisplayRole) ? item->name() : QVariant{ };
    }

    inline QModelIndex indexForItem(WorldItem* item) const {
        if (!item || item == rootItem.get())
            return { };
        auto parent = dynamic_cast<WorldItem*>(item->parent());
        if (!parent)
            return { };
        int row = parent->children().indexOf(item);
        return createIndex(row, 0, item);
    }

    void addItem(WorldItem* item) {
        // which folder should the item go in?
        WorldItem* parent{ nullptr };
        if (dynamic_cast<LightItem*>(item)) {
            parent = rootLight.get();
        } else if (dynamic_cast<ShapeItem*>(item)) {
            parent = rootShape.get();
        }

        if (parent != nullptr) {
            beginInsertRows(indexForItem(item), rootItem->children().size(),
                            rootItem->children().size());
            item->setParent(parent);
            endInsertRows();
        }
    }

    Qt::ItemFlags flags(const QModelIndex& index) const override {
        if (!index.isValid())
            return Qt::NoItemFlags;
        auto item = static_cast<WorldItem*>(index.internalPointer());
        Qt::ItemFlags flags = Qt::ItemIsSelectable | Qt::ItemIsEnabled
                              | Qt::ItemIsDragEnabled;
        if (item == rootLight.get() || item == rootShape.get()) {
            flags |= Qt::ItemIsDropEnabled;
        }
        return flags;
    }

    Qt::DropActions supportedDropActions() const override {
        return Qt::MoveAction;
    }

    QStringList mimeTypes() const override {
        return {"application/x-world-item"};
    }

    QMimeData* mimeData(const QModelIndexList& indexes) const override {
        if (indexes.isEmpty())
            return nullptr;
        // encode mime data to byte array
        auto mimeData = new QMimeData{ };
        QByteArray data{ };
        QDataStream stream{&data, QIODevice::WriteOnly};
        for (const auto& index: indexes) {
            auto item = static_cast<WorldItem*>(index.internalPointer());
            stream << item->name();
        }
        mimeData->setData("application/x-world-item", data);
        return mimeData;
    }

    bool dropMimeData(const QMimeData* data, Qt::DropAction action,
                      int row, int column, const QModelIndex& parent) override {
        // reject drop actions for invalid types identified in mime data
        if (action != Qt::MoveAction || !data->hasFormat("application/x-world-item"))
            return false;

        auto encoded = data->data("application/x-world-item");
        QDataStream stream{ &encoded, QIODevice::ReadOnly };
        QString name{ };
        stream >> name;

        auto parentItem = parent.isValid() ? static_cast<WorldItem*>(parent.internalPointer())
                                           : nullptr;
        if (!parentItem || (parentItem != rootLight.get()
                            && parentItem != rootShape.get())) {
            return false;
        }

        // reject the drop if it's in the wrong folder type for the item
        bool isLight = parentItem == rootLight.get();
        bool isShape = parentItem == rootShape.get();
        if ((isLight && dynamic_cast<ShapeItem*>(parentItem))
            || (isShape && dynamic_cast<LightItem*>(parentItem))) {
            return false;
        }

        return true;
    }


private:
    std::unique_ptr<WorldItem> rootItem{ nullptr };
    std::unique_ptr<WorldItem> rootLight{ nullptr };
    std::unique_ptr<WorldItem> rootShape{ nullptr };

private:
    Q_OBJECT
};
