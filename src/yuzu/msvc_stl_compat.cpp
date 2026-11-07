// SPDX-FileCopyrightText: Copyright 2026 Eden Emulator Project
// SPDX-License-Identifier: GPL-3.0-or-later

// The bundled Qt is built with MSVC STL 14.5x, whose headers call these vectorized helpers.
// VS 2022's STL (14.4x) lacks them, so they are defined here with the 14.52 signatures.

#include <cstddef>
#include <cstdint>
#include <cstring>
#include <yvals_core.h>

#if _MSVC_STL_VERSION < 145

namespace {

template <typename T>
void* Unique(void* first, void* last) noexcept {
    T* const begin = static_cast<T*>(first);
    T* const end = static_cast<T*>(last);
    if (begin == end) {
        return end;
    }
    T* out = begin;
    for (T* it = begin + 1; it != end; ++it) {
        if (*it != *out) {
            *++out = *it;
        }
    }
    return out + 1;
}

template <typename T>
void ReplaceCopy(const void* first, const void* last, void* dest, T old_val, T new_val) noexcept {
    const T* src = static_cast<const T*>(first);
    T* out = static_cast<T*>(dest);
    for (; src != static_cast<const T*>(last); ++src, ++out) {
        *out = *src == old_val ? new_val : *src;
    }
}

void Reverse(unsigned char* first, unsigned char* last) noexcept {
    while (first < last) {
        --last;
        const unsigned char tmp = *first;
        *first++ = *last;
        *last = tmp;
    }
}

} // namespace

extern "C" {

// Same layout as the STL's _Min_max_element_t.
struct MinMaxElementResult {
    const void* _Min;
    const void* _Max;
};

void __stdcall __std_rotate(void* first, void* mid, void* last) noexcept {
    auto* const f = static_cast<unsigned char*>(first);
    auto* const m = static_cast<unsigned char*>(mid);
    auto* const l = static_cast<unsigned char*>(last);
    Reverse(f, m);
    Reverse(m, l);
    Reverse(f, l);
}

void* __stdcall __std_unique_4(void* first, void* last) noexcept {
    return Unique<std::uint32_t>(first, last);
}

void* __stdcall __std_unique_8(void* first, void* last) noexcept {
    return Unique<std::uint64_t>(first, last);
}

void __stdcall __std_replace_copy_1(const void* first, const void* last, void* dest,
                                    std::uint8_t old_val, std::uint8_t new_val) noexcept {
    ReplaceCopy(first, last, dest, old_val, new_val);
}

void __stdcall __std_replace_copy_2(const void* first, const void* last, void* dest,
                                    std::uint16_t old_val, std::uint16_t new_val) noexcept {
    ReplaceCopy(first, last, dest, old_val, new_val);
}

// Like std::minmax_element: the first smallest and the last largest element.
MinMaxElementResult __stdcall __std_minmax_element_2u(const void* first, const void* last) noexcept {
    const auto* it = static_cast<const std::uint16_t*>(first);
    const auto* const end = static_cast<const std::uint16_t*>(last);
    MinMaxElementResult result{it, it};
    if (it == end) {
        return result;
    }
    std::uint16_t min = *it;
    std::uint16_t max = *it;
    for (++it; it != end; ++it) {
        if (*it < min) {
            min = *it;
            result._Min = it;
        }
        if (*it >= max) {
            max = *it;
            result._Max = it;
        }
    }
    return result;
}

} // extern "C"

#endif
