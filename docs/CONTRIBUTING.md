# 贡献指南

感谢您对冷库 DTU 远程监控系统项目的关注！我们欢迎各种形式的贡献。

## 🚀 快速开始

### 开发环境设置

1. **克隆仓库**

```bash
git clone https://github.com/lonycn/hy-cool-dtu.git
cd hy-cool-dtu
```

2. **设置开发环境**

```bash
./quick_setup.sh
```

3. **验证环境**

```bash
./tools/scripts/test_project.sh
```

## 📋 贡献方式

### 🐛 报告 Bug

1. 查看 [Issues](https://github.com/lonycn/hy-cool-dtu/issues) 确认问题未被报告
2. 使用 Bug 报告模板创建新 Issue
3. 提供详细的复现步骤和环境信息

### 💡 功能请求

1. 查看现有的功能请求
2. 使用功能请求模板描述您的想法
3. 说明功能的用例和价值

### 🔧 代码贡献

#### 开发流程

1. **Fork 项目**

```bash
# 在GitHub上Fork项目到您的账户
# 然后克隆您的Fork
git clone https://github.com/lonycn/hy-cool-dtu.git
```

2. **创建功能分支**

```bash
git checkout -b feature/amazing-feature
```

3. **进行开发**

```bash
# 构建项目
./tools/scripts/build.sh

# 运行测试
./tools/scripts/test.sh

# 格式化代码
./tools/scripts/format.sh
```

4. **提交更改**

```bash
git add .
git commit -m "feat: add amazing feature"
```

5. **推送并创建 PR**

```bash
git push origin feature/amazing-feature
# 在GitHub上创建Pull Request
```

## 📝 代码规范

### C/C++ 代码规范

- 遵循 [Google C++ Style Guide](https://google.github.io/styleguide/cppguide.html)
- 使用 `clang-format` 自动格式化
- 函数和变量使用 snake_case 命名
- 宏定义使用 UPPER_CASE

#### 示例代码风格

```c
// 函数定义
int ota_check_update(const char* version) {
    if (version == NULL) {
        return -1;
    }

    // 具体实现
    return 0;
}

// 结构体定义
typedef struct {
    char device_id[32];
    uint32_t version;
    bool is_online;
} device_info_t;
```

### 提交信息规范

使用 [Conventional Commits](https://www.conventionalcommits.org/) 格式：

```
<type>[optional scope]: <description>

[optional body]

[optional footer(s)]
```

#### 类型标识

- `feat`: 新功能
- `fix`: Bug 修复
- `docs`: 文档更改
- `style`: 代码格式(不影响功能)
- `refactor`: 重构
- `test`: 测试相关
- `chore`: 构建过程或辅助工具变动

#### 示例

```
feat(ota): add firmware signature verification

- Implement RSA signature verification
- Add certificate validation
- Update security documentation

Closes #123
```

## 🧪 测试要求

### 单元测试

```bash
# 运行所有测试
./tools/scripts/test.sh

# 运行特定模块测试
./tools/scripts/test.sh ota

# 生成覆盖率报告
./tools/scripts/coverage.sh
```

### 集成测试

```bash
# 硬件在环测试
./tools/scripts/hardware_test.sh

# OTA功能测试
python3 tools/test/test_ota.py test
```

## 📚 文档贡献

### 文档类型

- **开发文档**: `docs/development/` - 开发指南和技术文档
- **用户手册**: `docs/manuals/` - 用户使用手册
- **架构设计**: `docs/architecture/` - 系统设计文档
- **API 文档**: `docs/api/` - 接口文档

### 文档编写规范

- 使用 Markdown 格式
- 包含目录结构
- 提供代码示例
- 添加图表说明（使用 Mermaid）

## 🔍 代码审查

### PR 要求

- [ ] 通过所有自动化测试
- [ ] 代码格式化正确
- [ ] 包含必要的测试
- [ ] 更新相关文档
- [ ] 提交信息格式正确

### 审查清单

#### 功能性

- [ ] 功能按预期工作
- [ ] 边界条件处理正确
- [ ] 错误处理完整

#### 代码质量

- [ ] 代码结构清晰
- [ ] 变量和函数命名恰当
- [ ] 注释充分且准确
- [ ] 无明显性能问题

#### 安全性

- [ ] 输入验证充分
- [ ] 无内存泄漏
- [ ] 无安全漏洞

## 🌟 贡献者认可

### Hall of Fame

我们会在 README 中认可主要贡献者：

- 功能开发者
- Bug 修复者
- 文档贡献者
- 测试贡献者

### 贡献统计

- 提交代码行数
- 解决 Issue 数量
- PR 合并数量
- 文档更新次数

## 📞 获取帮助

### 联系方式

- **Issues**: GitHub Issues 讨论技术问题
- **Discussions**: GitHub Discussions 讨论想法和建议
- **Email**: contribute@your-domain.com

### 常见问题

#### Q: 如何设置调试环境？

A: 查看 [开发指南](docs/development/DEVELOP.md)

#### Q: 如何添加新的传感器支持？

A: 参考 [传感器模块文档](docs/architecture/modules/01_SENSOR_MODULE.md)

#### Q: 如何测试 OTA 功能？

A: 使用 `python3 tools/test/test_ota.py` 进行测试

## 📜 许可证

通过贡献代码，您同意将您的贡献置于与项目相同的 MIT 许可证下。

---

再次感谢您的贡献！🙏
