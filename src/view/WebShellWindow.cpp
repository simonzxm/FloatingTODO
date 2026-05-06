#include "view/WebShellWindow.h"

#include <QCoreApplication>
#include <QDir>
#include <QEvent>
#include <QFileInfo>
#include <QGuiApplication>
#include <QKeyEvent>
#include <QLabel>
#include <QMouseEvent>
#include <QPixmap>
#include <QResizeEvent>
#include <QScreen>
#include <QShowEvent>
#include <QTimer>
#include <QVBoxLayout>
#include <QWebEnginePage>
#include <QWebEngineView>
#include <QWindow>

#ifdef Q_OS_WIN
#include <dwmapi.h>
#include <windows.h>

#ifndef DWMWA_WINDOW_CORNER_PREFERENCE
#define DWMWA_WINDOW_CORNER_PREFERENCE 33
#endif

#ifndef DWMWA_BORDER_COLOR
#define DWMWA_BORDER_COLOR 34
#endif

#ifndef DWMWA_COLOR_NONE
#define DWMWA_COLOR_NONE 0xFFFFFFFE
#endif

#ifndef DWMWCP_DONOTROUND
#define DWMWCP_DONOTROUND 1
#endif
#endif

namespace {
constexpr int kWindowWidth = 440;
constexpr int kWindowHeight = 520;
constexpr int kScreenMargin = 24;
constexpr int kDragInset = 0;
}

WebShellWindow::WebShellWindow(QWidget *parent)
    : QWidget(parent)
    , m_webView(new QWebEngineView(this))
    , m_dragOverlay(new QWidget(this))
    , m_dragSnapshot(new QLabel(this))
{
    setWindowTitle(QStringLiteral("FloatingTODO"));
    setWindowFlags(Qt::FramelessWindowHint | Qt::NoDropShadowWindowHint | Qt::Window);
    setAttribute(Qt::WA_TranslucentBackground);
    setAttribute(Qt::WA_NoSystemBackground);
    setAutoFillBackground(false);
    resize(kWindowWidth, kWindowHeight);

    auto *layout = new QVBoxLayout(this);
    layout->setContentsMargins(0, 0, 0, 0);
    layout->setSpacing(0);
    layout->addWidget(m_webView);

    m_webView->setAttribute(Qt::WA_TranslucentBackground);
    m_webView->setAttribute(Qt::WA_NoSystemBackground);
    m_webView->setAutoFillBackground(false);
    m_webView->setStyleSheet(QStringLiteral("background: transparent;"));
    m_webView->page()->setBackgroundColor(Qt::transparent);
    m_webView->installEventFilter(this);
    m_webView->load(resolveIndexUrl());

    m_dragSnapshot->setAttribute(Qt::WA_TranslucentBackground);
    m_dragSnapshot->setAutoFillBackground(false);
    m_dragSnapshot->setScaledContents(false);
    m_dragSnapshot->hide();

    m_dragOverlay->setAttribute(Qt::WA_TranslucentBackground);
    m_dragOverlay->setCursor(Qt::SizeAllCursor);
    m_dragOverlay->setStyleSheet(QStringLiteral("background: transparent;"));
    m_dragOverlay->installEventFilter(this);
    m_dragOverlay->hide();
}

bool WebShellWindow::eventFilter(QObject *watched, QEvent *event)
{
    if (watched == m_webView) {
        if (event->type() == QEvent::KeyPress) {
            keyPressEvent(static_cast<QKeyEvent *>(event));
        } else if (event->type() == QEvent::KeyRelease) {
            keyReleaseEvent(static_cast<QKeyEvent *>(event));
        }
        return QWidget::eventFilter(watched, event);
    }

    if (watched != m_dragOverlay) {
        return QWidget::eventFilter(watched, event);
    }

    switch (event->type()) {
    case QEvent::MouseButtonPress: {
        auto *mouseEvent = static_cast<QMouseEvent *>(event);
        if (mouseEvent->button() == Qt::LeftButton) {
            beginDragFreeze();
            if (!startNativeDrag()) {
                endDragFreeze();
            }
            return true;
        }
        break;
    }
    case QEvent::MouseButtonRelease: {
        auto *mouseEvent = static_cast<QMouseEvent *>(event);
        if (mouseEvent->button() == Qt::LeftButton) {
            endDragFreeze();
            return true;
        }
        break;
    }
    default:
        break;
    }

    return QWidget::eventFilter(watched, event);
}

void WebShellWindow::keyPressEvent(QKeyEvent *event)
{
    if (event->key() == Qt::Key_Alt && !event->isAutoRepeat()) {
        m_altDragActive = true;
        updateDragOverlayVisibility();
    }
    QWidget::keyPressEvent(event);
}

void WebShellWindow::keyReleaseEvent(QKeyEvent *event)
{
    if (event->key() == Qt::Key_Alt && !event->isAutoRepeat()) {
        m_altDragActive = false;
        finishDragFreezeWhenReleased();
        updateDragOverlayVisibility();
    }
    QWidget::keyReleaseEvent(event);
}

void WebShellWindow::resizeEvent(QResizeEvent *event)
{
    QWidget::resizeEvent(event);
    updateDragOverlayGeometry();
}

void WebShellWindow::showEvent(QShowEvent *event)
{
    QWidget::showEvent(event);
    suppressNativeFrameEffects();
    if (!m_positioned) {
        moveToTopRight();
        m_positioned = true;
    }
}

QUrl WebShellWindow::resolveIndexUrl() const
{
    const QString appLocalPath = QDir(QCoreApplication::applicationDirPath()).filePath("web/index.html");
    if (QFileInfo::exists(appLocalPath)) {
        return QUrl::fromLocalFile(appLocalPath);
    }

    const QString workingDirPath = QDir(QDir::currentPath()).filePath("web/index.html");
    return QUrl::fromLocalFile(workingDirPath);
}

void WebShellWindow::moveToTopRight()
{
    QScreen *targetScreen = screen();
    if (!targetScreen) {
        targetScreen = QGuiApplication::primaryScreen();
    }
    if (!targetScreen) {
        return;
    }

    const QRect area = targetScreen->availableGeometry();
    move(area.right() - width() - kScreenMargin + 1, area.top() + kScreenMargin);
}

void WebShellWindow::suppressNativeFrameEffects()
{
#ifdef Q_OS_WIN
    const HWND hwnd = reinterpret_cast<HWND>(winId());
    if (!hwnd) {
        return;
    }

    const DWMNCRENDERINGPOLICY renderingPolicy = DWMNCRP_DISABLED;
    DwmSetWindowAttribute(hwnd, DWMWA_NCRENDERING_POLICY, &renderingPolicy, sizeof(renderingPolicy));

    const int cornerPreference = DWMWCP_DONOTROUND;
    DwmSetWindowAttribute(hwnd, DWMWA_WINDOW_CORNER_PREFERENCE, &cornerPreference, sizeof(cornerPreference));

    const DWORD borderColor = DWMWA_COLOR_NONE;
    DwmSetWindowAttribute(hwnd, DWMWA_BORDER_COLOR, &borderColor, sizeof(borderColor));
#endif
}

void WebShellWindow::updateDragOverlayGeometry()
{
    if (!m_dragOverlay) {
        return;
    }

    const QRect webUiRect(kDragInset,
                          kDragInset,
                          qMax(0, width() - kDragInset * 2),
                          qMax(0, height() - kDragInset * 2));
    m_dragOverlay->setGeometry(webUiRect);
    if (m_dragSnapshot) {
        m_dragSnapshot->setGeometry(m_webView ? m_webView->geometry() : rect());
    }
    updateDragOverlayVisibility();
}

void WebShellWindow::updateDragOverlayVisibility()
{
    if (!m_dragOverlay) {
        return;
    }

    m_dragOverlay->setVisible(m_altDragActive);
    if (m_altDragActive) {
        m_dragOverlay->raise();
    }
}

bool WebShellWindow::startNativeDrag()
{
    QWindow *handle = windowHandle();
    return handle && handle->startSystemMove();
}

void WebShellWindow::beginDragFreeze()
{
    if (!m_webView || !m_dragSnapshot || m_dragFreezeActive) {
        return;
    }

    const QPixmap snapshot = m_webView->grab();
    if (snapshot.isNull()) {
        return;
    }

    m_dragFreezeActive = true;
    m_dragSnapshot->setPixmap(snapshot);
    m_dragSnapshot->setGeometry(m_webView->geometry());
    m_dragSnapshot->show();
    m_dragSnapshot->raise();
    m_dragOverlay->raise();
}

void WebShellWindow::endDragFreeze()
{
    if (!m_dragFreezeActive) {
        return;
    }

    m_dragFreezeActive = false;
    if (m_dragSnapshot) {
        m_dragSnapshot->clear();
        m_dragSnapshot->hide();
    }
    updateDragOverlayVisibility();
}

void WebShellWindow::finishDragFreezeWhenReleased()
{
    if (!m_dragFreezeActive) {
        return;
    }

    QTimer::singleShot(80, this, [this]() {
        if (!m_altDragActive) {
            endDragFreeze();
        }
    });
}
