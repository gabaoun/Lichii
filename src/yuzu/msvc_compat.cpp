// SPDX-FileCopyrightText: Copyright 2026 Eden Emulator Project
// SPDX-License-Identifier: GPL-3.0-or-later

#ifdef _MSC_VER
#include <algorithm>
#include <cstdint>
#include <utility>

#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif
#include <windows.h>
#include <synchapi.h>

extern "C" {

#if !defined(_MSVC_STL_UPDATE) || _MSVC_STL_UPDATE < 202404L
struct _Min_max_2u {
    uint16_t _Min;
    uint16_t _Max;
};
#endif

enum class _Thrd_result : int { _Success, _Nomem, _Timedout, _Busy, _Error };

struct _Stl_critical_section {
    void* _Unused;
    void* _M_srw_lock;
};

struct _Mtx_internal_imp_t {
    int _Type;
    _Stl_critical_section _Critical_section;
    long _Thread_id;
    int _Count;
};
using _Mtx_t = _Mtx_internal_imp_t*;

struct _Stl_condition_variable {
    void* _Unused;
    void* _Win_cv;
};

struct _Cnd_internal_imp_t {
    _Stl_condition_variable _Stl_cv;
};
using _Cnd_t = _Cnd_internal_imp_t*;

void __stdcall __std_rotate(void* _First, void* _Mid, void* _Last) noexcept {
    auto first = static_cast<char*>(_First);
    auto mid = static_cast<char*>(_Mid);
    auto last = static_cast<char*>(_Last);
    std::rotate(first, mid, last);
}

void* __stdcall __std_unique_8(void* _First, void* _Last) noexcept {
    auto first = static_cast<uint64_t*>(_First);
    auto last = static_cast<uint64_t*>(_Last);
    return std::unique(first, last);
}

void* __stdcall __std_unique_4(void* _First, void* _Last) noexcept {
    auto first = static_cast<uint32_t*>(_First);
    auto last = static_cast<uint32_t*>(_Last);
    return std::unique(first, last);
}

const void* __stdcall __std_search_1(const void* _First1, const void* _Last1, const void* _First2, size_t _Count2) noexcept {
    auto f1 = static_cast<const char*>(_First1);
    auto l1 = static_cast<const char*>(_Last1);
    auto f2 = static_cast<const char*>(_First2);
    return std::search(f1, l1, f2, f2 + _Count2);
}

const void* __stdcall __std_find_end_1(const void* _First1, const void* _Last1, const void* _First2, size_t _Count2) noexcept {
    auto f1 = static_cast<const char*>(_First1);
    auto l1 = static_cast<const char*>(_Last1);
    auto f2 = static_cast<const char*>(_First2);
    return std::find_end(f1, l1, f2, f2 + _Count2);
}

const void* __stdcall __std_find_last_trivial_2(const void* _First, const void* _Last, uint16_t _Val) noexcept {
    auto f = static_cast<const uint16_t*>(_First);
    auto l = static_cast<const uint16_t*>(_Last);
    while (l > f) {
        --l;
        if (*l == _Val) {
            return l;
        }
    }
    return static_cast<const uint16_t*>(_Last);
}

size_t __stdcall __std_find_first_of_trivial_pos_2(
    const void* _Haystack, size_t _Haystack_length, const void* _Needle, size_t _Needle_length) noexcept {
    auto h = static_cast<const uint16_t*>(_Haystack);
    auto n = static_cast<const uint16_t*>(_Needle);
    for (size_t i = 0; i < _Haystack_length; ++i) {
        for (size_t j = 0; j < _Needle_length; ++j) {
            if (h[i] == n[j]) return i;
        }
    }
    return static_cast<size_t>(-1);
}

void __stdcall _Thrd_sleep_for(unsigned long ms) noexcept {
    Sleep(ms);
}

_Min_max_element_t __stdcall __std_minmax_element_2u(const void* _First, const void* _Last) noexcept {
    auto f = static_cast<const uint16_t*>(_First);
    auto l = static_cast<const uint16_t*>(_Last);
    auto res = std::minmax_element(f, l);
    return _Min_max_element_t{ res.first, res.second };
}

__declspec(noalias) _Min_max_2u __stdcall __std_minmax_2u(const void* _First, const void* _Last) noexcept {
    auto f = static_cast<const uint16_t*>(_First);
    auto l = static_cast<const uint16_t*>(_Last);
    auto res = std::minmax_element(f, l);
    return _Min_max_2u{ *res.first, *res.second };
}

size_t __stdcall __std_mismatch_4(const void* _First1, const void* _First2, size_t _Count) noexcept {
    auto f1 = static_cast<const uint32_t*>(_First1);
    auto f2 = static_cast<const uint32_t*>(_First2);
    for (size_t i = 0; i < _Count; ++i) {
        if (f1[i] != f2[i]) return i;
    }
    return _Count;
}

void __stdcall __std_replace_copy_1(const void* _First, const void* _Last, void* _Dest, uint8_t _Old_val, uint8_t _New_val) noexcept {
    auto first = static_cast<const uint8_t*>(_First);
    auto last = static_cast<const uint8_t*>(_Last);
    auto dest = static_cast<uint8_t*>(_Dest);
    std::replace_copy(first, last, dest, _Old_val, _New_val);
}

void __stdcall __std_replace_copy_2(const void* _First, const void* _Last, void* _Dest, uint16_t _Old_val, uint16_t _New_val) noexcept {
    auto first = static_cast<const uint16_t*>(_First);
    auto last = static_cast<const uint16_t*>(_Last);
    auto dest = static_cast<uint16_t*>(_Dest);
    std::replace_copy(first, last, dest, _Old_val, _New_val);
}

void* __stdcall __std_remove_8(void* _First, void* _Last, uint64_t _Val) noexcept {
    auto first = static_cast<uint64_t*>(_First);
    auto last = static_cast<uint64_t*>(_Last);
    return std::remove(first, last, _Val);
}

_Thrd_result __stdcall _Cnd_timedwait_for_unchecked(_Cnd_t cnd, _Mtx_t mtx, unsigned int ms) noexcept {
    if (SleepConditionVariableSRW(
            reinterpret_cast<PCONDITION_VARIABLE>(&cnd->_Stl_cv._Win_cv),
            reinterpret_cast<PSRWLOCK>(&mtx->_Critical_section._M_srw_lock),
            ms, 0)) {
        return _Thrd_result::_Success;
    }
    if (GetLastError() == ERROR_TIMEOUT) {
        return _Thrd_result::_Timedout;
    }
    return _Thrd_result::_Error;
}

} // extern "C"
#endif
