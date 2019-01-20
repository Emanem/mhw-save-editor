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
	static const char	VERSION[] = "0.0.2";

	enum LIST_TYPE {
		basic = 0,
		all,
		dump,
		mask_dump,
		items,
		inv,
		decos,
		edit
	} list_type = LIST_TYPE::basic;

	enum SLOT_ID {
		s_all = -1,
		s0 = 0,
		s1,
		s2,
	} slot_id = SLOT_ID::s_all;

	std::string	outfile,
			items_file = "items.csv";

	struct ADD_DATA {
		char	inv_type;
		int	item_id;
	} add_data = { 0, -1 };

	void parse_add_data(const char* d) {
		const char*	sep = strchr(d, ':');
		if(!sep)
			throw std::runtime_error("separator ':' missing in the add request");

		const std::string	type(d, sep);

		if (type == "d") add_data.inv_type ='d';
		else if (type == "m") add_data.inv_type = 'm';
		else throw std::runtime_error("invalid type in the add request (specify 'd' or 'm')");

		const int		item_id = atoi(sep+1);

		if(item_id <= 0)
			throw std::runtime_error("invalid item id in the add request");
	}

	// settings/options management
	void print_help(const char *prog, const char *version) {
		std::cerr <<	"Usage: " << prog << " [options] ... [savefile]\nExecutes mhw-save-editor " << version << "\n\n"
				"-b, --list-basic       lists the basic save information for all the slots or\n"
				"                       the specified slot\n"
				"-l, --list-all         lists all known information for a given slot (if\n"
				"                       no slot specified, lists for all slots)\n"
				"    --items-list (csv) specify a custom csv file with fir row header, containing\n"
				"                       at least columns \"id\" (numerical identifier) and\n"
				"                       \"name\" (name) of items. by default tries to load the\n"
				"                       file named 'items.csv'\n"
				"    --list-items       lists only the items (using custom csv - see '--items-list'\n"
				"                       option)\n"
				"    --list-inv         lists only investigations\n"
				"    --list-decos       lists all the decorations (both in builds and in inventory)\n"
				"-a, --add (type:item)  adds a single item of the type specified in appropriate container.\n"
				"                       Note that now onlt decorations (type 'd') and materials (type 'm')\n"
				"                       are currently supported, and the item specified will be added to\n"
				"                       specific inventory slot; item needs to be specified using the numeric\n"
				"                       identifier - example '-a d:741' will a Vitality decoration (see file\n"
				"                       items.csv for a numeric list of items)\n"
				"-d, --dump (file)      dumps the decrypted information in the specified file\n"
				"    --mask-dump (file) dumps the decrypted information in the specified file, by masking the\n"
				"                       know layout with its own description. Usefuly for understanding\n"
				"                       the file format\n"
				"-s, --slot (n)         specify which slot to select (0, 1 or 2)\n"
				"    --help             prints this help and exit\n\n"
		<< std::flush;
	}

	int parse_args(int argc, char *argv[], const char *prog, const char *version) {

		int			c;
		const static struct option	long_options[] = {
			{"add",			required_argument, 0,	'a'},
			{"dump",		required_argument, 0,	'd'},
			{"mask-dump",		required_argument, 0,	0},
			{"help",		no_argument,	   0,	0},
			{"items-list",		required_argument, 0,	0},
			{"slot",		required_argument, 0,	's'},
			{"list-basic",		no_argument,	   0,	'b'},
			{"list-all",		no_argument, 	   0,	'l'},
			{"list-items",		no_argument, 	   0,	0},
			{"list-inv",		no_argument, 	   0,	0},
			{"list-decos",		no_argument,	   0,	0},
			{0, 0, 0, 0}
		};

		bool	action_set = false;
		auto	fn_verify_action = [&]() -> void {
			if(action_set)
				throw std::runtime_error("another action (such as 'a', 'l', 'd' ...) has been already specified");
			action_set = true;
		};

		while (1) {
			// getopt_long stores the option index here
			int		option_index = 0;

			if(-1 == (c = getopt_long(argc, argv, "a:bd:ls:", long_options, &option_index)))
				break;

			switch (c) {
				case 0: {
					// If this option set a flag, do nothing else now
					if (long_options[option_index].flag != 0)
					break;
					if(!std::strcmp("mask-dump", long_options[option_index].name)) {
						outfile = optarg;
						list_type = LIST_TYPE::mask_dump;
						fn_verify_action();
					} else if(!std::strcmp("list-items", long_options[option_index].name)) {
						list_type = LIST_TYPE::items;
						fn_verify_action();
					} else if(!std::strcmp("list-inv", long_options[option_index].name)) {
						list_type = LIST_TYPE::inv;
						fn_verify_action();
					} else if(!std::strcmp("list-decos", long_options[option_index].name)) {
						list_type = LIST_TYPE::decos;
						fn_verify_action();
					} else if(!std::strcmp("items-list", long_options[option_index].name)) {
						items_file = optarg;
					} else if(!std::strcmp("help", long_options[option_index].name)) {
						print_help(prog, version);
						std::exit(0);
					}
				} break;

				case 'a': {
					list_type = LIST_TYPE::edit;
					parse_add_data(optarg);
					fn_verify_action();
				} break;

				case 'l': {
					list_type = LIST_TYPE::all;
					fn_verify_action();
				} break;

				case 'd': {
					outfile = optarg;
					list_type = LIST_TYPE::dump;
					fn_verify_action();
				} break;

				case 'b': {
					list_type = LIST_TYPE::basic;
					fn_verify_action();
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

	void load_items_csv(void) {
		// try to load the csv file with item listings
		static bool loaded_csv = false;
		if(!loaded_csv) {
			if(!io::load_items_csv(items_file))
				std::cerr << "warning: couldn't find items csv file (\"" << items_file << "\")" << std::endl;
			loaded_csv = true;
		}
	}

	void print_items_slot(const SLOT_ID slot, const io::buffer& data) {
		load_items_csv();

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

	void print_inv_slot(const SLOT_ID slot, const io::buffer& data) {
		const auto	items = layout::get_inv_data(data, slot);
		std::cout << "\t" << "investigations" << '\n';
		for(const auto& i : items) {
			std::cout << "\t\t"	<< i.selected << "," << i.seen << "," << (int)i.locale << "," << i.attempts << ","
						<< (int)i.rank << "," << i.mon1 << "," << i.mon2 << "," << i.mon3
						<< '\n';
		}
	}

	void print_deco_slot(const SLOT_ID slot, const io::buffer& data) {
		load_items_csv();

		const auto	decos = layout::get_all_decos_data(data, slot);
		for(const auto& i : decos) {
			std::cout << "\t" << i.first << '\n';
			for(const auto& item_desc : i.second) {
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

			case LIST_TYPE::mask_dump: {
				layout::mask_known_buffer(rv);
				io::write_dump(outfile, rv);
			} break;

			case LIST_TYPE::all:
			print_data(slot_id, rv, [](const SLOT_ID slot, const io::buffer& data) -> void { print_basic_slot(slot, data); print_items_slot(slot, data); print_inv_slot(slot, data); } );
			break;

			case LIST_TYPE::items:
			print_data(slot_id, rv, [](const SLOT_ID slot, const io::buffer& data) -> void { print_items_slot(slot, data); } );
			break;

			case LIST_TYPE::inv:
			print_data(slot_id, rv, [](const SLOT_ID slot, const io::buffer& data) -> void { print_inv_slot(slot, data); } );
			break;

			case LIST_TYPE::decos:
			print_data(slot_id, rv, [](const SLOT_ID slot, const io::buffer& data) -> void { print_deco_slot(slot, data); } );
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

