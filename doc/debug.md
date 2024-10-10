# 调试（Debug）配置与技巧

[TOC]

## NixOS + NeoVim + GDB

### 允许 GDB attach 到任意进程

较新版本的 Linux 内核默认 **禁止** 一个进程 **读取/修改** 另一个进程的内存，
除非后者是前者的子进程。
因此，GDB 无法 attach 到一个正在运行的进程。

通过如下修改可移除这一限制：

```bash
echo 0 | sudo tee /proc/sys/kernel/yama/ptrace_scope
# or
sudo sysctl kernel.yama.ptrace_scope=0
```

@note
在 NixOS 中，将 `boot.kernel.sysctl."kernel.yama.ptrace_scope" = 0` 选项添加至 NixOS 主机配置即可。


### 配置 NeoVim

1. 安装 [`nvim-dap`](https://github.com/mfussenegger/nvim-dap)
2. 配置 `nvim-dap` 使用 DAP 模式与 GDB 通信：

    ```lua
    dap.adapters.gdb = {
        type = "executable",
        command = "gdb",
        args = { "--interpreter=dap", "--eval-command", "set print pretty on" }
    }
    ```
3. 指定 GDB 调试方法

    ```lua
    local dap = require("dap")
    local dap_utils = require("dap.utils")


    local get_exe_path = function()
        local build_dir = vim.env.DIRENV_DIR:sub(2).."/build/Debug/"
        return vim.fn.input("Path to executable: ", build_dir, "file")
    end

    local get_exe_pid = function()
        local name = vim.fn.input("Executable name (filter): ")
        return dap_utils.pick_process({ filter = name })
    end


    dap.configurations.cpp = {
        {
            name = "attach",
            type = "gdb",
            request = "attach",
            program = get_exe_path,
            pid = get_exe_pid,
            cwd = "${workspaceFolder}",
        },
        {
            name = "launch",
            type = "gdb",
            request = "launch",
            program = get_exe_path,
            cwd = "${workspaceFolder}",
            stopAtBeginningOfMainSubprogram = true,
        },
    }
    ```

建议配合 `direnv` 使用。可参考本工程的 [`.envrc`](https://github.com/HamPepper/template-project-cpp/blob/master/.envrc)
和 [`.nvim/local.vim`](https://github.com/HamPepper/template-project-cpp/blob/master/.nvim/local.vim)
进行配置。
