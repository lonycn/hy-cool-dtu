# 脱机开发指南 - Offline Development Guide

> 用于在没有硬件连接的情况下进行编译测试和开发调试

## 🎯 概述

本指南提供了在脱机状态下（无硬件连接）进行 hy-cool-dtu 项目开发的完整方法，包括代码编辑、编译测试、错误调试等功能。

## 🔧 环境准备

### 必需工具
```bash
# 安装ARM GCC工具链
brew install --cask gcc-arm-embedded

# 安装CMake
brew install cmake

# 确认安装
arm-none-eabi-gcc --version
cmake --version
```

### 可选工具
```bash
# 安装PlatformIO（可选）
pip install platformio

# 安装Python开发环境
python3 --version
```

## 🚀 快速开始

### 1. 基本语法检查

创建并运行语法检查脚本：

```bash
# 创建语法检查脚本
cat > check_syntax.sh << 'EOF'
#!/bin/bash
echo "=== C代码语法检查 ==="

if ! command -v arm-none-eabi-gcc >/dev/null 2>&1; then
    echo "❌ ARM GCC 未安装"
    exit 1
fi

echo "✅ ARM GCC: $(arm-none-eabi-gcc --version | head -n1)"
echo ""

# 检查主要源文件
files=(
    "src/main_loop.c"
    "src/Modbus.c"
    "src/io.c"
    "src/sys_flash.c"
)

error_count=0
for file in "${files[@]}"; do
    if [[ -f "$file" ]]; then
        echo "检查: $file"
        if arm-none-eabi-gcc -mcpu=cortex-m0plus -mthumb -std=c99 -nostdlib -Isrc/include -fsyntax-only "$file" 2>/dev/null; then
            echo "  ✅ 语法正确"
        else
            echo "  ❌ 有语法错误"
            ((error_count++))
        fi
    fi
done

if [[ $error_count -eq 0 ]]; then
    echo "🎉 语法检查通过！"
else
    echo "❌ $error_count 个文件有错误"
fi
EOF

chmod +x check_syntax.sh
./check_syntax.sh
```

### 2. CMake编译测试

```bash
# 创建测试构建目录
mkdir -p build/test
cd build/test

# 配置CMake
cmake -DCMAKE_TOOLCHAIN_FILE=../../cmake/arm-none-eabi.cmake -DCMAKE_BUILD_TYPE=Debug ../..

# 编译
make -j4

# 检查结果
if [[ -f "hy-cool-dtu.elf" ]]; then
    echo "🎉 编译成功！"
    ls -la hy-cool-dtu.*
    arm-none-eabi-size hy-cool-dtu.elf
else
    echo "❌ 编译失败"
fi

cd ../..
```

### 3. 自动化测试脚本

创建综合测试脚本：

```bash
cat > offline_test.sh << 'EOF'
#!/bin/bash

echo "🚀 hy-cool-dtu 脱机编译测试"
echo "时间: $(date)"
echo ""

# 检查工具链
echo "检查工具链..."
for tool in arm-none-eabi-gcc cmake; do
    if command -v $tool >/dev/null 2>&1; then
        echo "✅ $tool: 已安装"
    else
        echo "❌ $tool: 未安装"
        exit 1
    fi
done

echo ""

# 语法检查
echo "语法检查..."
syntax_errors=0
for file in src/main_loop.c src/Modbus.c src/io.c; do
    if [[ -f "$file" ]]; then
        if arm-none-eabi-gcc -mcpu=cortex-m0plus -mthumb -std=c99 -nostdlib -Isrc/include -fsyntax-only "$file" 2>/dev/null; then
            echo "✅ $file"
        else
            echo "❌ $file"
            ((syntax_errors++))
        fi
    fi
done

# CMake测试
echo ""
echo "CMake编译..."
mkdir -p build/offline_test
cd build/offline_test

if cmake -DCMAKE_TOOLCHAIN_FILE=../../cmake/arm-none-eabi.cmake ../.. >/dev/null 2>&1; then
    if make >/dev/null 2>&1; then
        echo "✅ CMake编译成功"
        compile_success=true
    else
        echo "❌ CMake编译失败"
        compile_success=false
    fi
else
    echo "❌ CMake配置失败"
    compile_success=false
fi

cd ../..

# 总结
echo ""
echo "=== 测试总结 ==="
if [[ $syntax_errors -eq 0 && $compile_success == true ]]; then
    echo "🎉 所有测试通过！可以进行脱机开发"
    echo "💡 建议: 开始编辑代码并定期运行此脚本"
else
    echo "❌ 部分测试失败，请检查错误"
fi
EOF

chmod +x offline_test.sh
```

## 📝 开发工作流程

### 1. 日常开发流程

```bash
# 1. 编辑代码
vim src/main_loop.c

# 2. 语法检查
./check_syntax.sh

# 3. 编译测试
./offline_test.sh

# 4. 提交代码（如果测试通过）
git add .
git commit -m "更新功能代码"
```

### 2. 调试技巧

#### 使用编译器警告
```bash
# 启用更多警告
arm-none-eabi-gcc \
    -mcpu=cortex-m0plus -mthumb \
    -Wall -Wextra -Wshadow -Wundef \
    -std=c99 -Isrc/include \
    -fsyntax-only src/your_file.c
```

#### 预处理器检查
```bash
# 查看宏展开结果
arm-none-eabi-gcc \
    -mcpu=cortex-m0plus -mthumb \
    -Isrc/include -E src/main_loop.c \
    | grep -v '^#' | head -50
```

#### 生成汇编代码
```bash
# 生成汇编代码查看优化效果
arm-none-eabi-gcc \
    -mcpu=cortex-m0plus -mthumb \
    -Isrc/include -S -O2 \
    src/main_loop.c -o build/main_loop.s
```

## 🛠️ 高级功能

### 1. 内存布局分析

```bash
# 分析内存使用
arm-none-eabi-objdump -h build/test/hy-cool-dtu.elf

# 查看符号表
arm-none-eabi-nm build/test/hy-cool-dtu.elf | head -20

# 反汇编查看
arm-none-eabi-objdump -d build/test/hy-cool-dtu.elf | head -50
```

### 2. 代码质量检查

```bash
# 静态分析（需要安装cppcheck）
cppcheck --enable=all src/ 2>/dev/null

# 代码复杂度分析
find src/ -name "*.c" -exec wc -l {} + | sort -n
```

### 3. 自动化测试集成

创建 `.github/workflows/offline-test.yml`：

```yaml
name: 离线编译测试

on:
  push:
    branches: [ main, develop ]
  pull_request:
    branches: [ main ]

jobs:
  offline-test:
    runs-on: ubuntu-latest
    
    steps:
    - uses: actions/checkout@v3
    
    - name: 安装ARM工具链
      run: |
        sudo apt-get update
        sudo apt-get install gcc-arm-none-eabi
    
    - name: 语法检查
      run: |
        for file in src/*.c; do
          arm-none-eabi-gcc -mcpu=cortex-m0plus -mthumb -std=c99 -nostdlib -Isrc/include -fsyntax-only "$file"
        done
    
    - name: CMake编译测试
      run: |
        mkdir build
        cd build
        cmake -DCMAKE_TOOLCHAIN_FILE=../cmake/arm-none-eabi.cmake ..
        make
```

## 📊 日志和报告

### 1. 测试日志

```bash
# 创建详细日志
./offline_test.sh 2>&1 | tee build/test_$(date +%Y%m%d_%H%M%S).log
```

### 2. 生成测试报告

```bash
cat > generate_report.sh << 'EOF'
#!/bin/bash

REPORT_FILE="build/dev_report_$(date +%Y%m%d_%H%M%S).md"

cat > "$REPORT_FILE" << EOL
# hy-cool-dtu 开发环境报告

## 基本信息
- **生成时间**: $(date)
- **项目路径**: $(pwd)
- **Git分支**: $(git branch --show-current 2>/dev/null || echo "未知")

## 工具链状态
- **ARM GCC**: $(arm-none-eabi-gcc --version | head -n1 2>/dev/null || echo "未安装")
- **CMake**: $(cmake --version | head -n1 2>/dev/null || echo "未安装")

## 项目状态
- **源文件数量**: $(find src/ -name "*.c" | wc -l)
- **头文件数量**: $(find src/include/ -name "*.h" | wc -l)
- **最近修改**: $(git log -1 --format="%h %s" 2>/dev/null || echo "无Git历史")

## 测试状态
脱机编译测试: $(./offline_test.sh >/dev/null 2>&1 && echo "✅ 通过" || echo "❌ 失败")
EOL

echo "报告已生成: $REPORT_FILE"
EOF

chmod +x generate_report.sh
```

## 🔧 故障排除

### 常见问题

1. **ARM GCC未找到**
   ```bash
   # 检查PATH
   echo $PATH | grep arm
   
   # 重新安装
   brew uninstall --cask gcc-arm-embedded
   brew install --cask gcc-arm-embedded
   ```

2. **CMake配置失败**
   ```bash
   # 清理缓存
   rm -rf build/
   mkdir build && cd build
   
   # 详细输出
   cmake -DCMAKE_VERBOSE_MAKEFILE=ON -DCMAKE_TOOLCHAIN_FILE=../cmake/arm-none-eabi.cmake ..
   ```

3. **链接错误**
   ```bash
   # 检查链接脚本
   ls -la src/linker/
   
   # 验证符号
   arm-none-eabi-nm build/test/hy-cool-dtu.elf | grep main
   ```

## 💡 最佳实践

1. **频繁测试**: 每次代码修改后都运行语法检查
2. **版本控制**: 测试通过后再提交代码
3. **日志保留**: 保存测试日志用于问题追踪
4. **模块化开发**: 分模块测试，减少调试复杂度
5. **文档同步**: 及时更新开发文档

## 🎯 下一步

完成脱机测试后，您可以：

1. **连接硬件**: 使用 `./flash.sh` 烧录固件
2. **在线调试**: 使用 J-Link 或其他调试器
3. **功能测试**: 验证实际硬件功能
4. **性能优化**: 分析运行时性能

---

📝 **注意**: 脱机开发虽然不能验证硬件功能，但可以有效验证代码逻辑和编译系统，大大提高开发效率。 