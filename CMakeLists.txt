cmake_minimum_required(VERSION 3.16)
project(DepenceClone VERSION 1.0.0 LANGUAGES CXX)

# 设置 C++ 标准（Qt6 需要 C++17）
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# 自动处理 Qt 的 moc, uic, rcc，省心省力
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_INCLUDE_CURRENT_DIR ON) # 方便包含当前目录

# 重要：告诉 CMake 去哪里找 Qt6
# 请将此路径改为你自己的 Qt 安装路径，例如：C:/Qt/6.8.3/msvc2022_64
set(CMAKE_PREFIX_PATH "F:/Qt/6.8.3/msvc2022_64") 

# 查找 Qt6 的 Widgets 模块
find_package(Qt6 REQUIRED COMPONENTS Widgets)

# 添加可执行文件
qt_add_executable(DepenceClone
    src/main.cpp
    src/MainWindow.cpp
    src/MainWindow.h
)

# 链接 Qt Widgets 库
target_link_libraries(DepenceClone PRIVATE Qt6::Widgets)

# 在 Windows 上，隐藏控制台窗口
set_target_properties(DepenceClone PROPERTIES
    WIN32_EXECUTABLE ON
)