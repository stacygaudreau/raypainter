#include <QObject>
#include <QTest>
#include <QTreeView>
#include <memory>

#include "World/WorldModel.hpp"
#include "World/ShapeItem.hpp"
#include "World/LightItem.hpp"

/*
 * Tests which integrate view of World tree with its WorldModel
 */
class test_WorldTreeView : public QObject
{
private slots:
    void initTestCase() {
        model = std::make_unique<WorldModel>();

        view = std::make_unique<QTreeView>();
        view->setModel(model.get());

        light1 = std::make_unique<LightItem>("point light");
        model->addItem(light1.get());

        cube = std::make_unique<ShapeItem>("cube 1");
        model->addItem(cube.get());

        light2 = std::make_unique<LightItem>("ambient light");
        model->addItem(light2.get());

        sphere1 = std::make_unique<ShapeItem>("sphere 1");
        model->addItem(sphere1.get());

        sphere2 = std::make_unique<ShapeItem>("sphere 2");
        model->addItem(sphere2.get());
    }

    void testTitles() {
        // titles of tree and item type bins are correct
        auto i_root = QModelIndex{ };

        auto i_lights = model->index(0, 0, i_root);
        QVERIFY(i_lights.isValid());
        QCOMPARE(view->model()->data(i_lights, Qt::DisplayRole).toString(), "Lights");

        auto i_shapes = model->index(1, 0, i_root);
        QVERIFY(i_shapes.isValid());
    }

private:
    std::unique_ptr<WorldModel> model;
    std::unique_ptr<QTreeView> view;
    std::unique_ptr<ShapeItem> cube, sphere1, sphere2;
    std::unique_ptr<LightItem> light1, light2;

private:
    Q_OBJECT
};

QTEST_MAIN(test_WorldTreeView)
#include "test_WorldTreeView.moc"
