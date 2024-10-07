lua << EOF

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


EOF
