#pragma once

#include <QObject>
#include <QString>
#include <QList>
#include <algorithm>


class WorldItem : public QObject
{
public:
    explicit WorldItem(QObject* parent = nullptr);
    explicit WorldItem(const QString& name, QObject* parent = nullptr);
    virtual ~WorldItem() = default;

    /**
     *  @brief Get a display-ordered list of the item's children. The
     *  ordinary QObject .children() method does not allow ordering.
     */
    inline QList<WorldItem*> orderedChildren() const {
        QList<WorldItem*> ordered{ };
        for (const auto c: children()) {
            if (auto item = dynamic_cast<WorldItem*>(c))
                ordered.append(item);
        }
        const auto compareFn = [](const WorldItem* a, const WorldItem* b) {
            return a->order() < b->order();
        };
        std::sort(ordered.begin(), ordered.end(), compareFn);
        return ordered;
    }

public:
    inline QString name() const { return _name; }
    inline void setName(const QString& newName) {
        if (newName != _name) {
            _name = newName;
            emit nameChanged();
        }
    }
    inline size_t order() const { return _order; }
    inline void setOrder(size_t newOrder) {
        if (newOrder != _order) {
            _order = newOrder;
            emit orderChanged();
        }
    }

signals:
    void nameChanged();
    void orderChanged();

protected:
    QString _name{ };
    size_t _order{ 0 }; // order to display this item in

private:
    Q_PROPERTY(QString name READ name WRITE setName NOTIFY nameChanged)
    Q_PROPERTY(size_t order READ order WRITE setOrder NOTIFY orderChanged)
    Q_OBJECT
};
