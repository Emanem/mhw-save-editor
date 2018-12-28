/*
    This file is part of mhw-save-editor.

    mhw-save-editor is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    mhw-save-editor is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with mhw-save-editor.  If not, see <https://www.gnu.org/licenses/>.
 * */

#ifndef _LAYOUT_BIN_H_
#define _LAYOUT_BIN_H_

#define PACKED	__attribute__((packed))

namespace layout_bin {

	struct saveslot {
		uint8_t		name_utf8[64];
		uint32_t	rank;
		uint32_t	zenny;
		uint32_t	res_points;
		uint32_t	xp;
		uint32_t	playtime;
		uint8_t		unknown_[0xf6110 - (
				sizeof(name_utf8) +
				sizeof(rank) +
				sizeof(zenny) +
				sizeof(res_points) +
				sizeof(xp) +
				sizeof(playtime) +
				0)];
	} PACKED;

	struct savefile {
		uint32_t	magicnum;
		uint8_t		unknown0[12 - (
				sizeof(magicnum) +
				0)];
		uint8_t		sha1[20];
		uint8_t		unknown1[0x28 - (
				sizeof(magicnum) +
				sizeof(unknown0) +
				sizeof(sha1) +
				0)];
		int64_t		steamid;
		uint8_t		unknown_[0x3004dc - (
				sizeof(magicnum) +
				sizeof(unknown0) +
				sizeof(sha1) +
				sizeof(unknown1) +
				sizeof(steamid) +
				0)];
		saveslot	slots[3];
	} PACKED;

	static_assert(sizeof(layout_bin::savefile) == (0x3004dc + (3*0xf6110)), "Savefile structure size not as expected");
}

#endif //_LAYOUT_BIN_H_

