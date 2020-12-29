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

#ifndef PHYSADDRSPACE_H
#define PHYSADDRSPACE_H

#include "../machinedefs.h"
#include "../qtmipsexception.h"
#include "backend/backend_memory.h"
#include "frontend_memory.h"

#include <QMap>
#include <QObject>
#include <cstdint>

namespace machine {

/**
 * Memory management unit emulation
 *
 * Can be used as direct frontend memory or as backing memory for cache.
 * Maps guest virtual address (type Address) into internal addressing (type
 * Offset) of the backing backend memory or periphery.
 */
class MMU : public FrontendMemory {
    Q_OBJECT
public:
    MMU();
    ~MMU() override;

    WriteResult write(
        Address destination,
        const void* source,
        size_t size,
        WriteOptions options) override;

    ReadResult read(
        Address source,
        void* destination,
        size_t size,
        ReadOptions options) const override;

    uint32_t get_change_counter() const override;

    bool insert_range(
        BackendMemory* mem_access,
        Address start_addr,
        Address last_addr,
        bool move_ownership);

    bool remove_range(BackendMemory* mem_access);

    void clean_range(Address start_addr, Address last_addr);

    enum LocationStatus location_status(Address address) const override;

private slots:
    void range_backend_external_change(
        const BackendMemory* mem_access,
        Offset start_offset,
        Offset last_offset,
        bool external);

private:
    class RangeDesc {
    public:
        RangeDesc(
            BackendMemory* mem_access,
            Address start_addr,
            Address last_addr,
            bool owned);

        BackendMemory* backend_memory;
        Address start_addr;
        Address last_addr;
        bool owned;
        uint32_t access_counter {};
        LocationStatus location_status; // TODO
    };
    QMap<Address, RangeDesc*> ranges_by_addr;
    QMultiMap<BackendMemory*, RangeDesc*> ranges_by_access;
    RangeDesc* find_range(Address address) const;
    mutable uint32_t change_counter;
};

/**
 * Redirects memory requests from frontend to backend one-to-one.
 * Used in test without cache (core only accepts `FrontendMemory`).
 */
class TrivialMMU final : public FrontendMemory {
public:
    explicit TrivialMMU(BackendMemory* backend_memory);

    WriteResult write(
        Address destination,
        const void* source,
        size_t size,
        WriteOptions options) override;

    ReadResult read(
        Address source,
        void* destination,
        size_t size,
        ReadOptions options) const override;

    uint32_t get_change_counter() const override;

private:
    BackendMemory* backend_memory;
    uint32_t change_counter = 0;
};

} // namespace machine
#endif // PHYSADDRSPACE_H
