/***************************************************************************
 * gens-qt4: Gens Qt4 UI.                                                  *
 * GensMenuBar.hpp: Gens Menu Bar class.                                   *
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

#ifndef __GENS_QT4_GENSMENUBAR_HPP__
#define __GENS_QT4_GENSMENUBAR_HPP__

#include <QtGui/QMenuBar>
#include <QtGui/QMenu>
#include <QtCore/QSignalMapper>

#include <QtGui/QKeySequence>

// Menu IDs.
// TODO: Convert to enum?
// TODO: Move to separate file?
#define MNUID(menu, item) ((menu << 16) | (item))
#define MNUID_MENU(id) (id >> 16)
#define MNUID_ITEM(id) (id & 0xFFFF)

#define IDM_SEPARATOR		-1

#define IDM_FILE_MENU		1
#define IDM_FILE_BLIT		MNUID(IDM_FILE_MENU, 1)
#define IDM_FILE_QUIT		MNUID(IDM_FILE_MENU, 0xFFFF)

#define IDM_HELP_MENU		7
#define IDM_HELP_ABOUT		MNUID(IDM_HELP_MENU, 1)

#define IDM_RESTEST_MENU	64
#define IDM_RESTEST_1X		MNUID(IDM_RESTEST_MENU, 1)
#define IDM_RESTEST_2X		MNUID(IDM_RESTEST_MENU, 2)
#define IDM_RESTEST_3X		MNUID(IDM_RESTEST_MENU, 3)
#define IDM_RESTEST_4X		MNUID(IDM_RESTEST_MENU, 4)

namespace GensQt4
{

class GensMenuBar : public QMenuBar
{
	Q_OBJECT
	
	public:
		GensMenuBar(QWidget *parent = NULL);
		virtual ~GensMenuBar();
	
		enum MenuItemType
		{
			GMI_NORMAL,
			GMI_SEPARATOR,
			GMI_SUBMENU,
			
			GMI_MAX
		};
		
		struct MenuItem
		{
			int id;					// Menu identifier. (-1 == separator)
			MenuItemType type;			// Menu item type.
			const char *text;			// Menu item text.
			const MenuItem *submenu;		// First element of submenu.
			
			QKeySequence::StandardKey key_std;	// Standard key sequence. (Use QKeySequence::UnknownKey to ignore.)
			int key_custom;				// Custom key sequence. (Set key_std to QKeySequence::UnknownKey.)
			
			const char *icon_fdo;			// FreeDesktop.org icon.
			const char *icon_qrc;			// QRC icon. (Qt resources)
		};
		
		struct MainMenuItem
		{
			int id;				// Menu identifier.
			const char *text;		// Menu text.
			const MenuItem *submenu;	// First element of submenu.
		};

	protected:
		void parseMainMenu(const MainMenuItem *mainMenu);
		void parseMenu(const MenuItem *menu, QMenu *parent);
		
		QSignalMapper *m_signalMapper;
	
	signals:
		void triggered(int id);
};

}

#endif /* __GENS_QT4_GENSMENUBAR_HPP__ */
