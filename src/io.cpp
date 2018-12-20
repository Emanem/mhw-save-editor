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
}

