#include <QOpenGLWidget>
#include <QOpenGLFunctions>
#include <QWidget>
#include <QSurfaceFormat>
#include <QDebug>

class MyWidget : public QOpenGLWidget{
    public:
    MyWidget(QWidget* parent = nullptr):QOpenGLWidget(parent){}
protected:
    void initializeGL() override{

    }
    void paintGL() override{

    }
};

void Chapt1_2(){

    QSurfaceFormat format;
    format.setVersion(4,0);
    format.setProfile(QSurfaceFormat::CoreProfile);
    MyWidget widget = MyWidget();
    widget.setFormat(format);
    if(widget.context()==nullptr){
        qDebug() << "컨텍스트 없음";
    }
    QOpenGLFunctions* funcs = widget.context()->functions();

    GLenum err = funcs->glGetError();
    if(err != GL_NO_ERROR){
        qWarning() << "OpenGL Error: " << funcs->glGetError();
    }
}
// 자꾸 컨텍스트가 없다는데 모르겠음
