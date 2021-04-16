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

#include "memorymodel.h"

#include <QBrush>

constexpr auto INTERNAL = machine::AccessEffects::INTERNAL;

MemoryModel::MemoryModel(QObject *parent)
    : Super(parent)
    , data_font("Monospace") {
    cell_size = CELLSIZE_WORD;
    cells_per_row = 1;
    index0_offset = machine::Address::null();
    data_font.setStyleHint(QFont::TypeWriter);
    machine = nullptr;
    memory_change_counter = 0;
    cache_data_change_counter = 0;
    access_through_cache = 0;
}

const machine::FrontendMemory *MemoryModel::mem_access() const {
    if (machine == nullptr) {
        return nullptr;
    }
    if (machine->memory_data_bus() != nullptr) {
        return machine->memory_data_bus();
    }
    // Direct access to memory is not allowed, data bus must be used. At least a
    // trivial one. If this occurred, there is a misconfigured machine.
    throw std::logic_error(
        "No memory available on machine. This is a bug, please report it.");
}

machine::FrontendMemory *MemoryModel::mem_access_rw() const {
    if (machine == nullptr) {
        return nullptr;
    }
    if (machine->memory_data_bus_rw() != nullptr) {
        return machine->memory_data_bus_rw();
    }
    // Direct access to memory is not allowed, data bus must be used. At least a
    // trivial one. If this occurred, there is a misconfigured machine.
    throw std::logic_error(
        "No memory available on machine. This is u bug, please report it.");
}

int MemoryModel::rowCount(const QModelIndex & /*parent*/) const {
    // uint64_t rows = (0x2000 + cells_per_row - 1) / cells_per_row;
    return 750;
}

int MemoryModel::columnCount(const QModelIndex & /*parent*/) const {
    return cells_per_row + 1;
}

QVariant MemoryModel::headerData(
    int section,
    Qt::Orientation orientation,
    int role) const {
    if (orientation == Qt::Horizontal) {
        if (role == Qt::DisplayRole) {
            if (section == 0) {
                return tr("Address");
            } else {
                uint32_t addr = (section - 1) * cellSizeBytes();
                QString ret = "+" + QString::number(addr, 10);
                return ret;
            }
        }
    }
    return Super::headerData(section, orientation, role);
}

QVariant MemoryModel::data(const QModelIndex &index, int role) const {
    if (role == Qt::DisplayRole || role == Qt::EditRole) {
        QString s, t;
        machine::Address address;
        uint32_t data;
        const machine::FrontendMemory *mem;
        if (!get_row_address(address, index.row())) {
            return QString("");
        }
        if (index.column() == 0) {
            t = QString::number(address.get_raw(), 16);
            s.fill('0', 8 - t.count());
            return "0x" + s + t.toUpper();
        }
        if (machine == nullptr) {
            return QString("");
        }
        mem = mem_access();
        if (mem == nullptr) {
            return QString("");
        }
        if ((access_through_cache > 0) && (machine->cache_data() != nullptr)) {
            mem = machine->cache_data();
        }
        address += cellSizeBytes() * (index.column() - 1);
        if (address < index0_offset) {
            return QString("");
        }
        switch (cell_size) {
        case CELLSIZE_BYTE: data = mem->read_u8(address, INTERNAL); break;
        case CELLSIZE_HWORD: data = mem->read_u16(address, INTERNAL); break;
        default:
        case CELLSIZE_WORD: data = mem->read_u32(address, INTERNAL); break;
        }

        t = QString::number(data, 16);
        s.fill('0', cellSizeBytes() * 2 - t.count());
        t = s + t.toUpper();
#if 0
        machine::LocationStatus loc_stat = machine::LOCSTAT_NONE;
        if (machine->cache_data() != nullptr) {
            loc_stat = machine->cache_data()->location_status(address);
            if (loc_stat & machine::LOCSTAT_DIRTY)
                t += " D";
            else if (loc_stat & machine::LOCSTAT_CACHED)
                t += " C";
        }
#endif
        return t;
    }
    if (role == Qt::BackgroundRole) {
        machine::Address address;
        if (!get_row_address(address, index.row()) || machine == nullptr
            || index.column() == 0) {
            return QVariant();
        }
        address += cellSizeBytes() * (index.column() - 1);
        if (machine->cache_data() != nullptr) {
            machine::LocationStatus loc_stat;
            loc_stat = machine->cache_data()->location_status(address);
            if (loc_stat & machine::LOCSTAT_DIRTY) {
                QBrush bgd(Qt::yellow);
                return bgd;
            } else if (loc_stat & machine::LOCSTAT_CACHED) {
                QBrush bgd(Qt::lightGray);
                return bgd;
            }
        }
        return QVariant();
    }
    if (role == Qt::FontRole) {
        return data_font;
    }
    return QVariant();
}

void MemoryModel::setup(machine::QtMipsMachine *machine) {
    this->machine = machine;
    if (machine != nullptr) {
        connect(
            machine, &machine::QtMipsMachine::post_tick, this,
            &MemoryModel::check_for_updates);
    }
    if (mem_access() != nullptr) {
        connect(
            mem_access(), &machine::FrontendMemory::external_change_notify,
            this, &MemoryModel::check_for_updates);
    }
    emit update_all();
    emit setup_done();
}

void MemoryModel::setCellsPerRow(unsigned int cells) {
    beginResetModel();
    cells_per_row = cells;
    endResetModel();
}

void MemoryModel::set_cell_size(int index) {
    beginResetModel();
    cell_size = (enum MemoryCellSize)index;
    index0_offset -= index0_offset.get_raw() % cellSizeBytes();
    endResetModel();
    emit cell_size_changed();
}

void MemoryModel::update_all() {
    const machine::FrontendMemory *mem;
    mem = mem_access();
    if (mem != nullptr) {
        memory_change_counter = mem->get_change_counter();
        if (machine->cache_data() != nullptr) {
            cache_data_change_counter
                = machine->cache_data()->get_change_counter();
        }
    }
    emit dataChanged(index(0, 0), index(rowCount() - 1, columnCount() - 1));
}

void MemoryModel::check_for_updates() {
    bool need_update = false;
    const machine::FrontendMemory *mem;
    mem = mem_access();
    if (mem == nullptr) {
        return;
    }

    if (memory_change_counter != mem->get_change_counter()) {
        need_update = true;
    }
    if (machine->cache_data() != nullptr) {
        if (cache_data_change_counter
            != machine->cache_data()->get_change_counter()) {
            need_update = true;
        }
    }
    if (!need_update) {
        return;
    }
    update_all();
}

bool MemoryModel::adjustRowAndOffset(int &row, machine::Address address) {
    row = rowCount() / 2;
    address -= address.get_raw() % cellSizeBytes();
    uint32_t row_bytes = cells_per_row * cellSizeBytes();
    uint32_t diff = row * row_bytes;
    if (machine::Address(diff) > address) {
        row = address.get_raw() / row_bytes;
        if (row == 0) {
            index0_offset = machine::Address::null();
        } else {
            index0_offset = address - row * row_bytes;
        }
    } else {
        index0_offset = address - diff;
    }
    return get_row_for_address(row, address);
}

void MemoryModel::cached_access(int cached) {
    access_through_cache = cached;
    update_all();
}

Qt::ItemFlags MemoryModel::flags(const QModelIndex &index) const {
    if (index.column() == 0) {
        return QAbstractTableModel::flags(index);
    } else {
        return QAbstractTableModel::flags(index) | Qt::ItemIsEditable;
    }
}

bool MemoryModel::setData(
    const QModelIndex &index,
    const QVariant &value,
    int role) {
    if (role == Qt::EditRole) {
        bool ok;
        machine::Address address;
        machine::FrontendMemory *mem;
        uint32_t data = value.toString().toULong(&ok, 16);
        if (!ok) {
            return false;
        }
        if (!get_row_address(address, index.row())) {
            return false;
        }
        if (index.column() == 0 || machine == nullptr) {
            return false;
        }
        mem = mem_access_rw();
        if (mem == nullptr) {
            return false;
        }
        if ((access_through_cache > 0)
            && (machine->cache_data_rw() != nullptr)) {
            mem = machine->cache_data_rw();
        }
        address += cellSizeBytes() * (index.column() - 1);
        switch (cell_size) {
        case CELLSIZE_BYTE: mem->write_u8(address, data, INTERNAL); break;
        case CELLSIZE_HWORD: mem->write_u16(address, data, INTERNAL); break;
        default:
        case CELLSIZE_WORD: mem->write_u32(address, data, INTERNAL); break;
        }
    }
    return true;
}
