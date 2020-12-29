// SPDX-License-Identifier: GPL-2.0+
/*******************************************************************************
 * QtMips - MIPS 32-bit Architecture Subset Simulator
 *
 * Implemented to support following courses:
 *
 *   B35APO - Computer Architectures
 *   https://cw.fel.cvut.cz/wiki/courses/b35apo
 *
 *   B4M35PAP - Advanced Computer Architectures
 *   https://cw.fel.cvut.cz/wiki/courses/b4m35pap/start
 *
 * Copyright (c) 2017-2019 Karel Koci<cynerd@email.cz>
 * Copyright (c) 2019      Pavel Pisa <pisa@cmp.felk.cvut.cz>
 *
 * Faculty of Electrical Engineering (http://www.fel.cvut.cz)
 * Czech Technical University        (http://www.cvut.cz/)
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA  02110-1301, USA.
 *
 ******************************************************************************/

#ifndef UTILS_H
#define UTILS_H

#include <cstdint>
#include <cstdlib>
#include <stdexcept>
#include <type_traits>

#if __GNUC__ >= 7
#define FALLTROUGH __attribute__((fallthrough));
#else
#define FALLTROUGH
#endif

typedef uint8_t byte;

inline constexpr uint32_t sign_extend(uint16_t v)
{
    return ((v & 0x8000) ? 0xFFFF0000 : 0) | v;
}

#define UNIMPLEMENTED throw std::logic_error("Unimplemented");
#define PANIC throw std::logic_error("The program panicked.");
#define UNREACHABLE assert(false);
#define UNUSED(...) (void)__VA_ARGS__;

/**
 * Test whether given address is aligned to type
 *
 * @tparam Address      type used to store the address
 * @tparam T            type to check alignment
 * @param address       address to check
 * @return              true if is aligned
 */
template <typename Address, typename T>
inline bool is_aligned_generic(Address address)
{
    return static_cast<uintptr_t>(address) % std::alignment_of<T>::value;
}

/**
 * Divide and round up
 */
template <typename T>
inline constexpr T divide_and_ceil(T divident, T divisor)
{
    return ((divident + divisor - 1) / divisor);
}

#endif // UTILS_H
