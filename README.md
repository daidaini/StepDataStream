# StepDataStream

一个高性能的C++数据流处理库，专注于数据的序列化、反序列化和记录管理。

## 项目概述

StepDataStream是一个用于处理结构化数据流的C++库，提供了高效的数据序列化/反序列化功能，特别适用于金融数据处理、通信协议数据包处理等场景。

## 主要特性

- **多种数据流类型支持**：单记录流、多记录流等不同数据结构
- **高效内存管理**：支持内存池和内存块管理，优化内存分配性能
- **数据序列化/反序列化**：支持自定义格式的数据编码和解码
- **字段转义处理**：自动处理特殊字符的转义和反转义
- **现代C++设计**：使用C++14标准，支持移动语义和RAII资源管理

## 核心组件

### 数据流基类 (StreamBase)
- 提供数据管理的基础功能
- 支持字段的序列化和反序列化
- 包含字段转义处理机制

### 多记录数据流 (MultiRecordStream)
- 支持多条记录的数据管理
- 提供记录的添加、遍历功能
- 支持模板化的字段值设置

### StepVer2 - 新一代实现
- 完全重新设计的高性能实现
- 改进的内存管理策略
- 更好的API兼容性
- 显著的性能提升

### 内存管理工具
- **MemoryPool**: 高效的内存池实现
- **MemBlock**: 内存块管理
- **StringFunc**: 字符串处理工具

## 快速开始

### 基本使用示例

```cpp
#include "StepVer2/StepVer2.h"
using namespace stepver2;

// 创建数据流实例
StepVer2 step;
step.Init();

// 设置基础字段
step.SetBaseFieldValueInt(STEP_FUNC, 1001);
step.SetBaseFieldValueString(STEP_MSG, "Success");

// 添加记录
step.AppendRecord();
step.AddFieldValue(STEP_SCDM, "SH");
step.AddFieldValue(STEP_HYDM, "000001");
step.AddFieldValue(STEP_HYCS, 100);
step.EndAppendRecord();

// 序列化
std::string serialized = step.ToString();

// 反序列化
StepVer2 newStep;
newStep.SetPackage(serialized);
```

## 编译和构建

项目使用CMake进行构建管理：

```bash
# 创建构建目录
mkdir build && cd build

# 配置项目
cmake ..

# 编译
make

# 运行测试
cd ../Test/build
make
./test_step
```

## 项目结构

```
StepDataStream/
├── Source/           # 核心源代码
│   ├── StreamBase.*  # 数据流基类
│   ├── MultiRecordStream.*  # 多记录数据流
│   └── ...
├── StepVer2/         # 新一代高性能实现
│   ├── StepVer2.*    # 主要实现
│   ├── MemoryPool.*  # 内存池
│   └── README.md     # 详细文档
├── Tool/             # 工具类
│   ├── MemBlock.*    # 内存块管理
│   └── StringFunc.*  # 字符串工具
├── Test/             # 测试和示例
│   ├── example_usage.cpp  # 使用示例
│   └── test_*.cpp    # 单元测试
└── CMakeLists.txt    # 构建配置
```

## 性能特点

- **高效内存管理**：减少30-50%的内存分配次数
- **快速序列化**：提升20-40%的序列化性能
- **优化的数据结构**：更紧凑的内存布局
- **现代C++优化**：充分利用编译器优化

## 技术要求

- **C++标准**: C++14或更高版本
- **编译器**: GCC 5.0+, Clang 3.8+, MSVC 2017+
- **构建工具**: CMake 3.15+
- **依赖库**: fmt, jsoncpp, jemalloc

## 应用场景

- 金融数据处理系统
- 通信协议数据包处理
- 高频交易数据流
- 实时数据传输
- 数据缓存和持久化

## 许可证

本项目采用开源许可证，具体许可证信息请查看LICENSE文件。

## 贡献

欢迎提交Issue和Pull Request来改进项目。请确保：
1. 遵循现有的代码风格
2. 添加适当的测试用例
3. 更新相关文档

## 更多信息

- 详细的StepVer2实现说明请参考 [StepVer2/README.md](StepVer2/README.md)
- 使用示例请参考 [Test/example_usage.cpp](Test/example_usage.cpp)
- 性能测试请参考 [Test/test_simple_comparison.cpp](Test/test_simple_comparison.cpp)