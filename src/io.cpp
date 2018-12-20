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

#include "io.h"
#include <openssl/blowfish.h>
#include <fstream>
#include <cstring>
#include <sstream>

// utility class, taken from https://stackoverflow.com/a/1120224/159094
class CSVRow {
	std::vector<std::string>    m_data;
    public:
        std::string const& operator[](std::size_t index) const {
            return m_data[index];
        }

        std::size_t size() const {
            return m_data.size();
        }

	void readNextRow(std::istream& str) {
		std::string         line;
		std::getline(str, line);

		std::stringstream   lineStream(line);
		std::string         cell;

		m_data.clear();
		while(true) {
			int c = lineStream.peek();
			// if the firts char is a "
			// then we need to read through
			// until we find another one
			if(c == '\"') {
				// remove first character
				lineStream.get();
				if(!std::getline(lineStream, cell, '\"'))
					break;
			} else {
				if(!std::getline(lineStream, cell, ','))
					break;
			}
			m_data.push_back(cell);
		}
		// This checks for a trailing comma with no data after it.
		if (!lineStream && cell.empty()) {
			// If there was a trailing comma then add an empty element.
			m_data.push_back("");
		}
	}
};

std::istream& operator>>(std::istream& str, CSVRow& data) {
    data.readNextRow(str);
    return str;
}

namespace io {

	const char	MHW_KEY[] = "xieZjoe#P2134-3zmaghgpqoe0z8$3azeq";

	void swap_quadbytes(buffer& inout) {
		if(inout.size()%4)
			throw std::runtime_error("buffer not a multiple of 4 bytes");
		uint8_t	tmpquad[4];
		for(size_t i = 0; i < inout.size(); i += 4) {
			std::memcpy(tmpquad, &inout[i], sizeof(uint8_t)*4);
			inout[0 + i] = tmpquad[3];
			inout[1 + i] = tmpquad[2];
			inout[2 + i] = tmpquad[1];
			inout[3 + i] = tmpquad[0];
		}
	}

	buffer read_savegame(const std::string& fname) {
		buffer	out;
		// open the file and get size
		std::ifstream	istr(fname, std::ios_base::binary);
		if(!istr)
			throw std::runtime_error("can't open savegame for reading (check path/permission)");
		const auto sz = istr.seekg(0, std::ios_base::end).tellg();
		istr.seekg(0, std::ios_base::beg);
		// read the file
		out.resize(sz);
		if(sz != istr.read((char*)&out[0], sz).gcount())
			throw std::runtime_error("can't read the whole file");
		// swap the bytes
		swap_quadbytes(out);
		// decrypt
		{
			BF_KEY	bfkey = {0};
			BF_set_key(&bfkey, std::strlen(MHW_KEY), (const unsigned char*)MHW_KEY);
			buffer	data(out.size());
			// ensure size is multiples of 64 bits
			if(sz%8)
				throw std::runtime_error("invalid buffer size to use with Blowfish encryption/decryption");
			for(size_t i = 0; i < out.size(); i += 8)
				BF_ecb_encrypt((const unsigned char*)&out[i], (unsigned char*)&data[i], &bfkey, BF_DECRYPT);
			out.swap(data);
		}
		// swap them again
		swap_quadbytes(out);
		return out;
	}

	void write_savegame(const std::string& fname, const buffer& buf) {
		buffer	tmp(buf);
		// swap the bytes
		swap_quadbytes(tmp);
		// encrypt
		{
			BF_KEY	bfkey = {0};
			BF_set_key(&bfkey, std::strlen(MHW_KEY), (const unsigned char*)MHW_KEY);
			buffer	data(tmp.size());
			// ensure size is multiples of 64 bits
			if(tmp.size()%8)
				throw std::runtime_error("invalid buffer size to use with Blowfish encryption/decryption");
			for(size_t i = 0; i < buf.size(); i += 8)
				BF_ecb_encrypt((const unsigned char*)&tmp[i], (unsigned char*)&data[i], &bfkey, BF_ENCRYPT);
			tmp.swap(data);
		}
		// swap them again
		swap_quadbytes(tmp);

		// open the file and get size
		std::ofstream	ostr(fname, std::ios_base::binary);
		if(!ostr)
			throw std::runtime_error("can't open savegame for writing (check path/permission)");
		if(tmp.size() != (size_t)ostr.write((const char*)&tmp[0], tmp.size()).tellp())
			throw std::runtime_error("can't write the whole file");
	}

	void write_dump(const std::string& fname, const buffer& rv) {
		std::ofstream	ostr(fname, std::ios_base::binary);
		if(!ostr)
			throw std::runtime_error("can't open binary dump file for writing (check path/permission)");
		if(rv.size() != (size_t)ostr.write((const char*)&rv[0], rv.size()).tellp())
			throw std::runtime_error("can't write the whole dump file");
	}

	// the items list is basically a long vector of elements
	// where we execute the lookup based on index
	struct item_desc {
		std::string	name;
	};
	std::vector<item_desc>	items;

	bool load_items_csv(const std::string& fname) {
		std::ifstream	istr(fname);
		if(!istr)
			return false;
		// read first row and lookup the columns
		CSVRow	row;
		if(!(istr >> row))
			throw std::runtime_error("items list file isn't a valid CSV - header row needed");
		const static std::string	s_col_id = "id",
		      				s_col_name = "name";
		int				i_col_id = -1,
						i_col_name = -1;
		const size_t			columns_num = row.size();
		for(int i = 0; i < (int)columns_num; ++i) {
			if(row[i] == s_col_id) i_col_id = i;
			if(row[i] == s_col_name) i_col_name = i;
		}
		if(-1 == i_col_id)
			throw std::runtime_error("invalid items list file - column \"id\" is missing");
		if(-1 == i_col_name)
			throw std::runtime_error("invalid items list file - column \"name\" is missing");
		// pre-allocate 1k elements - we should have 999
		items.resize(1024);
		// scan the CSV and add elements to the vector
		int	cur_row = 1;
		while(istr >> row) {
			if(row.size() != columns_num)
				throw std::runtime_error(std::string("malformed items list file - check row: ") + std::to_string(cur_row));
			char*		endptr = 0;
			const auto	id_num = strtol(row[i_col_id].c_str(), &endptr, 10);
			if(*endptr || (id_num < 0))
				throw std::runtime_error(std::string("malformed items list file - check column \"id\" of row: ") + std::to_string(cur_row));
			// then add them to the vector of entries
			if(id_num > 16*1024)
				throw std::runtime_error("too large number of items");
			if(items.size() < (size_t)id_num+1)
				items.resize(id_num+1);
			items[id_num].name = row[i_col_name];
			++cur_row;
		}
		return true;
	}

	const char* lookup_item(const size_t id) {
		if(id >= items.size())
			return 0;
		return items[id].name.c_str();
	}
}

