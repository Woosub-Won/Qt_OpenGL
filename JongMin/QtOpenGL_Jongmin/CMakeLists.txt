cmake_minimum_required(VERSION 3.19)
project(QtOpenGL_Jongmin LANGUAGES CXX)

find_package(Qt6 6.5 REQUIRED COMPONENTS Core Widgets)
find_package(Qt6 REQUIRED COMPONENTS OpenGLWidgets)
find_package(OpenGL REQUIRED)
find_package(Qt6 COMPONENTS Gui REQUIRED)

qt_standard_project_setup()

qt_add_executable(QtOpenGL_Jongmin
    WIN32 MACOSX_BUNDLE
    main.cpp
    mainwindow.cpp
    mainwindow.h
    mainwindow.ui
    Shaders/basic.vert
    ch1_7_sendingdatatoshader.h ch1_7_sendingdatatoshader.cpp
    ch1_8_gettingalist.h ch1_8_gettingalist.cpp
    Shaders/fragShader.frag
    ch1_9_sendingdatatoashader.h ch1_9_sendingdatatoashader.cpp
    ch1_10_usinguniformblocks.h ch1_10_usinguniformblocks.cpp
    ch2_3implementingpervertex.cpp ch2_3implementingpervertex.h
    Shaders/fragShader2_3.frag
    Shaders/vertShader2_3.vert
    ch2_2_implementingdiffuse.cpp ch2_2_implementingdiffuse.h
    ch3_7_creatingacartoonshading.h ch3_7_creatingacartoonshading.cpp
    ch3_7_creatingacartoonshading.cpp ch3_7_creatingacartoonshading.h
    Shaders/vertShader3_7.vert
    Shaders/fragShader3_7.frag
)

target_link_libraries(QtOpenGL_Jongmin
    PRIVATE
        Qt::Core
        Qt::Widgets
        Qt6::OpenGLWidgets
        OpenGL::GL
        Qt6::Gui
)

include(GNUInstallDirs)

install(TARGETS QtOpenGL_Jongmin
    BUNDLE  DESTINATION .
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
)

qt_generate_deploy_app_script(
    TARGET QtOpenGL_Jongmin
    OUTPUT_SCRIPT deploy_script
    NO_UNSUPPORTED_PLATFORM_ERROR
)
install(SCRIPT ${deploy_script})
