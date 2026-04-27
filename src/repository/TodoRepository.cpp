#include "repository/TodoRepository.h"

#include <QDateTime>
#include <QSqlError>
#include <QSqlQuery>
#include <QUuid>
#include <QVariant>

TodoRepository::TodoRepository(QString databasePath, QString connectionName)
{
    m_connectionName = connectionName.isEmpty()
        ? QString("floatingtodo_%1").arg(QUuid::createUuid().toString(QUuid::Id128))
        : connectionName;
    m_db = QSqlDatabase::addDatabase("QSQLITE", m_connectionName);
    m_db.setDatabaseName(databasePath);
}

TodoRepository::~TodoRepository()
{
    if (m_db.isOpen()) {
        m_db.close();
    }
}

bool TodoRepository::initialize()
{
    if (!m_db.open()) {
        return false;
    }

    QSqlQuery query(m_db);
    return query.exec(
        "CREATE TABLE IF NOT EXISTS todos ("
        "id INTEGER PRIMARY KEY AUTOINCREMENT,"
        "parent_id INTEGER NULL,"
        "title TEXT NOT NULL,"
        "due_at TEXT NULL,"
        "completed INTEGER NOT NULL DEFAULT 0,"
        "launch_type TEXT NOT NULL DEFAULT 'none',"
        "launch_target TEXT,"
        "launch_display_name TEXT,"
        "created_at TEXT NOT NULL,"
        "updated_at TEXT NOT NULL"
        ")"
    );
}

QVector<TodoItem> TodoRepository::findAll() const
{
    QVector<TodoItem> items;
    QSqlQuery query(m_db);
    query.exec("SELECT * FROM todos ORDER BY id ASC");
    while (query.next()) {
        items.push_back(itemFromQuery(query));
    }
    return items;
}

std::optional<TodoItem> TodoRepository::findById(int id) const
{
    QSqlQuery query(m_db);
    query.prepare("SELECT * FROM todos WHERE id = ?");
    query.addBindValue(id);
    if (!query.exec() || !query.next()) {
        return std::nullopt;
    }
    return itemFromQuery(query);
}

int TodoRepository::add(const TodoItem &item)
{
    const auto now = QDateTime::currentDateTimeUtc();
    QSqlQuery query(m_db);
    query.prepare(
        "INSERT INTO todos "
        "(parent_id, title, due_at, completed, launch_type, launch_target, launch_display_name, created_at, updated_at) "
        "VALUES (?, ?, ?, ?, ?, ?, ?, ?, ?)"
    );
    query.addBindValue(item.parentId >= 0 ? QVariant(item.parentId) : QVariant());
    query.addBindValue(item.title);
    query.addBindValue(item.dueAt.isValid() ? QVariant(item.dueAt.toUTC().toString(Qt::ISODate)) : QVariant());
    query.addBindValue(item.completed ? 1 : 0);
    query.addBindValue(typeToString(item.launchAction.type));
    query.addBindValue(item.launchAction.target);
    query.addBindValue(item.launchAction.displayName);
    query.addBindValue(now.toString(Qt::ISODate));
    query.addBindValue(now.toString(Qt::ISODate));
    if (!query.exec()) {
        return -1;
    }
    return query.lastInsertId().toInt();
}

bool TodoRepository::update(const TodoItem &item)
{
    QSqlQuery query(m_db);
    query.prepare(
        "UPDATE todos SET parent_id = ?, title = ?, due_at = ?, completed = ?, "
        "launch_type = ?, launch_target = ?, launch_display_name = ?, updated_at = ? WHERE id = ?"
    );
    query.addBindValue(item.parentId >= 0 ? QVariant(item.parentId) : QVariant());
    query.addBindValue(item.title);
    query.addBindValue(item.dueAt.isValid() ? QVariant(item.dueAt.toUTC().toString(Qt::ISODate)) : QVariant());
    query.addBindValue(item.completed ? 1 : 0);
    query.addBindValue(typeToString(item.launchAction.type));
    query.addBindValue(item.launchAction.target);
    query.addBindValue(item.launchAction.displayName);
    query.addBindValue(QDateTime::currentDateTimeUtc().toString(Qt::ISODate));
    query.addBindValue(item.id);
    return query.exec();
}

bool TodoRepository::remove(int id)
{
    QSqlQuery query(m_db);
    query.prepare("DELETE FROM todos WHERE id = ?");
    query.addBindValue(id);
    return query.exec();
}

QVector<TodoItem> TodoRepository::childrenOf(int parentId) const
{
    QVector<TodoItem> items;
    QSqlQuery query(m_db);
    query.prepare("SELECT * FROM todos WHERE parent_id = ? ORDER BY id ASC");
    query.addBindValue(parentId);
    query.exec();
    while (query.next()) {
        items.push_back(itemFromQuery(query));
    }
    return items;
}

bool TodoRepository::removeSubtree(int id)
{
    for (const auto &child : childrenOf(id)) {
        if (!removeSubtree(child.id)) {
            return false;
        }
    }
    return remove(id);
}

TodoItem TodoRepository::itemFromQuery(const QSqlQuery &query) const
{
    TodoItem item;
    item.id = query.value("id").toInt();
    item.parentId = query.value("parent_id").isNull() ? -1 : query.value("parent_id").toInt();
    item.title = query.value("title").toString();
    item.dueAt = QDateTime::fromString(query.value("due_at").toString(), Qt::ISODate);
    item.completed = query.value("completed").toInt() != 0;
    item.launchAction.type = typeFromString(query.value("launch_type").toString());
    item.launchAction.target = query.value("launch_target").toString();
    item.launchAction.displayName = query.value("launch_display_name").toString();
    item.createdAt = QDateTime::fromString(query.value("created_at").toString(), Qt::ISODate);
    item.updatedAt = QDateTime::fromString(query.value("updated_at").toString(), Qt::ISODate);
    return item;
}

QString TodoRepository::typeToString(LaunchActionType type) const
{
    switch (type) {
    case LaunchActionType::Url:
        return "url";
    case LaunchActionType::File:
        return "file";
    case LaunchActionType::Folder:
        return "folder";
    case LaunchActionType::Application:
        return "application";
    case LaunchActionType::None:
        return "none";
    }
    return "none";
}

LaunchActionType TodoRepository::typeFromString(const QString &value) const
{
    if (value == "url") {
        return LaunchActionType::Url;
    }
    if (value == "file") {
        return LaunchActionType::File;
    }
    if (value == "folder") {
        return LaunchActionType::Folder;
    }
    if (value == "application") {
        return LaunchActionType::Application;
    }
    return LaunchActionType::None;
}
