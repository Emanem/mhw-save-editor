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

	bool		basic_checksum(const io::buffer& buf);
	int64_t 	get_steamid(const io::buffer& buf);
	hunter_data	get_slot_data(const io::buffer& buf, const size_t slot_id);
}

#endif //_LAYOUT_H_

