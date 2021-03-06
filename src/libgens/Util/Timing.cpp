/***************************************************************************
 * libgens: Gens Emulation Library.                                        *
 * Timing.hpp: Timing functions.                                           *
 *                                                                         *
 * Copyright (c) 1999-2002 by Stéphane Dallongeville.                      *
 * Copyright (c) 2003-2004 by Stéphane Akhoun.                             *
 * Copyright (c) 2008-2015 by David Korth.                                 *
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

#include "Timing.hpp"

namespace LibGens {

/**
 * Get the name of a timing method.
 * @param tMethod Timing method.
 * @return Timing method name. (ASCII)
 */
const char *Timing::GetTimingMethodName(TimingMethod tMethod)
{
	switch (tMethod) {
		default:
			return "(unknown)";
		case TM_GETTIMEOFDAY:
			return "gettimeofday";
		case TM_CLOCK_GETTIME:
			return "clock_gettime";
		case TM_GETTICKCOUNT:
			return "GetTickCount";
		case TM_GETTICKCOUNT64:
			return "GetTickCount64";
		case TM_QUERYPERFORMANCECOUNTER:
			return "QueryPerformanceCounter";
		case TM_MACH_ABSOLUTE_TIME:
			return "mach_absolute_time";
	}
}

}
