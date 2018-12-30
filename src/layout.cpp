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
	const static size_t	SAVESLOT_OFFSET = 0x003004DC,
		      		SAVESLOT_SIZE = 0xF6110;

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

	items_data get_items_data(const io::buffer& buf, const size_t slot_id) {
		const size_t	base_slot = SAVESLOT_OFFSET + SAVESLOT_SIZE*slot_id;
		if((base_slot+SAVESLOT_SIZE) > buf.size())
			throw std::runtime_error("Invalid slot, outside of boundaries of savegame");
		// local references
		SAVEFILE_PTR(sf, buf);
		const layout_bin::saveslot*	cs = &sf->slots[slot_id];
		// start filling in info
		items_data out;
		//
		out.containers[ITEMS_CONTAINER::pouch_item].resize(sizeof(layout_bin::itemcontainers::pouch_item)/sizeof(layout_bin::itemlist));
		for(const auto& i : cs->items.pouch_item)
			out.containers[ITEMS_CONTAINER::pouch_item].push_back(std::make_pair(i.id, i.number));
		//
		out.containers[ITEMS_CONTAINER::pouch_ammo].resize(sizeof(layout_bin::itemcontainers::pouch_ammo)/sizeof(layout_bin::itemlist));
		for(const auto& i : cs->items.pouch_ammo)
			out.containers[ITEMS_CONTAINER::pouch_ammo].push_back(std::make_pair(i.id, i.number));
		//
		out.containers[ITEMS_CONTAINER::box_item].resize(sizeof(layout_bin::itemcontainers::box_item)/sizeof(layout_bin::itemlist));
		for(const auto& i : cs->items.box_item)
			out.containers[ITEMS_CONTAINER::box_item].push_back(std::make_pair(i.id, i.number));
		//
		out.containers[ITEMS_CONTAINER::box_ammo].resize(sizeof(layout_bin::itemcontainers::box_ammo)/sizeof(layout_bin::itemlist));
		for(const auto& i : cs->items.box_ammo)
			out.containers[ITEMS_CONTAINER::box_ammo].push_back(std::make_pair(i.id, i.number));
		//
		out.containers[ITEMS_CONTAINER::box_materials].resize(sizeof(layout_bin::itemcontainers::box_materials)/sizeof(layout_bin::itemlist));
		for(const auto& i : cs->items.box_materials)
			out.containers[ITEMS_CONTAINER::box_materials].push_back(std::make_pair(i.id, i.number));
		//
		out.containers[ITEMS_CONTAINER::box_decos].resize(sizeof(layout_bin::itemcontainers::box_decos)/sizeof(layout_bin::itemlist));
		for(const auto& i : cs->items.box_decos)
			out.containers[ITEMS_CONTAINER::box_decos].push_back(std::make_pair(i.id, i.number));
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
		// TODO
		// Needs to be completely rewritten
	}
}

