QT = core openglwidgets opengl \
    widgets

CONFIG += c++17 cmdline

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
        1-1-Introduction.cpp \
        main.cpp \
        qt_opengl_window_example_main.cpp \
        qt_opengl_window_example_openglwindow.cpp \
        qt_opengl_window_example_trianglewindow.cpp

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

HEADERS += \
    chapter1.h \
    qt_opengl_window_example_main.h \
    qt_opengl_window_example_openglwindow.h \
    qt_opengl_window_example_trianglewindow.h
