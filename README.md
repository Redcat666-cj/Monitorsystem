# 智能监控系统 (Smart Monitor System)

基于 **Qt 5.14.2 + OpenCV 4.12.0** 的实时智能监控系统，通过摄像头采集视频画面，自动检测运动目标，持续跟踪并分配唯一 ID，满足报警条件时自动截图、记录日志并弹窗通知。

## 功能特性

| 功能 | 说明 |
|------|------|
| 实时视频采集 | 支持 USB 摄像头和视频文件输入 |
| 运动目标检测 | 帧差法 / 背景建模法 (MOG2) 可切换 |
| 目标持续跟踪 | 质心匹配算法，自动分配唯一 ID |
| 智能报警 | 停留超时报警 + 面积超阈值报警 |
| 报警截图 | 自动保存报警时刻画面至 `snapshots/` |
| 报警日志 | CSV 格式日志记录，界面内可查看历史 |
| 参数可调 | 检测灵敏度、最小面积、报警阈值等均可实时调节 |

## 系统架构

```
摄像头/视频文件
    ↓
VideoCaptureModule (视频采集，QTimer 定时读取)
    ↓ cv::Mat
MotionDetector (运动检测，帧差法/MOG2)
    ↓ vector<cv::Rect>
ObjectTracker (目标跟踪，质心距离匹配)
    ↓ vector<TrackedObject>
AlarmManager (报警判断，截图+日志)
    ↓
MainWindow (Qt GUI 显示 + 弹窗通知)
```

## 技术栈

| 组件 | 版本 | 用途 |
|------|------|------|
| OpenCV | 4.12.0 | 视频采集、图像处理、运动检测 |
| Qt | 5.14.2 | GUI 界面、信号槽、定时器 |
| C++ | C++14 | 编程语言 |
| CMake | 3.8+ | 构建系统 |
| MSVC | 2017/2022 | 编译器 |

## 环境配置

### 前置要求

- Windows 10/11
- Visual Studio 2022（含 MSVC 编译器）
- CMake 3.8+
- OpenCV 4.12.0（预编译版本）
- Qt 5.14.2（msvc2017_64）

### 环境变量

```
OpenCV_DIR = D:\kkk\opencv\build

PATH 追加:
  D:\kkk\opencv\build\x64\vc16\bin
  D:\Qt\5.14.2\msvc2017_64\bin
```

> 如果你的 OpenCV / Qt 安装路径不同，请修改 `CMakeLists.txt` 中对应的路径配置。

## 构建与运行

```powershell
# 1. 克隆仓库
git clone https://github.com/Redcat666-cj/Monitorsystem.git
cd Monitorsystem

# 2. 创建构建目录
mkdir build
cd build

# 3. CMake 配置
cmake .. -G "Visual Studio 17 2022" -A x64

# 4. 编译 (Release 模式)
cmake --build . --config Release

# 5. 部署 Qt DLL
D:\Qt\5.14.2\msvc2017_64\bin\windeployqt.exe .\Release\SmartMonitorSystem.exe

# 6. 运行
.\Release\SmartMonitorSystem.exe
```

## 使用说明

1. 启动程序后，点击 **开始监控** 按钮打开摄像头
2. 右侧控制面板可调节参数：
   - **检测方法**：帧差法（快速）或 背景建模法 MOG2（精确）
   - **二值化阈值**：值越小越灵敏，越大越迟钝
   - **最小目标面积**：过滤小噪点
   - **停留报警阈值**：目标持续出现多少帧后触发报警
   - **面积报警阈值**：目标面积超过多少像素时触发报警
3. 检测到运动目标后，画面上会显示绿色跟踪框和目标 ID
4. 触发报警时自动截图保存、弹窗通知，并记录到报警日志
5. 双击报警日志条目可查看对应截图
6. 点击 **清除报警日志** 可清空历史记录

## 项目结构

```
SmartMonitorSystem/
├── CMakeLists.txt              # 构建配置
├── main.cpp                    # 程序入口
├── MainWindow.h / .cpp / .ui   # 主窗口（界面 + 逻辑）
├── VideoCaptureModule.h / .cpp # 视频采集模块
├── MotionDetector.h / .cpp     # 运动检测模块
├── ObjectTracker.h / .cpp      # 目标跟踪模块
├── AlarmManager.h / .cpp       # 报警管理模块
├── Mat2QImage.h                # cv::Mat → QImage 转换
├── .gitignore                  # Git 忽略规则
└── README.md                   # 项目说明
```

## 常见问题

| 问题 | 解决方案 |
|------|---------|
| 摄像头无法打开 | 关闭占用摄像头的其他程序，或尝试修改 `openCamera(1)` |
| 找不到 opencv2/opencv.hpp | 检查 `CMakeLists.txt` 中 `OpenCV_DIR` 路径 |
| 找不到 Qt5Widgets | 检查 `CMAKE_PREFIX_PATH` 是否指向 Qt 安装路径 |
| 中文乱码 | 已在 CMake 中添加 `/utf-8` 编译选项 |
| LNK4006 警告 | OpenCV 4.12 + 新版 MSVC 的已知兼容性问题，不影响运行 |
| 界面卡顿 | 降低帧率 `setFps(15)` 或增大最小面积过滤值 |

## License

本项目仅用于学习交流。
