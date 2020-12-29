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
 * Copyright (c) 2020      Jakub Dupak <dupak.jakub@gmail.com>
 * Copyright (c) 2020      Max Hollmann <hollmmax@fel.cvut.cz>
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

#ifndef QTMIPS_MACHINE_MEMORY_H
#define QTMIPS_MACHINE_MEMORY_H

#include "../address.h"
#include "../memory_utils.h"
#include "./backend_memory.h"

namespace machine {

class MemorySection final : public BackendMemory {
public:
    explicit MemorySection(size_t length_bytes);
    MemorySection(const MemorySection& other);
    ~MemorySection() override = default;

    WriteResult write(
        const void* source,
        Offset destination,
        size_t size,
        WriteOptions options) override;

    ReadResult read(
        Offset source,
        void* destination,
        size_t size,
        ReadOptions options) const override;

    size_t length() const;
    const byte* data() const;

    bool operator==(const MemorySection&) const;
    bool operator!=(const MemorySection&) const;

private:
    std::vector<byte> dt;
};

//////////////////////////////////////////////////////////////////////////////
/// Some optimalization options
// How big memory sections will be in bits (2^8=256 bytes)
constexpr size_t MEMORY_SECTION_BITS = 8;
// How big one row of lookup tree will be in bits (2^4=16)
constexpr size_t MEMORY_TREE_BITS = 4;
//////////////////////////////////////////////////////////////////////////////
// Size of one section
constexpr size_t MEMORY_SECTION_SIZE = (1u << MEMORY_SECTION_BITS);
// Size of one memory row
constexpr size_t MEMORY_TREE_ROW_SIZE = (1u << MEMORY_TREE_BITS);
// Depth of tree
constexpr size_t MEMORY_TREE_DEPTH
    = ((32 - MEMORY_SECTION_BITS) / MEMORY_TREE_BITS);

union MemoryTree {
    union MemoryTree* subtree;
    MemorySection* sec;
};

class Memory final : public BackendMemory {
    Q_OBJECT
public:
    Memory();
    Memory(const Memory&);
    ~Memory() override;
    void reset(); // Reset whole content of memory (removes old tree and creates
                  // new one)
    void reset(const Memory&);

    MemorySection*
    get_section(std::uint32_t address, bool create) const; // returns section
                                                           // containing given
                                                           // address

    WriteResult write(
        const void* source,
        Offset offset,
        size_t size,
        WriteOptions options) override;

    ReadResult read(
        Offset source,
        void* destination,
        size_t size,
        ReadOptions options) const override;

    bool operator==(const Memory&) const;
    bool operator!=(const Memory&) const;

    const union MemoryTree* get_memorytree_root() const;

private:
    union MemoryTree* mt_root;
    uint32_t change_counter = 0;
    uint32_t write_counter = 0;

    static union MemoryTree* allocate_section_tree();
    static void free_section_tree(union MemoryTree*, size_t depth);
    static bool compare_section_tree(
        const union MemoryTree*,
        const union MemoryTree*,
        size_t depth);
    //    static bool is_zero_section_tree(const union MemoryTree*, size_t
    //    depth);
    static union MemoryTree*
    copy_section_tree(const union MemoryTree*, size_t depth);
};
}

#endif // QTMIPS_MACHINE_MEMORY_H
