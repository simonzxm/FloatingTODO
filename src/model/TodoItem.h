#pragma once

#include "model/LaunchAction.h"

#include <QDateTime>
#include <QString>
#include <QVector>

struct TodoItem {
    int id = -1;
    int parentId = -1;
    QString title;
    QDateTime dueAt;
    bool completed = false;
    LaunchAction launchAction;
    QDateTime createdAt;
    QDateTime updatedAt;
    QVector<TodoItem> children;

    bool hasParent() const;
    bool hasChildren() const;
};
