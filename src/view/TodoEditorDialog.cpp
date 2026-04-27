#include "view/TodoEditorDialog.h"

#include <QCheckBox>
#include <QDateTime>
#include <QDateTimeEdit>
#include <QDialogButtonBox>
#include <QFormLayout>
#include <QLineEdit>
#include <QMessageBox>
#include <QVBoxLayout>

TodoEditorDialog::TodoEditorDialog(QWidget *parent)
    : QDialog(parent)
{
    setupUi();
}

TodoEditorDialog::TodoEditorDialog(const TodoItem &item, QWidget *parent)
    : QDialog(parent)
{
    setupUi();
    fillFromItem(item);
}

QString TodoEditorDialog::title() const
{
    return m_titleEdit->text().trimmed();
}

QDateTime TodoEditorDialog::dueAt() const
{
    return m_dueEnabledCheck->isChecked() ? m_dueEdit->dateTime() : QDateTime();
}

void TodoEditorDialog::accept()
{
    if (title().isEmpty()) {
        QMessageBox::warning(this, QStringLiteral("FloatingTODO"), QStringLiteral("任务标题不能为空"));
        return;
    }
    QDialog::accept();
}

void TodoEditorDialog::setupUi()
{
    setWindowTitle(QStringLiteral("任务"));

    m_titleEdit = new QLineEdit(this);
    m_dueEnabledCheck = new QCheckBox(QStringLiteral("设置截止时间"), this);
    m_dueEdit = new QDateTimeEdit(QDateTime::currentDateTime(), this);
    m_dueEdit->setCalendarPopup(true);
    m_dueEdit->setDisplayFormat(QStringLiteral("yyyy-MM-dd HH:mm"));
    m_dueEdit->setEnabled(false);

    connect(m_dueEnabledCheck, &QCheckBox::toggled, m_dueEdit, &QDateTimeEdit::setEnabled);

    auto *form = new QFormLayout;
    form->addRow(QStringLiteral("标题"), m_titleEdit);
    form->addRow(QString(), m_dueEnabledCheck);
    form->addRow(QStringLiteral("截止时间"), m_dueEdit);

    m_buttons = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel, this);
    connect(m_buttons, &QDialogButtonBox::accepted, this, &TodoEditorDialog::accept);
    connect(m_buttons, &QDialogButtonBox::rejected, this, &TodoEditorDialog::reject);

    auto *layout = new QVBoxLayout(this);
    layout->addLayout(form);
    layout->addWidget(m_buttons);
}

void TodoEditorDialog::fillFromItem(const TodoItem &item)
{
    m_titleEdit->setText(item.title);
    if (item.dueAt.isValid()) {
        m_dueEnabledCheck->setChecked(true);
        m_dueEdit->setDateTime(item.dueAt);
    }
}
