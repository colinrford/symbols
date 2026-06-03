# SPDX-License-Identifier: CC0-1.0
# SPDX-FileCopyrightText: 2025-2026 Colin Ford
#
# Conan 2.x recipe for lam.symbols.
#
# Local development:
#   conan create . --profile <your-profile>
#
# Consumer projects can then declare:
#   requires = "lam_symbols/<version>"   # version comes from the VERSION file
#
# This recipe is supplementary to the project's primary CMake+CPS distribution
# path — see CMakeLists.txt and the install(PACKAGE_INFO) block for the
# canonical install layout. The recipe delegates entirely to CMake.
#
# lam.symbols is STANDALONE: it carries its own copy of the algebraic_traits
# vocabulary (see the DUPLICATED CODE block in src/symbols-engine.cppm), so this
# recipe has NO requires — no lam_concepts, no other lam packages.

from conan import ConanFile
from conan.tools.cmake import CMake, CMakeToolchain, CMakeDeps, cmake_layout
from conan.tools.files import copy, load
import os

class LamSymbolsConan(ConanFile):
    name = "lam_symbols"
    license = "CC0-1.0"
    author = "Colin Ford"
    url = "https://github.com/colinrford/symbols"
    description = (
        "Compile-time symbolic calculus engine in C++23 modules for the lam "
        "project. An outgrowth of Vincent Reverdy's CppCon 2023 talk."
    )
    topics = ("c++23", "modules", "symbolic", "calculus", "constexpr", "lam")

    settings = "os", "compiler", "build_type", "arch"
    package_type = "static-library"

    def set_version(self):
        # Single source of truth: the top-level VERSION file, which CMakeLists
        # also reads. Editing VERSION updates both the CMake project version and
        # this recipe — they never drift.
        self.version = load(
            self, os.path.join(self.recipe_folder, "VERSION")
        ).strip()

    # No external Conan deps — lam.symbols is standalone (see header note).

    exports_sources = (
        "VERSION",
        "CMakeLists.txt",
        "symbols_config.cppm.in",
        "src/*",
        "cmake/*",
        "LICENSE",
        "README.md",
    )

    def layout(self):
        cmake_layout(self)

    def validate(self):
        cppstd = self.settings.compiler.cppstd
        if cppstd is not None:
            std = int(str(cppstd).replace("gnu", ""))
            if std < 23:
                raise Exception(
                    "lam_symbols requires C++23 (compiler.cppstd >= 23)."
                )

    def generate(self):
        tc = CMakeToolchain(self)
        # Mirror what the project's CMakeLists already assumes.
        tc.cache_variables["CMAKE_CXX_STANDARD"] = "23"
        tc.cache_variables["CMAKE_CXX_SCAN_FOR_MODULES"] = "ON"
        # tests/ and examples/ aren't shipped in exports_sources (and examples
        # need external tools), so skip the add_subdirectory()s.
        tc.cache_variables["LAM_SYMBOLS_BUILD_TESTS"] = "OFF"
        tc.generate()
        deps = CMakeDeps(self)
        deps.generate()

    def build(self):
        cmake = CMake(self)
        cmake.configure()
        cmake.build()

    def package(self):
        cmake = CMake(self)
        cmake.install()
        copy(
            self,
            "LICENSE",
            src=self.source_folder,
            dst=os.path.join(self.package_folder, "licenses"),
        )

    def package_info(self):
        # Match the CMake export names so find_package(lam_symbols) and the
        # Conan-generated CMakeDeps both yield the same target. Path A (per-
        # submodule primary CPS) namespaces this as lam_symbols::symbols.
        self.cpp_info.set_property("cmake_file_name", "lam_symbols")
        self.cpp_info.set_property("cmake_target_name", "lam_symbols::symbols")
        self.cpp_info.libs = ["lam_symbols"]
        # CPS metadata lives under <pkg>/lib/cps for CMake 4.3+ consumers that
        # read CPS in addition to the legacy *Config.cmake.
        self.cpp_info.builddirs = [
            os.path.join("lib", "cmake", "lam_symbols"),
            os.path.join("lib", "cps"),
        ]
