# CrashExporter CMake 构建说明

## 项目结构

本项目已转换为 CMake 工程，支持跨平台构建（主要针对 Windows）。

## 前置要求

- CMake 3.15 或更高版本
- Windows 平台：Visual Studio 2010 或更高版本（需要 ATL/WTL 支持）
- 对于非 Windows 平台：可能需要额外的适配工作

## 快速开始

### Windows (Visual Studio)

```bash
# 创建构建目录
mkdir build
cd build

# 配置项目（生成 Visual Studio 解决方案）
cmake .. -G "Visual Studio 16 2019"

# 或者使用命令行构建
cmake --build . --config Release

# 安装（可选）
cmake --install . --prefix ../install
```

### Windows (MinGW)

```bash
mkdir build
cd build
cmake .. -G "MinGW Makefiles"
cmake --build . --config Release
```

### Linux/macOS (仅用于代码分析，无法实际运行)

```bash
mkdir build
cd build
cmake ..
cmake --build .
```

## CMake 选项

| 选项 | 默认值 | 说明 |
|------|--------|------|
| `BUILD_SHARED_LIBS` | ON | 构建 CrashRpt 为共享库（DLL），OFF 则为静态库 |
| `BUILD_TESTS` | OFF | 是否构建测试程序 |

### 使用示例

```bash
# 构建静态库版本
cmake .. -DBUILD_SHARED_LIBS=OFF

# 构建带测试的版本
cmake .. -DBUILD_TESTS=ON
```

## 输出文件

构建成功后，输出文件位于 `build/bin` 和 `build/lib` 目录：

- `bin/CrashRpt.dll` (或 `lib/CrashRpt.lib`) - CrashRpt 库
- `bin/CrashExporter.exe` - CrashExporter 可执行文件

## 依赖项

项目包含以下第三方依赖（已 bundled）：

- **zlib** - 压缩库（自动从 thirdparty/zlib 构建）
- **libpng** - PNG 图像处理（自动从 thirdparty/libpng 构建）
- **WTL** - Windows Template Library（头文件 only，位于 thirdparty/wtl）
- **dbghelp** - Windows 调试帮助库（系统库）

## 与原始项目的区别

1. 不再依赖 Visual Studio 项目文件（.vcxproj）
2. 支持跨平台构建配置
3. 自动处理第三方依赖的构建
4. 支持标准 CMake 安装流程

## 故障排除

### 找不到 ATL/WTL

确保已安装 Visual Studio 并包含 ATL 组件。

### 预编译头错误

CMake 会自动处理预编译头（stdafx.h）。如果遇到相关问题，尝试清理构建目录后重新构建。

### 链接错误

确保所有必要的 Windows SDK 库可用。在 Visual Studio 中，这些通常会自动配置。

## 许可证

请参考原项目的许可证文件。
