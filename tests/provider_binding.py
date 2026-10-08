#!/usr/bin/env python3
"""Run actual provider binding/probe entries with bounded IOKit object mocks."""
from pathlib import Path
import re
import subprocess
import tempfile

root = Path(__file__).resolve().parent
source = (root.parent / "AppleIntelTGLCompat/AppleIntelTGLCompat.cpp").read_text()
core = source[source.index("template <typename T>"):source.index('extern "C" uint32_t tglRead32')]
probe = re.search(r'extern "C" uint32_t tglProbePort\(void \*port\) \{.*?\n\}', source, re.S).group()
fixture = (root / "provider_binding_fixture.cpp").read_text().replace("// PROVIDER_SOURCE", core + probe)
with tempfile.TemporaryDirectory(prefix="xelp-provider-binding-") as directory:
    directory = Path(directory)
    cpp, exe = directory / "test.cpp", directory / "test"
    cpp.write_text(fixture)
    subprocess.run(["clang++", "-std=c++14", "-O1", "-g", "-fsanitize=address,undefined",
                    "-fno-omit-frame-pointer", "-I" + str(root.parent / "AppleIntelTGLCompat"),
                    "-I" + str(root.parent / "WhateverXe"),
                    str(cpp), "-o", str(exe)], check=True)
    for mode in ["tgl", "adln"]:
        subprocess.run([str(exe), mode], check=True)
