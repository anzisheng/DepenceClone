cmake_minimum_required(VERSION 3.16)
project(DepenceClone VERSION 1.0.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_INCLUDE_CURRENT_DIR ON)

# 请替换为你的 Qt 路径
set(CMAKE_PREFIX_PATH "C:/Qt/6.8.3/msvc2022_64")

# 添加 OpenGL 支持
find_package(Qt6 REQUIRED COMPONENTS Widgets Core Gui OpenGL OpenGLWidgets)

qt_add_executable(DepenceClone
    src/main.cpp
    src/MainWindow.cpp
    src/MainWindow.h
    src/opengl/WaterCurtainWidget.cpp
    src/opengl/WaterCurtainWidget.h
    src/core/IComponent.h
    src/core/IObserver.h
    src/core/ISerializable.h
    src/core/Singleton.h
    src/models/FixtureModel.h
    src/models/GroupModel.h
    src/utils/ScreenAdapter.h
)

target_link_libraries(DepenceClone PRIVATE 
    Qt6::Widgets 
    Qt6::Core 
    Qt6::OpenGL 
    Qt6::OpenGLWidgets
)

# Windows 下链接 OpenGL 库
if(WIN32)
    target_link_libraries(DepenceClone PRIVATE opengl32)
endif()

set_target_properties(DepenceClone PROPERTIES
    WIN32_EXECUTABLE ON
)

target_include_directories(DepenceClone PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/src
)