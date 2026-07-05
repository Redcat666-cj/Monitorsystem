cmake_minimum_required(VERSION 3.8)
project(SmartMonitorSystem)

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# ===== Qt 配置 =====
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTORCC ON)

# 设置 Qt 路径（根据实际安装路径修改）
set(CMAKE_PREFIX_PATH "D:/Qt/5.14.2/msvc2017_64")

find_package(Qt5 REQUIRED COMPONENTS
    Widgets
    Core
    Gui
)

# ===== OpenCV 配置 =====
set(OpenCV_DIR "D:/kkk/opencv/build")
find_package(OpenCV REQUIRED)

# ===== 源文件 =====
set(SOURCES
    main.cpp
    MainWindow.cpp
    VideoCaptureModule.cpp
    MotionDetector.cpp
    ObjectTracker.cpp
    AlarmManager.cpp
)

set(HEADERS
    MainWindow.h
    VideoCaptureModule.h
    MotionDetector.h
    ObjectTracker.h
    AlarmManager.h
    Mat2QImage.h
)

set(FORMS
    MainWindow.ui
)

# ===== 可执行文件 =====
add_executable(${PROJECT_NAME} ${SOURCES} ${HEADERS} ${FORMS})

# 链接库
target_link_libraries(${PROJECT_NAME}
    Qt5::Widgets
    Qt5::Core
    Qt5::Gui
    ${OpenCV_LIBS}
)

# MSVC 编译选项
if(MSVC)
    target_compile_options(${PROJECT_NAME} PRIVATE /utf-8)
    target_compile_definitions(${PROJECT_NAME} PRIVATE _CRT_SECURE_NO_WARNINGS)
    set_target_properties(${PROJECT_NAME} PROPERTIES LINK_FLAGS "/FORCE:MULTIPLE")
endif()

# 复制 OpenCV DLL 到输出目录
add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
    "D:/kkk/opencv/build/x64/vc16/bin/opencv_world4120.dll"
    $<TARGET_FILE_DIR:${PROJECT_NAME}>
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
    "D:/kkk/opencv/build/x64/vc16/bin/opencv_world4120d.dll"
    $<TARGET_FILE_DIR:${PROJECT_NAME}>
)
