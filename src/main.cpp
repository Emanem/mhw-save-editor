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

#include <iostream>
#include "io.h"
#include "layout.h"

int main(int argc, char *argv[]) {
	try {
		auto		rv = io::read_savegame("SAVEDATA1000");
		std::cout << "Basic checksum: " << layout::basic_checksum(rv) << std::endl;
		std::cout << "Steam id: " << layout::get_steamid(rv) << std::endl;
		const auto	h_data0 = layout::get_slot_data(rv, 0);
		std::wcout << "name 0: " << h_data0.name << std::endl;
		const auto	h_data1 = layout::get_slot_data(rv, 1);
		const auto	h_data2 = layout::get_slot_data(rv, 2);
	} catch(const std::exception& e) {
		std::cerr << "Exception: " << e.what() << std::endl;
	} catch(...) {
		std::cerr << "Unknown exception" << std::endl;

	}
}

