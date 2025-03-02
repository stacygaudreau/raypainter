#include <QObject>
#include <QTest>
#include <QAbstractItemModelTester>
#include <memory>

#include "World/WorldModel.hpp"
#include "World/ShapeItem.hpp"
#include "World/LightItem.hpp"


class test_WorldModel : public QObject
{
private slots:
    void initTestCase() {
        model = std::make_unique<WorldModel>();

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

    void initialItemOrderInTree() {
        // different WorldItem types are placed into the correct bins and ordered
        // the way they were added to the tree initially
        QCOMPARE(model->rootLight->children().size(), 2U);
        QCOMPARE(model->rootShape->children().size(), 3U);
        auto shapes = model->rootShape->children();
        QCOMPARE(shapes.at(0), cube.get());
        QCOMPARE(shapes.at(1), sphere1.get());
        QCOMPARE(shapes.at(2), sphere2.get());
        auto lights = model->rootLight->children();
        QCOMPARE(lights.at(0), light1.get());
        QCOMPARE(lights.at(1), light2.get());
    }

    void testDropToReorderShapes() {
        // shape orders are all initially 0 and orderedChildren() is based on order added
        auto beforeReorder = model->rootShape->orderedChildren();
        QCOMPARE(cube->order(), 0);
        QCOMPARE(sphere1->order(), 0);
        QCOMPARE(sphere2->order(), 0);
        QCOMPARE(beforeReorder.at(0), cube.get());
        QCOMPARE(beforeReorder.at(1), sphere1.get());
        QCOMPARE(beforeReorder.at(2), sphere2.get());
        // drop sphere2 at top of shapes bin
        cube->setOrder(1);
        sphere1->setOrder(2);
        sphere2->setOrder(0);   // move to top
        // should now be reordered
        auto afterReorder = model->rootShape->orderedChildren();
        QCOMPARE(afterReorder.at(0), sphere2.get());
        QCOMPARE(afterReorder.at(1), cube.get());
        QCOMPARE(afterReorder.at(2), sphere1.get());
    }

    void testOrderedChildrenIsSorted() {
        // a correctly sorted list is returned from orderedChildren()
        auto sortedChildren = model->rootShape->orderedChildren();
        for (int i{ 1 }; i < sortedChildren.size(); ++i) {
            QVERIFY(sortedChildren.at(i - 1)->order() <= sortedChildren.at(i)->order());
        }
    }

    void testAbstractItemModelIntegrity() {
        // This helper class automatically runs a series of standard
        //  model unit tests and will report to QtTest any of the most common
        //  failures if for example the rowCount(), index() or any of the other
        //  required model methods are invalid
        QAbstractItemModelTester tester{ model.get() };
    }

private:
    std::unique_ptr<WorldModel> model;
    std::unique_ptr<ShapeItem> cube, sphere1, sphere2;
    std::unique_ptr<LightItem> light1, light2;

private:
    Q_OBJECT
};


QTEST_MAIN(test_WorldModel)
#include "test_WorldModel.moc"
