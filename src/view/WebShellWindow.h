#pragma once

#include <QString>
#include <QUrl>
#include <QWidget>

class QWebChannel;
class QEvent;
class QCloseEvent;
class QKeyEvent;
class QLabel;
class QResizeEvent;
class QShowEvent;
class QWebEngineView;
class TodoController;
class TodoRepository;
class TodoService;
class WebTodoBridge;

class WebShellWindow : public QWidget
{
    Q_OBJECT

public:
    explicit WebShellWindow(QWidget *parent = nullptr);
    ~WebShellWindow() override;

protected:
    bool eventFilter(QObject *watched, QEvent *event) override;
    void closeEvent(QCloseEvent *event) override;
    void keyPressEvent(QKeyEvent *event) override;
    void keyReleaseEvent(QKeyEvent *event) override;
    void resizeEvent(QResizeEvent *event) override;
    void showEvent(QShowEvent *event) override;

private:
    QUrl resolveIndexUrl() const;
    QString resolveDatabasePath() const;
    void setupTodoBackend();
    void moveToTopRight();
    void suppressNativeFrameEffects();
    void beginDragFreeze();
    void endDragFreeze();
    void finishDragFreezeWhenReleased();
    void updateDragOverlayGeometry();
    void updateDragOverlayVisibility();
    bool startNativeDrag();

    QWebEngineView *m_webView = nullptr;
    QWebChannel *m_webChannel = nullptr;
    TodoRepository *m_repository = nullptr;
    TodoService *m_service = nullptr;
    TodoController *m_controller = nullptr;
    WebTodoBridge *m_bridge = nullptr;
    QWidget *m_dragOverlay = nullptr;
    QLabel *m_dragSnapshot = nullptr;
    bool m_altDragActive = false;
    bool m_dragFreezeActive = false;
    bool m_closeAfterSnapshot = false;
    bool m_positioned = false;
};
