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

#ifndef CACHE_H
#define CACHE_H

#include "../../machineconfig.h"
#include "../frontend_memory.h"
#include "cache_policy.h"
#include "cache_types.h"

#include <cstdint>
#include <memory>

namespace machine {

constexpr size_t BLOCK_ITEM_SIZE = sizeof(uint32_t);

class Cache : public FrontendMemory {
    Q_OBJECT

public:
    Cache(
        FrontendMemory* memory,
        const CacheConfig* config,
        uint32_t memory_access_penalty_r = 1,
        uint32_t memory_access_penalty_w = 1,
        uint32_t memory_access_penalty_b = 0);

    ~Cache() override;

    WriteResult write(
        Address destination,
        const void* source,
        size_t size) override;

    ReadResult read(
        Address source,
        void* destination,
        size_t size,
        ReadOptions options) const override;

    uint32_t get_change_counter() const override;

    void flush(); // flush cache
    void sync() override; // Same as flush

    uint32_t get_hit_count() const; // Number of recorded hits
    uint32_t get_miss_count() const; // Number of recorded misses
    uint32_t get_read_count() const; // Number backing/main memory reads
    uint32_t get_write_count() const; // Number backing/main memory writes
    uint32_t get_stall_count() const; // Number of wasted get_cycle_count in memory waiting statistic
    double get_speed_improvement()
        const; // Speed improvement in percents in comare with no used cache
    double get_hit_rate() const; // Usage efficiency in percents

    void reset(); // Reset whole state of cache

    const CacheConfig& get_config() const;

    enum LocationStatus location_status(Address address) const override;

signals:

    void hit_update(uint32_t) const;

    void miss_update(uint32_t) const;

    void statistics_update(
        uint32_t stalled_cycles,
        double speed_improv,
        double hit_rate) const;

    void cache_update(
        size_t associat,
        size_t set,
        size_t col,
        bool valid,
        bool dirty,
        size_t tag,
        const uint32_t* data,
        bool write) const;

    void memory_writes_update(uint32_t) const;

    void memory_reads_update(uint32_t) const;

private:
    const CacheConfig cache_config;
    FrontendMemory* const mem = nullptr;
    const Address uncached_start;
    const Address uncached_last;
    const uint32_t access_pen_r, access_pen_w, access_pen_b;
    const std::unique_ptr<CachePolicy> replacement_policy;

    mutable std::vector<std::vector<CacheBlock>> dt;

    mutable uint32_t hit_read = 0,
                     miss_read = 0,
                     hit_write = 0,
                     miss_write = 0,
                     mem_reads = 0,
                     mem_writes = 0,
                     burst_reads = 0,
                     burst_writes = 0,
                     change_counter = 0;

    void debug_rword(
        Address source,
        void* destination,
        size_t size) const;

    bool access(
        Address address,
        void* buffer,
        size_t size,
        AccessType access_type) const;

    void kick(
        size_t assoc_index,
        size_t set_index) const;

    Address calc_base_address(
        size_t tag,
        size_t set_index) const;

    void update_all_statistics() const;

    CacheLocation compute_location(Address address) const;

    /**
     * Searches for given tag in a set
     *
     * @param loc       requested location in cache
     * @return          associatity index of found block, max index + 1 if not found
     */
    size_t find_block_index(const CacheLocation& loc) const;
};

} // namespace machine

#endif // CACHE_H