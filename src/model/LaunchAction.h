#pragma once

#include <QString>

enum class LaunchActionType {
    None,
    Url,
    File,
    Folder,
    Application
};

struct LaunchAction {
    LaunchActionType type = LaunchActionType::None;
    QString target;
    QString displayName;

    QString buttonText() const;
};
