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

    if (!migrateLegacySchemaIfNeeded()) {
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
        "sort_order INTEGER NOT NULL DEFAULT 0,"
        "created_at TEXT NOT NULL,"
        "updated_at TEXT NOT NULL"
        ")"
    );
}

QVector<TodoItem> TodoRepository::findAll() const
{
    QVector<TodoItem> items;
    QSqlQuery query(m_db);
    query.exec("SELECT * FROM todos ORDER BY parent_id IS NOT NULL, parent_id ASC, sort_order ASC, id ASC");
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
        "(parent_id, title, due_at, completed, sort_order, created_at, updated_at) "
        "VALUES (?, ?, ?, ?, ?, ?, ?)"
    );
    query.addBindValue(item.parentId >= 0 ? QVariant(item.parentId) : QVariant());
    query.addBindValue(item.title);
    query.addBindValue(item.dueAt.isValid() ? QVariant(item.dueAt.toUTC().toString(Qt::ISODate)) : QVariant());
    query.addBindValue(item.completed ? 1 : 0);
    query.addBindValue(item.sortOrder);
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
        "UPDATE todos SET parent_id = ?, title = ?, due_at = ?, completed = ?, sort_order = ?, "
        "updated_at = ? WHERE id = ?"
    );
    query.addBindValue(item.parentId >= 0 ? QVariant(item.parentId) : QVariant());
    query.addBindValue(item.title);
    query.addBindValue(item.dueAt.isValid() ? QVariant(item.dueAt.toUTC().toString(Qt::ISODate)) : QVariant());
    query.addBindValue(item.completed ? 1 : 0);
    query.addBindValue(item.sortOrder);
    query.addBindValue(QDateTime::currentDateTimeUtc().toString(Qt::ISODate));
    query.addBindValue(item.id);
    return query.exec();
}

bool TodoRepository::updateParentAndOrder(int id, int parentId, int sortOrder)
{
    QSqlQuery query(m_db);
    query.prepare(
        "UPDATE todos SET parent_id = ?, sort_order = ?, updated_at = ? WHERE id = ?"
    );
    query.addBindValue(parentId >= 0 ? QVariant(parentId) : QVariant());
    query.addBindValue(sortOrder);
    query.addBindValue(QDateTime::currentDateTimeUtc().toString(Qt::ISODate));
    query.addBindValue(id);
    return query.exec();
}

bool TodoRepository::remove(int id)
{
    QSqlQuery query(m_db);
    query.prepare("DELETE FROM todos WHERE id = ?");
    query.addBindValue(id);
    return query.exec();
}

bool TodoRepository::removeAll()
{
    QSqlQuery query(m_db);
    return query.exec("DELETE FROM todos");
}

QVector<TodoItem> TodoRepository::childrenOf(int parentId) const
{
    QVector<TodoItem> items;
    QSqlQuery query(m_db);
    query.prepare("SELECT * FROM todos WHERE parent_id = ? ORDER BY sort_order ASC, id ASC");
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

bool TodoRepository::migrateLegacySchemaIfNeeded()
{
    QSqlQuery columnsQuery(m_db);
    if (!columnsQuery.exec("PRAGMA table_info(todos)")) {
        return false;
    }

    QStringList columns;
    while (columnsQuery.next()) {
        columns.push_back(columnsQuery.value("name").toString());
    }

    if (columns.isEmpty()
        || (!columns.contains("launch_type")
            && !columns.contains("launch_target")
            && !columns.contains("launch_display_name")
            && columns.contains("sort_order"))) {
        return true;
    }

    QSqlQuery query(m_db);
    if (!query.exec("BEGIN TRANSACTION")) {
        return false;
    }
    const bool ok =
        query.exec(
            "CREATE TABLE todos_new ("
            "id INTEGER PRIMARY KEY AUTOINCREMENT,"
            "parent_id INTEGER NULL,"
            "title TEXT NOT NULL,"
            "due_at TEXT NULL,"
            "completed INTEGER NOT NULL DEFAULT 0,"
            "sort_order INTEGER NOT NULL DEFAULT 0,"
            "created_at TEXT NOT NULL,"
            "updated_at TEXT NOT NULL"
            ")"
        )
        && query.exec(
            QString("INSERT INTO todos_new (id, parent_id, title, due_at, completed, sort_order, created_at, updated_at) "
                    "SELECT id, parent_id, title, due_at, completed, %1, created_at, updated_at FROM todos")
                .arg(columns.contains("sort_order") ? QStringLiteral("sort_order") : QStringLiteral("id"))
        )
        && query.exec("DROP TABLE todos")
        && query.exec("ALTER TABLE todos_new RENAME TO todos");

    if (ok) {
        return query.exec("COMMIT");
    }
    query.exec("ROLLBACK");
    return false;
}

TodoItem TodoRepository::itemFromQuery(const QSqlQuery &query) const
{
    TodoItem item;
    item.id = query.value("id").toInt();
    item.parentId = query.value("parent_id").isNull() ? -1 : query.value("parent_id").toInt();
    item.title = query.value("title").toString();
    item.dueAt = QDateTime::fromString(query.value("due_at").toString(), Qt::ISODate);
    item.completed = query.value("completed").toInt() != 0;
    item.sortOrder = query.value("sort_order").toInt();
    item.createdAt = QDateTime::fromString(query.value("created_at").toString(), Qt::ISODate);
    item.updatedAt = QDateTime::fromString(query.value("updated_at").toString(), Qt::ISODate);
    return item;
}
