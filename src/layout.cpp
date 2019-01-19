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
#include <map>
#include <stdexcept>
#include <iconv.h>
#include <algorithm>

namespace {
	const static size_t	SAVESLOT_OFFSET = 0x003004DC,
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
			char		*pIn = (char*)&cs->info.name_utf8[0],
					*pOut = (char*)&out.name[0];
			size_t		sIn = 64,
					sOut = 64;
			if(((void*)-1) == conv)
				throw std::runtime_error("this system can't convert from UTF-8 to WCHAR_T");
			iconv(conv, &pIn, &sIn, &pOut, &sOut);
			iconv_close(conv);
		}
		out.rank = cs->info.rank;
		out.zenny = cs->info.zenny;
		out.res_points = cs->info.res_points;
		out.xp = cs->info.xp;
		out.playtime = cs->info.playtime;
		out.gender = cs->info.hunter.gender;
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

	decos_data get_all_decos_data(const io::buffer& buf, const size_t slot_id) {
		const size_t	base_slot = SAVESLOT_OFFSET + SAVESLOT_SIZE*slot_id;
		if((base_slot+SAVESLOT_SIZE) > buf.size())
			throw std::runtime_error("Invalid slot, outside of boundaries of savegame");
		// local references
		SAVEFILE_PTR(sf, buf);
		const layout_bin::saveslot*	cs = &sf->slots[slot_id];
		// start putting decos into the vector and remove the items in case of no decos
		// I know this is suboptimal...
		decos_data	out;

		for(int i = 0; i < -1; ++i) {
			const auto&	cur_loadout = cs->equip_loadouts[i];
			if(cur_loadout.weapon_idx != 0xFFFFFFFF) {
				auto fn_cnv = [&]() -> std::string {
					auto		conv = iconv_open("ASCII", "UTF-8");
					char		*pIn = (char*)&cur_loadout.name_utf8[0],
							tmp[256],
							*pOut = (char*)&tmp[0];
					size_t		sIn = 256,
							sOut = 256;
					if(((void*)-1) == conv)
						throw std::runtime_error("this system can't convert from UTF-8 to ASCII");
					iconv(conv, &pIn, &sIn, &pOut, &sOut);
					iconv_close(conv);

					return tmp;
				};

				std::map<uint32_t, uint32_t>	deco_qty;

#define	GET_DECO(d) { if(cur_loadout.d != 0xFFFFFFFF) ++deco_qty[cur_loadout.d]; }

				GET_DECO(weapondecos_id1);
				GET_DECO(weapondecos_id2);
				GET_DECO(weapondecos_id3);
				GET_DECO(helmetdecos_id1);
				GET_DECO(helmetdecos_id2);
				GET_DECO(helmetdecos_id3);
				GET_DECO(torsodecos_id1);
				GET_DECO(torsodecos_id2);
				GET_DECO(torsodecos_id3);
				GET_DECO(armsdecos_id1);
				GET_DECO(armsdecos_id2);
				GET_DECO(armsdecos_id3);
				GET_DECO(coildecos_id1);
				GET_DECO(coildecos_id2);
				GET_DECO(coildecos_id3);
				GET_DECO(feetdecos_id1);
				GET_DECO(feetdecos_id2);
				GET_DECO(feetdecos_id3);

#undef GET_DECO
				if(!deco_qty.empty()) {
					std::pair<std::string, std::vector<std::pair<uint32_t, uint32_t>>>	cur_data;
					cur_data.first = fn_cnv();
					for(const auto& i : deco_qty)
						cur_data.second.emplace_back(std::pair<uint32_t, uint32_t>(cs->items.box_decos[i.first].id, i.second));
					out.emplace_back(cur_data);
				}
			}
		}

		// add the box
		{
			std::pair<std::string, std::vector<std::pair<uint32_t, uint32_t>>>	cur_data;
			cur_data.first = items_data::names[ITEMS_CONTAINER::box_decos];
			out.emplace_back(cur_data);
			auto&	latest = *out.rbegin();
			for(const auto& i : cs->items.box_decos)
				latest.second.emplace_back(std::make_pair(i.id, i.number));
		}

		return out;
	}

	inv_data get_inv_data(const io::buffer& buf, const size_t slot_id) {
		const size_t	base_slot = SAVESLOT_OFFSET + SAVESLOT_SIZE*slot_id;
		if((base_slot+SAVESLOT_SIZE) > buf.size())
			throw std::runtime_error("Invalid slot, outside of boundaries of savegame");
		// local references
		SAVEFILE_PTR(sf, buf);
		const layout_bin::saveslot*	cs = &sf->slots[slot_id];
		// scan each investigation
		std::vector<inv_info>	ret;
		for(const auto& i : cs->invs) {
			static const uint8_t	FILLED[] = {0x30, 0x75, 0x00, 0x00};
			if(memcmp(FILLED, &i.filled, sizeof(FILLED)/sizeof(uint8_t)))
				continue;

			inv_info	tmp;
			// bitmap info from https://github.com/AsteriskAmpersand/MHW-Save-Editor/blob/master/MHW%20Save%20Editor/src/InvestigationEditing/Investigation.cs
			tmp.selected = i.selected == 0x00;
			tmp.attempts = i.attempts;
			tmp.seen = i.seen == 0x03;
			tmp.locale = i.locale;
			tmp.rank = i.rank;
			tmp.mon1 = i.mon1;
			tmp.mon2 = i.mon2;
			tmp.mon3 = i.mon3;
			ret.push_back(tmp);
		}
		return ret;
	}

	void mask_known_buffer(io::buffer& buf) {
		auto fn_fill_buf = [&buf](void* ptr, const size_t sz, const char* pattern) -> void {
			uint8_t*	ptr_buf = (uint8_t*)ptr;

			if(ptr_buf + sz >= (&buf[0] + buf.size()))
				throw std::runtime_error("Can't fill buffer, offset specified is too large!");
			const size_t	pattern_sz = std::strlen(pattern);
			for(size_t i = 0; i < sz; ++i) {
				ptr_buf[i] = pattern[i%pattern_sz];
			}
		};

		// local references
		layout_bin::savefile	*sf = reinterpret_cast<layout_bin::savefile*>(&buf[0]);
// simple macro to call fill in function
#define FILL_FN(d, p)	fn_fill_buf(&(d), sizeof((d)), p)

		// start with savefile
		FILL_FN(sf->magicnum, "magic");
		FILL_FN(sf->sha1, "sha1");
		FILL_FN(sf->steamid, "steamid");

		for(size_t i = 0; i < sizeof(layout_bin::savefile::slots)/sizeof(layout_bin::saveslot); ++i) {
			// do each saveslot
			layout_bin::saveslot*	cs = &sf->slots[i];

			FILL_FN(cs->info, "charinfo");
			FILL_FN(cs->huntergc, "huntergc");
			FILL_FN(cs->othersgc, "othersgc");
			FILL_FN(cs->item_loadouts, "item_loadouts");
			FILL_FN(cs->items, "items");
			FILL_FN(cs->inventory, "inventory");
			FILL_FN(cs->inventory_idx, "inventory_idx");
			FILL_FN(cs->invs, "investigations");
			FILL_FN(cs->equip_loadouts, "equip_loadouts");
			FILL_FN(cs->harvestbox, "harvestbox");
			FILL_FN(cs->harvest_flag, "harvest_flag");
		}

#undef FILL_FN
	}
}

