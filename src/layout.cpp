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
#include "layout_bin.h"
#include <cstring>
#include <stdexcept>
#include <iconv.h>
#include <algorithm>

namespace {
	const static size_t	STEAMID_OFFSET = 0x28,
				SAVESLOT_OFFSET = 0x003004DC,
		      		SAVESLOT_SIZE = 0xF6110;

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

	const static size_t		INV_OFFSET = 0xDA8D5,
					INV_TOTAL = 250,
					INV_SIZE = 42;
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

// simple macro to avoid repeating the below cast
#define	SAVEFILE_PTR(ptr, buf) const layout_bin::savefile	*ptr = reinterpret_cast<const layout_bin::savefile*>(&buf[0])
	
	bool basic_checksum(const io::buffer& buf) {
		SAVEFILE_PTR(sf, buf);
		const static uint8_t	CHECKSUM[] = { 0x01, 0x00, 0x00, 0x00 };
		return !std::memcmp(&sf->magicnum, CHECKSUM, sizeof(uint8_t)*sizeof(CHECKSUM));
	}

	int64_t get_steamid(const io::buffer& buf) {
		SAVEFILE_PTR(sf, buf);
		return sf->steamid;
	}

	hunter_data get_slot_data(const io::buffer& buf, const size_t slot_id) {
		const size_t	base_slot = SAVESLOT_OFFSET + SAVESLOT_SIZE*slot_id;
		if((base_slot+SAVESLOT_SIZE) > buf.size())
			throw std::runtime_error("Invalid slot, outside of boundaries of savegame");
		SAVEFILE_PTR(sf, buf);
		hunter_data	out = {0};
		const layout_bin::saveslot*	cs = &sf->slots[slot_id];
		// TODO - should check conversion from UTF-8 to wchar_t
		{
			auto		conv = iconv_open("WCHAR_T", "UTF-8");
			char		*pIn = (char*)&cs->name_utf8,
					*pOut = (char*)&out.name[0];
			size_t		sIn = 64,
					sOut = 64;
			if(((void*)-1) == conv)
				throw std::runtime_error("this system can't convert from UTF-8 to WCHAR_T");
			iconv(conv, &pIn, &sIn, &pOut, &sOut);
			iconv_close(conv);
		}
		out.rank = cs->rank;
		out.zeny = cs->zeny;
		out.res_points = cs->res_points;
		out.xp = cs->xp;
		out.playtime = cs->playtime;
		out.gender = cs->gender;
		return out;
	}

	struct items_containers {
		size_t	offset,
			count;
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

	inv_data get_inv_data(const io::buffer& buf, const size_t slot_id) {
		const size_t	base_slot = SAVESLOT_OFFSET + SAVESLOT_SIZE*slot_id;
		if((base_slot+SAVESLOT_SIZE) > buf.size())
			throw std::runtime_error("Invalid slot, outside of boundaries of savegame");
		// scan each investigation
		std::vector<inv_info>	ret;
		for(size_t i = 0; i < INV_TOTAL; ++i) {
			const uint8_t	*cur_inv = &buf[base_slot + INV_OFFSET + i*INV_SIZE];
			static const uint8_t	FILLED[] = {0x30, 0x75, 0x00, 0x00};
			if(memcmp(FILLED, cur_inv, sizeof(FILLED)/sizeof(uint8_t)))
				continue;

			auto	fn_get_uint32_t = [cur_inv](const size_t offset) -> uint32_t {
				uint32_t	rv = 0;
				std::memcpy(&rv, cur_inv + offset, sizeof(uint32_t));
				return rv;
			};

			inv_info	tmp;
			// bitmap info from https://github.com/AsteriskAmpersand/MHW-Save-Editor/blob/master/MHW%20Save%20Editor/src/InvestigationEditing/Investigation.cs
			tmp.selected = cur_inv[4] != 0x00;
			tmp.attempts = fn_get_uint32_t(5);
			tmp.seen = cur_inv[9] == 0x03;
			tmp.locale = cur_inv[13];
			tmp.rank = cur_inv[14];
			tmp.mon1 = fn_get_uint32_t(15);
			tmp.mon2 = fn_get_uint32_t(19);
			tmp.mon3 = fn_get_uint32_t(23);
			ret.push_back(tmp);
		}
		return ret;
	}

	void mask_known_buffer(io::buffer& buf) {
		auto fn_mask = [&buf](const size_t offset, const size_t sz, const char* mask) -> void {
			const size_t	mask_sz = std::strlen(mask);
			for(size_t i = 0; i < sz; ++i) {
				buf[offset + i] = mask[i%mask_sz];
			}
		};

		auto fn_mask_inc = [&fn_mask](size_t& offset, const size_t sz, const char* mask) -> void {
			fn_mask(offset, sz, mask);
			offset += sz;
		};

		// steam id
		fn_mask(STEAMID_OFFSET, sizeof(int64_t), "steamid");
		// slot info
		for(size_t i = 0; i < 3; ++i) {
			const size_t	base_slot = SAVESLOT_OFFSET + SAVESLOT_SIZE*i;
			// save slot basic info
			{
				size_t base_slot_inc = base_slot;
				fn_mask_inc(base_slot_inc, 64, "name");
				fn_mask_inc(base_slot_inc, sizeof(uint32_t), "rank");
				fn_mask_inc(base_slot_inc, sizeof(uint32_t), "zenny");
				fn_mask_inc(base_slot_inc, sizeof(uint32_t), "res");
				fn_mask_inc(base_slot_inc, sizeof(uint32_t), "xp");
				fn_mask_inc(base_slot_inc, sizeof(uint32_t), "time");
				fn_mask(base_slot + 0xB0, sizeof(uint32_t), "gender");
			}

			// inventories
			for(size_t j = 0; j < sizeof(LIST_CONTAINERS)/sizeof(items_containers); ++j) {
				const size_t	cur_offset = base_slot + LIST_CONTAINERS[j].offset;
				fn_mask(cur_offset, LIST_CONTAINERS[j].count*sizeof(uint32_t)*2, items_data::names[j]);
			}

			// investigations
			fn_mask(base_slot + INV_OFFSET, INV_TOTAL*INV_SIZE, "investigations");
		}
	}
}

