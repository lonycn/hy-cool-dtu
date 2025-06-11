#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""
冷库DTU重构项目 - 进度跟踪自动化脚本
功能：自动提取模块文档中的任务状态，生成进度报告

创建时间: 2024年12月
作者: 项目组
版本: V1.0.0
"""

import os
import re
import json
import sys
from pathlib import Path
from datetime import datetime
from collections import defaultdict
from typing import Dict, List, Tuple, Optional


class TaskStatusExtractor:
    """任务状态提取器"""

    def __init__(self, project_root: str = None):
        """初始化提取器

        Args:
            project_root: 项目根目录路径
        """
        self.project_root = Path(project_root) if project_root else Path.cwd()
        self.doc_path = self.project_root / "DOC" / "MODULES"
        self.output_path = self.project_root / "DOC" / "PROGRESS"

        # 状态标识映射
        self.status_mapping = {
            "⚫": "NOT_STARTED",
            "🟡": "IN_PROGRESS",
            "🔵": "IN_REVIEW",
            "🟢": "COMPLETED",
            "🔴": "BLOCKED",
        }

        # 状态中文名
        self.status_chinese = {
            "⚫": "未开始",
            "🟡": "进行中",
            "🔵": "评审中",
            "🟢": "已完成",
            "🔴": "有问题",
        }

        # 任务状态正则模式
        self.task_pattern = re.compile(
            r"([⚫🟡🔵🟢🔴])\s*([^|]+?)\|\s*([^|]+?)\|\s*([^|]+?)\|\s*([^|]+?)\|\s*([^|]+?)\|"
        )

        # 模块信息正则模式
        self.module_pattern = re.compile(r"#\s+(.+?)模块设计文档")

        self.tasks = []
        self.modules = {}

    def extract_tasks_from_file(self, file_path: Path) -> List[Dict]:
        """从单个模块文档提取任务信息

        Args:
            file_path: 模块文档文件路径

        Returns:
            任务信息列表
        """
        try:
            with open(file_path, "r", encoding="utf-8") as f:
                content = f.read()
        except Exception as e:
            print(f"⚠️ 读取文件失败: {file_path} - {e}")
            return []

        # 提取模块名
        module_match = self.module_pattern.search(content)
        module_name = module_match.group(1) if module_match else file_path.stem

        # 提取任务状态
        tasks = []
        matches = self.task_pattern.findall(content)

        for match in matches:
            status_icon, task_name, assignee, due_date, progress, notes = match

            task_info = {
                "module": module_name,
                "module_file": file_path.name,
                "status_icon": status_icon,
                "status_name": self.status_mapping.get(status_icon, "UNKNOWN"),
                "status_chinese": self.status_chinese.get(status_icon, "未知"),
                "task_name": task_name.strip(),
                "assignee": assignee.strip(),
                "due_date": due_date.strip(),
                "progress": progress.strip(),
                "notes": notes.strip(),
                "extraction_time": datetime.now().isoformat(),
            }
            tasks.append(task_info)

        return tasks

    def extract_all_tasks(self) -> None:
        """提取所有模块的任务信息"""

        if not self.doc_path.exists():
            print(f"❌ 文档目录不存在: {self.doc_path}")
            return

        print(f"📁 扫描文档目录: {self.doc_path}")

        # 扫描所有.md文件（排除README.md）
        md_files = [f for f in self.doc_path.glob("*.md") if f.name != "README.md"]

        if not md_files:
            print("⚠️ 未找到模块文档文件")
            return

        print(f"📄 找到 {len(md_files)} 个模块文档文件")

        # 提取每个文件的任务
        all_tasks = []
        for md_file in md_files:
            print(f"   处理: {md_file.name}")
            tasks = self.extract_tasks_from_file(md_file)
            all_tasks.extend(tasks)

            # 统计模块信息
            if tasks:
                module_name = tasks[0]["module"]
                self.modules[module_name] = {
                    "file": md_file.name,
                    "total_tasks": len(tasks),
                    "status_count": defaultdict(int),
                }

                for task in tasks:
                    self.modules[module_name]["status_count"][task["status_icon"]] += 1

        self.tasks = all_tasks
        print(f"✅ 总共提取 {len(all_tasks)} 个任务")

    def generate_statistics(self) -> Dict:
        """生成统计信息

        Returns:
            统计信息字典
        """
        if not self.tasks:
            return {}

        # 总体统计
        total_stats = defaultdict(int)
        for task in self.tasks:
            total_stats[task["status_icon"]] += 1

        total_tasks = len(self.tasks)

        # 计算完成率
        completed_tasks = total_stats["🟢"]
        completion_rate = (
            (completed_tasks / total_tasks * 100) if total_tasks > 0 else 0
        )

        # 模块统计
        module_stats = {}
        for module_name, module_info in self.modules.items():
            module_total = module_info["total_tasks"]
            module_completed = module_info["status_count"]["🟢"]
            module_completion_rate = (
                (module_completed / module_total * 100) if module_total > 0 else 0
            )

            module_stats[module_name] = {
                "total_tasks": module_total,
                "completed_tasks": module_completed,
                "completion_rate": round(module_completion_rate, 1),
                "status_distribution": dict(module_info["status_count"]),
            }

        statistics = {
            "extraction_time": datetime.now().isoformat(),
            "total_tasks": total_tasks,
            "total_modules": len(self.modules),
            "completion_rate": round(completion_rate, 1),
            "status_distribution": dict(total_stats),
            "module_statistics": module_stats,
            "task_details": self.tasks,
        }

        return statistics

    def save_statistics(self, stats: Dict) -> None:
        """保存统计信息到文件

        Args:
            stats: 统计信息
        """
        # 确保输出目录存在
        self.output_path.mkdir(parents=True, exist_ok=True)

        # 保存JSON格式
        json_file = self.output_path / "current_status.json"
        with open(json_file, "w", encoding="utf-8") as f:
            json.dump(stats, f, ensure_ascii=False, indent=2)
        print(f"💾 统计数据已保存: {json_file}")

        # 保存Markdown格式报告
        md_file = self.output_path / "progress_report.md"
        self.generate_markdown_report(stats, md_file)
        print(f"📊 进度报告已生成: {md_file}")

    def generate_markdown_report(self, stats: Dict, output_file: Path) -> None:
        """生成Markdown格式的进度报告

        Args:
            stats: 统计信息
            output_file: 输出文件路径
        """

        report_time = datetime.now().strftime("%Y-%m-%d %H:%M:%S")

        md_content = f"""# 冷库DTU重构项目进度报告

## 📊 报告概要

- **生成时间**: {report_time}
- **统计范围**: 全项目模块
- **数据来源**: 模块设计文档自动提取

---

## 📈 总体进度统计

### 🎯 核心指标

```
📊 项目概况
├─ 总模块数: {stats['total_modules']} 个
├─ 总任务数: {stats['total_tasks']} 个
├─ 项目完成率: {stats['completion_rate']}%
└─ 数据更新: {report_time}
```

### 📋 任务状态分布

| 状态 | 数量 | 占比 | 说明 |
|------|------|------|------|
"""

        total_tasks = stats["total_tasks"]
        for status_icon, status_name in self.status_mapping.items():
            count = stats["status_distribution"].get(status_icon, 0)
            percentage = (count / total_tasks * 100) if total_tasks > 0 else 0
            chinese_name = self.status_chinese[status_icon]
            md_content += f"| {status_icon} {chinese_name} | {count} | {percentage:.1f}% | {status_name} |\n"

        md_content += f"""
### 📊 进度可视化

```
进度条: {'🟢' * int(stats['completion_rate'] // 5)}{'⚫' * (20 - int(stats['completion_rate'] // 5))} {stats['completion_rate']}%
```

---

## 🗂️ 模块详细进度

| 模块名称 | 总任务 | 已完成 | 完成率 | 状态分布 | 风险等级 |
|----------|--------|--------|--------|----------|----------|
"""

        # 模块详细统计
        for module_name, module_stats in stats["module_statistics"].items():
            total = module_stats["total_tasks"]
            completed = module_stats["completed_tasks"]
            rate = module_stats["completion_rate"]

            # 状态分布字符串
            status_dist = " ".join(
                [
                    f"{icon}{count}"
                    for icon, count in module_stats["status_distribution"].items()
                    if count > 0
                ]
            )

            # 风险等级评估
            if rate >= 80:
                risk_level = "🟢 低"
            elif rate >= 50:
                risk_level = "🟡 中"
            elif rate >= 20:
                risk_level = "🟠 高"
            else:
                risk_level = "🔴 极高"

            md_content += f"| {module_name} | {total} | {completed} | {rate}% | {status_dist} | {risk_level} |\n"

        md_content += f"""

---

## ⚠️ 风险分析

### 🚨 高风险模块 (完成率 < 20%)

"""

        high_risk_modules = [
            (name, stats)
            for name, stats in stats["module_statistics"].items()
            if stats["completion_rate"] < 20
        ]

        if high_risk_modules:
            for module_name, module_stats in high_risk_modules:
                blocked_count = module_stats["status_distribution"].get("🔴", 0)
                md_content += (
                    f"- **{module_name}**: 完成率 {module_stats['completion_rate']}%"
                )
                if blocked_count > 0:
                    md_content += f" (🔴 {blocked_count}个阻塞任务)"
                md_content += "\n"
        else:
            md_content += "✅ 当前无高风险模块\n"

        md_content += f"""

### 🔴 阻塞任务统计

"""

        blocked_tasks = [
            task for task in stats["task_details"] if task["status_icon"] == "🔴"
        ]

        if blocked_tasks:
            md_content += f"当前共有 **{len(blocked_tasks)}** 个阻塞任务需要关注：\n\n"
            for i, task in enumerate(blocked_tasks, 1):
                md_content += f"{i}. **{task['task_name']}** ({task['module']})\n"
                md_content += f"   - 负责人: {task['assignee']}\n"
                md_content += f"   - 截止日期: {task['due_date']}\n"
                if task["notes"]:
                    md_content += f"   - 备注: {task['notes']}\n"
                md_content += "\n"
        else:
            md_content += "✅ 当前无阻塞任务\n"

        md_content += f"""

---

## 📅 里程碑进度

### 🎯 关键里程碑状态

| 里程碑 | 计划时间 | 当前状态 | 风险评估 |
|--------|----------|----------|----------|
| M1 - 设计完成 | 2025-01-15 | ⚫ 未开始 | 🟢 正常 |
| M2 - P1模块完成 | 2025-04-01 | ⚫ 未开始 | 🟡 关注 |
| M3 - P2模块完成 | 2025-06-01 | ⚫ 未开始 | 🟢 正常 |
| M4 - 系统验收 | 2025-08-01 | ⚫ 未开始 | 🟢 正常 |

---

## 💡 建议与行动项

### 📋 近期行动建议

1. **立即处理**: 解决所有 🔴 阻塞任务
2. **重点关注**: 监控完成率 < 50% 的模块
3. **资源调配**: 为高风险模块增加技术支持
4. **进度同步**: 加强日常进度跟踪和沟通

### 🔮 趋势预测

基于当前进度，预计：
- **设计阶段**: 按计划完成概率 85%
- **开发阶段**: 可能需要调整资源配置
- **测试阶段**: 建议提前准备测试环境

---

## 📞 联系信息

- **项目经理**: 负责整体进度协调
- **技术负责人**: 负责技术难题解决  
- **各模块负责人**: 负责具体任务执行

---

*本报告由进度跟踪系统自动生成，数据来源于各模块设计文档*  
*下次更新时间: 根据文档修改自动触发*
"""

        # 写入文件
        with open(output_file, "w", encoding="utf-8") as f:
            f.write(md_content)

    def run(self) -> bool:
        """运行完整的进度提取和报告生成流程

        Returns:
            是否成功执行
        """
        try:
            print("🚀 开始进度跟踪...")

            # 提取任务
            self.extract_all_tasks()

            if not self.tasks:
                print("⚠️ 未提取到任何任务信息")
                return False

            # 生成统计
            stats = self.generate_statistics()

            # 保存结果
            self.save_statistics(stats)

            # 输出概要信息
            print(f"\n📊 进度统计概要:")
            print(f"   总任务数: {stats['total_tasks']}")
            print(f"   总完成率: {stats['completion_rate']}%")
            print(f"   模块数量: {stats['total_modules']}")

            print("\n✅ 进度跟踪完成！")
            return True

        except Exception as e:
            print(f"❌ 执行失败: {e}")
            import traceback

            traceback.print_exc()
            return False


def main():
    """主函数"""

    print("=" * 60)
    print("🏗️ 冷库DTU重构项目 - 进度跟踪系统")
    print("=" * 60)

    # 检查命令行参数
    project_root = sys.argv[1] if len(sys.argv) > 1 else None

    # 创建提取器并运行
    extractor = TaskStatusExtractor(project_root)
    success = extractor.run()

    if success:
        print("\n🎉 任务完成！进度报告已生成。")
        print(f"📁 查看报告: {extractor.output_path}")
    else:
        print("\n💥 任务失败！请检查错误信息。")
        sys.exit(1)


if __name__ == "__main__":
    main()
