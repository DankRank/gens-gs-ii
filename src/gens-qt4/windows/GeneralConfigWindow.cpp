/******************************************************************************
 * gens-qt4: Gens Qt4 UI.                                                     *
 * GeneralConfigWindow.cpp: General Configuration Window.                     *
 *                                                                            *
 * Copyright (c) 1999-2002 by Stéphane Dallongeville.                         *
 * Copyright (c) 2003-2004 by Stéphane Akhoun.                                *
 * Copyright (c) 2008-2011 by David Korth.                                    *
 *                                                                            *
 * This program is free software; you can redistribute it and/or modify       *
 * it under the terms of the GNU General Public License as published by       *
 * the Free Software Foundation; either version 2 of the License, or          *
 * (at your option) any later version.                                        *
 *                                                                            *
 * This program is distributed in the hope that it will be useful,            *
 * but WITHOUT ANY WARRANTY; without even the implied warranty of             *
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the              *
 * GNU General Public License for more details.                               *
 *                                                                            *
 * You should have received a copy of the GNU General Public License          *
 * along with this program; if not, write to the Free Software                *
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA *
 ******************************************************************************/

#include "GeneralConfigWindow.hpp"
#include "gqt4_main.hpp"
#include "GensQApplication.hpp"

// C includes.
#include <stdint.h>
#include <math.h>

// zlib
#include <zlib.h>

// Qt4 includes.
#include <QtGui/QFileDialog>
#include <QtGui/QColorDialog>
#include <QtGui/QPainter>
#include <QtGui/QKeyEvent>

// libgens: RAR decompressor
#include "libgens/Decompressor/DcRar.hpp"

// EmuManager is needed for region code strings.
#include "EmuManager.hpp"

namespace GensQt4
{

// Static member initialization.
GeneralConfigWindow *GeneralConfigWindow::m_GeneralConfigWindow = nullptr;

/**
 * Check if the warranty is void.
 * If it is, we'll show some super secret settings.
 * @return True if the warranty is void.
 */
static inline bool isWarrantyVoid(void)
{
	return (gqt4_cfg->get(QLatin1String("iKnowWhatImDoingAndWillVoidTheWarranty")).toBool());
}


/**
 * Initialize the General Configuration window.
 */
GeneralConfigWindow::GeneralConfigWindow(QWidget *parent)
	: QMainWindow(parent,
		Qt::Dialog |
		Qt::WindowTitleHint |
		Qt::WindowSystemMenuHint |
		Qt::WindowMinimizeButtonHint |
		Qt::WindowCloseButtonHint)
{
	// Initialize the Qt4 UI.
	setupUi(this);

	// Make sure the window is deleted on close.
	this->setAttribute(Qt::WA_DeleteOnClose, true);

	// Initialize the "Warning" string.
	m_sWarning = QLatin1String("<span style='color: red'><b>") +
			GeneralConfigWindow::tr("Warning:") +
			QLatin1String("</b></span> ");

	if (!isWarrantyVoid()) {
		// Hide the super secret settings.
		delete grpAdvancedVDP;
		grpAdvancedVDP = nullptr;
	}

	// Create the action group for the toolbar buttons.
	m_agBtnGroup = new QActionGroup(this);
	m_agBtnGroup->setExclusive(true);
	connect(m_agBtnGroup, SIGNAL(triggered(QAction*)),
		this, SLOT(toolbarTriggered(QAction*)));

	// FreeDesktop.org icon names for the toolbar buttons.
	static const char *const icon_fdo[] = {
		"configure",			// General
		"applications-graphics",	// Graphics
		"cpu",				// VDP
		"applications-system",		// System
		"",				// Genesis (TODO)
		"media-optical",		// Sega CD
		"utilities-terminal",		// External Programs
		nullptr
	};

	// Initialize the toolbar buttons.
	int i = 0;
	foreach (QAction *action, toolBar->actions()) {
		action->setIcon(GensQApplication::IconFromTheme(QLatin1String(icon_fdo[i])));
		action->setData(i);
		m_agBtnGroup->addAction(action);

		// Next action.
		i++;
	}

	// Select the "General" tab.
	actionGeneral->setChecked(true);

#ifndef GCW_APPLY_IMMED
	// Set up a signal for the Apply button.
	QPushButton *btnApply = buttonBox->button(QDialogButtonBox::Apply);
	if (btnApply)
		connect(btnApply, SIGNAL(clicked()), this, SLOT(apply()));
#else
	{
		// Apply settings immediately.
		// Remove the buttonBox.
		QSize szBbox = buttonBox->size();
		delete buttonBox;
		buttonBox = nullptr;

		// Reduce the size of the QMainWindow.
		QSize szWindow = this->maximumSize();
		int left, top, right, bottom;
		vboxDialog->getContentsMargins(&left, &top, &right, &bottom);
		szWindow.rheight() -= (szBbox.height() + ((top + bottom) / 2));
		this->setMinimumSize(szWindow);
		this->setMaximumSize(szWindow);
		this->setBaseSize(szWindow);
	}
#endif

#ifdef Q_WS_MAC
	// Set up the Mac OS X-specific UI elements.
	setupUi_mac();
#endif

	/** Intro effect. **/

	// Intro Effect Color: Add the color entries.
	cboIntroColor->addItem(Qt::black);
	cboIntroColor->addItem(Qt::blue);
	cboIntroColor->addItem(Qt::green);
	cboIntroColor->addItem(Qt::cyan);
	cboIntroColor->addItem(Qt::red);
	cboIntroColor->addItem(Qt::magenta);
	cboIntroColor->addItem(Qt::yellow);
	cboIntroColor->addItem(Qt::white);

	/** System. **/

	// Set the icons for the up/down buttons.
	btnRegionDetectUp->setIcon(GensQApplication::IconFromTheme(QLatin1String("arrow-up")));
	btnRegionDetectDown->setIcon(GensQApplication::IconFromTheme(QLatin1String("arrow-down")));

	/** Sega Genesis. **/

	// Sega Genesis: TMSS ROM.
	txtMDTMSSRom->setIcon(style()->standardIcon(QStyle::SP_MessageBoxQuestion));
	const QString sMDTMSSRom_PlaceholderText = tr("Select a %1 TMSS ROM...");
	txtMDTMSSRom->setPlaceholderText(sMDTMSSRom_PlaceholderText.arg(tr("Genesis")));

	/** Sega CD. **/

	// Sega CD: Initialize the Boot ROM textbox icons.
	txtMcdRomUSA->setIcon(style()->standardIcon(QStyle::SP_MessageBoxQuestion));
	txtMcdRomEUR->setIcon(style()->standardIcon(QStyle::SP_MessageBoxQuestion));
	txtMcdRomJPN->setIcon(style()->standardIcon(QStyle::SP_MessageBoxQuestion));
	txtMcdRomAsia->setIcon(style()->standardIcon(QStyle::SP_MessageBoxQuestion));

	// Sega CD: Set the placeholder text.
#if QT_VERSION >= 0x040700
	const QString sMcdBootRom_PlaceholderText = tr("Select a %1 Boot ROM...");
	txtMcdRomUSA->setPlaceholderText(sMcdBootRom_PlaceholderText.arg(tr("Sega CD (U)")));
	txtMcdRomEUR->setPlaceholderText(sMcdBootRom_PlaceholderText.arg(tr("Mega CD (E)")));
	txtMcdRomJPN->setPlaceholderText(sMcdBootRom_PlaceholderText.arg(tr("Mega CD (J)")));
	txtMcdRomAsia->setPlaceholderText(sMcdBootRom_PlaceholderText.arg(tr("Mega CD (Asia)")));
#endif /* QT_VERSION >= 0x040700 */

	// External Programs: Set the textbox icon and placeholder text.
	txtExtPrgUnRAR->setIcon(style()->standardIcon(QStyle::SP_MessageBoxQuestion));
#ifdef Q_OS_WIN32
	lblExtPrgUnRAR->setText(tr("UnRAR DLL:"));
#if QT_VERSION >= 0x040700
	txtExtPrgUnRAR->setPlaceholderText(tr("Select an UnRAR DLL..."));
#endif /* QT_VERSION >= 0x040700 */
#else /* !Q_OS_WIN32 */
#if QT_VERSION >= 0x040700
	txtExtPrgUnRAR->setPlaceholderText(tr("Select a RAR or UnRAR binary..."));
#endif /* QT_VERSION >= 0x040700 */
#endif /* Q_OS_WIN32 */

	// Load configuration.
	reload();
}


/**
 * Shut down the Controller Configuration window.
 */
GeneralConfigWindow::~GeneralConfigWindow()
{
	// Clear the m_GeneralConfigWindow pointer.
	m_GeneralConfigWindow = nullptr;
}


/**
 * Show a single instance of the General Configuration window.
 * @param parent Parent window.
 */
void GeneralConfigWindow::ShowSingle(QWidget *parent)
{
	if (m_GeneralConfigWindow != nullptr) {
		// General Configuration Window is already displayed.
		// NOTE: This doesn't seem to work on KDE 4.4.2...
		QApplication::setActiveWindow(m_GeneralConfigWindow);
	} else {
		// General Configuration Window is not displayed.
		m_GeneralConfigWindow = new GeneralConfigWindow(parent);
		m_GeneralConfigWindow->show();
	}
}


/**
 * Key press handler.
 * @param event Key event.
 */
void GeneralConfigWindow::keyPressEvent(QKeyEvent *event)
{
	// TODO: Handle Cmd-Period on Mac?
	// NOTE: Cmd-W triggers the "Close ROM" action...
#ifndef GCW_APPLY_IMMED
	// Changes are not applied immediately.
	// Check for special dialog keys.
	// Adapted from QDialog::keyPressEvent().
	
	if (!event->modifiers() || ((event->modifiers() & Qt::KeypadModifier) && event->key() == Qt::Key_Enter))
	{
		switch (event->key()) {
			case Qt::Key_Enter:
			case Qt::Key_Return:
				// Accept the dialog changes.
				accept();
				break;

			case Qt::Key_Escape:
				// Reject the dialog changes.
				reject();
				break;

			default:
				// Pass the event to the base class.
				this->QMainWindow::keyPressEvent(event);
				return;
		}
	} else {
		// Pass the event to the base class.
		this->QMainWindow::keyPressEvent(event);
	}
#else /* !GCW_APPLY_IMMED */
	// Changes are applied immediately.
	// Don't handle special dialog keys.
	Q_UNUSED(event)
	return;
#endif /* GCW_APPLY_IMMED */
}


/**
 * Widget state has changed.
 * @param event State change event.
 */
void GeneralConfigWindow::changeEvent(QEvent *event)
{
	if (event->type() == QEvent::LanguageChange) {
		// Retranslate the UI.
		retranslateUi(this);

		// Update Sega Genesis TMSS ROM file status.
		// TODO: Update the display for the last selected ROM.
		QString sNewRomStatus;
		sNewRomStatus = mdUpdateTmssRomFileStatus(txtMDTMSSRom);
		if (!sNewRomStatus.isEmpty())
			sMDTmssRomStatus = sNewRomStatus;

		// Update Sega CD Boot ROM file status.
		// TODO: Update the display for the last selected ROM.
		sNewRomStatus = mcdUpdateRomFileStatus(txtMcdRomUSA, MCD_REGION_USA);
		if (!sNewRomStatus.isEmpty())
			sMcdRomStatus_USA = sNewRomStatus;
		sNewRomStatus = mcdUpdateRomFileStatus(txtMcdRomEUR, MCD_REGION_EUROPE);
		if (!sNewRomStatus.isEmpty())
			sMcdRomStatus_EUR = sNewRomStatus;
		sNewRomStatus = mcdUpdateRomFileStatus(txtMcdRomJPN, MCD_REGION_JAPAN);
		if (!sNewRomStatus.isEmpty())
			sMcdRomStatus_JPN = sNewRomStatus;
		sNewRomStatus = mcdUpdateRomFileStatus(txtMcdRomAsia, MCD_REGION_ASIA);
		if (!sNewRomStatus.isEmpty())
			sMcdRomStatus_Asia = sNewRomStatus;

		// Update external program status.
		// TODO: Split the RAR check code out of the on_txtExtPrgUnRAR_textChanged() function.
		// NOTE: Calling on_txtExtPrgUnRAR_textChanged() will enable the Apply button!
		on_txtExtPrgUnRAR_textChanged();
	}

	// Pass the event to the base class.
	this->QMainWindow::changeEvent(event);
}


/**
 * Accept the configuration changes.
 * Triggered if "OK" is clicked.
 */
void GeneralConfigWindow::accept(void)
{
	apply();
	this->close();
}


/**
 * Reject the configuration changes.
 * Triggered if "Cancel" is clicked.
 */
void GeneralConfigWindow::reject(void)
{
	this->close();
}


#ifndef GCW_APPLY_IMMED
/**
 * Enable or disable the Apply button.
 * @param enabled True to enable; false to disable.
 */
void GeneralConfigWindow::setApplyButtonEnabled(bool enabled)
{
	QPushButton *btnApply = buttonBox->button(QDialogButtonBox::Apply);
	if (btnApply)
		btnApply->setEnabled(enabled);
}
#endif


static inline bool ValByPath_bool(const char *path)
	{ return gqt4_cfg->get(QLatin1String(path)).toBool(); }
static inline QColor ValByPath_QColor(const char *path)
	{ return gqt4_cfg->get(QLatin1String(path)).value<QColor>(); }
static inline int ValByPath_int(const char *path)
	{ return gqt4_cfg->getInt(QLatin1String(path)); }
static inline int ValByPath_uint(const char *path)
	{ return gqt4_cfg->getUInt(QLatin1String(path)); }
static inline QString ValByPath_QString(const char *path)
	{ return gqt4_cfg->get(QLatin1String(path)).toString(); }

/**
 * Reload configuration.
 */
void GeneralConfigWindow::reload(void)
{
	// Onscreen Display.
	QColor colorText;

	/** Onscreen display: FPS counter. **/
	chkOsdFpsEnable->setChecked(ValByPath_bool("OSD/fpsEnabled"));
	m_osdFpsColor = ValByPath_QColor("OSD/fpsColor");
	btnOsdFpsColor->setBgColor(m_osdFpsColor);
	btnOsdFpsColor->setText(m_osdFpsColor.name().toUpper());

	/** Onscreen display: Messages. **/
	chkOsdMsgEnable->setChecked(ValByPath_bool("OSD/msgEnabled"));
	m_osdMsgColor = ValByPath_QColor("OSD/msgColor");
	btnOsdMsgColor->setBgColor(m_osdMsgColor);
	btnOsdMsgColor->setText(m_osdMsgColor.name().toUpper());

	/** General settings. **/
	chkAutoFixChecksum->setChecked(ValByPath_bool("autoFixChecksum"));
	chkAutoPause->setChecked(ValByPath_bool("autoPause"));
	chkPauseTint->setChecked(ValByPath_bool("pauseTint"));

	/** Intro effect. **/
	cboIntroStyle->setCurrentIndex(ValByPath_int("Intro_Effect/introStyle"));
	cboIntroColor->setCurrentIndex(ValByPath_int("Intro_Effect/introColor"));

	/** Sega Genesis TMSS. **/
	chkMDTMSS->setChecked(ValByPath_bool("Genesis/tmssEnabled"));
	txtMDTMSSRom->setText(ValByPath_QString("Genesis/tmssRom"));

	/** Sega CD Boot ROMs. **/
	txtMcdRomUSA->setText(ValByPath_QString("Sega_CD/bootRomUSA"));
	txtMcdRomEUR->setText(ValByPath_QString("Sega_CD/bootRomEUR"));
	txtMcdRomJPN->setText(ValByPath_QString("Sega_CD/bootRomJPN"));
	txtMcdRomAsia->setText(ValByPath_QString("Sega_CD/bootRomAsia"));
	on_txtMcdRomUSA_focusIn();

	/** External programs. **/
	txtExtPrgUnRAR->setText(ValByPath_QString("External_Programs/UnRAR"));

	/** Graphics settings. **/
	chkAspectRatioConstraint->setChecked(ValByPath_bool("Graphics/aspectRatioConstraint"));
	chkFastBlur->setChecked(ValByPath_bool("Graphics/fastBlur"));
	chkBilinearFilter->setChecked(ValByPath_bool("Graphics/bilinearFilter"));
	cboInterlacedMode->setCurrentIndex(ValByPath_int("Graphics/interlacedMode"));

	/** VDP settings. **/
	chkSpriteLimits->setChecked(ValByPath_bool("VDP/spriteLimits"));
	chkBorderColor->setChecked(ValByPath_bool("VDP/borderColorEmulation"));
	chkNtscV30Rolling->setChecked(ValByPath_bool("VDP/ntscV30Rolling"));
	if (isWarrantyVoid()) {
		chkVScrollBug->setChecked(ValByPath_bool("VDP/vscrollBug"));
		chkZeroLengthDMA->setChecked(ValByPath_bool("VDP/zeroLengthDMA"));
	}

	/** System. **/
	cboRegionCurrent->setCurrentIndex(ValByPath_int("System/regionCode") + 1);

	// Region auto-detection settings.
	lstRegionDetect->clear();
	uint16_t regionCodeOrder = (uint16_t)ValByPath_uint("System/regionCodeOrder");
	for (int i = 0; i < 4; i++, regionCodeOrder >>= 4) {
		const QString str = EmuManager::LgRegionCodeStrMD(regionCodeOrder & 0xF);
		if (!str.isEmpty()) {
			QListWidgetItem *item = new QListWidgetItem(str);
			item->setData(Qt::UserRole, (regionCodeOrder & 0xF));
			lstRegionDetect->insertItem(0, item);
		}
	}

#ifndef GCW_APPLY_IMMED
	// Disable the Apply button.
	setApplyButtonEnabled(false);
#endif
}


static inline void SetValByPath_bool(const char *path, bool value)
	{ gqt4_cfg->set(QLatin1String(path), value); }
static inline void SetValByPath_QColor(const char *path, const QColor& value)
	{ gqt4_cfg->set(QLatin1String(path), value.name()); }
static inline void SetValByPath_int(const char *path, int value)
	{ gqt4_cfg->set(QLatin1String(path), value); }
static inline void SetValByPath_uint(const char *path, unsigned int value)
	{ gqt4_cfg->set(QLatin1String(path), value); }
static inline void SetValByPath_QString(const char *path, QString value)
	{ gqt4_cfg->set(QLatin1String(path), value); }

/**
 * Apply the configuration changes.
 * Triggered if "Apply" is clicked.
 */
void GeneralConfigWindow::apply(void)
{
#ifndef GCW_APPLY_IMMED
	/** Onscreen display. **/
	SetValByPath_bool("OSD/fpsEnabled", chkOsdFpsEnable->isChecked());
	SetValByPath_QColor("OSD/fpsColor", m_osdFpsColor);
	SetValByPath_bool("OSD/msgEnabled", chkOsdMsgEnable->isChecked());
	SetValByPath_QColor("OSD/msgColor", m_osdMsgColor);

	/** Intro effect. **/
	SetValByPath_int("Intro_Effect/introStyle", cboIntroStyle->currentIndex());
	SetValByPath_int("Intro_Effect/introColor", cboIntroColor->currentIndex());

	/** General settings. **/
	SetValByPath_bool("autoFixChecksum", chkAutoFixChecksum->isChecked());
	SetValByPath_bool("autoPause", chkAutoPause->isChecked());
	SetValByPath_bool("pauseTint", chkPauseTint->isChecked());

	/** Sega Genesis TMSS. **/
	SetValByPath_bool("Genesis/tmssEnabled", chkMDTMSS->isChecked());
	SetValByPath_QString("Genesis/tmssRom", txtMDTMSSRom->text());

	/** Sega CD Boot ROMs. **/
	SetValByPath_QString("Sega_CD/bootRomUSA", txtMcdRomUSA->text());
	SetValByPath_QString("Sega_CD/bootRomEUR", txtMcdRomEUR->text());
	SetValByPath_QString("Sega_CD/bootRomJPN", txtMcdRomJPN->text());
	SetValByPath_QString("Sega_CD/bootRomAsia", txtMcdRomAsia->text());

	/** External programs. **/
	SetValByPath_QString("External_Programs/UnRAR", txtExtPrgUnRAR->text());

	/** Graphics settings. **/
	SetValByPath_bool("Graphics/aspectRatioConstraint", chkAspectRatioConstraint->isChecked());
	SetValByPath_bool("Graphics/fastBlur", chkFastBlur->isChecked());
	SetValByPath_bool("Graphics/bilinearFilter", chkBilinearFilter->isChecked());
	SetValByPath_int("Graphics/interlacedMode", cboInterlacedMode->currentIndex());

	/** VDP settings. **/
	SetValByPath_bool("VDP/spriteLimits", chkSpriteLimits->isChecked());
	SetValByPath_bool("VDP/borderColorEmulation", chkBorderColor->isChecked());
	SetValByPath_bool("VDP/ntscV30Rolling", chkNtscV30Rolling->isChecked());
	if (isWarrantyVoid()) {
		SetValByPath_bool("VDP/vscrollBug", chkVScrollBug->isChecked());
		SetValByPath_bool("VDP/zeroLengthDMA", chkZeroLengthDMA->isChecked());
	}

	/** System. **/
	SetValByPath_int("System/regionCode", (cboRegionCurrent->currentIndex() - 1));
	SetValByPath_uint("System/regionCodeOrder", regionCodeOrder());

	// Disable the Apply button.
	// TODO: If Apply was clicked, set focus back to the main window elements.
	// Otherwise, Cancel will receive focus.
	setApplyButtonEnabled(false);
#endif
}


/**
 * A toolbar button was clicked.
 * @param action Toolbar button.
 */
void GeneralConfigWindow::toolbarTriggered(QAction *action)
{
	QVariant data = action->data();
	if (!data.isValid() || !data.canConvert(QVariant::Int))
		return;
	
	int tab = data.toInt();
	if (tab < 0 || tab >= stackedWidget->count())
		return;
	
	stackedWidget->setCurrentIndex(tab);
}


/** Onscreen display **/


/**
 * Select a color for the OSD.
 * @param color_id	[in] Color ID.
 * @param init_color	[in] Initial color.
 * @return Selected color, or invalid QColor if cancelled.
 */
QColor GeneralConfigWindow::osdSelectColor(QString color_id, const QColor& init_color)
{
	// Create the dialog title.
	QString title = tr("Select OSD %1 Color").arg(color_id);
	
	// QColorDialog::getColor() returns an invalid color
	// if the dialog is cancelled.
	return QColorDialog::getColor(init_color, this, title);
}

void GeneralConfigWindow::on_btnOsdFpsColor_clicked(void)
{
	QColor color = osdSelectColor(tr("FPS counter"), m_osdFpsColor);
	if (!color.isValid() || m_osdFpsColor == color)
		return;
	
	m_osdFpsColor = color;
	btnOsdFpsColor->setBgColor(m_osdFpsColor);
	btnOsdFpsColor->setText(m_osdFpsColor.name().toUpper());
	
	// Settings have been changed.
#ifndef GCW_APPLY_IMMED
	setApplyButtonEnabled(true);
#else
	SetValByPath_QColor("OSD/fpsColor", m_osdMsgColor);
#endif
}

void GeneralConfigWindow::on_btnOsdMsgColor_clicked(void)
{
	QColor color = osdSelectColor(tr("messages"), m_osdMsgColor);
	if (!color.isValid() || m_osdMsgColor == color)
		return;
	
	m_osdMsgColor = color;
	btnOsdMsgColor->setBgColor(color);
	btnOsdMsgColor->setText(m_osdMsgColor.name().toUpper());
	
	// Settings have been changed.
#ifndef GCW_APPLY_IMMED
	setApplyButtonEnabled(true);
#else
	SetValByPath_QColor("OSD/msgColor", m_osdMsgColor);
#endif
}


/** System. **/
// TODO: Detect drag-and-drop of items within the QListWidget.


/**
 * Up button was clicked.
 */
void GeneralConfigWindow::on_btnRegionDetectUp_clicked(void)
{
	QListWidgetItem *cur = lstRegionDetect->currentItem();
	if (!cur)
		return;
	int cur_idx = lstRegionDetect->row(cur);
	
	QListWidgetItem *prev = lstRegionDetect->takeItem(cur_idx - 1);
	if (!prev)
		return;
	
	lstRegionDetect->insertItem(cur_idx, prev);
	
	// Settings have been changed.
#ifndef GCW_APPLY_IMMED
	setApplyButtonEnabled(true);
#else
	SetValByPath_uint("System/regionCodeOrder", regionCodeOrder());
#endif
}


/**
 * Down button was clicked.
 */
void GeneralConfigWindow::on_btnRegionDetectDown_clicked(void)
{
	QListWidgetItem *cur = lstRegionDetect->currentItem();
	if (!cur)
		return;
	int cur_idx = lstRegionDetect->row(cur);
	
	QListWidgetItem *next = lstRegionDetect->takeItem(cur_idx + 1);
	if (!next)
		return;
	
	lstRegionDetect->insertItem(cur_idx, next);
	
	// Settings have been changed.
#ifndef GCW_APPLY_IMMED
	setApplyButtonEnabled(true);
#else
	SetValByPath_uint("System/regionCodeOrder", regionCodeOrder());
#endif
}


/**
 * Get the region code order from lstRegionDetect.
 * @return Region code order.
 */
uint16_t GeneralConfigWindow::regionCodeOrder(void) const
{
	uint16_t ret = 0;
	for (int i = 0; i < lstRegionDetect->count(); i++)
	{
		const QListWidgetItem *item = lstRegionDetect->item(i);
		ret <<= 4;
		ret |= (uint16_t)item->data(Qt::UserRole).toUInt();
	}
	return ret;
}


/** Sega Genesis. **/

/**
 * Select a ROM file.
 * @param rom_desc	[in] ROM file description.
 * @param txtRomFile	[in] ROM file textbox.
 */
void GeneralConfigWindow::selectRomFile(QString rom_desc, QLineEdit *txtRomFile)
{
	// TODO: Proper compressed file support.
	#define ZLIB_EXT " *.zip *.zsg *.gz"
	#define LZMA_EXT " *.7z"
	#define RAR_EXT " *.rar"

	// Create the dialog title.
	QString title = tr("Select %1").arg(rom_desc);

	// TODO: Specify the current Boot ROM filename as the default filename.
	// TODO: Move the filename filters somewhere else.
	QString filename = QFileDialog::getOpenFileName(this, title,
			txtRomFile->text(),	// Default filename.
			tr("ROM Images") +
			QLatin1String(
				" (*.bin *.gen *.md *.smd"
#ifdef HAVE_ZLIB
				ZLIB_EXT
#endif /* HAVE_ZLIB */
#ifdef HAVE_LZMA
				LZMA_EXT
#endif /* HAVE_LZMA */
				RAR_EXT
				");;") +
			tr("All Files") + QLatin1String(" (*.*)"));

	if (filename.isEmpty())
		return;

	// Convert to native pathname separators.
	filename = QDir::toNativeSeparators(filename);

	// Set the filename text.
	// ROM file status will be updated automatically by
	// the textChanged() signal from QLineEdit.
	txtRomFile->setText(filename);

	// Set focus to the textbox.
	txtRomFile->setFocus(Qt::OtherFocusReason);
}

void GeneralConfigWindow::on_btnMDTMSSRom_clicked(void)
{
	const QString tmssRom = tr("%1 TMSS ROM");
	selectRomFile(tmssRom.arg(tr("Genesis")), txtMDTMSSRom);
}

void GeneralConfigWindow::on_txtMDTMSSRom_focusIn(void)
{
	// TODO
	//mcdDisplayRomFileStatus(tr("Sega CD (U)"), sMcdRomStatus_USA);
}

/**
 * Sega Genesis: Update TMSS ROM file status.
 * @param txtRomFile ROM file textbox.
 * @return Updated ROM status.
 */
QString GeneralConfigWindow::mdUpdateTmssRomFileStatus(GensLineEdit *txtRomFile)
{
	// ROM data buffer.
	uint8_t *rom_data = nullptr;
	int data_len;
	uint32_t rom_crc32;

	// Line break string.
	const QString sLineBreak = QLatin1String("<br/>\n");

	// Check if the file exists.
	QString filename = txtRomFile->text();
	if (!QFile::exists(filename)) {
		// File doesn't exist.
		// NOTE: KDE 4's Oxygen theme doesn't have a question icon.
		// SP_MessageBoxQuestion is redirected to SP_MessageBoxInformation on KDE 4.
		// TODO: Set ROM file notes.
		txtRomFile->setIcon(style()->standardIcon(QStyle::SP_MessageBoxQuestion));
		if (filename.isEmpty())
			return tr("No ROM filename specified.");
		else
			return tr("The specified ROM file was not found.");
	}

	// Check the ROM file.
	// TODO: Decompressor support.
	QStyle::StandardPixmap filename_icon = QStyle::SP_DialogYesButton;
	QString rom_id = tr("Unknown");
	QString rom_notes;
	QString rom_size_warning;

	// Open the ROM file using LibGens::Rom.
	QScopedPointer<LibGens::Rom> rom(new LibGens::Rom(filename.toUtf8().constData()));
	if (!rom->isOpen()) {
		// Error opening ROM file.
		txtRomFile->setIcon(style()->standardIcon(QStyle::SP_MessageBoxQuestion));
		return tr("Error opening ROM file.");
	}

	// Multi-file ROM archives are not supported for TMSS ROMs.
	if (rom->isMultiFile()) {
		txtRomFile->setIcon(style()->standardIcon(QStyle::SP_MessageBoxWarning));
		return (m_sWarning +
				tr("This archive has multiple files.") + sLineBreak +
				tr("Multi-file ROM archives are not currently supported for the TMSS ROM."));
	}

	// Check the ROM filesize.
	// Valid TMSS ROMs are 2 KB.
	static const int TMSS_ROM_FILESIZE = 2048;
	if (rom->romSize() != TMSS_ROM_FILESIZE) {
		// Wrong ROM size.
		filename_icon = QStyle::SP_MessageBoxWarning;

		rom_size_warning = m_sWarning + tr("ROM size is incorrect.") + sLineBreak +
				tr("(expected %L1 bytes; found %L2 bytes)")
				.arg(TMSS_ROM_FILESIZE).arg(rom->romSize());

		// Identify the ROM even if it's too big.
		// (Some copies of the TMSS ROM are overdumped.)
		// Also, don't check ridiculously large TMSS ROMs.
		if (rom->romSize() < TMSS_ROM_FILESIZE || rom->romSize() > 1048576) {
			// ROM is too small, so it's guaranteed to not match anything in the database.
			goto rom_identified;
		}
	}

	// Load the ROM data and calculate the CRC32..
	// TODO: LibGens::Rom::loadRom() fails if the ROM buffer isn't >= the ROM size.
	rom_data = (uint8_t*)malloc(rom->romSize());
	data_len = rom->loadRom(rom_data, rom->romSize());
	if (data_len != rom->romSize()) {
		// Error reading data from the file.
		rom_notes = tr("Error reading file.") + sLineBreak +
			    tr("(expected %L1 bytes; read %L2 bytes)")
			    .arg(rom->romSize()).arg(data_len);
		goto rom_identified;
	}

	// Calculate the CRC32 using zlib.
	// NOTE: rom->rom_crc32() will be incorrect if the ROM is too bi.
	rom_crc32 = crc32(0, rom_data, TMSS_ROM_FILESIZE);

	// Check what ROM this is.
	switch (rom_crc32) {
		case 0x3F888CF4:
			// Standard TMSS ROM.
			rom_id = tr("Genesis TMSS ROM");
			rom_notes = tr("This is a known good dump of the Genesis TMSS ROM.");
			break;

		default:
			// Unknown TMSS ROM.
			// TODO: Add more variants.
			filename_icon = QStyle::SP_MessageBoxQuestion;
			rom_notes = tr("Unknown ROM image.");
			break;
	}

rom_identified:
	// Free the ROM data buffer if it was allocated.
	free(rom_data);

	// Set the Boot ROM filename textbox icon.
	txtRomFile->setIcon(style()->standardIcon(filename_icon));

	// Set the Boot ROM description.
	QString s_ret;
	s_ret = tr("ROM identified as: %1").arg(rom_id);
	if (!rom_notes.isEmpty())
		s_ret += sLineBreak + sLineBreak + rom_notes;
	if (!rom_size_warning.isEmpty())
		s_ret += sLineBreak + sLineBreak + rom_size_warning;
	return QString(s_ret);
}


void GeneralConfigWindow::on_txtMDTMSSRom_textChanged(void)
{
	QString sNewRomStatus = mdUpdateTmssRomFileStatus(txtMDTMSSRom);
	if (!sNewRomStatus.isEmpty()) {
		sMDTmssRomStatus = sNewRomStatus;
		lblMDTMSSSelectedRom->setText(sMDTmssRomStatus);
		lblMDTMSSSelectedRom->setTextFormat(Qt::RichText);
	}

	// Settings have been changed.
#ifndef GCW_APPLY_IMMED
	setApplyButtonEnabled(true);
#else
	SetValByPath_QString("Genesis/tmssRom", txtMDTMSSRom->text());
#endif
}


/** Sega CD. **/

void GeneralConfigWindow::on_btnMcdRomUSA_clicked(void)
{
	const QString bootRom = tr("%1 Boot ROM");
	selectRomFile(bootRom.arg(tr("Sega CD (U)")), txtMcdRomUSA);
}
void GeneralConfigWindow::on_btnMcdRomEUR_clicked(void)
{
	const QString bootRom = tr("%1 Boot ROM");
	selectRomFile(bootRom.arg(tr("Mega CD (E)")), txtMcdRomEUR);
}
void GeneralConfigWindow::on_btnMcdRomJPN_clicked(void)
{
	const QString bootRom = tr("%1 Boot ROM");
	selectRomFile(bootRom.arg(tr("Mega CD (J)")), txtMcdRomJPN);
}
void GeneralConfigWindow::on_btnMcdRomAsia_clicked(void)
{
	const QString bootRom = tr("%1 Boot ROM");
	selectRomFile(bootRom.arg(tr("Mega CD (Asia)")), txtMcdRomAsia);
}


/**
 * Sega CD: Update Boot ROM file status.
 * @param txtRomFile ROM file textbox.
 * @param region_code Expected ROM region code. (bitmask)
 * @return Updated ROM status.
 */
QString GeneralConfigWindow::mcdUpdateRomFileStatus(GensLineEdit *txtRomFile, int region_code)
{
	// ROM data buffer.
	uint8_t *rom_data = nullptr;
	int data_len;
	uint32_t rom_crc32;
	int boot_rom_id;
	int boot_rom_region_code;
	MCD_RomStatus_t boot_rom_status;

	// Line break string.
	const QString sLineBreak = QLatin1String("<br/>\n");

	// Check if the file exists.
	QString filename = txtRomFile->text();
	if (!QFile::exists(filename)) {
		// File doesn't exist.
		// NOTE: KDE 4's Oxygen theme doesn't have a question icon.
		// SP_MessageBoxQuestion is redirected to SP_MessageBoxInformation on KDE 4.
		// TODO: Set ROM file notes.
		txtRomFile->setIcon(style()->standardIcon(QStyle::SP_MessageBoxQuestion));
		if (filename.isEmpty())
			return tr("No ROM filename specified.");
		else
			return tr("The specified ROM file was not found.");
	}

	// Check the ROM file.
	// TODO: Decompressor support.
	QStyle::StandardPixmap filename_icon = QStyle::SP_DialogYesButton;
	QString rom_id = tr("Unknown");
	QString rom_notes;
	QString rom_size_warning;

	// Open the ROM file using LibGens::Rom.
	QScopedPointer<LibGens::Rom> rom(new LibGens::Rom(filename.toUtf8().constData()));
	if (!rom->isOpen()) {
		// Error opening ROM file.
		txtRomFile->setIcon(style()->standardIcon(QStyle::SP_MessageBoxQuestion));
		return tr("Error opening ROM file.");
	}

	// Multi-file ROM archives are not supported for Sega CD Boot ROMs.
	if (rom->isMultiFile()) {
		txtRomFile->setIcon(style()->standardIcon(QStyle::SP_MessageBoxWarning));
		return (sLineBreak + m_sWarning +
				tr("This archive has multiple files.") + sLineBreak +
				tr("Multi-file ROM archives are not currently supported for Sega CD Boot ROMs."));
	}

	// Check the ROM filesize.
	// Valid boot ROMs are 128 KB.
	// Smaller ROMs will not work; larger ROMs may work, but warn anyway.
	if (rom->romSize() != MCD_ROM_FILESIZE) {
		// Wrong ROM size.
		filename_icon = QStyle::SP_MessageBoxWarning;

		rom_size_warning = m_sWarning + tr("ROM size is incorrect.") + sLineBreak +
				tr("(expected %L1 bytes; found %L2 bytes)").arg(MCD_ROM_FILESIZE).arg(rom->romSize());

		// Identify the ROM even if it's too big.
		// (Some copies of the TMSS ROM are overdumped.)
		// Also, don't check ridiculously large Sega CD boot ROMs.
		if (rom->romSize() < MCD_ROM_FILESIZE || rom->romSize() > 1048576) {
			// ROM is too small, so it's guaranteed to not match anything in the database.
			goto rom_identified;
		}
	}

	// Load the ROM data and calculate the CRC32..
	// TODO: LibGens::Rom::loadRom() fails if the ROM buffer isn't >= the ROM size.
	rom_data = (uint8_t*)malloc(rom->romSize());
	data_len = rom->loadRom(rom_data, rom->romSize());
	if (data_len != rom->romSize()) {
		// Error reading data from the file.
		rom_notes = tr("Error reading file.") + sLineBreak +
			    tr("(expected %L1 bytes; read %L2 bytes)").arg(MCD_ROM_FILESIZE).arg(data_len);
		goto rom_identified;
	}

	// Fix up the ROM's Initial SP and Initial HINT vector.
	memcpy(&rom_data[0x00], lg_mcd_rom_InitSP, sizeof(lg_mcd_rom_InitSP));
	memcpy(&rom_data[0x70], lg_mcd_rom_InitHINT, sizeof(lg_mcd_rom_InitHINT));

	// Calculate the CRC32 using zlib.
	// NOTE: rom->rom_crc32() will be incorrect if the ROM is too bi.
	rom_crc32 = crc32(0, rom_data, MCD_ROM_FILESIZE);

	// Look up the CRC32 in the Sega CD Boot ROM database.
	boot_rom_id = lg_mcd_rom_FindByCRC32(rom_crc32);
	if (boot_rom_id < 0) {
		// Boot ROM was not found in the database.
		filename_icon = QStyle::SP_MessageBoxWarning;
		goto rom_identified;
	}

	// ROM found. Get the description and other information.
	rom_id = QString::fromUtf8(lg_mcd_rom_GetDescription(boot_rom_id));

	// Check the region code.
	boot_rom_region_code = lg_mcd_rom_GetRegion(boot_rom_id);
	if ((boot_rom_region_code & region_code) == 0) {
		// ROM doesn't support this region.
		int boot_rom_region_primary = lg_mcd_rom_GetPrimaryRegion(boot_rom_id);
		QString expected_region = QString::fromUtf8(lg_mcd_rom_GetRegionCodeString(region_code));
		QString boot_rom_region = QString::fromUtf8(lg_mcd_rom_GetRegionCodeString(boot_rom_region_primary));

		rom_notes += m_sWarning + tr("Region code is incorrect.") + sLineBreak +
			     tr("(expected %1; found %2)").arg(expected_region).arg(boot_rom_region) + sLineBreak;

		// Set the icon to warning.
		filename_icon = QStyle::SP_MessageBoxWarning;
	}

	// Check the ROM's support status.
	boot_rom_status = lg_mcd_rom_GetSupportStatus(boot_rom_id);
	switch (boot_rom_status) {
		case RomStatus_Recommended:
		case RomStatus_Supported:
			// ROM is either recommended or supported.
			// TODO: Make a distinction between the two?
			break;

		case RomStatus_Unsupported:
		default:
			// ROM is unsupported.
			rom_notes += m_sWarning + tr("This Boot ROM is not supported by Gens/GS II.") + sLineBreak;
			filename_icon = QStyle::SP_MessageBoxWarning;
			break;

		case RomStatus_Broken:
			// ROM is known to be broken.
			rom_notes += m_sWarning + tr("This Boot ROM is known to be broken on all emulators.") + sLineBreak;
			filename_icon = QStyle::SP_MessageBoxCritical;
			break;
	}

	// Get the ROM's notes.
	rom_notes += QString::fromUtf8(lg_mcd_rom_GetNotes(boot_rom_id)).replace(QChar(L'\n'), sLineBreak);

rom_identified:
	// Free the ROM data buffer if it was allocated.
	free(rom_data);

	// Set the Boot ROM filename textbox icon.
	txtRomFile->setIcon(style()->standardIcon(filename_icon));

	// Set the Boot ROM description.
	QString s_ret;
	s_ret = tr("ROM identified as: %1").arg(rom_id);
	if (!rom_notes.isEmpty())
		s_ret += sLineBreak + sLineBreak + rom_notes;
	if (!rom_size_warning.isEmpty())
		s_ret += sLineBreak + sLineBreak + rom_size_warning;
	return QString(s_ret);
}


/**
 * Sega CD: Display Boot ROM file status.
 * @param rom_id Sega CD Boot ROM ID.
 * @param rom_desc ROM file description. (detected by examining the ROM)
 */
void GeneralConfigWindow::mcdDisplayRomFileStatus(QString rom_id, QString rom_desc)
{
	// Set the ROM description.
	QString sel_rom = tr("Selected ROM: %1");
	lblMcdSelectedRom->setText(sel_rom.arg(rom_id) +
				QLatin1String("<br/>\n") + rom_desc);
	lblMcdSelectedRom->setTextFormat(Qt::RichText);
}

void GeneralConfigWindow::on_txtMcdRomUSA_focusIn(void)
	{ mcdDisplayRomFileStatus(tr("Sega CD (U)"), sMcdRomStatus_USA); }
void GeneralConfigWindow::on_txtMcdRomEUR_focusIn(void)
	{ mcdDisplayRomFileStatus(tr("Mega CD (E)"), sMcdRomStatus_EUR); }
void GeneralConfigWindow::on_txtMcdRomJPN_focusIn(void)
	{ mcdDisplayRomFileStatus(tr("Mega CD (J)"), sMcdRomStatus_JPN); }
void GeneralConfigWindow::on_txtMcdRomAsia_focusIn(void)
	{ mcdDisplayRomFileStatus(tr("Mega CD (Asia)"), sMcdRomStatus_Asia); }

void GeneralConfigWindow::on_txtMcdRomUSA_textChanged(void)
{
	QString sNewRomStatus = mcdUpdateRomFileStatus(txtMcdRomUSA, MCD_REGION_USA);
	if (!sNewRomStatus.isEmpty()) {
		sMcdRomStatus_USA = sNewRomStatus;
		mcdDisplayRomFileStatus(tr("Sega CD (U)"), sMcdRomStatus_USA);
	}

	// Settings have been changed.
#ifndef GCW_APPLY_IMMED
	setApplyButtonEnabled(true);
#else
	SetValByPath_QString("Sega_CD/bootRomUSA", txtMcdRomUSA->text());
#endif
}

void GeneralConfigWindow::on_txtMcdRomEUR_textChanged(void)
{
	QString sNewRomStatus = mcdUpdateRomFileStatus(txtMcdRomEUR, MCD_REGION_EUROPE);
	if (!sNewRomStatus.isEmpty()) {
		sMcdRomStatus_EUR = sNewRomStatus;
		mcdDisplayRomFileStatus(tr("Mega CD (E)"), sMcdRomStatus_EUR);
	}

	// Settings have been changed.
#ifndef GCW_APPLY_IMMED
	setApplyButtonEnabled(true);
#else
	SetValByPath_QString("Sega_CD/bootRomEUR", txtMcdRomEUR->text());
#endif
}

void GeneralConfigWindow::on_txtMcdRomJPN_textChanged(void)
{
	QString sNewRomStatus = mcdUpdateRomFileStatus(txtMcdRomJPN, MCD_REGION_JAPAN);
	if (!sNewRomStatus.isEmpty()) {
		sMcdRomStatus_JPN = sNewRomStatus;
		mcdDisplayRomFileStatus(tr("Mega CD (J)"), sMcdRomStatus_JPN);
	}

	// Settings have been changed.
#ifndef GCW_APPLY_IMMED
	setApplyButtonEnabled(true);
#else
	SetValByPath_QString("Sega_CD/bootRomJPN", txtMcdRomJPN->text());
#endif
}

void GeneralConfigWindow::on_txtMcdRomAsia_textChanged(void)
{
	QString sNewRomStatus = mcdUpdateRomFileStatus(txtMcdRomAsia, MCD_REGION_ASIA);
	if (!sNewRomStatus.isEmpty()) {
		sMcdRomStatus_Asia = sNewRomStatus;
		mcdDisplayRomFileStatus(tr("Mega CD (Asia)"), sMcdRomStatus_Asia);
	}

	// Settings have been changed.
#ifndef GCW_APPLY_IMMED
	setApplyButtonEnabled(true);
#else
	SetValByPath_QString("Sega_CD/bootRomAsia", txtMcdRomAsia->text());
#endif
}


/** External programs **/


/**
 * Select a RAR/UnRAR binary.
 */
void GeneralConfigWindow::on_btnExtPrgUnRAR_clicked(void)
{
	// Create the dialog title.
#ifdef Q_OS_WIN32
	const QString title = tr("Select UnRAR DLL");
#else
	const QString title = tr("Select RAR or UnRAR binary");
#endif
	
	QString filename = QFileDialog::getOpenFileName(this, title,
			txtExtPrgUnRAR->text(),		// Default filename.
#ifdef Q_OS_WIN32
			tr("DLL files") + QLatin1String(" (*.dll);;") +
#else
			tr("rar or unrar") + QLatin1String(" (rar unrar);;") +
#endif
			tr("All Files") + QLatin1String(" (*.*)"));
	
	if (filename.isEmpty())
		return;
	
	// Convert to native pathname separators.
	filename = QDir::toNativeSeparators(filename);
	
	// Set the filename text.
	// Program file status will be updated automatically by
	// the textChanged() signal from QLineEdit.
	txtExtPrgUnRAR->setText(filename);
	
	// Set focus to the textbox.
	txtExtPrgUnRAR->setFocus(Qt::OtherFocusReason);
}


/**
 * Display external program file status.
 * @param file_id File ID.
 * @param file_desc File description. (detected by examining the file)
 */
void GeneralConfigWindow::extprgDisplayFileStatus(QString file_id, QString file_desc)
{
	// Set the file description.
	QString sel_prg = tr("Selected Program: %1");
	lblExtPrgSel->setText(sel_prg.arg(file_id) +
				QLatin1String("<br/>\n") + file_desc);
	lblExtPrgSel->setTextFormat(Qt::RichText);
}

void GeneralConfigWindow::on_txtExtPrgUnRAR_focusIn(void)
{
#ifdef Q_OS_WIN32
	extprgDisplayFileStatus(tr("UnRAR DLL"), sExtPrgStatus_UnRAR);
#else
	extprgDisplayFileStatus(tr("RAR or UnRAR binary"), sExtPrgStatus_UnRAR);
#endif
}

void GeneralConfigWindow::on_txtExtPrgUnRAR_textChanged(void)
{
	// Check the RAR binary to make sure it's valid.
	// TODO: Split status detection into a separate function like Sega CD Boot ROMs?
	QString prg_id = tr("Unknown");
	QString prg_status;
	QStyle::StandardPixmap filename_icon = QStyle::SP_MessageBoxQuestion;
	LibGens::DcRar::ExtPrgInfo prg_info;
	
	// Check if the file exists.
	QString filename = txtExtPrgUnRAR->text();
	if (filename.isEmpty())
		prg_status = tr("No filename specified.");
	else
	{
		int status = LibGens::DcRar::CheckExtPrg(txtExtPrgUnRAR->text().toUtf8().constData(), &prg_info);
		switch (status)
		{
			case 0:
				// RAR is usable.
				filename_icon = QStyle::SP_DialogYesButton;
				break;
			
			case -1:
				// RAR not found.
				prg_status = tr("The specified file was not found.");
				break;
			
			case -2:
				// RAR not executable.
				prg_status = m_sWarning + tr("The specified file is not executable.");
				filename_icon = QStyle::SP_MessageBoxWarning;
				break;
			
			case -3:
				// File isn't a regular file.
				prg_status = m_sWarning + tr("The specified file is not a regular file.");
				filename_icon = QStyle::SP_MessageBoxCritical;
				break;
			
			case -4:
				// Error calling stat().
				// TODO: Get the stat() error code?
				prg_status = m_sWarning + tr("Error calling stat().");
				filename_icon = QStyle::SP_MessageBoxCritical;
				break;
			
#ifdef Q_OS_WIN32
			case -5:
				// UnRAR.dll API version is too old. (Win32 only)
				prg_status = m_sWarning + tr("UnRAR.dll API version is too old.") + QLatin1String("<br/>\n") +
							   tr("Gens/GS II requires API version %1 or later.").arg(RAR_DLL_VERSION);
				filename_icon = QStyle::SP_MessageBoxCritical;
				break;
#endif
			
			case -6:
				// Version information not found.
#ifdef Q_OS_WIN32
				prg_status = m_sWarning + tr("DLL version information not found.");
#else
				prg_status = m_sWarning + tr("Program version information not found.");
#endif
				filename_icon = QStyle::SP_MessageBoxCritical;
				break;
			
			case -7:
				// Not RAR, UnRAR, or UnRAR.dll.
#ifdef Q_OS_WIN32
				prg_status = m_sWarning + tr("Selected DLL is not UnRAR.dll.");
#else
				prg_status = m_sWarning + tr("Selected program is neither RAR nor UnRAR.");
#endif
				filename_icon = QStyle::SP_MessageBoxCritical;
				break;
			
			default:
				// Unknown error.
				prg_status = m_sWarning + tr("Unknown error code %1 received from RAR file handler.").arg(status);
				filename_icon = QStyle::SP_MessageBoxWarning;
				break;
		}
	}
	
	// Set program ID.
	if (prg_info.dll_major != 0 || prg_info.dll_minor != 0 ||
	    prg_info.dll_revision != 0 || prg_info.dll_build != 0)
	{
		switch (prg_info.rar_type)
		{
			case LibGens::DcRar::ExtPrgInfo::RAR_ET_UNKNOWN:
			default:
				break;
			
			case LibGens::DcRar::ExtPrgInfo::RAR_ET_UNRAR:
				prg_id = tr("UnRAR");
				break;
			
			case LibGens::DcRar::ExtPrgInfo::RAR_ET_RAR:
				prg_id = tr("RAR");
				break;
			
			case LibGens::DcRar::ExtPrgInfo::RAR_ET_UNRAR_DLL:
				prg_id = tr("UnRAR.dll");
				break;
		}
	}
	sExtPrgStatus_UnRAR = tr("Identified as: %1").arg(prg_id);
	
	// Line break string.
	const QString sLineBreak = QLatin1String("<br/>\n");
	
	// Print DLL version information, if available.
	if (prg_info.dll_major != 0 || prg_info.dll_minor != 0 ||
	    prg_info.dll_revision != 0 || prg_info.dll_build != 0)
	{
		QString rar_version;
#ifdef Q_OS_WIN32
		rar_version = tr("%1 version %2.%3.%4.%5");
		rar_version = rar_version.arg(prg_id);
		rar_version = rar_version.arg(prg_info.dll_major);
		rar_version = rar_version.arg(prg_info.dll_minor);
		rar_version = rar_version.arg(prg_info.dll_revision);
		rar_version = rar_version.arg(prg_info.dll_build);
#else
		rar_version = tr("%1 version %2.%3");
		rar_version = rar_version.arg(prg_id);
		rar_version = rar_version.arg(prg_info.dll_major);
		rar_version = rar_version.arg(prg_info.dll_minor);
#endif
		sExtPrgStatus_UnRAR += sLineBreak + sLineBreak + rar_version;
#ifdef Q_OS_WIN32
		if (prg_info.api_version > 0)
			sExtPrgStatus_UnRAR += sLineBreak + tr("API version %1").arg(prg_info.api_version);
#endif
	}
	
	if (!prg_status.isEmpty())
		sExtPrgStatus_UnRAR += sLineBreak + sLineBreak + prg_status;
	
	// Set the textbox's icon.
	txtExtPrgUnRAR->setIcon(style()->standardIcon(filename_icon));
	
	// TODO: Create a constant string for DLL vs. binary.
	// For now, just call focusIn() to update the description.
	on_txtExtPrgUnRAR_focusIn();
	
	// Settings have been changed.
#ifndef GCW_APPLY_IMMED
	setApplyButtonEnabled(true);
#else
	SetValByPath_QString("External_Programs/UnRAR", txtExtPrgUnRAR->text());
#endif
}

}