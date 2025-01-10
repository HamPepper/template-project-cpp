#!/usr/bin/env python3

import json
import os


def strip_flags(compile_commands_path):
    with open(compile_commands_path, "r") as file:
        compile_commands = json.load(file)

    for command in compile_commands:
        command_parts = command["command"].split()
        command_parts = [
            part
            for part in command_parts
            if not part.startswith("-fmodules-ts")
            and not part.startswith("-fmodule-mapper=")
            and not part.startswith("-fdeps-format=p1689r5")
        ]
        command["command"] = " ".join(command_parts)

    with open(compile_commands_path, "w") as file:
        json.dump(compile_commands, file, indent=2)


if __name__ == "__main__":
    compile_commands_path = os.path.join(
        os.path.dirname(__file__), "..", "compile_commands.json"
    )
    strip_flags(compile_commands_path)
    print(f"Stripped flags unsupported by clang from {compile_commands_path}")
