# Personal Coding Standards

本文件是个人编码规范，适用于所有项目。任何 agent 或开发者在编写代码时都应遵循此规范。

---

# 第一部分：C++

## 一、命名规范

### 1.1 文件命名

| 规则 | 示例 |
|---|---|
| 全小写 + 下划线分隔 | `render_driver.h`, `dx12_device.cpp` |
| 头文件扩展名 | `.h` |
| 实现文件扩展名 | `.cpp` |

### 1.2 代码命名

| 类别 | 规则 | 示例 |
|---|---|---|
| **类 / 结构体** | `PascalCase` | `Device`, `RenderSystem`, `GraphicsContext` |
| **枚举类型名** | `PascalCase` | `Backend`, `ShaderStage` |
| **枚举值** | `e_` + `snake_case` | `e_graphics`, `e_vulkan` |
| **成员变量** | `m_` + `snake_case` | `m_device`, `m_allocator`, `m_ref_adapter` |
| **全局常量 / constexpr** | `PascalCase` | `Max_Allowable_Error`, `Default_Timeout` |
| **函数 / 方法** | `snake_case` | `fetch_queue()`, `import_scene()`, `compile()` |
| **局部变量** | `snake_case` | `queue_desc`, `render_node`, `vertex_shader` |
| **宏** | `ALL_CAPS` + `_` | `CHECK_RESULT`, `SAFE_FREE` |
| **模板参数** | 单大写字母或 PascalCase | `T`, `F`, `Args`, `Key`, `Value` |
| **头文件保护（`#ifndef` 风格）** | `ALL_CAPS` + `_H` | `DEVICE_H`, `BUFFER_H` |

### 1.3 特殊命名约定

- **`m_ref_` 前缀**：指向外部对象的成员指针，表示本类不拥有该对象（引用语义）
- **工厂函数前缀**：用模块名缩写 + `_` 做前缀，如 `GPU_create_device()`, `GFX_create_context()`
- **所有代码包裹在项目根命名空间中**（如 `Amazing`、`Gfx`、`Core` 等）
- **`const` 放置在类型右侧**：`Type const* ptr`（而非 `const Type* ptr`），统一从右向左阅读

---

## 二、文件结构

### 2.1 文件头注释

每个 `.h` / `.cpp` 文件以以下格式开头：

```cpp
//
// Created by AmazingBuff on YYYY/MM/DD.
//
```

### 2.2 头文件保护

优先使用 `#ifndef` 风格：

```cpp
#ifndef FILE_NAME_H
#define FILE_NAME_H

// ...

#endif //FILE_NAME_H
```

基础头文件（如 `base.h`、`pch.h`）可使用 `#pragma once`。同一子模块内保持一致。

### 2.3 Include 顺序

严格按以下顺序分组，组间用空行分隔：

```cpp
// 1. 本文件对应的头文件
#include "my_class.h"

// 2. 同项目的内部头文件
#include "core/base.h"
#include "utils/math.h"

// 3. 第三方库头文件
#include <Eigen/Eigen>
#include <assimp/Importer.hpp>

// 4. 标准库头文件
#include <fstream>
#include <algorithm>
```

- 禁止使用 `../` 相对路径，始终从项目 include 根路径开始
- 优先前向声明，减少头文件中的 `#include`

### 2.4 命名空间

所有代码包裹在项目根命名空间中：

```cpp
namespace project_name
{

// 所有代码

} // namespace project_name
```

### 2.5 预编译头

预编译头仅包含：
- 标准库头文件
- 平台宏定义
- 全局配置宏

禁止在预编译头中添加项目内部头文件。

---

## 三、代码风格

### 3.1 缩进与空格

- **缩进**：每级 4 空格，禁止使用 tab
- **行尾**：不得有尾随空格
- **空行**：逻辑段落之间空一行

### 3.2 大括号风格（Allman）

大括号独占一行：

```cpp
class Device
{
public:
    Device() : m_adapter(nullptr) {}
    virtual ~Device() = default;
};

void process()
{
    if (condition)
    {
        do_something();
    }
    else
    {
        do_other();
    }
}
```

`switch` 语句中需要局部变量时 `case` 与大括号同行：

```cpp
switch (value)
{
case 0:
    handle_zero();
    break;
case 1: {
    int temp = compute();
    handle_one(temp);
    break;
}
default:
    break;
}
```

### 3.3 访问修饰符

- 使用 `public:`, `protected:`, `private:`（无缩进，对齐 class 关键字）
- 顺序：`public` → `protected` → `private`
- 同一修饰符可以出现多次，用于分组逻辑相关的成员

### 3.4 初始化列表

放在构造函数签名同一行，冒号紧跟参数列表右括号：

```cpp
MyClass::MyClass(Arg const* arg, Info const& info) : m_arg(nullptr), m_handle(nullptr), m_pool(nullptr)
{
    // 构造体
}
```

### 3.5 类型修饰符位置

`const` 统一放在类型右侧，从右向左阅读：

```cpp
Type const* ptr;        // 指向常量的指针
Type* const ptr = &obj; // 常量指针
Type const& ref = obj;  // 常量引用
```

### 3.6 类型安全

- 继承自虚基类的具体类**必须标记 `final`**
- 优先使用 `= default` / `= delete` 声明构造、析构和运算符
- 接口类使用纯虚函数 `= 0`，具体实现标记 `override`
- 返回值不应忽略的函数使用 `[[nodiscard]]`
- 避免裸 `new` / `delete`，优先使用 RAII 和智能指针

---

## 四、类设计规范

### 4.1 抽象接口

纯虚接口，只定义契约，不包含实现细节：

```cpp
class ISystem
{
public:
    virtual ~ISystem() = default;

    virtual Entity import(Scene const& scene) = 0;
    virtual Entity create_pipeline(PipelineInfo const& info) = 0;
};
```

### 4.2 具体实现

继承抽象接口，标记 `final`：

```cpp
class SystemImpl final : public ISystem
{
public:
    explicit SystemImpl(SystemInfo const& info);
    ~SystemImpl() override;

    Entity import(Scene const& scene) override;
    Entity create_pipeline(PipelineInfo const& info) override;
private:
    Driver m_driver;

    friend class SceneRenderer;
    friend class GraphBuilder;
};
```

### 4.3 多后端 / 策略模式

抽象公共接口，各后端/策略独立实现：

```cpp
// 公共接口
class Device
{
public:
    Device() : m_adapter(nullptr) {}
    virtual ~Device() = default;
    virtual Queue const* fetch_queue(QueueType type, uint32_t index) const = 0;
protected:
    Adapter const* m_adapter;
};

// 具体后端
class D3D12Device final : public Device
{
public:
    D3D12Device(Adapter const* adapter, DeviceInfo const& info);
    ~D3D12Device() override;
    Queue const* fetch_queue(QueueType type, uint32_t index) const override;
private:
    void* m_native_handle;

    friend class D3D12Buffer;
};
```

### 4.4 friend 使用规范

- 允许在后端/实现类中使用 `friend`，使关联类可访问私有成员
- `friend` 声明放在 `private` 区域末尾
- 禁止跨模块/跨后端的 `friend`

---

## 五、枚举约定

```cpp
enum class Backend : uint8_t
{
    e_d3d12 = 0,
    e_vulkan = 1,
};

enum class Format : uint8_t
{
    e_undefined,
    e_r8g8b8a8_unorm,
    e_r32_sfloat,
    e_count
};
```

- 枚举值使用 `e_` 前缀
- 显式指定底层类型（尽量用最小必要宽度，如 `uint8_t`）
- 结尾使用 `e_count` 表示元素数量

---

## 六、配置结构体约定

所有创建参数通过命名结构体传递，使用 C++20 Designated Initializers：

```cpp
struct DeviceInfo
{
    bool disable_cache;
    uint32_t frame_count;
    std::vector<QueueGroup> queue_groups;
};

// 调用
DeviceInfo info{
    .disable_cache = false,
    .queue_groups = { /* ... */ }
};
```

- 字段顺序：bool → 基本类型 → 容器/结构体 → 指针
- 使用 `.field = value` 语法
- 配置结构体与目标类放在同一头文件

---

# 第二部分：CMake

## 一、文件结构

### 1.1 根 `CMakeLists.txt` 组织顺序

严格按以下顺序组织根 `CMakeLists.txt`：

```
1. cmake_minimum_required(VERSION 3.25)
2. project()
3. option()
4. set() 全局变量
5. 编译器检测与编译选项
6. add_subdirectory() 子目录
7. find_package() 外部依赖
8. file(GLOB_RECURSE ...) 源文件收集
9. add_library() / add_executable() 目标定义
10. target_sources()
11. target_include_directories()
12. target_precompile_headers()
13. target_link_libraries()
14. target_compile_features()
15. target_compile_definitions()
16. add_custom_command()
17. 平台特定分支 (if (WIN32)...)
18. 构建后钩子 / 自定义函数调用
```

### 1.2 子目录 `CMakeLists.txt`

- 每个子目录一个 `CMakeLists.txt`
- 子目录用 `add_subdirectory()` 从父级引入
- 子目录中定义的函数使用 `function()` / `endfunction()`，紧跟在目标定义之后
- 自定义函数名称全小写 + 下划线分隔，如 `add_sample()`, `add_rendering_sample()`

---

## 二、命名规范

| 类别 | 规则 | 示例 |
|---|---|---|
| **CMake 函数名** | `snake_case` | `add_sample()`, `add_rendering_sample()` |
| **CMake 自定义宏/函数** | `snake_case` | `All_INCLUDE_DIRS()`, `EXECUTE_REFLECT()` |
| **Project / Target 名** | `snake_case` 或 `PascalCase` | `engine`, `rendering_common`, `assimp::assimp` |
| **缓存变量** | `UPPER_SNAKE_CASE` | `THIRD_PARTY_LIBS`, `CMAKE_BUILD_TYPE` |
| **函数局部变量** | `snake_case` | `current_libs`, `source_files`, `sample_name` |
| **函数参数** | `snake_case` | `target`, `lib_name`, `sample_list` |
| **CMake Preset 名** | 全小写 + 空格分隔 | `"msvc debug"`, `"clang release"` |

---

## 三、格式风格

### 3.1 命令调用

- 命令名全小写：`add_library()`, `target_sources()`, `set()`
- 左括号紧跟在命令名后，不允许空格：`add_library(...)`
- 条件/循环关键字带空格：`if ()`, `elseif ()`, `else ()`, `endif ()`, `foreach ()`, `endforeach ()`, `function ()`, `endfunction ()`
- 参数数量较少时单行书写：

```cmake
add_subdirectory(ext)
add_subdirectory(3rd)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
```

- 参数数量较多时多行书写，每行一个参数，闭括号独立一行对齐命令首列：

```cmake
target_link_libraries(
    ${PROJECT_NAME}
    PUBLIC
    ${THIRD_PARTY_LIBS}
    SDL3::SDL3
    Eigen3::Eigen
)

target_compile_definitions(
    ${PROJECT_NAME}
    PUBLIC
    NOMINMAX
    WIN32_LEAN_AND_MEAN
)
```

### 3.2 缩进

- `function()` / `endfunction()` 体内缩进 4 空格
- `if()` / `endif()` 体内缩进 4 空格
- `foreach()` / `endforeach()` 体内缩进 4 空格

### 3.3 条件风格

```cmake
if(WIN32)
    target_link_libraries(
        ${PROJECT_NAME}
        PUBLIC
        d3d12
        dxgi
    )
endif()

if(${CMAKE_CXX_COMPILER_ID} STREQUAL "Clang")
    add_compile_options(-Wno-nontrivial-memaccess -Wno-unsafe-buffer-usage)
endif()
```

- 条件表达式无空格：`if(WIN32)`, `if(${VAR} STREQUAL "value")`
- 逻辑运算符使用大写：`AND`, `OR`, `NOT`

---

## 四、项目声明

```cmake
cmake_minimum_required(VERSION 3.25)

project(
    my_project
    LANGUAGES CXX C
    DESCRIPTION "brief description"
)

option(BUILD_SHARED_LIBS "Build shared libraries" OFF)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
```

- `project()` 多行书写，`LANGUAGES` 必须显式指定
- `option()` 紧跟 `project()` 之后
- `CMAKE_EXPORT_COMPILE_COMMANDS` 始终开启

---

## 五、目标属性设置

所有目标属性通过 `target_*` 系列命令设置，**禁止**使用全局 `include_directories()` / `link_libraries()` / `add_definitions()`。

```cmake
target_sources(
    ${PROJECT_NAME}
    PRIVATE
    ${HEADER_FILES}
    ${SOURCE_FILES}
)

target_include_directories(
    ${PROJECT_NAME}
    PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/include/
    PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/src/
)

target_precompile_headers(
    ${PROJECT_NAME}
    PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/src/pch.h
)

target_compile_features(
    ${PROJECT_NAME}
    PUBLIC
    cxx_std_23
)
```

- 可见性关键字（`PUBLIC`, `PRIVATE`, `INTERFACE`）独占一行，与参数列表对齐
- 可见性关键字按从上到下排列
- 每个源文件/路径独占一行

---

## 六、函数定义

自定义函数使用 `function()` / `endfunction()`，函数体缩进 4 空格：

```cmake
function(add_sample sample_name)
    file(GLOB_RECURSE source_files ${sample_name}/*.cpp)

    set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_DEBUG   ${CMAKE_CURRENT_BINARY_DIR}/${sample_name})
    set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_RELEASE ${CMAKE_CURRENT_BINARY_DIR}/${sample_name})

    add_executable(${sample_name})
    target_sources(${sample_name} PRIVATE ${source_files})
    target_link_libraries(${sample_name} PUBLIC ${PROJECT_NAME})
endfunction()

function(add_samples sample_list)
    foreach(sample ${sample_list})
        add_sample(${sample})
    endforeach()
endfunction()
```

- 函数内使用 `${PROJECT_NAME}` 引用项目名，避免硬编码
- 对列表参数使用 `foreach()` 循环分派
- `set()` 赋值时变量名和值对齐可增强可读性

---

## 七、自定义命令

`add_custom_command()` 多行形式，闭括号独立：

```cmake
add_custom_command(
    TARGET
    ${sample_name}
    POST_BUILD
    COMMAND
    ${CMAKE_COMMAND} -E copy_if_different ${THIRD_PARTY_DLLS} $<TARGET_FILE_DIR:${sample_name}>
    VERBATIM
)
```

- `TARGET`, `PRE_BUILD` / `POST_BUILD`, `COMMAND` 等关键字每个独占一行
- 长命令内容也独占一行
- 使用生成器表达式 `$<...>` 而非硬编码输出路径

---

## 八、CMakePresets.json

使用 `CMakePresets.json` 管理构建配置，而非命令行传参。

### 8.1 预设结构

```json
{
    "version": 3,
    "configurePresets": [
        {
            "name": "msvc",
            "hidden": true,
            "generator": "Ninja",
            "binaryDir": "${sourceDir}/build/${presetName}",
            "installDir": "${sourceDir}/install/${presetName}",
            "cacheVariables": {
                "CMAKE_C_COMPILER": "cl.exe",
                "CMAKE_CXX_COMPILER": "cl.exe"
            },
            "condition": {
                "type": "equals",
                "lhs": "${hostSystemName}",
                "rhs": "Windows"
            }
        },
        {
            "name": "msvc debug",
            "displayName": "MSVC Debug",
            "inherits": "msvc",
            "architecture": {
                "value": "x64",
                "strategy": "external"
            },
            "cacheVariables": {
                "CMAKE_BUILD_TYPE": "Debug"
            }
        },
        {
            "name": "msvc release",
            "displayName": "MSVC Release",
            "inherits": "msvc debug",
            "cacheVariables": {
                "CMAKE_BUILD_TYPE": "Release"
            }
        }
    ]
}
```

### 8.2 预设规则

- 使用 `"version": 3`
- 基础预设（编译器配置）标记 `"hidden": true`
- 使用 `"inherits"` 继承基础预设
- `debug` 和 `release` 各一个预设，通过 `cacheVariables.CMAKE_BUILD_TYPE` 区分
- `binaryDir` 使用 `${sourceDir}/build/${presetName}`
- `installDir` 使用 `${sourceDir}/install/${presetName}`
- 使用 `"condition"` 限制平台适用性
- `"generator"` 固定为 `"Ninja"`

---

## 九、第三方库管理

- 预编译第三方库放在 `3rd/` 目录，通过封装的 `add_lib()` 函数导入
- 源码级第三方依赖使用 Git submodule 放在 `ext/` 目录
- 在 `ext/CMakeLists.txt` 中用 `add_subdirectory()` 引入
- 使用缓存变量 (`CACHE STRING`) 在层级间传递库路径

```cmake
# 3rd/CMakeLists.txt
function(add_lib lib_name libs dlls include_dirs)
    set(current_libs "${${libs}}")
    set(current_dlls "${${dlls}}")
    set(current_include_dirs "${${include_dirs}}")

    if(WIN32)
        file(GLOB LIBS ${lib_name}/lib/x64/*.lib)
        file(GLOB DLLS ${lib_name}/bin/x64/*.dll)
    endif()

    list(APPEND current_libs ${LIBS})
    list(APPEND current_dlls ${DLLS})
    list(APPEND current_include_dirs ${CMAKE_CURRENT_SOURCE_DIR}/${lib_name}/include/)

    set(${libs} ${current_libs} PARENT_SCOPE)
    set(${dlls} ${current_dlls} PARENT_SCOPE)
    set(${include_dirs} ${current_include_dirs} PARENT_SCOPE)
endfunction()

add_lib(dxc LIBS DLLS INCLUDE_DIRS)
add_lib(vulkan LIBS DLLS INCLUDE_DIRS)

set(THIRD_PARTY_LIBS ${LIBS} CACHE STRING "" FORCE)
set(THIRD_PARTY_DLLS ${DLLS} CACHE STRING "" FORCE)
set(THIRD_PARTY_INCLUDE_DIRS ${INCLUDE_DIRS} CACHE STRING "" FORCE)
```

---

# 通用规则

## 一、注释规范

- **禁止**在代码中添加注释，除非功能极度复杂
- 代码应通过清晰的命名和结构自解释
- 如果某段代码复杂到需要注释，优先考虑重构简化

## 二、禁止事项

| 禁止 | 说明 |
|---|---|
| C++ 中 `#include` 使用 `../` 相对路径 | 使用基于项目根目录的 include 路径 |
| C++ 中 `using namespace std;` | 违反命名空间约定 |
| C++ 中头文件不必要的 `#include` | 优先前向声明 |
| C++ 中 `= default` 可用时写空 `{}` | 对虚析构函数等优先使用 `= default` |
| CMake 中使用全局 `include_directories()` | 使用 `target_include_directories()` |
| CMake 中使用全局 `link_libraries()` | 使用 `target_link_libraries()` |
| CMake 中使用全局 `add_definitions()` | 使用 `target_compile_definitions()` |
| CMake 命令参数无空格写 `add_library(name)` | 写 `add_library(name ...)` |
| 缩进混用 tab 和空格 | 统一使用 4 空格 |
| 行尾尾随空格 | 提交前清理 |
| 公共接口文件中包含实现细节 | 保持接口与实现分离 |
