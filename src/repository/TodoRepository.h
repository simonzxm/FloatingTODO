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
    bool updateParentAndOrder(int id, int parentId, int sortOrder);
    bool remove(int id);
    bool removeAll();
    QVector<TodoItem> childrenOf(int parentId) const;
    bool removeSubtree(int id);

private:
    bool migrateLegacySchemaIfNeeded();
    TodoItem itemFromQuery(const QSqlQuery &query) const;

    QString m_connectionName;
    QSqlDatabase m_db;
};
