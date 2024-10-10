# template-project-cpp

[![github CI 状态](https://github.com/HamPepper/template-project-cpp/actions/workflows/main.yml/badge.svg)](https://github.com/HamPepper/template-project-cpp/actions?query=workflow%3ACI)

C++ 项目模板。
本项目包含一个简单的命令行计算器 `tpcpp`。
该计算器基于 Robert Nystrom 的 [_Crafting Interpreters_](https://craftinginterpreters.com/) 一书中的代码实现。
项目文档地址：[hampepper.github.io/template-project-cpp](https://hampepper.github.io/template-project-cpp)


## 目标

- ✅ 使用 `cmake` 构建项目
- ✅ 使用 `ctest` 进行单元测试
- ✅ 使用 `doxygen + dot` 生成文档
- ✅ 配置 Github Actions 自动化构建项目并生成文档
- ❌ 自动配置开发环境
    - ✅ 在 Linux/macOS 下使用 `nix` 安装依赖
    - ❌ 在 Windows 下使用 PowerShell 脚本安装依赖
