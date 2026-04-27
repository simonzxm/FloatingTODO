#pragma once

#include "model/LaunchAction.h"

#include <QString>

struct LaunchResult {
    bool success = false;
    QString message;
};

class LaunchActionService
{
public:
    LaunchResult launch(const LaunchAction &action) const;

private:
    LaunchResult openWithDesktopServices(const QString &target) const;
};
