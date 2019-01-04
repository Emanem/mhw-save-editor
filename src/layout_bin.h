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

	struct charinfo {
		uint8_t		name_utf8[64];
		uint32_t	rank;
		uint32_t	zenny;
		uint32_t	res_points;
		uint32_t	xp;
		uint32_t	playtime;
		uint8_t		unknown0[0xb0 - (
				sizeof(name_utf8) +
				sizeof(rank) +
				sizeof(zenny) +
				sizeof(res_points) +
				sizeof(xp) +
				sizeof(playtime) +
				0)];
		uint32_t	gender;
	} PACKED;

	struct itemlist {
		uint32_t	id;
		uint32_t	number;
	} PACKED;

	struct itemcontainers {
		itemlist	pouch_item[24];
		itemlist	pouch_ammo[16];
		uint8_t		unknown0[0x260 - (
				sizeof(pouch_item) +
				sizeof(pouch_ammo) +
				0)];
		itemlist	box_item[200];
		itemlist	box_ammo[200];
		itemlist	box_materials[800];
		itemlist	box_decos[200];
	} PACKED;

	struct investigation {
		uint8_t		filled[4];
		uint8_t 	selected;
		int32_t		attempts;
		int32_t		seen; // is the first byte which matters (0x03 to be seen)
		uint8_t		locale;
		uint8_t		rank;
		uint32_t	mon1;
		uint32_t	mon2;
		uint32_t	mon3; //27 bytes so far
		uint8_t		mon1_temper;
		uint8_t		mon2_temper;
		uint8_t		mon3_temper;
		uint8_t		hp; // ???
		uint8_t		attack; // ???
		uint8_t		defense; // ??? = 33 bytes so far
		uint8_t		X3; // ???
		uint8_t		Y0; // ???
		uint8_t		flourish_idx;
		uint8_t		time_amount;
		uint8_t		Y3; // ???
		uint8_t		faints_limits;
		uint8_t		max_players;
		uint8_t		mon_rewards; // ???
		uint8_t		zenny_multiplier;
	} PACKED;

	struct equiploadout {
		uint32_t	slotid;
		uint8_t		name[256];
		uint32_t	weapon_idx;
		uint32_t	helmet_idx;
		uint32_t	torso_idx;
		uint32_t	arms_idx;
		uint32_t	coil_idx;
		uint32_t	feet_idx;
		uint32_t	charm_idx;
		uint32_t	mantle1_idx;
		uint32_t	mantel2_idx;
		uint32_t	weapondecos_id1;
		uint32_t	weapondecos_id2;
		uint32_t	weapondecos_id3;
		uint32_t	helmetdecos_id1;
		uint32_t	helmetdecos_id2;
		uint32_t	helmetdecos_id3;
		uint32_t	torsodecos_id1;
		uint32_t	torsodecos_id2;
		uint32_t	torsodecos_id3;
		uint32_t	armsdecos_id1;
		uint32_t	armsdecos_id2;
		uint32_t	armsdecos_id3;
		uint32_t	coildecos_id1;
		uint32_t	coildecos_id2;
		uint32_t	coildecos_id3;
		uint32_t	feetdecos_id1;
		uint32_t	feetdecos_id2;
		uint32_t	feetdecos_id3;
		uint32_t	unknown0[27];
		uint32_t	charmdecos_idx1;
		uint32_t	charmdecos_idx2;
		uint32_t	charmdecos_idx3;
		uint32_t	mantle1decos_idx1;
		uint32_t	mantle1decos_idx2;
		uint32_t	mantle1decos_idx3;
		uint32_t	mantle2decos_idx1;
		uint32_t	mantle2decos_idx2;
		uint32_t	mantle2decos_idx3;
		uint32_t	bitmask_customcolours;
		uint32_t	bitmask_rainbow;
		uint32_t	rgb_head;
		uint32_t	rgb_torso;
		uint32_t	rgb_arms;
		uint32_t	rgb_coil;
		uint32_t	rgb_wholeset;
		uint32_t	unknown_[1];
	} PACKED;

	struct saveslot {
		charinfo	info;
		uint8_t		unknown0[0xa2c79 - (
				sizeof(info) +
				0)];
		itemcontainers	items;
		uint8_t		unknown1[0xda8d5 - (
				sizeof(info) + 
				sizeof(unknown0) +
				sizeof(items) +
				0)];
		investigation	invs[250];
		uint8_t		unknown2[0xfb9];
		equiploadout	loadouts[0x70];
		uint8_t		unknown_[0xf6110 - (
				sizeof(info) + 
				sizeof(unknown0) +
				sizeof(items) +
				sizeof(unknown1) +
				sizeof(invs) + 
				sizeof(unknown2) +
				sizeof(loadouts) +
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

	static_assert(sizeof(layout_bin::investigation) == (42), "Investigation structure size not as expected");
	static_assert(sizeof(layout_bin::savefile) == (0x3004dc + (3*0xf6110)), "Savefile structure size not as expected");
}

#endif //_LAYOUT_BIN_H_

