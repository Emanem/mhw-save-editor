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

	struct hunterapp {
		uint32_t	makeup2color;
		float		makeup2posx;
		float     	makeup2posy;
		float     	makeup2sizex;
		float     	makeup2sizey;
		float     	makeup2glossy;
		float     	makeup2metallic;
		uint32_t     	makeup2type;
		uint32_t     	makeup1color;
		float     	makeup1posx;
		float     	makeup1posy;
		float     	makeup1sizex;
		float     	makeup1sizey;
		float     	makeup1gloss;
		float     	makeup1metallic;
		uint32_t     	makeup1type;
		uint32_t     	lefteyecolor;
		uint32_t     	righteyecolor;
		uint32_t     	eyebrowcolor;
		uint32_t     	facialhaircolor;
		uint8_t      	eyewidth;
		uint8_t      	eyeheight;
		uint8_t     	skincolorx;
		uint8_t     	skincolory;
		uint8_t     	age;
		uint8_t     	wrinkles;
		uint8_t      	noseheight;
		uint8_t      	mouthheight;
		uint32_t     	gender;
		uint8_t     	browtype;
		uint8_t     	facetype;
		uint8_t     	eyetype;
		uint8_t     	nosetype;
		uint8_t     	mouthtype;
		uint8_t     	eyebrowtype;
		uint8_t     	eyelashlength;
		uint8_t     	facialhairtype;
		uint8_t     	unused[4];
		uint32_t     	haircolor;
		uint32_t     	clothingcolor;
		uint16_t    	hairtype;
		uint8_t     	clothingtype;
		uint8_t     	voice;
		uint32_t     	expression;
	} PACKED;

	struct palicoapp {
		uint32_t	patterncolor1;
		uint32_t	patterncolor2;
		uint32_t	patterncolor3;
		uint32_t	furcolor;
		uint32_t	lefteyecolor;
		uint32_t	righteyecolor;
		uint32_t	clothingcolor;
		float		furlength;
		float		furthickness;
		uint8_t		patterntype;
		uint8_t		eyetype;
		uint8_t		eartype;
		uint8_t		tailtype;
		uint16_t	voicetype;
		uint16_t	voicepitch;
	} PACKED;

	struct charinfo {
		uint8_t		name_utf8[64];
		uint32_t	rank;
		uint32_t	zenny;
		uint32_t	res_points;
		uint32_t	xp;
		uint32_t	playtime;
		uint8_t		unknown0[4];
		hunterapp	hunter;
		palicoapp	palico;
	} PACKED;

	struct guildcard {
		struct creatures8 {
			uint8_t     great_jagras;
			uint8_t     kulu_ya_ku;
			uint8_t     pukei_pukei;
			uint8_t     barroth;
			uint8_t     jyuratodus;
			uint8_t     tobi_kadachi;
			uint8_t     anjanath;
			uint8_t     rathian;
			uint8_t     tzitzi_ya_ku;
			uint8_t     paolumu;
			uint8_t     great_girros;
			uint8_t     radobaan;
			uint8_t     legiana;
			uint8_t     odogaron;
			uint8_t     rathalos;
			uint8_t     diablos;
			uint8_t     kirin;
			uint8_t     zorah_magdaros;
			uint8_t     dodogama;
			uint8_t     pink_rathian;
			uint8_t     bazelgeuse;
			uint8_t     lavasioth;
			uint8_t     uragaan;
			uint8_t     azure_rathalos;
			uint8_t     black_diablos;
			uint8_t     nergigante;
			uint8_t     teostra;
			uint8_t     kushala_daora;
			uint8_t     vaal_hazak;
			uint8_t     xenojiiva;
			uint8_t     deviljho;
			uint8_t     unused[0x21];
		} PACKED;

		struct creatures16 {
			uint16_t    great_jagras;
			uint16_t    kulu_ya_ku;
			uint16_t    pukei_pukei;
			uint16_t    barroth;
			uint16_t    jyuratodus;
			uint16_t    tobi_kadachi;
			uint16_t    anjanath;
			uint16_t    rathian;
			uint16_t    tzitzi_ya_ku;
			uint16_t    paolumu;
			uint16_t    great_girros;
			uint16_t    radobaan;
			uint16_t    legiana;
			uint16_t    odogaron;
			uint16_t    rathalos;
			uint16_t    diablos;
			uint16_t    kirin;
			uint16_t    zorah_magdaros;
			uint16_t    dodogama;
			uint16_t    pink_rathian;
			uint16_t    bazelgeuse;
			uint16_t    lavasioth;
			uint16_t    uragaan;
			uint16_t    azure_rathalos;
			uint16_t    black_diablos;
			uint16_t    nergigante;
			uint16_t    teostra;
			uint16_t    kushala_daora;
			uint16_t    vaal_hazak;
			uint16_t    xenojiiva;
			uint16_t    deviljho;
			uint16_t    unused[0x21];
		} PACKED;

		struct weaponusage {
			uint16_t    gs;
			uint16_t    ls;
			uint16_t    sns;
			uint16_t    db;
			uint16_t    hammer;
			uint16_t    hh;
			uint16_t    lance;
			uint16_t    gl;
			uint16_t    sa;
			uint16_t    cb;
			uint16_t    ig;
			uint16_t    lbg;
			uint16_t    hbg;
			uint16_t    bow;
		} PACKED;

		struct arenastats {
			struct arenarecord {
				uint8_t		unknown[0x04];
				uint8_t		partnername[0x20];
				uint64_t	partnersteamid;
				int64_t		partnercreated;
				int64_t		date;
			} PACKED;
			uint16_t	unknown;
			arenarecord	records[5];
		} PACKED;

		uint64_t	steamid;
		int64_t		created;
		uint8_t		unknown0;
		uint32_t	hunterrank;
		uint32_t	playtime_s;
		int64_t		lastupdated;
		uint8_t		name_utf8[0x40];
		uint8_t		primarygroup[0x36];
		uint8_t		unknown1[0x10];
		hunterapp	hunterappearance;
		palicoapp	palicoappearance;
		uint32_t	weapontype;
		uint32_t	weaponid;
		uint32_t	headarmortype;
		uint32_t	headarmorid;
		uint32_t	chestarmortype;
		uint32_t	chestarmorid;
		uint32_t	armarmortype;
		uint32_t	armarmorid;
		uint32_t	waistarmortype;
		uint32_t	waistarmorid;
		uint32_t	legarmortype;
		uint32_t	legarmorid;
		uint32_t	charmtype;
		uint32_t	charmid;
		uint32_t	tool1type;
		uint32_t	tool1id;
		uint32_t	tool2type;
		uint32_t	tool2id;
		uint8_t		unknown2[0x5c];
		uint8_t		paliconame[0x40];
		uint32_t	palicorank_minus_1;
		uint32_t	palicohealth;
		uint32_t	palicoattm;
		uint32_t	palicoattr;
		uint32_t	palicoaffinity;
		uint32_t	palicodef;
		int32_t		palicovsfire;
		int32_t		palicovswater;
		int32_t		palicovsthunder;
		int32_t		palicovsice;
		int32_t		palicovsdragon;
		uint8_t   	unknown3;
		uint32_t	palicoweapontype;
		uint32_t	palicoweaponid;
		uint32_t	palicoheadarmortype;
		uint32_t	palicoheadarmorid;
		uint32_t	palicobodyarmortype;
		uint32_t	palicobodyarmorid;
		uint32_t	palicogadgettype;
		uint32_t	palicogadgetid;
		uint8_t		unknown4[0x04];
		uint8_t		palicog1;
		uint8_t		palicog2;
		uint8_t		palicog3;
		uint8_t		palicog4;
		uint8_t		palicog5;
		uint8_t		palicog6;
		uint32_t	unity;
		uint8_t		unknown5[0x10];
		uint16_t	questslr;
		uint16_t	questshr;
		uint16_t	questsinvest;
		uint16_t	questsarena;
		uint32_t	tailraiderunity[0x05];
		uint8_t		unknown6[0x0f];
		weaponusage	lowrank;
		weaponusage	highrank;
		weaponusage	investigations;
		uint8_t		poseid;
		uint8_t		expressionid;
		uint8_t		backgroundid;
		uint8_t		stickerid;
		uint8_t		greeting[0x0100];
		uint8_t		title[0x0100];
		uint16_t	titlefirst;
		uint16_t	titlemiddle;
		uint16_t	titlelast;
		float		positionx;   // -60 to 60
		float		positiony;   // -70 to 70
		float		zoom;        // -50 to 180
		arenastats	arenarecords[0x0a];
		creatures16	captured;
		creatures16	slayed;
		creatures16	largest;
		creatures16	smallest;
		creatures8	researchlevel;
	} PACKED;

	struct itemlist {
		uint32_t	id;
		uint32_t	number;
	} PACKED;

	struct itemloadouts {
		struct loadout {
			struct       shortcuts {
				struct items {
					uint32_t	category_id;
					uint32_t	entry_id;
					uint32_t        unknown;
				} PACKED;
				uint8_t	name_utf8[32];
				items	circle[8];
			} PACKED;
			uint8_t		name_utf8[32];
			itemlist	default_pouch_items[24];
			itemlist	default_pouch_ammo[16];
			uint8_t		unknown0[264];
			shortcuts	top_left_right_bottom[4];
		} PACKED;

		loadout		list[56];
		uint8_t		sort_idx[56];
	} PACKED;

	struct itemcontainers {
		itemlist	pouch_item[24];
		itemlist	pouch_ammo[16];
		uint8_t		unknown0[288];
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

	struct bounties_data {
		int32_t		listed[6];
		uint32_t	progress[6];
	} PACKED;

	struct equiploadout {
		uint32_t	slotid;
		uint8_t		name_utf8[256];
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

	struct inventoryitem {
		uint32_t	type;	// 0 armor, 1 weapon, 2 charm
		uint32_t	slot;	// 0 head, 1 chest, 2 arms, 3 waist, 4 legs, ... if armor
					// otherwise weapon type
		uint32_t	id;	// item id
		uint8_t		unknown[56];
	} PACKED;

	struct harvestdata {
		itemlist	harvestable_items[3]; 	// the quantity are the turns harvested for (increasing)
		itemlist	active_fertilizers[5];	// id and how many turns is going to be on (decreasing)
		itemlist	harvestbox[30];
	} PACKED;

	struct saveslot {
		charinfo	info;
		guildcard	huntergc;
		guildcard	othersgc[100];
		uint8_t		unknown0[106038];
		itemloadouts	item_loadouts;
		uint8_t		unknown1[8];
		itemcontainers	items;
		uint8_t		unknown2[4];
		inventoryitem	inventory[1000];
		uint8_t		unknown3[102676];
		uint32_t	inventory_idx[1000];
		uint8_t		unknown4[41956];
		investigation	invs[250];
		uint8_t		unknown5[1872];
		bounties_data	bounties;
		uint8_t		unknown6[2105];
		equiploadout	equip_loadouts[112];
		uint8_t		unknown7[7008];
		harvestdata	harvestbox;
		uint8_t		unknown8[32];
		uint8_t		harvest_flag; // set to 1 when fertilizer has been used
		uint8_t		unknown_[29901];
	} PACKED;

	struct savefile {
		uint32_t	magicnum;
		uint8_t		unknown0[8];
		uint8_t		sha1[20];
		uint8_t		unknown1[8];
		int64_t		steamid;
		uint8_t		unknown_[3146924];
		saveslot	slots[3];
	} PACKED;

	static_assert(sizeof(layout_bin::investigation) == (42), "Investigation structure size not as expected");
	static_assert(sizeof(layout_bin::savefile) == (0x3004dc + (3*0xf6110)), "Savefile structure size not as expected");
}

#endif //_LAYOUT_BIN_H_

