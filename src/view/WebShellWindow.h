#pragma once

#include <QUrl>
#include <QWidget>

class QEvent;
class QKeyEvent;
class QLabel;
class QResizeEvent;
class QShowEvent;
class QWebEngineView;

class WebShellWindow : public QWidget
{
    Q_OBJECT

public:
    explicit WebShellWindow(QWidget *parent = nullptr);

protected:
    bool eventFilter(QObject *watched, QEvent *event) override;
    void keyPressEvent(QKeyEvent *event) override;
    void keyReleaseEvent(QKeyEvent *event) override;
    void resizeEvent(QResizeEvent *event) override;
    void showEvent(QShowEvent *event) override;

private:
    QUrl resolveIndexUrl() const;
    void moveToTopRight();
    void suppressNativeFrameEffects();
    void beginDragFreeze();
    void endDragFreeze();
    void finishDragFreezeWhenReleased();
    void updateDragOverlayGeometry();
    void updateDragOverlayVisibility();
    bool startNativeDrag();

    QWebEngineView *m_webView = nullptr;
    QWidget *m_dragOverlay = nullptr;
    QLabel *m_dragSnapshot = nullptr;
    bool m_altDragActive = false;
    bool m_dragFreezeActive = false;
    bool m_positioned = false;
};
