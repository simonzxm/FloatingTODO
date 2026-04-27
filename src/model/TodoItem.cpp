#include "model/TodoItem.h"

bool TodoItem::hasParent() const
{
    return parentId >= 0;
}

bool TodoItem::hasChildren() const
{
    return !children.isEmpty();
}
