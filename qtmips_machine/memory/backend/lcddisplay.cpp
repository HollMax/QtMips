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

#include "lcddisplay.h"

using namespace machine;

LcdDisplay::LcdDisplay() {
    change_counter = 0;
    size_t need_bytes;
    fb_size = 0x4b000;
    fb_bpp = 16;
    fb_width = 480;
    fb_height = 320;
    if (fb_bpp > 12) {
        fb_linesize = ((fb_bpp + 7) >> 3) * fb_width;
    } else {
        fb_linesize = (fb_bpp * fb_width + 7) >> 3;
    }
    need_bytes = fb_linesize * fb_height;

    if (fb_size <= need_bytes)
        fb_size = need_bytes;

    fb_data = new uchar[fb_size];
    std::fill(fb_data, fb_data + fb_size, 0);
}

LcdDisplay::~LcdDisplay() {
    if (fb_data != nullptr)
        delete[] fb_data;
}

std::uint32_t LcdDisplay::pixel_address(uint x, uint y)
{
    std::uint32_t address;
    address = y * fb_linesize;
    if (fb_bpp > 12)
        address += x * ((fb_bpp + 7) >> 3);
    else
        address += x * fb_bpp / 8;

    return address;
}

bool LcdDisplay::write(
    const void *source,
    Offset offset,
    size_t count
) {
    UNIMPLEMENTED
    // TODO new api

    offset &= ~3;
    uint x, y, r, g, b;
    std::uint32_t c;
    std::uint32_t last_addr = offset + 3;
    std::uint32_t pixel_addr;

    if (offset + 3 >= fb_size)
        return 0;
#if 0
    printf("LcdDisplay::wword address 0x%08lx data 0x%08lx\n",
           (unsigned long)address, (unsigned long)value);
#endif
    if (source == read(offset, WORD, 0, true))
        return false;

    fb_data[offset + 0] = (source >> 24) & 0xff;
    fb_data[offset + 1] = (source >> 16) & 0xff;
    fb_data[offset + 2] = (source >> 8) & 0xff;
    fb_data[offset + 3] = (source >> 0) & 0xff;

    change_counter++;

    y = offset / fb_linesize;
    if (fb_bpp > 12)
        x = (offset - y * fb_linesize) / ((fb_bpp + 7) >> 3);
    else
        x = (offset - y * fb_linesize) * 8 / fb_bpp;

    while ((pixel_addr = pixel_address(x, y)) <= last_addr) {
        c = fb_data[pixel_addr] << 8;
        c |= fb_data[pixel_addr + 1];

        r = ((c >> 11) & 0x1f) << 3;
        g = ((c >> 5) & 0x3f) << 2;
        b = ((c >> 0) & 0x1f) << 3;

        emit pixel_update(x, y, r, g, b);

        if (++x >= fb_width) {
            x = 0;
            y++;
        }
    }

//    emit write_notification(offset, source);

    return true;
}

void LcdDisplay::read(
    Offset source,
    void *destination,
    size_t count,
    bool debug_read
) const {
    source &= ~3u;
    (void)debug_read;
    std::uint32_t value;

    if (source + 3 >= fb_size)
        return 0;

    value = (std::uint32_t)fb_data[source + 0] << 24u;
    value |= (std::uint32_t)fb_data[source + 1] << 16u;
    value |= (std::uint32_t)fb_data[source + 2] << 8u;
    value |= (std::uint32_t)fb_data[source + 3] << 0u;

#if 0
    printf("LcdDisplay::rword address 0x%08lx data 0x%08lx\n",
           (unsigned long)address, (unsigned long)value);
#endif

    emit read_notification(source, &value);

    return value;
}

//std::uint32_t LcdDisplay::get_change_counter() const {
//    return change_counter;
//}
