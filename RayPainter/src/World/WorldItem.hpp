#pragma once

#include <QObject>
#include <QString>


class WorldItem : public QObject
{
public:
    explicit WorldItem(QObject* parent = nullptr);
    explicit WorldItem(const QString& name, QObject* parent = nullptr);
    virtual ~WorldItem() = default;

public:
    inline QString name() const { return _name; }
    void setName(const QString& newName) {
        if (newName != _name) {
            _name = newName;
            emit nameChanged();
        }
    }

signals:
    void nameChanged();

protected:
    QString _name{ };

private:
    Q_PROPERTY(QString name READ name WRITE setName NOTIFY nameChanged)
    Q_OBJECT
};
