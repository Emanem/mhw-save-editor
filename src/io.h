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

#ifndef	_IO_H_ 
#define _IO_H_

#include <string>
#include <vector>
#include <cstdint>

namespace io {
	// declarations
	typedef std::vector<uint8_t>	buffer;

	buffer	read_savegame(const std::string& fname);
	void	write_savegame(const std::string& fname, const buffer& buf);
}

#endif //_IO_H_

