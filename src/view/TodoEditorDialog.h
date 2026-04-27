#pragma once

#include "model/TodoItem.h"

#include <QDialog>

class QCheckBox;
class QComboBox;
class QDateTimeEdit;
class QDialogButtonBox;
class QLineEdit;

class TodoEditorDialog : public QDialog
{
    Q_OBJECT

public:
    explicit TodoEditorDialog(QWidget *parent = nullptr);
    explicit TodoEditorDialog(const TodoItem &item, QWidget *parent = nullptr);

    QString title() const;
    QDateTime dueAt() const;
    LaunchAction launchAction() const;

private slots:
    void accept() override;

private:
    void setupUi();
    void fillFromItem(const TodoItem &item);
    LaunchActionType currentLaunchType() const;
    void setCurrentLaunchType(LaunchActionType type);

    QLineEdit *m_titleEdit = nullptr;
    QCheckBox *m_dueEnabledCheck = nullptr;
    QDateTimeEdit *m_dueEdit = nullptr;
    QComboBox *m_launchTypeCombo = nullptr;
    QLineEdit *m_launchTargetEdit = nullptr;
    QLineEdit *m_launchDisplayNameEdit = nullptr;
    QDialogButtonBox *m_buttons = nullptr;
};
