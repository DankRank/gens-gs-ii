/***************************************************************************
 * gens-qt4: Gens Qt4 UI.                                                  *
 * GensCtrlCfgWidget.hpp: Controller configuration widget.                 *
 *                                                                         *
 * Copyright (c) 2011 by David Korth.                                      *
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

#include "GensCtrlCfgWidget.hpp"

// Qt includes.
#include <QtGui/QLayout>
#include <QtGui/QGridLayout>
#include <QtGui/QLabel>
#include <QtGui/QSpacerItem>
#include <QtGui/QPushButton>
#include <QtGui/QHBoxLayout>
#include <QtCore/QSignalMapper>

// C includes. (C++ namespace)
#include <cassert>

// NUM_ELEMENTS(x)
#include "libgens/macros/common.h"

// Controller I/O manager.
#include "libgens/IoManager.hpp"
using LibGens::IoManager;

// GensKeySequenceWidget.
// TODO: Add property for "single key" and add gamepad support.
#include "GensKeySequenceWidget.hpp"
#include "GensCtrlKeyWidget.hpp"

// Controller configuration.
#include "Config/CtrlConfig.hpp"

namespace GensQt4
{

class GensCtrlCfgWidgetPrivate
{
	public:
		GensCtrlCfgWidgetPrivate(GensCtrlCfgWidget *q);
		void init(void);

		inline IoManager::IoType_t ioType(void) const;
		void setIoType(IoManager::IoType_t newIoType);

		QString buttonName_l(IoManager::ButtonName_t buttonName) const;

		void clearAllButtons(void);

	private:
		GensCtrlCfgWidget *const q;
		Q_DISABLE_COPY(GensCtrlCfgWidgetPrivate)

	public:
		IoManager::IoType_t m_ioType;
		int numButtons;		// Cached from IoManager.

		QGridLayout *layout;
		QLabel *lblButtonName[IoManager::BTNI_MAX];
		QLabel *lblKeyDisplay[IoManager::BTNI_MAX];
		GensCtrlKeyWidget *btnCfg[IoManager::BTNI_MAX];
		int btnIdx[IoManager::BTNI_MAX];
		QSignalMapper *mapperBtnCfg;

		QSpacerItem *vspcCfg;

		// "Change All", "Clear All".
		QPushButton *btnChangeAll;
		QPushButton *btnClearAll;
		QHBoxLayout *hboxOptions;
};


/***************************************
 * GensCtrlCfgWidgetPrivate functions. *
 ***************************************/

GensCtrlCfgWidgetPrivate::GensCtrlCfgWidgetPrivate(GensCtrlCfgWidget *q)
	: q(q)
	, m_ioType(IoManager::IOT_NONE)
	, numButtons(0)
	, layout(new QGridLayout(q))
	, mapperBtnCfg(new QSignalMapper(q))
{
	// Eliminate margins and reduce vertical spacing.
	layout->setContentsMargins(0, 0, 0, 0);
	layout->setVerticalSpacing(0);

	// Signal mapper for configuration buttons.
	QObject::connect(mapperBtnCfg, SIGNAL(mapped(int)),
			  q, SLOT(keyChanged_slot(int)));
}


/**
 * Initialize the grid layout.
 */
void GensCtrlCfgWidgetPrivate::init(void)
{
	// Monospaced font.
	QFont fntMonospace(QLatin1String("Monospace"));
	fntMonospace.setStyleHint(QFont::TypeWriter);

	// Add CtrlConfig::MAX_BTNS items to the grid layout.
	for (int i = 0; i < NUM_ELEMENTS(btnCfg); i++) {
		// Create the widgets.
		lblButtonName[i] = new QLabel(q);
		lblButtonName[i]->setVisible(false);
		lblKeyDisplay[i] = new QLabel(q);
		lblKeyDisplay[i]->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);
		lblKeyDisplay[i]->setVisible(false);
		lblKeyDisplay[i]->setFont(fntMonospace);

		// Configuration button.
		btnCfg[i] = new GensCtrlKeyWidget(q, lblKeyDisplay[i]);
		btnCfg[i]->setVisible(false);
		QObject::connect(btnCfg[i], SIGNAL(keyChanged(GensKey_t)),
				  mapperBtnCfg, SLOT(map()));
		mapperBtnCfg->setMapping(btnCfg[i], i);

		// Add the widgets to the grid layout.
		layout->addWidget(lblButtonName[i], i, 0, Qt::AlignLeft);
		layout->addWidget(lblKeyDisplay[i], i, 1, Qt::AlignLeft);
		layout->addWidget(btnCfg[i], i, 2, Qt::AlignRight);
	}

	// Add a vertical spacer at the bottom of the layout.
	vspcCfg = new QSpacerItem(128, 128, QSizePolicy::Expanding, QSizePolicy::Expanding);
	layout->addItem(vspcCfg, IoManager::BTNI_MAX, 1, 1, 1, Qt::AlignCenter);

	// Create the HBox.
	// TODO: Verify that this doesn't leak memory.
	hboxOptions = new QHBoxLayout();
	hboxOptions->setContentsMargins(0, 8, 0, 0); // TODO: Use style default for Top margin.
	layout->addLayout(hboxOptions, IoManager::BTNI_MAX+1, 0, 1, 3, Qt::AlignCenter);

	// Add the "Change All" and "Clear All" buttons.
	// TODO: Icons.
	btnChangeAll = new QPushButton(GensCtrlCfgWidget::tr("&Change All Buttons"), q);
	hboxOptions->addWidget(btnChangeAll);

	btnClearAll = new QPushButton(GensCtrlCfgWidget::tr("C&lear All Buttons"), q);
	QObject::connect(btnClearAll, SIGNAL(clicked(bool)),
			 q, SLOT(clearAllButtons()));
	hboxOptions->addWidget(btnClearAll);
}


/**
 * Get the current I/O device type.
 * @return Current I/O device type.
 */
inline IoManager::IoType_t GensCtrlCfgWidgetPrivate::ioType(void) const
	{ return m_ioType; }

/**
 * Set the I/O device type.
 * @param newIoType New I/O device type.
 */
void GensCtrlCfgWidgetPrivate::setIoType(IoManager::IoType_t newIoType)
{
	if (m_ioType == newIoType)
		return;

	// Save the new I/O type.
	m_ioType = newIoType;

	// Update the grid layout based on the specified controller type.
	numButtons = IoManager::NumDevButtons(newIoType);
	if (numButtons > NUM_ELEMENTS(btnCfg))
		numButtons = NUM_ELEMENTS(btnCfg);

	// Show the buttons, in logical button order.
	QString sBtnLabel;
	for (int i = 0, button = 0;
	     i < numButtons && button >= 0; i++) {
		IoManager::ButtonName_t buttonName =
					IoManager::ButtonName(newIoType, button);
		sBtnLabel = buttonName_l(buttonName) + QChar(L':');

		// Update the widgets.
		lblButtonName[i]->setText(sBtnLabel);
		lblButtonName[i]->setVisible(true);
		lblKeyDisplay[i]->setVisible(true);
		btnCfg[i]->setVisible(true);
		btnIdx[i] = button;

		// Get the next logical button. (TODO: Update for IoManager.)
		button = IoManager::NextLogicalButton(newIoType, button);
	}

	// Hide other buttons.
	for (int i = numButtons; i < NUM_ELEMENTS(btnCfg); i++) {
		lblButtonName[i]->setVisible(false);
		lblKeyDisplay[i]->setVisible(false);
		btnCfg[i]->setVisible(false);
		btnIdx[i] = -1;
	}
}


/**
 * Get a localized LibGens button name.
 * @param buttonName LibGens button name.
 * @return Localized button name, or empty string on error.
 */
QString GensCtrlCfgWidgetPrivate::buttonName_l(IoManager::ButtonName_t buttonName) const
{
	assert(buttonName > IoManager::BTNNAME_UNKNOWN && buttonName <= IoManager::BTNNAME_MAX);
	switch (buttonName) {
		// Standard controller buttons.
		case IoManager::BTNNAME_UP:
			//: Standard controller: D-Pad UP.
			return GensCtrlCfgWidget::tr("Up", "controller-standard");
		case IoManager::BTNNAME_DOWN:
			//: Standard controller: D-Pad DOWN.
			return GensCtrlCfgWidget::tr("Down", "controller-standard");
		case IoManager::BTNNAME_LEFT:
			//: Standard controller: D-Pad LEFT.
			return GensCtrlCfgWidget::tr("Left", "controller-standard");
		case IoManager::BTNNAME_RIGHT:
			//: Standard controller: D-Pad RIGHT.
			return GensCtrlCfgWidget::tr("Right", "controller-standard");
		case IoManager::BTNNAME_B:
			//: Standard controller: B button.
			return GensCtrlCfgWidget::tr("B", "controller-standard");
		case IoManager::BTNNAME_C:
			//: Standard controller: C button.
			return GensCtrlCfgWidget::tr("C", "controller-standard");
		case IoManager::BTNNAME_A:
			//: Standard controller: A button.
			return GensCtrlCfgWidget::tr("A", "controller-standard");
		case IoManager::BTNNAME_START:
			//: Standard controller: START button.
			return GensCtrlCfgWidget::tr("Start", "controller-standard");
		case IoManager::BTNNAME_Z:
			//: Standard controller: Z button.
			return GensCtrlCfgWidget::tr("Z", "controller-standard");
		case IoManager::BTNNAME_Y:
			//: Standard controller: Y button.
			return GensCtrlCfgWidget::tr("Y", "controller-standard");
		case IoManager::BTNNAME_X:
			//: Standard controller: X button.
			return GensCtrlCfgWidget::tr("X", "controller-standard");
		case IoManager::BTNNAME_MODE:
			//: Standard controller: MODE button.
			return GensCtrlCfgWidget::tr("Mode", "controller-standard");

		/** SMS/GG buttons. **/

		case IoManager::BTNNAME_1:
			//: SMS/Game Gear: 1 button.
			return GensCtrlCfgWidget::tr("1", "controller-sms-gg");
		case IoManager::BTNNAME_2:
			//: SMS/Game Gear: 2 button.
			return GensCtrlCfgWidget::tr("2", "controller-sms-gg");

		/** Sega Mega Mouse buttons. **/

		case IoManager::BTNNAME_MOUSE_LEFT:
			//: Sega Mega Mouse: LEFT mouse button.
			return GensCtrlCfgWidget::tr("Left", "controller-mouse");
		case IoManager::BTNNAME_MOUSE_RIGHT:
			//: Sega Mega Mouse: RIGHT mouse button.
			return GensCtrlCfgWidget::tr("Right", "controller-mouse");
		case IoManager::BTNNAME_MOUSE_MIDDLE:
			//: Sega Mega Mouse: MIDDLE mouse button.
			return GensCtrlCfgWidget::tr("Middle", "controller-mouse");
		case IoManager::BTNNAME_MOUSE_START:
			//: Sega Mega Mouse: START button.
			return GensCtrlCfgWidget::tr("Start", "controller-mouse");

		default:
			return QString();
	}
	
	// Should not get here...
	return QString();
}


/**
 * Clear all mapped buttons.
 */
void GensCtrlCfgWidgetPrivate::clearAllButtons(void)
{
	for (int i = 0; i < NUM_ELEMENTS(btnCfg); i++)
		btnCfg[i]->clearKey();
}


/********************************
 * GensCtrlCfgWidget functions. *
 ********************************/

GensCtrlCfgWidget::GensCtrlCfgWidget(QWidget* parent)
	: QWidget(parent)
	, d(new GensCtrlCfgWidgetPrivate(this))
{
	// Initialize the private members.
	d->init();
}

GensCtrlCfgWidget::~GensCtrlCfgWidget()
{
	delete d;
}


/**
 * Get the current I/O device type.
 * WRAPPER FUNCTION for GensCtrlCfgWidgetPrivate.
 * @return Current I/O type.
 */
IoManager::IoType_t GensCtrlCfgWidget::ioType(void) const
	{ return d->ioType(); }

/**
 * Set the current I/O device type.
 * WRAPPER FUNCTION for GensCtrlCfgWidgetPrivate.
 * @param newIoType New I/O type.
 */
void GensCtrlCfgWidget::setIoType(IoManager::IoType_t newIoType)
	{ d->setIoType(newIoType); }


/**
 * Get the current keymap.
 * @return Current keymap.
 */
QVector<GensKey_t> GensCtrlCfgWidget::keyMap(void)
{
	QVector<GensKey_t> keyMap(d->numButtons);
	for (int i = 0; i < d->numButtons; i++) {
		keyMap[i] = d->btnCfg[d->btnIdx[i]]->key();
	}

	return keyMap;
}

/**
 * Set the current keymap.
 * @param keyMap New keymap.
 */
void GensCtrlCfgWidget::setKeyMap(QVector<GensKey_t> keyMap)
{
	const int maxButtons = std::min(d->numButtons, keyMap.count());

	for (int i = 0; i < maxButtons; i++) {
		d->btnCfg[d->btnIdx[i]]->setKey(keyMap[i]);
	}

	if (maxButtons < d->numButtons) {
		// Clear the rest of the keys.
		for (int i = maxButtons; i < d->numButtons; i++) {
			d->btnCfg[i]->clearKey();
		}
	}
}


/**
 * Clear all mapped buttons.
 * WRAPPER SLOT for GensCtrlCfgWidgetPrivate.
 */
void GensCtrlCfgWidget::clearAllButtons(void)
{
	d->clearAllButtons();
}

/**
 * A key's configuration has changed.
 * @param idx Button index.
 */
void GensCtrlCfgWidget::keyChanged_slot(int idx)
{
	// Emit a signal with the new key value.
	emit keyChanged(idx, d->btnCfg[idx]->key());
}

}
