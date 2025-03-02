#pragma once

#include <QMainWindow>
#include <QTreeView>
#include <QDockWidget>
#include <memory>
#include "World/WorldModel.hpp"
#include "World/ShapeItem.hpp"
#include "World/LightItem.hpp"

class MainWindow : public QMainWindow
{
public:
    MainWindow(QWidget *parent = nullptr) {

        worldModel = std::make_unique<WorldModel>();
        auto s1 = new ShapeItem{ "shape 1" };
        worldModel->addItem(s1);
        auto s2 = new ShapeItem{ "shape 2" };
        worldModel->addItem(s2);
        auto l1 = new LightItem{ "point light" };
        worldModel->addItem(l1);
        auto l2 = new LightItem{ "ambient light" };
        worldModel->addItem(l2);
        auto s3 = new ShapeItem{ "shape 3" };
        worldModel->addItem(s3);

        treeView = std::make_unique<QTreeView>();
        treeView->setModel(worldModel.get());
        treeView->setSelectionMode(QAbstractItemView::SingleSelection);
        treeView->setHeaderHidden(true);
        // drag and drop to reorder items
        treeView->setDragEnabled(true);
        treeView->setAcceptDrops(true);
        treeView->setDropIndicatorShown(true);
        treeView->setDragDropMode(QAbstractItemView::InternalMove);

        auto dock = new QDockWidget("World", this);
        dock->setWidget(treeView.get());
        addDockWidget(Qt::LeftDockWidgetArea, dock);

    }
    ~MainWindow() = default;

private:
    std::unique_ptr<WorldModel> worldModel;
    std::unique_ptr<QTreeView> treeView;

private:
    Q_OBJECT
};
