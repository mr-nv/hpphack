#include "main.h"

CDrawing g_Drawing;

void CDrawing::FillArea(GLint x, GLint y, GLint w, GLint h, GLubyte r, GLubyte g, GLubyte b, GLubyte a)
{
	glPushMatrix();
	glLoadIdentity();
	glDisable(GL_TEXTURE_2D);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glColor4ub(r, g, b, a);
	glBegin(GL_QUADS);
	glVertex2i(x, y);
	glVertex2i(x + w, y);
	glVertex2i(x + w, y + h);
	glVertex2i(x, y + h);
	glEnd();
	glDisable(GL_BLEND);
	glEnable(GL_TEXTURE_2D);
	glPopMatrix();
}

void CDrawing::DrawBox(GLint x, GLint y, GLint w, GLint h, GLint linewidth, GLubyte r, GLubyte g, GLubyte b, GLubyte a)
{
	FillArea(x, y, w, linewidth, r, g, b, a);
	FillArea(x + w - linewidth, y + linewidth, linewidth, h - linewidth, r, g, b, a);
	FillArea(x, y + linewidth, linewidth, h - linewidth, r, g, b, a);
	FillArea(x + linewidth, y + h - linewidth, w - linewidth * 2, linewidth, r, g, b, a);
}