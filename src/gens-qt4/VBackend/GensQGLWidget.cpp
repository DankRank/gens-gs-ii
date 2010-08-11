/***************************************************************************
 * gens-qt4: Gens Qt4 UI.                                                  *
 * GensQGLWidget.cpp: QGLWidget subclass.                                  *
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

#include <config.h>

#include "GensQGLWidget.hpp"
#include "gqt4_main.hpp"

// C includes.
#include <stdio.h>

// LibGens includes.
#include "libgens/MD/VdpRend.hpp"
#include "libgens/macros/log_msg.h"
#include "libgens/Util/Timing.hpp"
#include "libgens/MD/EmuMD.hpp"

// Win32 requires GL/glext.h for OpenGL 1.2/1.3.
// TODO: Check the GL implementation to see what functionality is available at runtime.
#ifdef _WIN32
#include <GL/glext.h>
#endif

// Qt includes.
#include <QtGui/QKeyEvent>
#include <QtGui/QMouseEvent>


namespace GensQt4
{

#ifdef HAVE_GLEW
/** Fragment programs. **/

/**
 * ms_fragPaused_asm(): Paused effect fragment program.
 * Based on grayscale shader from http://arstechnica.com/civis/viewtopic.php?f=19&t=445912
 */
const char *GensQGLWidget::ms_fragPaused_asm =
	"!!ARBfp1.0\n"
	"OPTION ARB_precision_hint_fastest;\n"
	"PARAM grayscale = {0.30, 0.59, 0.11, 0.0};\n"		// Standard RGB to Grayscale algorithm.
	"TEMP t0, color;\n"
	"TEX t0, fragment.texcoord[0], texture[0], 2D;\n"	// Get color coordinate.
	"DP3 color, t0, grayscale;\n"				// Calculate grayscale value.
	"ADD_SAT color.z, color.z, color.z;\n"			// Double the blue component.
	"MOV result.color, color;\n"
	"END\n";
#endif /* HAVE_GLEW */


GensQGLWidget::GensQGLWidget(QWidget *parent)
	: QGLWidget(QGLFormat(QGL::NoAlphaChannel | QGL::NoDepthBuffer), parent)
{
	m_tex = 0;
	
#ifdef HAVE_GLEW
	// ARB fragment programs.
	m_fragPaused = 0;
#endif /* HAVE_GLEW */
	
	// Accept keyboard focus.
	setFocusPolicy(Qt::StrongFocus);
	
	// Initialize mouse tracking.
	// TODO: Only do this if a Mega Mouse is connected.
	// TODO: IoMegaMouse doesn't work right.
#if 0
	m_lastMousePosValid = false;
	setMouseTracking(true);
#endif
}

GensQGLWidget::~GensQGLWidget()
{
	if (m_tex > 0)
	{
		glEnable(GL_TEXTURE_2D);
		glDeleteTextures(1, &m_tex);
		m_tex = 0;
		glDisable(GL_TEXTURE_2D);
	}
	
#ifdef HAVE_GLEW
	if (m_fragPaused > 0)
	{
		glDeleteProgramsARB(1, &m_fragPaused);
		m_fragPaused = 0;
	}
#endif /* HAVE_GLEW */
}


/**
 * reallocTexture(): (Re-)Allocate the OpenGL texture.
 */
void GensQGLWidget::reallocTexture(void)
{
	if (m_tex > 0)
		glDeleteTextures(1, &m_tex);
	
	// Create and initialize a GL texture.
	glEnable(GL_TEXTURE_2D);
	glGenTextures(1, &m_tex);
	glBindTexture(GL_TEXTURE_2D, m_tex);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
	
	// GL filtering.
	// TODO: Make this customizable!
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	
	// Determine the texture format and type.
	m_lastBpp = LibGens::VdpRend::Bpp;
	switch (m_lastBpp)
	{
		case LibGens::VdpRend::BPP_15:
			m_colorComponents = 4;
			m_texFormat = GL_BGRA;
			m_texType = GL_UNSIGNED_SHORT_1_5_5_5_REV;
			break;
		
		case LibGens::VdpRend::BPP_16:
			m_colorComponents = 3;
			m_texFormat = GL_RGB;
			m_texType = GL_UNSIGNED_SHORT_5_6_5;
			break;
		
		case LibGens::VdpRend::BPP_32:
		default:
			m_colorComponents = 4;
			m_texFormat = GL_BGRA;
			m_texType = GL_UNSIGNED_BYTE;
			break;
	}
	
	// Allocate the texture.
	glTexImage2D(GL_TEXTURE_2D, 0,
		     m_colorComponents,
		     512, 256,	// 512x256 (320x240 rounded up to nearest powers of two)
		     0,		// No border.
		     m_texFormat, m_texType, NULL);
	
	glDisable(GL_TEXTURE_2D);
	
	// Texture is dirty.
	m_vbDirty = true;
}


/**
 * initializeGL(): Called when GL is initialized.
 */
void GensQGLWidget::initializeGL(void)
{
	// OpenGL initialization.
	
	// Disable various OpenGL functionality.
	glDisable(GL_DEPTH_TEST);	// Depth buffer.
	glDisable(GL_BLEND);		// Blending.
	
	// Enable face culling.
	// This disables drawing the backsides of polygons.
	// Also, set the front face to GL_CW.
	// TODO: GL_CCW is default; rework everything to use CCW instead?
	glFrontFace(GL_CW);
	glEnable(GL_CULL_FACE);
	
#ifdef HAVE_GLEW
	// Initialize GLEW.
	GLenum err = glewInit();
	if (err != GLEW_OK)
	{
		// Error initializing GLEW!
		LOG_MSG(video, LOG_MSG_LEVEL_ERROR,
			"Error initializing GLEW: %s", glewGetErrorString(err));
	}
	
	if (GLEW_ARB_fragment_program)
	{
		// Fragment programs are supported.
		// Load the fragment programs.
		
		// Load the Paused effect fragment program.
		// TODO: Check for errors!
		glGenProgramsARB(1, &m_fragPaused);
		glBindProgramARB(GL_FRAGMENT_PROGRAM_ARB, m_fragPaused);
		glGetError();	// Clear the error flag.
		glProgramStringARB(GL_FRAGMENT_PROGRAM_ARB, GL_PROGRAM_FORMAT_ASCII_ARB,
				   strlen(ms_fragPaused_asm), ms_fragPaused_asm);
		
		GLenum err = glGetError();
		if (err != GL_NO_ERROR)
		{
			// An error occured while loading the fragment program.
			// TODO: Remove the extra newline at the end of err_str.
			const char *err_str = (const char*)glGetString(GL_PROGRAM_ERROR_STRING_ARB);
			LOG_MSG(video, LOG_MSG_LEVEL_ERROR,
				"Error creating Paused effect FP: %s", (err_str ? err_str : "(unknown)"));
			
			// Delete the fragment program.
			if (m_fragPaused > 0)
			{
				glDeleteProgramsARB(1, &m_fragPaused);
				m_fragPaused = 0;
			}
		}
	}
#endif /* HAVE_GLEW */

	// Initialize the GL viewport and projection.
	resizeGL(320, 240);
	
	// Allocate the texture.
	reallocTexture();
}


void GensQGLWidget::resizeGL(int width, int height)
{
	glViewport(0, 0, width, height);
	
	// Set the OpenGL projection.
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	
	// Aspect ratio constraint.
	bool aspect_constraint = true;	// TODO: Make this configurable.
	if (!aspect_constraint)
	{
		// No aspect ratio constraint.
		glOrtho(-1, 1, -1, 1, -1, 1);
	}
	else
	{
		// Aspect ratio constraint.
		if ((width * 3) > (height * 4))
		{
			// Image is wider than 4:3.
			glOrtho(-((double)(width * 3) / (double)(height * 4)),
				 ((double)(width * 3) / (double)(height * 4)),
				-1, 1, -1, 1);
		}
		else if ((width * 3) < (height * 4))
		{
			// Image is taller than 4:3.
			glOrtho(-1, 1,
				-((double)(height * 4) / (double)(width * 3)),
				 ((double)(height * 4) / (double)(width * 3)),
				-1, 1);
		}
		else
		{
			// Image has the correct aspect ratio.
			glOrtho(-1, 1, -1, 1, -1, 1);
		}
	}
	
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
}


void GensQGLWidget::paintGL(void)
{
	/**
	 * bFromMD: If this is true after all effects are applied,
	 * use LibGens::VDP_Rend::MD_Screen[] directly.
	 * Otherwise, use m_intScreen[].
	 */
	bool bFromMD = true;
	
	glClearColor(1.0, 0.0, 0.0, 1.0);
	glClear(GL_COLOR_BUFFER_BIT);
	
	if (m_vbDirty)
	{
		// MD_Screen is dirty.
		
		// Check if the Bpp has changed.
		if (LibGens::VdpRend::Bpp != m_lastBpp)
		{
			// Bpp has changed. Reallocate the texture.
			reallocTexture();
		}
		
		/** START: Apply effects. **/
		
		// If Fast Blur is enabled, update the Fast Blur effect.
		if (fastBlur())
		{
			updateFastBlur(bFromMD);
			bFromMD = false;
		}
		
		// If emulation is paused, update the pause effect.
#ifdef HAVE_GLEW
		if (paused() && m_fragPaused == 0)
		{
			// Paused, but the fragment program isn't usable.
			// Apply the effect in software.
			updatePausedEffect(bFromMD);
			bFromMD = false;
		}
#else
		if (paused())
		{
			updatePausedEffect(bFromMD);
			bFromMD = false;
		}
#endif /* HAVE_GLEW */
		
		// Determine which screen buffer should be used for video output.
		GLvoid *screen = (bFromMD ? (GLvoid*)LibGens::VdpRend::MD_Screen.u16 : (GLvoid*)m_intScreen);
		
		/** END: Apply effects. **/
		
		// Bind the texture.
		glEnable(GL_TEXTURE_2D);
		glBindTexture(GL_TEXTURE_2D, m_tex);
		
		// (Re-)Upload the texture.
		glPixelStorei(GL_UNPACK_ROW_LENGTH, 336);
		glPixelStorei(GL_UNPACK_SKIP_PIXELS, 8);
		glPixelStorei(GL_UNPACK_ALIGNMENT, 8);
		
		glTexSubImage2D(GL_TEXTURE_2D, 0,
				0, 0,		// x/y offset
				320, 240,	// width/height
				m_texFormat, m_texType, screen);
		
		glPixelStorei(GL_UNPACK_ROW_LENGTH, 0);
		glPixelStorei(GL_UNPACK_SKIP_PIXELS, 0);
		glPixelStorei(GL_UNPACK_ALIGNMENT, 0);
		
		// Texture is no longer dirty.
		m_vbDirty = false;
	}
	else
	{
		// MD Screen isn't dirty.
		// Simply bind the texture.
		glEnable(GL_TEXTURE_2D);
		glBindTexture(GL_TEXTURE_2D, m_tex);
	}
	
#ifdef HAVE_GLEW
	// Enable the fragment program.
	if (paused() && m_fragPaused > 0)
	{
		glEnable(GL_FRAGMENT_PROGRAM_ARB);
		glBindProgramARB(GL_FRAGMENT_PROGRAM_ARB, m_fragPaused);
	}
#endif /* HAVE_GLEW */
	
	// Draw the texture.
	glBegin(GL_QUADS);
	glTexCoord2d(0.0, 0.0);
	glVertex2i(-1, 1);
	glTexCoord2d((320.0/512.0), 0.0);
	glVertex2i(1, 1);
	glTexCoord2d((320.0/512.0), (240.0/256.0));
	glVertex2i(1, -1);
	glTexCoord2d(0.0, (240.0/256.0));
	glVertex2i(-1, -1);
	glEnd();
	
#ifdef HAVE_GLEW
	// Disable the fragment program.
	if (paused() && m_fragPaused > 0)
		glDisable(GL_FRAGMENT_PROGRAM_ARB);
#endif /* HAVE_GLEW */
	
	glDisable(GL_TEXTURE_2D);
	
	// Print text to the screen.
	// TODO:
	// * renderText() doesn't support wordwrapping.
	// * renderText() doesn't properly handle newlines.
	// * fm.boundingRect() doesn't seem to handle wordwrapping correctly, either.
	QFontMetrics fm(m_osdFont, this);
	QRect boundRect;
	
	// TODO: Make the text colors customizable.
	QColor clShadow(0, 0, 0);
	QColor clText(255, 255, 255);
	
	int y = this->height();
	double curTime = LibGens::Timing::GetTimeD();
	
	// Check if the FPS should be drawn.
	// TODO: Integrate this with the for loop.
	if (showFps())
	{
		QString sFps = QString::number(m_fpsAvg, 'f', 1);
		boundRect = fm.boundingRect(8, 0, this->width() - 16, y, 0, sFps);
		y -= boundRect.height();
		
		// TODO: Make the drop shadow optional or something.
		qglColor(clShadow);
		renderText(8+1, y+1, sFps, m_osdFont);
		qglColor(clText);
		renderText(8-1, y-1, sFps, m_osdFont);
	}
	
	// NOTE: QList internally uses an array of pointers.
	// We can use array indexing instead of iterators.
	for (int i = (m_osdList.size() - 1); i >= 0; i--)
	{
		if (curTime >= m_osdList[i].endTime)
		{
			// Message duration has elapsed.
			// Remove the message from the list.
			m_osdList.removeAt(i);
			continue;
		}
		
		const QString msg = m_osdList[i].msg;
		boundRect = fm.boundingRect(8, 0, this->width() - 16, y, 0, msg);
		y -= boundRect.height();
		
		// TODO: Make the drop shadow optional or something.
		qglColor(clShadow);
		renderText(8+1, y+1, msg, m_osdFont);
		qglColor(clText);
		renderText(8-1, y-1, msg, m_osdFont);
	}
	
	// Reset the GL color.
	qglColor(QColor(255, 255, 255, 255));
}


/**
 * keyPressEvent(): Key press handler.
 * TODO: Move somewhere else?
 * @param event Key event.
 */
void GensQGLWidget::keyPressEvent(QKeyEvent *event)
{
	switch (event->key())
	{
		case Qt::Key_Escape:
			// Toggle the Pause effect.
			setPaused(!paused());
			vbUpdate();
			break;
		
		case Qt::Key_F1:
			// Toggle the Fast Blur effect.
			setFastBlur(!fastBlur());
			vbUpdate();
			break;
		
		default:
			if (!gqt4_emuThread)
				return;
			
			// Forward the key to the I/O devices.
			// NOTE: Port E isn't forwarded, since it isn't really usable as a controller.
			LibGens::EmuMD::m_port1->keyPress(event->key());
			LibGens::EmuMD::m_port2->keyPress(event->key());
			break;
	}
}


/**
 * keyReleaseEvent(): Key release handler.
 * TODO: Move somewhere else?
 * @param event Key event.
 */
void GensQGLWidget::keyReleaseEvent(QKeyEvent *event)
{
	if (!gqt4_emuThread)
		return;
	
	// Forward the key to the I/O devices.
	// NOTE: Port E isn't forwarded, since it isn't really usable as a controller.
	LibGens::EmuMD::m_port1->keyRelease(event->key());
	LibGens::EmuMD::m_port2->keyRelease(event->key());
}


#if 0
/**
 * mouseMoveEvent(): Mouse movement handler.
 * TODO: Move somewhere else?
 * @param event Mouse event.
 */
void GensQGLWidget::mouseMoveEvent(QMouseEvent *event)
{
	if (!gqt4_emuThread)
	{
		m_lastMousePosValid = false;
		return;
	}
	
	if (!m_lastMousePosValid)
	{
		// Last mouse movement event was invalid.
		m_lastMousePos = event->pos();
		m_lastMousePosValid = true;
		return;
	}
	
	// Calculate the relative movement.
	QPoint posDiff = (event->pos() - m_lastMousePos);
	m_lastMousePos = event->pos();
	
	// Forward the relative movement to the I/O devices.
	// NOTE: Port E isn't forwarded, since it isn't really usable as a controller.
	LibGens::EmuMD::m_port1->mouseMove(posDiff.x(), posDiff.y());
	LibGens::EmuMD::m_port2->mouseMove(posDiff.x(), posDiff.y());
}
#endif


/**
 * mousePressEvent(): Mouse button press handler.
 * TODO: Move somewhere else?
 * @param event Mouse event.
 */
void GensQGLWidget::mousePressEvent(QMouseEvent *event)
{
	if (!gqt4_emuThread)
		return;
	
	// Forward the mouse button press to the I/O devices.
	// NOTE: Port E isn't forwarded, since it isn't really usable as a controller.
	LibGens::EmuMD::m_port1->mousePress(event->button());
	LibGens::EmuMD::m_port2->mousePress(event->button());
}


/**
 * mouseReleaseEvent(): Mouse button release handler.
 * TODO: Move somewhere else?
 * @param event Mouse event.
 */
void GensQGLWidget::mouseReleaseEvent(QMouseEvent *event)
{
	if (!gqt4_emuThread)
		return;
	
	LibGens::EmuMD::m_port1->mouseRelease(event->button());
	LibGens::EmuMD::m_port2->mouseRelease(event->button());
}

}
