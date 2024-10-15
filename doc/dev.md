# 配置开发环境

[TOC]

## Windows

下述脚本会调用 `winget` 安装程序。确保 `winget` 可用。


### 安装 cmake + MSVC + doxygen + uv

编译本项目需要 `cmake` 和 `msvc`。简化开发环境配置的额外脚本依赖于`uv`。

```powershell
pwsh -File .\tools\install-deps.ps1
```


### 配置 pre-commit 钩子

`pre-commit` 配置后可在 `git commit` 前检查修改的文件是否符合格式要求。

```powershell
pwsh -File .\tools\configure-pre-commit.ps1
```


### 生成 `compile_commands.json`

`compile_commands.json` 用于向 VS code 等 IDE 的 language server 提供工程信息，
以供自动补全。

生成该命令需要安装 `ninja`。可使用如下命令安装

```powershell
uv tool install ninja
```

安装完成后，运行如下脚本：

```powershell
pwsh -File .\tools\gen-compdb.ps1
```
