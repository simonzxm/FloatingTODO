#include "view/TodoItemWidget.h"

#include <QHBoxLayout>
#include <QLabel>
#include <QPushButton>

TodoItemWidget::TodoItemWidget(const TodoItem &item, const ChildStats &stats, int depth, QWidget *parent)
    : QWidget(parent)
    , m_id(item.id)
{
    auto *layout = new QHBoxLayout(this);
    layout->setContentsMargins(depth * 24, 4, 4, 4);
    layout->setSpacing(8);

    const QString mark = item.completed ? QStringLiteral("✓") : QStringLiteral("□");
    auto *stateLabel = new QLabel(mark, this);
    stateLabel->setMinimumWidth(20);

    auto *titleLabel = new QLabel(item.title, this);
    titleLabel->setMinimumWidth(180);

    const QString dueText = item.dueAt.isValid()
        ? item.dueAt.toString(QStringLiteral("yyyy-MM-dd HH:mm"))
        : QStringLiteral("无截止时间");
    auto *dueLabel = new QLabel(dueText, this);
    dueLabel->setMinimumWidth(140);

    auto *statsLabel = new QLabel(this);
    if (stats.total > 0) {
        statsLabel->setText(QStringLiteral("子任务：%1/%2").arg(stats.completed).arg(stats.total));
    }
    statsLabel->setMinimumWidth(90);

    auto *toggleButton = new QPushButton(item.completed ? QStringLiteral("取消完成") : QStringLiteral("完成"), this);
    auto *addChildButton = new QPushButton(QStringLiteral("新增子任务"), this);
    auto *editButton = new QPushButton(QStringLiteral("编辑"), this);
    auto *deleteButton = new QPushButton(QStringLiteral("删除"), this);

    connect(toggleButton, &QPushButton::clicked, this, [this]() { emit toggleRequested(m_id); });
    connect(addChildButton, &QPushButton::clicked, this, [this]() { emit addChildRequested(m_id); });
    connect(editButton, &QPushButton::clicked, this, [this]() { emit editRequested(m_id); });
    connect(deleteButton, &QPushButton::clicked, this, [this]() { emit deleteRequested(m_id); });

    layout->addWidget(stateLabel);
    layout->addWidget(titleLabel, 1);
    layout->addWidget(dueLabel);
    layout->addWidget(statsLabel);
    layout->addWidget(toggleButton);
    layout->addWidget(addChildButton);
    layout->addWidget(editButton);
    layout->addWidget(deleteButton);
}
