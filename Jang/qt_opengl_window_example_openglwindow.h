#ifndef QT_OPENGL_WINDOW_EXAMPLE_OPENGLWINDOW_H
#define QT_OPENGL_WINDOW_EXAMPLE_OPENGLWINDOW_H

#include <QOpenGLFunctions>
#include <QOpenGLPaintDevice>
#include <QWindow>
#include <QPainter>

class OpenGLWindow : public QWindow, protected QOpenGLFunctions
{
    Q_OBJECT
public:
    explicit OpenGLWindow(QWindow *parent = nullptr);
    ~OpenGLWindow();

    virtual void render(QPainter *painter);
    virtual void render();

    virtual void initialize();

    void setAnimating(bool animating);

public slots:
    void renderLater();
    void renderNow();

protected:
    bool event(QEvent *event) override;

    void exposeEvent(QExposeEvent *event) override;

private:
    bool m_animating = false;

    QOpenGLContext *m_context = nullptr;
    QOpenGLPaintDevice *m_device = nullptr;
};

#endif // QT_OPENGL_WINDOW_EXAMPLE_OPENGLWINDOW_H
