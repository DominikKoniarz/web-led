Import("env")

from SCons.Script import COMMAND_LINE_TARGETS  # type: ignore
import subprocess

if "compiledb" not in COMMAND_LINE_TARGETS:
    subprocess.run(["pio", "run", "-t", "compiledb"], check=False)
