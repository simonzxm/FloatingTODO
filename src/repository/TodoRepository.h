#pragma once

#include "model/TodoItem.h"

#include <QSqlDatabase>
#include <QSqlQuery>
#include <QString>
#include <QVector>
#include <optional>

class TodoRepository
{
public:
    explicit TodoRepository(QString databasePath, QString connectionName = QString());
    ~TodoRepository();

    bool initialize();
    QVector<TodoItem> findAll() const;
    std::optional<TodoItem> findById(int id) const;
    int add(const TodoItem &item);
    bool update(const TodoItem &item);
    bool remove(int id);
    QVector<TodoItem> childrenOf(int parentId) const;
    bool removeSubtree(int id);

private:
    TodoItem itemFromQuery(const QSqlQuery &query) const;
    QString typeToString(LaunchActionType type) const;
    LaunchActionType typeFromString(const QString &value) const;

    QString m_connectionName;
    QSqlDatabase m_db;
};
