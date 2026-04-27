#include "model/LaunchAction.h"

QString LaunchAction::buttonText() const
{
    return displayName.trimmed().isEmpty() ? QStringLiteral("启动") : displayName;
}
