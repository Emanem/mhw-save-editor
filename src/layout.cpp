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

namespace layout {
	
	bool basic_checksum(const io::buffer& buf) {
		const static uint8_t	CHECKSUM[] = { 0x01, 0x00, 0x00, 0x00 };
		return !std::memcmp(&buf[0], CHECKSUM, sizeof(uint8_t)*sizeof(CHECKSUM));
	}

	int64_t get_steamid(const io::buffer& buf) {
		const static size_t	STEAMID_OFFSET = 0x28;
		int64_t	out = 0;
		std::memcpy(&out, &buf[STEAMID_OFFSET], sizeof(int64_t));
		return out;
	}

	hunter_data get_slot_data(const io::buffer& buf, const size_t slot_id) {
		const static size_t	SAVESLOT_OFFSET = 0x003004DC,
		      			SAVESLOT_SIZE = 0xF6110;

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
}

