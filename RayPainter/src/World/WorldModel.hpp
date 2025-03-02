#pragma once

#include <QObject>
#include <QAbstractItemModel>
#include <QMimeData>
#include <QByteArray>
#include <QDataStream>
#include <QIODevice>
#include <QDebug>
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
        auto children = parentItem->orderedChildren();
        return (row >= 0 && row < children.size())
                   ? createIndex(row, column, children.at(row)) : QModelIndex{ };
    }

    QModelIndex parent(const QModelIndex& index) const override {
        if (!index.isValid())
            return { };
        auto childItem = static_cast<WorldItem*>(index.internalPointer());
        auto parentItem = dynamic_cast<WorldItem*>(childItem->parent());
        if (!parentItem || parentItem == rootItem.get())
            return { };
        auto grandparentItem = dynamic_cast<WorldItem*>(parentItem->parent());
        if (!grandparentItem)
            return { };
        auto row = grandparentItem->orderedChildren().indexOf(parentItem);
        return createIndex(row, 0, parentItem);
    }

    int rowCount(const QModelIndex& parent = { }) const override {
        auto parentItem = parent.isValid()
                              ? static_cast<WorldItem*>(parent.internalPointer()) : rootItem.get();
        return parentItem ? parentItem->orderedChildren().size() : 0;
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

    /** @brief Search for a child WorldItem by the given name */
    WorldItem* findItemByName(WorldItem* parent, const QString& name) const {
        WorldItem* item{ nullptr };
        if (parent != nullptr) {
            for (auto c: parent->children()) {
                auto child = static_cast<WorldItem*>(c);
                if (child->name() == name) {
                    item = child;
                    break;
                } else {
                    auto *found = findItemByName(child, name);
                    if (found != nullptr) {
                        item = found;
                        break;
                    }
                }
            }
        }
        return item;
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

        // find parent root/folder item
        auto parentItem = parent.isValid() ? static_cast<WorldItem*>(parent.internalPointer())
                                           : nullptr;
        if (!parentItem || (parentItem != rootLight.get()
                            && parentItem != rootShape.get())) {
            return false;
        }

        // get item instance being dragged
        auto draggedItem = findItemByName(rootItem.get(), name);
        if (!draggedItem)
            return false;

        // reject the drop if it's in the wrong folder type for the item
        bool isLight = dynamic_cast<LightItem*>(draggedItem) != nullptr;
        bool isShape = dynamic_cast<ShapeItem*>(draggedItem) != nullptr;
        if ((isLight && parentItem != rootLight.get())
            || (isShape && parentItem != rootShape.get())) {
            // wrong folder!
            return false;
        }

        // remove at dropped position
        auto currentParent = static_cast<WorldItem*>(draggedItem->parent());
        if (currentParent != nullptr) {
            int oldRow = currentParent->orderedChildren().indexOf(draggedItem);
            beginRemoveRows(indexForItem(currentParent), oldRow, oldRow);
            draggedItem->setParent(nullptr);
            endRemoveRows();
        }

        int newRow = (row >= 0 && row < parentItem->orderedChildren().size())
                         ? row : parentItem->orderedChildren().size();
        // before inserting, ordering is updated to new row
        draggedItem->setOrder(newRow);
        // insert at new row
        beginInsertRows(indexForItem(parentItem), newRow, newRow);
        draggedItem->setParent(parentItem);
        endInsertRows();

        // display ordering of all parent's children must be updated
        auto ordered = parentItem->orderedChildren();
        ordered.insert(newRow, draggedItem);
        for (size_t i{}; i < ordered.size(); ++i)
            ordered.at(i)->setOrder(i);

        return true;
    }


private:
    std::unique_ptr<WorldItem> rootItem{ nullptr };
    std::unique_ptr<WorldItem> rootLight{ nullptr };
    std::unique_ptr<WorldItem> rootShape{ nullptr };

#ifdef IS_TEST_SUITE
    friend class test_WorldModel;
    friend class test_WorldTreeView;
#endif

private:
    Q_OBJECT
};
