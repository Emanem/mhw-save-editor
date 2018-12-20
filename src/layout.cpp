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

#include "layout.h"
#include <cstring>
#include <stdexcept>
#include <iconv.h>

namespace {
	const static size_t	STEAMID_OFFSET = 0x28,
				SAVESLOT_OFFSET = 0x003004DC,
		      		SAVESLOT_SIZE = 0xF6110;
}

namespace layout {

	const char	*items_data::names[ITEMS_CONTAINER::last] = {
		"pouch item",
		"pouch ammo",
		"box item",
		"box ammo",
		"box materials",
		"box decos"
	};
	
	bool basic_checksum(const io::buffer& buf) {
		const static uint8_t	CHECKSUM[] = { 0x01, 0x00, 0x00, 0x00 };
		return !std::memcmp(&buf[0], CHECKSUM, sizeof(uint8_t)*sizeof(CHECKSUM));
	}

	int64_t get_steamid(const io::buffer& buf) {
		int64_t	out = 0;
		std::memcpy(&out, &buf[STEAMID_OFFSET], sizeof(int64_t));
		return out;
	}

	hunter_data get_slot_data(const io::buffer& buf, const size_t slot_id) {
		const size_t	base_slot = SAVESLOT_OFFSET + SAVESLOT_SIZE*slot_id;
		size_t		cur_slot = base_slot;
		if((cur_slot+SAVESLOT_SIZE) > buf.size())
			throw std::runtime_error("Invalid slot, outside of boundaries of savegame");
		hunter_data	out = {0};
		// TODO - should check conversion from UTF-8 to wchar_t
		{
			auto		conv = iconv_open("WCHAR_T", "UTF-8");
			char		*pIn = (char*)&buf[cur_slot],
					*pOut = (char*)&out.name[0];
			size_t		sIn = 64,
					sOut = 64;
			if(((void*)-1) == conv)
				throw std::runtime_error("this system can't convert from UTF-8 to WCHAR_T");
			iconv(conv, &pIn, &sIn, &pOut, &sOut);
			iconv_close(conv);
		}
		cur_slot += 64;
		// read all the uint32_t 
		auto fn_read_uint32_t = [&buf, &cur_slot](uint32_t& v) -> void {
			std::memcpy(&v, &buf[cur_slot], sizeof(uint32_t));
			cur_slot += sizeof(uint32_t);
		};
		fn_read_uint32_t(out.rank);
		fn_read_uint32_t(out.zenny);
		fn_read_uint32_t(out.res_points);
		fn_read_uint32_t(out.xp);
		fn_read_uint32_t(out.playtime);
		std::memcpy(&out.gender, &buf[base_slot + 0xB0], sizeof(uint32_t));
		return out;
	}

	struct items_containers {
		size_t	offset,
			count;
	};

	const static items_containers	LIST_CONTAINERS[] = {
		{0xa2c79, 24},	//pouch_item
		{0xa2d39, 16},	//pouch_ammo
		{0xa2ed9, 200},	//box_item
		{0xa3519, 200}, //box_ammo
		{0xa3b59, 800},	//box_material
		{0xa5459, 200}	//box_deco
	};

	items_data get_items_data(const io::buffer& buf, const size_t slot_id) {
		const size_t	base_slot = SAVESLOT_OFFSET + SAVESLOT_SIZE*slot_id;
		if((base_slot+SAVESLOT_SIZE) > buf.size())
			throw std::runtime_error("Invalid slot, outside of boundaries of savegame");
		// start filling in info
		items_data out;
		for(size_t i = 0; i < sizeof(LIST_CONTAINERS)/sizeof(items_containers); ++i) {
			auto&		cur_items = out.containers[i];
			cur_items.resize(LIST_CONTAINERS[i].count);
			const size_t	cur_offset = base_slot + LIST_CONTAINERS[i].offset;
			auto fn_read_two_uint32_t = [&buf, &cur_offset](const size_t idx, std::pair<uint32_t, uint32_t>& v) -> void {
				std::memcpy(&v.first, &buf[cur_offset + idx*sizeof(uint32_t)*2], sizeof(uint32_t));
				std::memcpy(&v.second, &buf[cur_offset + idx*sizeof(uint32_t)*2 + sizeof(uint32_t)], sizeof(uint32_t));
			};

			for(size_t j = 0; j < LIST_CONTAINERS[i].count; ++j)
				fn_read_two_uint32_t(j, cur_items[j]);
		}
		return out;
	}
}

