/***************************************************************************
 * gens-qt4: Gens Qt4 UI.                                                  *
 * AboutDialog.cpp: About Dialog.                                          *
 *                                                                         *
 * Copyright (c) 1999-2002 by Stéphane Dallongeville.                      *
 * Copyright (c) 2003-2004 by Stéphane Akhoun.                             *
 * Copyright (c) 2008-2014 by David Korth.                                 *
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

#include <gens-qt4/config.gens-qt4.h>

#ifdef HAVE_GLEW
// GL Extension Wrangler.
#include <GL/glew.h>
#endif

#include "AboutDialog.hpp"
#include "gqt4_main.hpp"
#include "GensQApplication.hpp"

// Included libraries.
#include <zlib.h>
#include "lzma/7z/7zVersion.h"

// LibGens includes.
#include "libgens/lg_main.hpp"
#include "libgens/Util/cpuflags.h"
#include "libgens/Util/Timing.hpp"
#include "libgens/credits.h"
#include "libgens/macros/common.h"

// C includes.
#include <string.h>

// Qt includes.
#include <QtCore/QString>
#include <QtCore/QStringList>
#include <QtCore/QDir>
#include <QtGui/QScrollArea>

// OpenGL includes.
// Needed for GL_VENDOR, GL_RENDERER, GL_VERSION, and glGetString()
// if Gens/GS II isn't compiled with GLEW support.
// TODO: #include <QtGui/QGLWidget> instead?
#ifndef __APPLE__
#include <GL/gl.h>
#else
#include <OpenGL/gl.h>
#endif

// GLBackend is required in order to obtain list of extensions.
#include "VBackend/GLBackend.hpp"


namespace GensQt4
{

// Static member initialization.
AboutDialog *AboutDialog::m_AboutDialog = nullptr;

/**
 * AboutDialog(): Initialize the About window.
 */
AboutDialog::AboutDialog(QWidget *parent)
	: QDialog(parent,
		Qt::Dialog |
		Qt::WindowTitleHint |
		Qt::WindowSystemMenuHint |
		Qt::WindowMinimizeButtonHint |
		Qt::WindowCloseButtonHint)
{
	setupUi(this);
	
	// Make sure the window is deleted on close.
	this->setAttribute(Qt::WA_DeleteOnClose, true);
	
#ifdef Q_WS_MAC
	// Remove the window icon. (Mac "proxy icon")
	this->setWindowIcon(QIcon());
	
	// Hide the frames.
	fraCopyrights->setFrameShape(QFrame::NoFrame);
	fraCopyrights->layout()->setContentsMargins(0, 0, 0, 0);
	fraIncLibraries->setFrameShape(QFrame::NoFrame);
	fraIncLibraries->layout()->setContentsMargins(0, 0, 0, 0);
	fraDebugInfo->setFrameShape(QFrame::NoFrame);
	fraDebugInfo->layout()->setContentsMargins(0, 0, 0, 0);
	fraCredits->setFrameShape(QFrame::NoFrame);
	fraCredits->layout()->setContentsMargins(0, 0, 0, 0);
#endif
	
	// Scroll areas aren't initialized.
	m_scrlAreaInit = false;
	
	// Initialize the About window teXt.
	initAboutDialogText();
}


/**
 * ~AboutDialog(): Shut down the About window.
 */
AboutDialog::~AboutDialog()
{
	// Clear the m_AboutDialog pointer.
	m_AboutDialog = nullptr;
}


/**
 * ShowSingle(): Show a single instance of the About window.
 * @param parent Parent window.
 */
void AboutDialog::ShowSingle(QWidget *parent)
{
	if (m_AboutDialog != nullptr) {
		// About Dialog is already displayed.
		// NOTE: This doesn't seem to work on KDE 4.4.2...
		QApplication::setActiveWindow(m_AboutDialog);
	} else {
		// About Dialog is not displayed.
		m_AboutDialog = new AboutDialog(parent);
		m_AboutDialog->show();
	}
}


/**
 * changeEvent(): Widget state has changed.
 * @param event State change event.
 */
void AboutDialog::changeEvent(QEvent *event)
{
	if (event->type() == QEvent::LanguageChange)
	{
		// Retranslate the UI.
		retranslateUi(this);
		
		// Reinitialize the About Dialog text.
		initAboutDialogText();
	}
	
	// Pass the event to the base class.
	this->QDialog::changeEvent(event);
}


/**
 * Initialize the About Dialog text.
 */
void AboutDialog::initAboutDialogText(void)
{
	// Line break string.
	const QString sLineBreak = QLatin1String("<br/>\n");
	
	// Build the copyright string.
	QString sCopyrights = QString::fromUtf8(
			"(c) 1999-2002 by Stéphane Dallongeville.<br/>\n"
			"(c) 2003-2004 by Stéphane Akhoun.<br />\n<br />\n"
			"Gens/GS (c) 2008-2013 by David Korth.<br />\n<br />\n");
	
	sCopyrights += tr("Visit the Gens homepage:") + sLineBreak +
			QLatin1String(
				"<a href=\"http://www.gens.me/\">"
				"http://www.gens.me/</a>") +
			sLineBreak + sLineBreak;
	
	sCopyrights += tr("For news on Gens/GS, visit Sonic Retro:") + sLineBreak +
			QLatin1String(
				"<a href=\"http://www.sonicretro.org/\">"
				"http://www.sonicretro.org/</a>");
	
	// Set the copyright string.
	lblCopyrights->setText(sCopyrights);
	lblCopyrights->setTextFormat(Qt::RichText);
	
	// Build the program title text.
	QString sPrgTitle =
		QLatin1String("<b>Gens/GS II</b>") + sLineBreak +
		tr("Version %1").arg(gqt4_app->applicationVersion()) + sLineBreak;

	if (LibGens::version_desc != nullptr) {
		// Append the version description.
		// TODO: Translate it?
		sPrgTitle += QLatin1String(LibGens::version_desc) + sLineBreak;
	}

#if !defined(GENS_ENABLE_EMULATION)
	//: "NO-EMULATION BUILD" means CPU cores aren't compiled in. Used for testing Gens/GS II on new platforms.
	sPrgTitle += QLatin1String("<b>") +
			tr("NO-EMULATION BUILD") +
			QLatin1String("</b>") + sLineBreak;
#endif

	if (LibGens::version_vcs != nullptr) {
		// Append the VCS revision to the title text.
		sPrgTitle += QLatin1String(LibGens::version_vcs) + sLineBreak;
	}

	// TODO: Should this be translatable?
	sPrgTitle += sLineBreak +
		tr("Sega Genesis / Mega Drive,") + sLineBreak +
		tr("Sega CD / Mega CD,") + sLineBreak +
		tr("Sega 32X emulator");
	
	// Set the program title text.
        lblPrgTitle->setText(sPrgTitle);
	
	// Set the included libraries text.
	lblIncLibraries->setText(AboutDialog::GetIncLibraries());
	lblIncLibraries->setTextFormat(Qt::RichText);
	
	// Set the debug information text.
	lblDebugInfo->setText(AboutDialog::GetDebugInfo());
	lblDebugInfo->setTextFormat(Qt::RichText);
	
	// Build the credits text.
	QString sCredits;
	sCredits.reserve(4096); // Preallocate the string.
	const GensGS_credits_t *p_credits = &GensGS_credits[0];
	const QString sIndent = QLatin1String("&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;");
	const QChar chrBullet(0x2022);	// U+2022: BULLET
	for (; p_credits->credit_title || p_credits->credit_name; p_credits++)
	{
		if (p_credits->credit_title)
		{
			// Title specified.
			if (!strncmp(p_credits->credit_title, "-", 2))
			{
				// Title is "-". Next line.
				sCredits += sLineBreak;
				continue;
			}
			else if (p_credits->credit_title[0] == '*')
			{
				// Subtitle.
				// TODO: Translate language translation subtitles?
				sCredits += sIndent + chrBullet;
				sCredits += QChar(L' ');
				sCredits += QString::fromUtf8(&p_credits->credit_title[1]) +
					QLatin1String(": ");
			}
			else
			{
				// Title is not "-". Print it.
				sCredits += QLatin1String("<b>") +
					QString::fromUtf8(p_credits->credit_title) +
					QLatin1String("</b>") + sLineBreak;
			}
		}
		
		if (p_credits->credit_name)
		{
			// Name specified.
			if (!p_credits->credit_title ||
			    p_credits->credit_title[0] != '*')
			{
				// No subtitle specified.
				// Indent the name.
				sCredits += sIndent;
			}
			
			// Append the name to the credits.
			sCredits += QString::fromUtf8(p_credits->credit_name) + sLineBreak;
		}
	}
	
	// Set the credits text.
	lblCredits->setText(sCredits);
	lblCredits->setTextFormat(Qt::RichText);
	
	if (!m_scrlAreaInit)
	{
		// Create the scroll areas.
		// Qt Designer's QScrollArea implementation is horribly broken.
		// Also, this has to be done after the labels are set, because
		// QScrollArea is kinda dumb.
		QScrollArea *scrlIncLibraries = new QScrollArea();
		scrlIncLibraries->setFrameShape(QFrame::NoFrame);
		scrlIncLibraries->setFrameShadow(QFrame::Plain);
		scrlIncLibraries->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
		scrlIncLibraries->setWidget(lblIncLibraries);
		scrlIncLibraries->setWidgetResizable(true);
		vboxIncLibraries->addWidget(scrlIncLibraries);
		scrlIncLibraries->setAutoFillBackground(false);
		
		QScrollArea *scrlDebugInfo = new QScrollArea();
		scrlDebugInfo->setFrameShape(QFrame::NoFrame);
		scrlDebugInfo->setFrameShadow(QFrame::Plain);
		scrlDebugInfo->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
		scrlDebugInfo->setWidget(lblDebugInfo);
		scrlDebugInfo->setWidgetResizable(true);
		vboxDebugInfo->addWidget(scrlDebugInfo);
		scrlDebugInfo->setAutoFillBackground(false);
		
		QScrollArea *scrlCredits = new QScrollArea();
		scrlCredits->setFrameShape(QFrame::NoFrame);
		scrlCredits->setFrameShadow(QFrame::Plain);
		scrlCredits->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
		scrlCredits->setWidget(lblCredits);
		scrlCredits->setWidgetResizable(true);
		vboxCredits->addWidget(scrlCredits);
		scrlCredits->setAutoFillBackground(false);
		
		// Scroll areas initialized.
		m_scrlAreaInit = true;
	}
}


/**
 * GetIncLibraries(): Get included libraries.
 * @return Included libraries.
 */
QString AboutDialog::GetIncLibraries(void)
{
	// Common strings.
	const QString sIntCopyOf = tr("Internal copy of %1.");
	const QString sLineBreak = QLatin1String("<br/>\n");
	
	// Included libraries string.
	QString sIncLibraries;
	
#if defined(HAVE_ZLIB) && defined(USE_INTERNAL_ZLIB)
	// ZLIB is included.
	sIncLibraries += sIntCopyOf.arg(QLatin1String("zlib-" ZLIB_VERSION)) + sLineBreak +
		QLatin1String("Copyright (c) 1995-2012 Jean-loup Gailly and Mark Adler.") + sLineBreak +
		QLatin1String("<a href=\"http://www.zlib.net/\">http://www.zlib.net/</a>");
#endif
	
#if defined(HAVE_ZLIB)
	// MiniZip is included.
	// TODO: Find a MiniZip version macro.
	if (!sIncLibraries.isEmpty())
		sIncLibraries += sLineBreak + sLineBreak;
	sIncLibraries += sIntCopyOf.arg(QLatin1String("MiniZip 1.1")) + sLineBreak +
		QLatin1String("Copyright (c) 1998-2010 by Gilles Vollant.") + sLineBreak +
		QLatin1String("<a href=\"http://www.winimage.com/zLibDll/minizip.html\">"
					"http://www.winimage.com/zLibDll/minizip.html</a>") + sLineBreak +
		QLatin1String("Zip64/Unzip Copyright (c) 2007-2008 by Even Rouault.") + sLineBreak +
		QLatin1String("Zip64/Zip Copyright (c) 2009-2001 by Mathias Svensson.");
#endif
	
#if defined(HAVE_LZMA) && defined(USE_INTERNAL_LZMA)
	// LZMA is included.
	if (!sIncLibraries.isEmpty())
		sIncLibraries += sLineBreak + sLineBreak;
	sIncLibraries += sIntCopyOf.arg(QLatin1String("the LZMA SDK " MY_VERSION)) + sLineBreak +
		QLatin1String("Copyright (c) 1999-2010 by Igor Pavlov.") + sLineBreak +
		QLatin1String("<a href=\"http://www.7-zip.org/sdk.html\">"
					"http://www.7-zip.org/sdk.html</a>");
#endif
	
#if defined(HAVE_GLEW) && defined(USE_INTERNAL_GLEW)
	// GLEW is included.
	// TODO: Make sure we have a valid OpenGL context.
	if (!sIncLibraries.isEmpty())
		sIncLibraries += sLineBreak + sLineBreak;
	
	const char *glewVersion = (const char*)glewGetString(GLEW_VERSION);
	QString sGlewVersion = (glewVersion ? QLatin1String(glewVersion) : QString());
	sIncLibraries += sIntCopyOf.arg(QLatin1String("GLEW ") + sGlewVersion) + sLineBreak +
		QLatin1String("Copyright (c) 2002-2008 by Milan Ikits.") + sLineBreak +
		QLatin1String("Copyright (c) 2002-2008 by Marcelo E. Magallon.") + sLineBreak +
		QLatin1String("Copyright (c) 2002 by Lev Povalahev.") + sLineBreak +
		QLatin1String("Mesa 3D code Copyright (c) 1999-2007 by Brian Paul.") + sLineBreak +
		QLatin1String("OpenGL code Copyright (c) 2007 The Khronos Group Inc.") + sLineBreak +
		QLatin1String("<a href=\"http://glew.sourceforge.net/\">"
					"http://glew.sourceforge.net/</a>");
#endif
	
	// Return the included libraries string.
	return sIncLibraries;
}


/**
 * Get debug information.
 * @return Debug information.
 */
QString AboutDialog::GetDebugInfo(void)
{
	// Line break string.
	const QString sLineBreak = QLatin1String("<br/>\n");

	// Debug information.
	QString sDebugInfo =
		tr("Compiled using Qt %1.").arg(QLatin1String(QT_VERSION_STR)) + sLineBreak +
		tr("Using Qt %1.").arg(QLatin1String(qVersion())) + sLineBreak + sLineBreak;

	// Reserve at least 4 KB for the debug information.
	sDebugInfo.reserve(4096);

	// CPU flags.
	// TODO: Move the array of CPU flag names to LibGens.
	//: CPU flags are extra features found in a CPU, such as SSE.
	sDebugInfo += tr("CPU flags:");
#if defined(__i386__) || defined(__amd64__)
	static const char *const CpuFlagNames[11] = {
		"MMX", "MMXEXT", "3DNow!", "3DNow! EXT",
		"SSE", "SSE2", "SSE3", "SSSE3",
		"SSE4.1", "SSE4.2", "SSE4a"
	};

	if (CPU_Flags == 0) {
		//: Used to indicate no special CPU features were found.
		sDebugInfo += tr("(none)");
	} else {
		int cnt = 0;
		for (int i = 0; i < ARRAY_SIZE(CpuFlagNames); i++) {
			if (CPU_Flags & (1 << i)) {
				if (cnt != 0)
					sDebugInfo += QLatin1String(", ");
				sDebugInfo += QLatin1String(CpuFlagNames[i]);
				cnt++;
			}
		}
	}
#else
	//: Used to indicate no special CPU features were found.
	sDebugInfo += tr("(none)");
#endif /* defined(__i386__) || defined(__amd64__) */
	sDebugInfo += sLineBreak;

	//: Timing method: Function used to handle emulation timing.
	sDebugInfo += tr("Timing method:") +
		QLatin1String(LibGens::Timing::GetTimingMethodName(LibGens::Timing::GetTimingMethod())) +
		QLatin1String("()") + sLineBreak + sLineBreak;

	//: Save directory: Directory where configuration and savestate files are saved.
	// TODO: Verify that the link works on Windows and Mac OS X.
	sDebugInfo += tr("Save directory:") + sLineBreak +
		QLatin1String("<a href=\"file://") + gqt4_cfg->configPath() + QLatin1String("\">") +
		QDir::toNativeSeparators(gqt4_cfg->configPath()) + QLatin1String("</a>") +
		sLineBreak + sLineBreak;

#ifdef Q_OS_WIN32
	// Win32 code page information.
	sDebugInfo += GetCodePageInfo() + sLineBreak;
#endif /* Q_OS_WIN32 */

#ifndef HAVE_OPENGL
	//: Displayed if Gens/GS II is compiled without OpenGL support.
	sDebugInfo += tr("OpenGL disabled.") + sLineBreak;
#else
	const char *glVendor = (const char*)glGetString(GL_VENDOR);
	const char *glRenderer = (const char*)glGetString(GL_RENDERER);
	const char *glVersion = (const char*)glGetString(GL_VERSION);

	// Translatable strings.

	//: String identifying the manufacturer of the OpenGL implementation. (e.g. "X.Org R300 Project")
	const QString qsid_glVendor = tr("OpenGL vendor string:");
	//: String identifying the specific OpenGL renderer. (e.g. "Gallium 0.4 on ATI RV530")
	const QString qsid_glRenderer = tr("OpenGL renderer string:");
	//: String identifying the OpenGL renderer version. (e.g. "2.1 Mesa 7.11-devel")
	const QString qsid_glVersion = tr("OpenGL version string:");
	//: Placeholder used if an OpenGL version string could not be retrieved.
	const QString qsid_unknown = tr("(unknown)");

	sDebugInfo += qsid_glVendor +
			QString(glVendor ? QLatin1String(glVendor) : qsid_unknown) + sLineBreak +
			qsid_glRenderer +
			QString(glRenderer ? QLatin1String(glRenderer) : qsid_unknown) + sLineBreak +
			qsid_glVersion +
			QString(glVersion ? QLatin1String(glVersion) : qsid_unknown) + sLineBreak;

#ifdef GL_SHADING_LANGUAGE_VERSION
	if (glVersion && glVersion[0] >= '2' && glVersion[1] == '.') {
		const char *glslVersion = (const char*)glGetString(GL_SHADING_LANGUAGE_VERSION);
		
		//: String identifying the OpenGL Shading Language version. (e.g. "1.20")
		const QString qsid_glslVersion = tr("GLSL version string:");
		sDebugInfo += qsid_glslVersion +
				QString(glslVersion
					? QLatin1String(glslVersion)
					: qsid_unknown) + sLineBreak;
	}
#endif /* GL_SHADING_LANGUAGE_VERSION */

	// OpenGL extensions.
	sDebugInfo += sLineBreak;
#ifndef HAVE_GLEW
	//: GL Extension Wrangler support was not compiled in.
	sDebugInfo += tr("GLEW disabled; no GL extensions supported.") + sLineBreak;
#else
	const char *glewVersion = (const char*)glewGetString(GLEW_VERSION);

	//: String identifying the GLEW version.
	const QString qsid_glewVersion = tr("GLEW version");
	sDebugInfo += qsid_glewVersion + QChar(L' ') +
			QString(glewVersion
				? QLatin1String(glewVersion)
				: tr("(unknown)")) + sLineBreak;

	// Get a list of OpenGL extensions that are in use.
	const QChar chrBullet(0x2022);	// U+2022: BULLET
	const QStringList& extsInUse = GLBackend::GLExtsInUse();

	if (extsInUse.isEmpty()) {
		//: No OpenGL extensions are being used.
		sDebugInfo += tr("No GL extensions in use.");
	} else {
		//: List what OpenGL extensions are in use.
		sDebugInfo += tr("Using GL extensions:");
		foreach (QString ext, extsInUse) {
			sDebugInfo += sLineBreak;
			sDebugInfo += chrBullet;
			sDebugInfo += QChar(L' ');
			sDebugInfo += ext;
		}
	}

#endif /* HAVE_GLEW */

#endif /* HAVE_OPENGL */

	// Trim whitespace at the end of sDebugInfo.
	return sDebugInfo.trimmed();
}


#ifdef Q_OS_WIN32
#define WIN32_LEAN_AND_MEAN
#ifndef NOMINMAX
#define NOMINMAX
#endif
#include <windows.h>

/**
 * GetCodePageInfo(): Get information about the system code pages.
 * @return System code page information.
 */
QString AboutDialog::GetCodePageInfo(void)
{
	QString sCodePageInfo;

	// Get the ANSI and OEM code pages.
	struct cpInfo {
		unsigned int cp;
		const char *cpStr;
	};

	cpInfo m_cpInfo[2] = {
		//: Win32: ANSI code page. (e.g. 1252 for US/English, 932 for Japanese)
		{CP_ACP,	QT_TR_NOOP("System ANSI code page:")},
		//: Win32: OEM code page. (e.g. 437 for US/English)
		{CP_OEMCP,	QT_TR_NOOP("System OEM code page:")}
	};

	// TODO: GetCPInfoExU() support?
	for (int i = 0; i < 2; i++) {
		sCodePageInfo += tr(m_cpInfo[i].cpStr);

		// Get the code page information.
		CPINFOEX cpix;
		BOOL bRet = GetCPInfoExA(m_cpInfo[i].cp, 0, &cpix);
		if (!bRet) {
			//: GetCPInfoExA() call failed.
			sCodePageInfo += tr("Unknown [GetCPInfoExA() failed]") + QChar(L'\n');
			continue;
		}

		sCodePageInfo += QString::number(cpix.CodePage);

		// if the code page name is blank, don't add extra parentheses.
		if (cpix.CodePageName[0] == 0x00) {
			sCodePageInfo += QChar(L'\n');
			continue;
		}

		// Add the code page name.
		sCodePageInfo += QLatin1String(" (");

		// Windows XP has the code page number in cpix.CodePageName,
		// followed by two spaces, and then the code page name in parentheses.
		char *parenStart = strchr(cpix.CodePageName, '(');
		if (!parenStart) {
			// No parentheses. Use the code page name as-is.
			sCodePageInfo += QString::fromLocal8Bit(cpix.CodePageName);
		} else {
			// Found starting parenthesis. Check for ending parenthesis.
			char *parenEnd = strrchr(parenStart, ')');
			if (parenEnd) {
				// Found ending parenthesis. Null it out.
				*parenEnd = 0x00;
			}

			sCodePageInfo += QString::fromLocal8Bit(parenStart + 1);
		}

		sCodePageInfo += QLatin1String(")\n");
	}

	// Is Gens/GS II using Unicode?
	if (GetModuleHandleW(nullptr) != nullptr) {
		//: Win32: Unicode strings are being used. (WinNT)
		sCodePageInfo += tr("Using Unicode strings for Win32 API.");
	} else {
		//: Win32: ANSI strings are being used. (Win9x)
		sCodePageInfo += tr("Using ANSI strings for Win32 API.");
	}
	sCodePageInfo += QChar(L'\n');

	return sCodePageInfo;
}
#endif /* Q_OS_WIN32 */

}
