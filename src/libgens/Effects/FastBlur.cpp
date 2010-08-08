/***************************************************************************
 * libgens: Gens Emulation Library.                                        *
 * FastBlur.cpp: Fast Blur effect.                                         *
 *                                                                         *
 * Copyright (c) 1999-2002 by Stéphane Dallongeville.                      *
 * Copyright (c) 2003-2004 by Stéphane Akhoun.                             *
 * Copyright (c) 2008-2010 by David Korth.                                 *
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

/**
 * NOTE: The video effects here are applied to MD_Screen[].
 */

#include "FastBlur.hpp"
#include "MD/VdpRend.hpp"
#include "Util/cpuflags.h"

// C includes.
#include <math.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>

// Mask constants.
#define MASK_DIV2_15		((uint16_t)(0x3DEF))
#define MASK_DIV2_16		((uint16_t)(0x7BCF))
#define MASK_DIV2_32		((uint32_t)(0x007F7F7F))

namespace LibGens
{

/**
 * T_DoFastBlur: Apply a fast blur effect to the screen buffer.
 * @param mask MSB mask for pixel data.
 * @param mdScreen Screen buffer.
 */
template<typename pixel, pixel mask>
inline void FastBlur::T_DoFastBlur(pixel *mdScreen)
{
	pixel px = 0, px_prev = 0;
	
	// Start at the 8th pixel.
	// MD screen has an 8-pixel-wide buffer at the left-most side.
	mdScreen += 8;
	
	// Process the framebuffer.
	for (unsigned int i = ((336*240)-16); i != 0; i--)
	{
		px = (*mdScreen >> 1) & mask;	// Get pixel.
		px_prev += px;			// Blur with previous pixel.
		*(mdScreen - 1) = px_prev;	// Write new pixel.
		px_prev = px;			// Save pixel.
		
		// Increment the MD screen pointer.
		mdScreen++;
	}
}


#ifdef HAVE_MMX
const uint32_t FastBlur::MASK_DIV2_15_MMX[2] = {0x3DEF3DEF, 0x3DEF3DEF};
const uint32_t FastBlur::MASK_DIV2_16_MMX[2] = {0x7BCF7BCF, 0x7BCF7BCF};
const uint32_t FastBlur::MASK_DIV2_32_MMX[2] = {0x007F7F7F, 0x007F7F7F};

void FastBlur::DoFastBlur_32_MMX(uint32_t *mdScreen)
{
	// Start at the 8th pixel.
	// MD screen has an 8-pixel-wide buffer at the left-most side.
	mdScreen += 8;
	
	// Load the 32-bit color mask.
	__asm__ (
		"movq (%0), %%mm7"
		:
		: "m" (MASK_DIV2_32_MMX[0])
		);
	
	// Blur the pixels.
	for (unsigned int i = ((336*240)-16)/2; i != 0; i--)
	{
		__asm__ (
			/* Get source pixels. */
			"movq	 (%0), %%mm0\n"
			"movq	4(%0), %%mm1\n"
			
			/* Blur source pixels. */
			"psrld	$1, %%mm0\n"
			"psrld	$1, %%mm1\n"
			"pand	%%mm7, %%mm0\n"
			"pand	%%mm7, %%mm1\n"
			"paddd	%%mm1, %%mm0\n"
			
			/* Put destination pixels. */
			"movq	%%mm0, (%0)\n"
			:
			: "r" (mdScreen)
			);
		
		// Next group of pixels.
		mdScreen += 2;
	}
	
	// Reset MMX state.
	__asm__ ("emms");
}

#endif /* HAVE_MMX */


/**
 * DoFastBlur(): Apply a fast blur effect to the screen buffer.
 * @param outScreen Output screen. (MUST BE 336x240 with enough space for the current color depth!)
 * @param fromMdScreen If true, uses VdpRend::MD_Screen[] as the source buffer.
 * Otherwise, outScreen is used as both source and destination.
 */
void FastBlur::DoFastBlur(void *outScreen, bool fromMdScreen)
{
	if (fromMdScreen)
	{
		// Copy MD_Screen[] to outScreen.
		switch(VdpRend::Bpp)
		{
			case VdpRend::BPP_15:
			case VdpRend::BPP_16:
				memcpy(outScreen, VdpRend::MD_Screen.u16, sizeof(VdpRend::MD_Screen.u16));
				break;
			
			case VdpRend::BPP_32:
			default:
				memcpy(outScreen, VdpRend::MD_Screen.u32, sizeof(VdpRend::MD_Screen.u32));
				break;
		}
	}
	
	switch (VdpRend::Bpp)
	{
		case VdpRend::BPP_15:
			T_DoFastBlur<uint16_t, MASK_DIV2_15>((uint16_t*)outScreen);
			break;
		case VdpRend::BPP_16:
			T_DoFastBlur<uint16_t, MASK_DIV2_16>((uint16_t*)outScreen);
			break;
		case VdpRend::BPP_32:
		default:
#ifdef HAVE_MMX
			if (CPU_Flags & MDP_CPUFLAG_X86_MMX)
				DoFastBlur_32_MMX((uint32_t*)outScreen);
			else
#endif /* HAVE_MMX */
				T_DoFastBlur<uint32_t, MASK_DIV2_32>((uint32_t*)outScreen);
			break;
	}
}

}
