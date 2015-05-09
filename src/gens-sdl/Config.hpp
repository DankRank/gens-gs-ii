/***************************************************************************
 * gens-sdl: Gens/GS II basic SDL frontend.                                *
 * Config.hpp: Emulator configuration.                                     *
 *                                                                         *
 * Copyright (c) 2015 by David Korth.                                      *
 *                                                                         *
 * This program is free software; you can redistribute it and/or modify it *
 * under the terms of the GNU General Public License as published by the   *
 * Free Software Foundation; either version 2 of the License, or (at your  *
 * option) any later version.                                              *
 *                                                                         *
 * This program is distributed in the hope that it will be useful, but     *
 * WITHOUT ANY WARRANTY; without even the implied warranty of              *
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the           *
 * GNU General Public License for more details.                            *
 *                                                                         *
 * You should have received a copy of the GNU General Public License along *
 * with this program; if not, write to the Free Software Foundation, Inc., *
 * 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.           *
 ***************************************************************************/

#ifndef __GENS_SDL_CONFIG_HPP__
#define __GENS_SDL_CONFIG_HPP__

#include "libgens/macros/common.h"

// Directory separator character.
#ifdef _WIN32
#define DIR_SEP_CHR '\\'
#else
#define DIR_SEP_CHR '/'
#endif

// C++ includes.
#include <string>

namespace LibGens {
	class MdFb;
	class Vdp;
}

namespace GensSdl {

/**
 * Get the configuration directory.
 * @param subdir [in, opt] If not null, append a subdirectory.
 * @return Configuration directory, or empty string on error.
 */
const std::string getConfigDir(const utf8_str *subdir = nullptr);

/**
 * Take a screenshot.
 * @param fb MdFb.
 * @param vdp Vdp. (TODO: Move required variables to MdFb.)
 * @return 0 on success; non-zero on error.
 */
int doScreenShot(const LibGens::MdFb *fb,
		 const LibGens::Vdp *vdp,
		 const utf8_str *basename);

}

#endif /* __GENS_SDL_CONFIG_HPP__ */