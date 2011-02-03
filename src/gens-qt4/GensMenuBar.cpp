/***************************************************************************
 * gens-qt4: Gens Qt4 UI.                                                  *
 * GensMenuBar.cpp: Gens Menu Bar class.                                   *
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

#include "GensMenuBar.hpp"

// Qt includes.
#include <QtGui/QApplication>

// Text translation macro.
#define TR(text) \
	QCoreApplication::translate("GensMenuBar", (text), NULL, QCoreApplication::UnicodeUTF8)

/**
 * QICON_FROMTHEME(): Icon loading function.
 * Qt 4.6 supports FreeDesktop.org icon themes.
 * Older versions do not, unfortunately.
 */
#if QT_VERSION >= 0x040600
#define QICON_FROMTHEME(name, fallback) \
	(QIcon::hasThemeIcon(name) ? QIcon::fromTheme(name) : QIcon(fallback))
#else
#define QICON_FROMTHEME(name, fallback) \
	QIcon(fallback)
#endif


namespace GensQt4
{

GensMenuBar::GensMenuBar(QWidget *parent)
	: QMenuBar::QMenuBar(parent)
{
	// Create the signal mapper.
	m_signalMapper = new QSignalMapper(this);
	connect(this->m_signalMapper, SIGNAL(mapped(int)),
		this, SIGNAL(triggered(int)));
	
	// Populate the menu bar.
	// TODO
	
	static const MenuItem gmiFile[] =
	{
		{IDM_FILE_OPEN, GMI_NORMAL, "&Open ROM...", NULL, MACCEL_OPEN, Qt::CTRL + Qt::Key_O, "document-open", ":/oxygen-16x16/document-open.png"},
		{IDM_FILE_CLOSE, GMI_NORMAL, "&Close ROM", NULL, MACCEL_CLOSE, Qt::CTRL + Qt::Key_W, "document-close", ":/oxygen-16x16/document-close.png"},
		{IDM_SEPARATOR, GMI_SEPARATOR, NULL, NULL, MACCEL_NONE, 0, NULL, NULL},
		{IDM_FILE_SAVESTATE, GMI_NORMAL, "&Save State", NULL, MACCEL_NONE, Qt::Key_F5, NULL, NULL},
		{IDM_FILE_LOADSTATE, GMI_NORMAL, "&Load State", NULL, MACCEL_NONE, Qt::Key_F8, NULL, NULL},
		{IDM_SEPARATOR, GMI_SEPARATOR, NULL, NULL, MACCEL_NONE, 0, NULL, NULL},
		{IDM_FILE_GENCONFIG, GMI_NORMAL, "&General Configuration", NULL, MACCEL_PREFERENCES, 0, "configure", ":/oxygen-16x16/configure.png"},
		{IDM_FILE_MCDCONTROL, GMI_NORMAL, "Sega C&D Control Panel", NULL, MACCEL_NONE, 0, "media-optical", ":/oxygen-16x16/media-optical.png"},
		{IDM_SEPARATOR, GMI_SEPARATOR, NULL, NULL, MACCEL_NONE, 0, NULL, NULL},
		{IDM_FILE_QUIT, GMI_NORMAL, "&Quit", NULL, MACCEL_QUIT, Qt::CTRL + Qt::Key_Q, "application-exit", ":/oxygen-16x16/application-exit.png"},
		
		{0, GMI_NORMAL, NULL, NULL, MACCEL_NONE, 0, NULL, NULL}
	};
	
	static const MenuItem gmiResBppTest[] =
	{
		{IDM_RESBPPTEST_1X, GMI_NORMAL, "320x240 (&1x)", NULL, MACCEL_NONE, 0, NULL, NULL},
		{IDM_RESBPPTEST_2X, GMI_NORMAL, "640x480 (&2x)", NULL, MACCEL_NONE, 0, NULL, NULL},
		{IDM_RESBPPTEST_3X, GMI_NORMAL, "960x720 (&3x)", NULL, MACCEL_NONE, 0, NULL, NULL},
		{IDM_RESBPPTEST_4X, GMI_NORMAL, "1280x960 (&4x)", NULL, MACCEL_NONE, 0, NULL, NULL},
		{IDM_SEPARATOR, GMI_SEPARATOR, NULL, NULL, MACCEL_NONE, 0, NULL, NULL},
		{IDM_RESBPPTEST_15, GMI_NORMAL, "15-bit (555)", NULL, MACCEL_NONE, 0, NULL, NULL},
		{IDM_RESBPPTEST_16, GMI_NORMAL, "16-bit (565)", NULL, MACCEL_NONE, 0, NULL, NULL},
		{IDM_RESBPPTEST_32, GMI_NORMAL, "32-bit (888)", NULL, MACCEL_NONE, 0, NULL, NULL},
		{IDM_SEPARATOR, GMI_SEPARATOR, NULL, NULL, MACCEL_NONE, 0, NULL, NULL},
		{IDM_RESBPPTEST_SCRSHOT, GMI_NORMAL, "&Screenshot", NULL, MACCEL_NONE, Qt::SHIFT + Qt::Key_Backspace, NULL, NULL},
		
		{0, GMI_NORMAL, NULL, NULL, MACCEL_NONE, 0, NULL, NULL}
	};
	
	static const MenuItem gmiCtrlTest[] =
	{
		{IDM_CTRLTEST_NONE, GMI_NORMAL, "&None", NULL, MACCEL_NONE, 0, NULL, NULL},
		{IDM_CTRLTEST_3BT, GMI_NORMAL, "&3-button", NULL, MACCEL_NONE, 0, NULL, NULL},
		{IDM_CTRLTEST_6BT, GMI_NORMAL, "&6-button", NULL, MACCEL_NONE, 0, NULL, NULL},
		{IDM_CTRLTEST_2BT, GMI_NORMAL, "&2-button", NULL, MACCEL_NONE, 0, NULL, NULL},
		{IDM_CTRLTEST_MEGAMOUSE, GMI_NORMAL, "&Mega Mouse", NULL, MACCEL_NONE, 0, NULL, NULL},
		{IDM_CTRLTEST_TEAMPLAYER, GMI_NORMAL, "&Teamplayer", NULL, MACCEL_NONE, 0, NULL, NULL},
		{IDM_CTRLTEST_4WP, GMI_NORMAL, "EA &4-Way Play", NULL, MACCEL_NONE, 0, NULL, NULL},
		{IDM_SEPARATOR, GMI_SEPARATOR, NULL, NULL, MACCEL_NONE, 0, NULL, NULL},
		{IDM_CTRLTEST_CONFIG, GMI_NORMAL, "&Configure...", NULL, MACCEL_NONE, 0, "input-gaming", NULL},
		
		{0, GMI_NORMAL, NULL, NULL, MACCEL_NONE, 0, NULL, NULL}
	};
	
	static const MenuItem gmiSoundTest[] =
	{
		{IDM_SOUNDTEST_11025, GMI_NORMAL, "&11,025 Hz", NULL, MACCEL_NONE, 0, NULL, NULL},
		{IDM_SOUNDTEST_16000, GMI_NORMAL, "1&6,000 Hz", NULL, MACCEL_NONE, 0, NULL, NULL},
		{IDM_SOUNDTEST_22050, GMI_NORMAL, "&22,050 Hz", NULL, MACCEL_NONE, 0, NULL, NULL},
		{IDM_SOUNDTEST_32000, GMI_NORMAL, "&32,000 Hz", NULL, MACCEL_NONE, 0, NULL, NULL},
		{IDM_SOUNDTEST_44100, GMI_NORMAL, "&44,100 Hz", NULL, MACCEL_NONE, 0, NULL, NULL},
		{IDM_SOUNDTEST_48000, GMI_NORMAL, "4&8,000 Hz", NULL, MACCEL_NONE, 0, NULL, NULL},
		{IDM_SEPARATOR, GMI_SEPARATOR, NULL, NULL, MACCEL_NONE, 0, NULL, NULL},
		{IDM_SOUNDTEST_MONO, GMI_NORMAL, "Mono", NULL, MACCEL_NONE, 0, NULL, NULL},
		{IDM_SOUNDTEST_STEREO, GMI_NORMAL, "Stereo", NULL, MACCEL_NONE, 0, NULL, NULL},
		
		{0, GMI_NORMAL, NULL, NULL, MACCEL_NONE, 0, NULL, NULL}
	};
	
	static const MenuItem gmiHelp[] =
	{
		{IDM_HELP_ABOUT, GMI_NORMAL, "&About Gens/GS II", NULL, MACCEL_NONE, 0, "help-about", ":/oxygen-16x16/help-about.png"},
		
		{0, GMI_NORMAL, NULL, NULL, MACCEL_NONE, 0, NULL, NULL}
	};
	
	static const MainMenuItem gmmiMain[] =
	{
		{IDM_FILE_MENU, "&File", &gmiFile[0]},
		{IDM_RESBPPTEST_MENU, "&ResBppTest", &gmiResBppTest[0]},
		{IDM_CTRLTEST_MENU, "&CtrlTest", &gmiCtrlTest[0]},
		{IDM_SOUNDTEST_MENU, "&SoundTest", &gmiSoundTest[0]},
		{IDM_HELP_MENU, "&Help", &gmiHelp[0]},
		
		{0, NULL, NULL}
	};
	
	parseMainMenu(&gmmiMain[0]);
}

GensMenuBar::~GensMenuBar()
{
	// Delete the signal mapper.
	delete m_signalMapper;
	
	// Clear the menu maps.
	clearHashTables();
}


/**
 * clearHashTables(): Clear the menu hash tables and lists.
 * - m_hashActions: Hash table of menu actions.
 * - m_lstSeparators: List of menu separators.
 * - m_hashMenus: Hash table of top-level menus.
 */
void GensMenuBar::clearHashTables(void)
{
	// TODO: Consider using QScopedPointer or QSharedPointer instead?
	
	// Actions map.
	QAction *action;
	foreach(action, m_hashActions)
		delete action;
	m_hashActions.clear();
	
	// Separators list.
	while (!m_lstSeparators.isEmpty())
		delete m_lstSeparators.takeFirst();
	
	// Menus map.
	QMenu *menu;
	foreach(menu, m_hashMenus)
		delete menu;
	m_hashMenus.clear();
}


/**
 * parseMainMenu(): Parse an array of GensMainMenuItem items.
 * @param mainMenu Pointer to the first item in the GensMainMenuItem array.
 */
void GensMenuBar::parseMainMenu(const GensMenuBar::MainMenuItem *mainMenu)
{
	QMenu *mnuSubMenu;
	
	// Clear the menu hash tables and lists.
	clearHashTables();
	
	for (; mainMenu->id != 0; mainMenu++)
	{
		// Create a new submenu.
		mnuSubMenu = new QMenu(this);
		mnuSubMenu->setTitle(TR(mainMenu->text));
		
		// Parse the menu.
		parseMenu(mainMenu->submenu, mnuSubMenu);
		
		// Add the menu to the menu bar.
		this->addMenu(mnuSubMenu);
		
		// Add the QMenu to the menus map.
		m_hashMenus.insert(mainMenu->id, mnuSubMenu);
	}
}


/**
 * parseMenuBar(): Parse an array of GensMenuItem items.
 * @param menu Pointer to the first item in the GensMenuItem array.
 * @param parent QMenu to add the menu items to.
 */
void GensMenuBar::parseMenu(const GensMenuBar::MenuItem *menu, QMenu *parent)
{
	QAction *mnuItem;
	
	for (; menu->id != 0; menu++)
	{
		// TODO: Add other menu item types.
		// For now, only GMI_NORMAL and GMI_SEPARATOR are supported.
		
		if (menu->type == GMI_SEPARATOR)
		{
			// Menu separator.
			m_lstSeparators.append(parent->addSeparator());
			continue;
		}
		
		if (menu->type != GMI_NORMAL && menu->type != GMI_CHECK)
			continue;
		
		mnuItem = new QAction(parent);
		mnuItem->setText(TR(menu->text));
		
		if (menu->type == GMI_CHECK)
			mnuItem->setCheckable(true);
		
#ifndef __APPLE__
		// Set the menu icon.
		// (This isn't done on Mac OS X, since icons in menus look out of place there.)
		mnuItem->setIcon(QICON_FROMTHEME(menu->icon_fdo, menu->icon_qrc));
#endif /* __APPLE__ */
		
		// Set the shortcut key.
		if (menu->key_std != MACCEL_NONE)
		{
			// StandardKey sequence specified.
			QKeySequence key((QKeySequence::StandardKey)menu->key_std);
			if (!key.isEmpty())
			{
				// Key sequence is valid.
				mnuItem->setShortcut(QKeySequence((QKeySequence::StandardKey)menu->key_std));
			}
			else if (menu->key_custom != 0)
			{
				// Key sequence is not valid.
				// Use the custom key sequence.
				mnuItem->setShortcut(menu->key_custom);
			}
		}
		else if (menu->key_custom != 0)
		{
			// Custom key sequence specified.
			mnuItem->setShortcut(menu->key_custom);
		}
		
		// Connect the signal to the signal mapper.
		connect(mnuItem, SIGNAL(triggered()),
			this->m_signalMapper, SLOT(map()));
		m_signalMapper->setMapping(mnuItem, menu->id);
		
		// Add the menu item to the menu.
		parent->addAction(mnuItem);
		
		// Add the QAction to the actions map.
		m_hashActions.insert(menu->id, mnuItem);
	}
}


/**
 * menuItemCheckState(): Get a menu item's check state.
 * @param id Menu item ID.
 * @return True if checked; false if not checked or not checkable.
 */
bool GensMenuBar::menuItemCheckState(int id)
{
	QHash<int, QAction*>::const_iterator iter = m_hashActions.find(id);
	if (iter == m_hashActions.end())
		return false;
	
	// TODO: Is the isCheckable() check needed?
	QAction *mnuItem = (*iter);
	if (!mnuItem->isCheckable())
		return false;
	return mnuItem->isChecked();
}


/**
 * setMenuItemCheckState(): Set a menu item's check state.
 * @param id Menu item ID.
 * @param newCheck New check state.
 * @return 0 on success; non-zero on error.
 */
int GensMenuBar::setMenuItemCheckState(int id, bool newCheck)
{
	QHash<int, QAction*>::const_iterator iter = m_hashActions.find(id);
	if (iter == m_hashActions.end())
		return 1;
	
	// TODO: Is the isCheckable() check needed?
	QAction *mnuItem = (*iter);
	if (!mnuItem->isCheckable())
		return 2;
	
	mnuItem->setChecked(newCheck);
	return 0;
}

}
