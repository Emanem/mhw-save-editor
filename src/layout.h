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

#ifndef _LAYOUT_H_
#define _LAYOUT_H_

#include "io.h"

namespace layout {
	// declarations
	struct hunter_data {
		wchar_t		name[64];
		uint32_t	rank,
				zenny,
				res_points,
				xp,
				playtime,
				gender;
	};

	typedef std::vector<std::pair<uint32_t, uint32_t>>	items_list;

	enum ITEMS_CONTAINER {
		pouch_item = 0,
		pouch_ammo,
		box_item,
		box_ammo,
		box_materials,
		box_decos,
		last
	};

	struct items_data {
		items_list		containers[ITEMS_CONTAINER::last];
		static const char	*names[ITEMS_CONTAINER::last];
	};

	struct inv_info {
		bool		selected,
				seen;
		uint8_t		locale,
				rank;
		uint32_t	attempts,
				mon1,
				mon2,
				mon3;

	};

	typedef std::vector<inv_info>	inv_data;

	bool		basic_checksum(const io::buffer& buf);
	int64_t 	get_steamid(const io::buffer& buf);
	hunter_data	get_slot_data(const io::buffer& buf, const size_t slot_id);
	items_data	get_items_data(const io::buffer& buf, const size_t slot_id);
	inv_data	get_inv_data(const io::buffer& buf, const size_t slot_id);
}

#endif //_LAYOUT_H_

