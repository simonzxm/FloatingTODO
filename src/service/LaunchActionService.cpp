#include "service/LaunchActionService.h"

#include <QDesktopServices>
#include <QProcess>
#include <QUrl>

LaunchResult LaunchActionService::launch(const LaunchAction &action) const
{
    if (action.type == LaunchActionType::None) {
        return {false, QStringLiteral("该任务没有设置启动动作")};
    }

    const QString target = action.target.trimmed();
    if (target.isEmpty()) {
        return {false, QStringLiteral("启动目标为空")};
    }

    if (action.type == LaunchActionType::Application) {
        const bool started = QProcess::startDetached(target);
        return started
            ? LaunchResult{true, QStringLiteral("已启动应用程序")}
            : LaunchResult{false, QStringLiteral("无法启动应用程序")};
    }

    return openWithDesktopServices(target);
}

LaunchResult LaunchActionService::openWithDesktopServices(const QString &target) const
{
    const QUrl url = QUrl::fromUserInput(target);
    const bool opened = QDesktopServices::openUrl(url);
    return opened
        ? LaunchResult{true, QStringLiteral("已打开启动目标")}
        : LaunchResult{false, QStringLiteral("无法打开启动目标")};
}
