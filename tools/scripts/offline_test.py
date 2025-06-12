#!/usr/bin/env python3
"""
脱机编译测试脚本 - Offline Build Test
用于在没有硬件连接的情况下进行编译测试和开发调试

Usage:
    python3 tools/scripts/offline_test.py [options]
"""

import os
import sys
import subprocess
import datetime
import shutil
from pathlib import Path


class OfflineTester:
    def __init__(self):
        self.project_root = Path(__file__).parent.parent.parent
        self.log_dir = self.project_root / "build" / "test_logs"
        self.timestamp = datetime.datetime.now().strftime("%Y%m%d_%H%M%S")
        self.main_log = self.log_dir / f"offline_test_{self.timestamp}.log"

        # 创建日志目录
        self.log_dir.mkdir(parents=True, exist_ok=True)

        # 初始化日志
        self.log_file = open(self.main_log, "w", encoding="utf-8")

    def log(self, message, level="INFO"):
        """记录日志"""
        colors = {
            "INFO": "\033[0;34m",  # 蓝色
            "SUCCESS": "\033[0;32m",  # 绿色
            "WARNING": "\033[1;33m",  # 黄色
            "ERROR": "\033[0;31m",  # 红色
            "SECTION": "\033[0;35m",  # 紫色
        }
        reset = "\033[0m"

        timestamp = datetime.datetime.now().strftime("%H:%M:%S")
        colored_msg = f"{colors.get(level, '')}{level}{reset} [{timestamp}] {message}"
        plain_msg = f"{level} [{timestamp}] {message}"

        print(colored_msg)
        self.log_file.write(plain_msg + "\n")
        self.log_file.flush()

    def log_section(self, title):
        """记录章节标题"""
        separator = "=" * 50
        self.log(f"\n{separator}", "SECTION")
        self.log(f" {title}", "SECTION")
        self.log(separator, "SECTION")

    def check_command(self, command):
        """检查命令是否可用"""
        return shutil.which(command) is not None

    def run_command(self, command, cwd=None, capture=True):
        """运行命令并返回结果"""
        try:
            if capture:
                result = subprocess.run(
                    command,
                    shell=True,
                    cwd=cwd,
                    capture_output=True,
                    text=True,
                    timeout=300,
                )
                return result.returncode == 0, result.stdout, result.stderr
            else:
                result = subprocess.run(command, shell=True, cwd=cwd)
                return result.returncode == 0, "", ""
        except subprocess.TimeoutExpired:
            return False, "", "命令执行超时"
        except Exception as e:
            return False, "", str(e)

    def check_toolchain(self):
        """检查编译工具链"""
        self.log_section("检查编译工具链")

        tools = {
            "arm-none-eabi-gcc": "ARM GCC编译器",
            "cmake": "CMake构建工具",
            "make": "Make构建工具",
            "python3": "Python3",
        }

        tools_ok = True

        for tool, description in tools.items():
            if self.check_command(tool):
                success, stdout, _ = self.run_command(f"{tool} --version")
                if success:
                    version_line = stdout.split("\n")[0] if stdout else "已安装"
                    self.log(f"✅ {description}: {version_line}", "SUCCESS")
                else:
                    self.log(f"✅ {description}: 已安装", "SUCCESS")
            else:
                self.log(f"❌ {description}: 未安装", "ERROR")
                if tool in ["arm-none-eabi-gcc", "cmake"]:
                    tools_ok = False

        # 检查PlatformIO（可选）
        if self.check_command("pio"):
            success, stdout, _ = self.run_command("pio --version")
            version = stdout.strip() if success and stdout else "已安装"
            self.log(f"✅ PlatformIO: {version}", "SUCCESS")
        else:
            self.log("⚠️  PlatformIO: 未安装（可选）", "WARNING")

        return tools_ok

    def syntax_check(self):
        """语法检查"""
        self.log_section("C代码语法检查")

        # 重要的源文件
        important_files = [
            "src/main_loop.c",
            "src/Modbus.c",
            "src/uart0_Modbus_uart3.c",
            "src/io.c",
            "src/sys_flash.c",
            "src/RI300CRC.c",
            "src/datastruct.c",
        ]

        error_count = 0
        check_count = 0

        for file_path in important_files:
            full_path = self.project_root / file_path
            if full_path.exists():
                self.log(f"检查: {file_path}")
                check_count += 1

                # 语法检查命令
                cmd = (
                    "arm-none-eabi-gcc "
                    "-mcpu=cortex-m0plus -mthumb -mfloat-abi=soft "
                    "-Wall -Wno-unused-variable -Wno-unused-function "
                    "-std=c99 -nostdlib -fno-builtin "
                    "-Isrc/include "
                    f"-fsyntax-only {file_path}"
                )

                success, _, stderr = self.run_command(cmd, cwd=self.project_root)

                if success:
                    self.log(f"  ✅ 语法正确", "SUCCESS")
                else:
                    self.log(f"  ❌ 有语法错误", "ERROR")
                    if stderr:
                        # 显示前几行错误
                        error_lines = stderr.split("\n")[:3]
                        for line in error_lines:
                            if line.strip():
                                self.log(f"    {line.strip()}", "ERROR")
                    error_count += 1
            else:
                self.log(f"⚠️  文件不存在: {file_path}", "WARNING")

        self.log(f"语法检查完成: 检查了 {check_count} 个文件")

        if error_count == 0:
            self.log("🎉 所有文件语法检查通过！", "SUCCESS")
            return True
        else:
            self.log(f"❌ {error_count} 个文件有语法错误", "ERROR")
            return False

    def cmake_test(self):
        """CMake编译测试"""
        self.log_section("CMake编译测试")

        # 检查必要文件
        cmake_file = self.project_root / "CMakeLists.txt"
        toolchain_file = self.project_root / "cmake" / "arm-none-eabi.cmake"

        if not cmake_file.exists():
            self.log("❌ CMakeLists.txt 不存在", "ERROR")
            return False

        if not toolchain_file.exists():
            self.log("❌ ARM工具链文件不存在", "ERROR")
            return False

        # 创建构建目录
        build_dir = self.project_root / "build" / "test_cmake"
        if build_dir.exists():
            shutil.rmtree(build_dir)
        build_dir.mkdir(parents=True)

        self.log("配置CMake项目...")

        # CMake配置
        cmake_cmd = (
            f"cmake "
            f"-DCMAKE_TOOLCHAIN_FILE={toolchain_file} "
            f"-DCMAKE_BUILD_TYPE=Debug "
            f"{self.project_root}"
        )

        success, stdout, stderr = self.run_command(cmake_cmd, cwd=build_dir)

        if not success:
            self.log("❌ CMake配置失败", "ERROR")
            if stderr:
                error_lines = stderr.split("\n")[:5]
                for line in error_lines:
                    if line.strip():
                        self.log(f"  {line.strip()}", "ERROR")
            return False

        self.log("✅ CMake配置成功", "SUCCESS")

        # 编译
        self.log("开始编译...")
        make_cmd = "make -j4"

        success, stdout, stderr = self.run_command(make_cmd, cwd=build_dir)

        if success:
            self.log("🎉 CMake编译成功！", "SUCCESS")

            # 检查编译产物
            elf_file = build_dir / "hy-cool-dtu.elf"
            if elf_file.exists():
                self.log("✅ 编译产物生成成功")

                # 显示大小信息
                if self.check_command("arm-none-eabi-size"):
                    success, stdout, _ = self.run_command(
                        f"arm-none-eabi-size {elf_file}"
                    )
                    if success and stdout:
                        self.log("📊 固件大小信息:")
                        for line in stdout.split("\n")[:3]:
                            if line.strip():
                                self.log(f"  {line.strip()}")

            return True
        else:
            self.log("❌ CMake编译失败", "ERROR")
            if stderr:
                error_lines = stderr.split("\n")[-10:]  # 显示最后10行错误
                for line in error_lines:
                    if line.strip():
                        self.log(f"  {line.strip()}", "ERROR")
            return False

    def generate_report(self):
        """生成测试报告"""
        self.log_section("生成测试报告")

        report_file = self.log_dir / f"test_report_{self.timestamp}.md"

        with open(report_file, "w", encoding="utf-8") as f:
            f.write(
                f"""# hy-cool-dtu 脱机编译测试报告

## 基本信息
- **测试时间**: {datetime.datetime.now().strftime('%Y-%m-%d %H:%M:%S')}
- **项目路径**: {self.project_root}
- **日志文件**: {self.main_log}

## 系统环境
- **操作系统**: {os.uname().sysname} {os.uname().release}
- **Python版本**: {sys.version.split()[0]}

## 工具链状态
"""
            )

            # 检查工具链状态
            tools = ["arm-none-eabi-gcc", "cmake", "make", "pio"]
            for tool in tools:
                status = "✅ 已安装" if self.check_command(tool) else "❌ 未安装"
                f.write(f"- **{tool}**: {status}\n")

            f.write(
                f"""
## 项目文件
- **CMakeLists.txt**: {'✅ 存在' if (self.project_root / 'CMakeLists.txt').exists() else '❌ 缺失'}
- **platformio.ini**: {'✅ 存在' if (self.project_root / 'platformio.ini').exists() else '❌ 缺失'}
- **src/ 目录**: {'✅ 存在' if (self.project_root / 'src').exists() else '❌ 缺失'}

## 测试日志
详细测试日志请查看: {self.main_log}

---
*报告由脱机测试脚本自动生成*
"""
            )

        self.log(f"测试报告已生成: {report_file}", "SUCCESS")

    def run_tests(self, test_types=None):
        """运行测试"""
        if test_types is None:
            test_types = ["toolchain", "syntax", "cmake"]

        self.log_section("hy-cool-dtu 脱机编译测试")
        self.log(f"🚀 开始脱机开发环境测试")
        self.log(f"时间戳: {self.timestamp}")
        self.log(f"项目路径: {self.project_root}")
        self.log(f"日志目录: {self.log_dir}")

        results = {}

        # 检查工具链
        if "toolchain" in test_types:
            results["toolchain"] = self.check_toolchain()
            if not results["toolchain"]:
                self.log("工具链检查失败，无法继续测试", "ERROR")
                return False

        # 语法检查
        if "syntax" in test_types:
            results["syntax"] = self.syntax_check()

        # CMake测试
        if "cmake" in test_types:
            results["cmake"] = self.cmake_test()

        # 生成报告
        self.generate_report()

        # 总结
        self.log_section("测试总结")
        total_tests = len([t for t in test_types if t != "toolchain"])
        passed_tests = sum(1 for k, v in results.items() if k != "toolchain" and v)

        self.log(f"总测试数: {total_tests}")
        self.log(f"通过测试: {passed_tests}")
        self.log(f"失败测试: {total_tests - passed_tests}")

        if passed_tests == total_tests:
            self.log("🎉 所有测试通过！脱机开发环境正常！", "SUCCESS")
            self.log("✅ 您可以继续进行脱机开发，编译系统已验证可用")
            return True
        else:
            self.log("⚠️  部分测试失败，请检查错误并修复", "WARNING")
            return False

    def __del__(self):
        """清理资源"""
        if hasattr(self, "log_file"):
            self.log_file.close()


def main():
    """主函数"""
    import argparse

    parser = argparse.ArgumentParser(description="hy-cool-dtu 脱机编译测试")
    parser.add_argument("-s", "--syntax", action="store_true", help="只进行语法检查")
    parser.add_argument("-c", "--cmake", action="store_true", help="只测试CMake编译")
    parser.add_argument("-a", "--all", action="store_true", help="运行所有测试")

    args = parser.parse_args()

    # 确定测试类型
    test_types = ["toolchain"]  # 始终检查工具链

    if args.syntax:
        test_types.append("syntax")
    elif args.cmake:
        test_types.append("cmake")
    elif args.all:
        test_types.extend(["syntax", "cmake"])
    else:
        # 默认运行语法检查和CMake测试
        test_types.extend(["syntax", "cmake"])

    # 运行测试
    tester = OfflineTester()
    success = tester.run_tests(test_types)

    sys.exit(0 if success else 1)


if __name__ == "__main__":
    main()
