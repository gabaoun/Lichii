// SPDX-FileCopyrightText: Copyright 2026 Eden Emulator Project
// SPDX-License-Identifier: GPL-3.0-or-later

#include <cstdio>
#include <cstdlib>

extern "C" [[noreturn]] void lichii_missing_msvc_stl_helper() {
    std::fputs("Qt called an MSVC STL helper this toolset does not have\n", stderr);
    std::abort();
}
