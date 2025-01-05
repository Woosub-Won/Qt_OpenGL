#ifndef OPENGLSHADERWINDOW_H
#define OPENGLSHADERWINDOW_H

#include <QOpenGLFunctions>
#include <QOpenGLPaintDevice>
#include <QOpenGLShaderProgram>
#include <QWindow>


class OpenGLShaderWindow : public QWindow, protected QOpenGLFunctions
{
    Q_OBJECT
public:
    explicit OpenGLShaderWindow(QWindow *parent = nullptr);
    ~OpenGLShaderWindow();

    void setAnimating(bool animating);
    virtual void initialize(const QString &vertexShaderPath, const QString &fragmentShaderPath);
    virtual void render();

protected:
    void exposeEvent(QExposeEvent *event) override;
    bool event(QEvent *event) override;

    virtual QString loadShaderSource(const QString &filePath);

private:
    void renderNow();
    void renderLater();

    QOpenGLContext *m_context = nullptr;
    QOpenGLPaintDevice *m_device = nullptr;
    QOpenGLShaderProgram *m_program = nullptr;
    bool m_animating = false;
};


#endif // OPENGLSHADERWINDOW_H
