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
#include <getopt.h>
#include <cstring>
#include "io.h"
#include "layout.h"

namespace {
	static const char	VERSION[] = "0.0.1";

	enum LIST_TYPE {
		basic = 0,
		all,
		dump
	} list_type = LIST_TYPE::basic;

	enum SLOT_ID {
		s_all = -1,
		s0 = 0,
		s1,
		s2,
	} slot_id = SLOT_ID::s_all;

	std::string	outfile,
			items_file = "items.csv";

	// settings/options management
	void print_help(const char *prog, const char *version) {
		std::cerr <<	"Usage: " << prog << " [options] ... [savefile]\nExecutes mhw-save-editor " << version << "\n\n"
				"-l, --list             lists the basic save information for all the slots or\n"
				"                       the specified slot\n"
				"    --list-all         lists all known information for a given slot (if\n"
				"                       no slot specified, lists for all slots)\n"
				"    --items-list (csv) specify a custom csv file with fir row header, containing\n"
				"                       at least columns \"id\" (numerical identifier) and\n"
				"                       \"name\" (name) of items. by default tries to load the\n"
				"                       file named 'items.csv'\n"
				"-d, --dump (file)      dumps the decrypted information in the specified file\n"
				"-s, --slot (n)         specify which slot to select (0, 1 or 2)\n"
				"    --help             prints this help and exit\n\n"
		<< std::flush;
	}

	int parse_args(int argc, char *argv[], const char *prog, const char *version) {

		int			c;
		static struct option	long_options[] = {
			{"dump",		required_argument, 0,	'd'},
			{"help",		no_argument,	   0,	0},
			{"items-list",		required_argument, 0,	0},
			{"slot",		required_argument, 0,	's'},
			{"list-all",		no_argument, 	   0,	0},
			{0, 0, 0, 0}
		};

		while (1) {
			// getopt_long stores the option index here
			int		option_index = 0;

			if(-1 == (c = getopt_long(argc, argv, "d:s:", long_options, &option_index)))
				break;

			switch (c) {
				case 0: {
					// If this option set a flag, do nothing else now
					if (long_options[option_index].flag != 0)
					break;
					if(!std::strcmp("list-all", long_options[option_index].name)) {
						list_type = LIST_TYPE::all;
					} if(!std::strcmp("items-list", long_options[option_index].name)) {
						items_file = optarg;
					} else if(!std::strcmp("help", long_options[option_index].name)) {
						print_help(prog, version);
						std::exit(0);
					}
				} break;

				case 'd': {
					outfile = optarg;
					list_type = LIST_TYPE::dump;
				} break;

				case 's': {
					const int slot = std::atoi(optarg);
					switch(slot) {
						case 0:
						slot_id = SLOT_ID::s0;
						break;

						case 1:
						slot_id = SLOT_ID::s1;
						break;

						case 2:
						slot_id = SLOT_ID::s2;
						break;

						default:
						throw std::runtime_error("invalid slot id specified (use 0, 1, 2 or don't specify this option for 'all')");
					}
				} break;

				case '?':
				break;

				default:
					throw std::runtime_error(std::string("Invalid option '") + ((char)c) + "'");
				break;
			}
		}

		return optind;
	}

	void print_basic_slot(const SLOT_ID slot, const io::buffer& data) {
		const auto	hd = layout::get_slot_data(data, slot);
		const auto	namelen = std::wcslen(hd.name);
		if(!namelen) {
			std::cout << "\t" << "(no info)" << std::endl;
			return;
		}

		std::wcout << "\t" << "Name:\t" << hd.name << '\n';
		std::cout  << "\t" << "Rank:\t" << hd.rank << '\n'
			   << "\t" << "Zenny:\t" << hd.zenny << '\n'
			   << "\t" << "Research:\t" << hd.res_points << '\n'
			   << "\t" << "Experience:\t" << hd.xp << '\n'
			   << "\t" << "Playtime:\t" << hd.playtime << '\n'
			   << "\t" << "Gender:\t" << ((hd.gender==1) ? 'F' : 'M') << '\n'
			   << std::endl;
	}

	void print_items_slot(const SLOT_ID slot, const io::buffer& data) {
		// try to load the csv file with item listings
		static bool loaded_csv = false;
		if(!loaded_csv) {
			if(!io::load_items_csv(items_file))
				std::cerr << "warning: couldn't find items csv file (\"" << items_file << "\")" << std::endl;
			loaded_csv = true;
		}

		const auto	items = layout::get_items_data(data, slot);
		for(size_t i = 0; i < layout::ITEMS_CONTAINER::last; ++i) {
			const auto&	cur_items = items.containers[i];
			std::cout << "\t" << layout::items_data::names[i] << '\n';
			for(const auto& item_desc : cur_items) {
				if(0 == item_desc.first)
					continue;
				const char*	item_name = io::lookup_item(item_desc.first);
				if(item_name)
					std::cout << "\t\t" << item_name << ", " << item_desc.second << '\n';
				else
					std::cout << "\t\t" << item_desc.first << ", " << item_desc.second << '\n';
			}
			std::cout << std::endl;
		}
	}

	template<typename func>
	void print_data(const SLOT_ID slot, const io::buffer& data, func&& f) {
		switch(slot) {
			case SLOT_ID::s0:
			case SLOT_ID::s1:
			case SLOT_ID::s2:
			std::cout  << "Slot: " << slot << std::endl;
			f(slot, data);
			break;

			default:
			for(int i = 0; i < 3; i++) {
				std::cout  << "Slot: " << (SLOT_ID)i << std::endl;
				f((SLOT_ID)i, data);
			}
			break;
		}
	}
}


int main(int argc, char *argv[]) {
	try {
		const int	savefile = parse_args(argc, argv, argv[0], VERSION);
		if(savefile >= argc)
			throw std::runtime_error("input [savefile] not specified");
		auto		rv = io::read_savegame(argv[savefile]);
		std::cout << "Basic checksum: " << layout::basic_checksum(rv) << '\n'
			  << "Steam id:       " << layout::get_steamid(rv) << '\n'
			  << std::endl;
		// print out dedicated info
		switch(list_type) {
			case LIST_TYPE::dump: {
				io::write_dump(outfile, rv);
			} break;

			case LIST_TYPE::all:
			print_data(slot_id, rv, [](const SLOT_ID slot, const io::buffer& data) -> void { print_basic_slot(slot, data); print_items_slot(slot, data); } );
			break;

			case LIST_TYPE::basic:
			default:
			print_data(slot_id, rv, [](const SLOT_ID slot, const io::buffer& data) -> void { print_basic_slot(slot, data); } );
			break;
		}
	} catch(const std::exception& e) {
		std::cerr << "Exception: " << e.what() << std::endl;
	} catch(...) {
		std::cerr << "Unknown exception" << std::endl;

	}
}

