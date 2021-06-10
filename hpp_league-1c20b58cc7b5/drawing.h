class CDrawing
{
public:
	void FillArea(GLint x, GLint y, GLint w, GLint h, GLubyte r, GLubyte g, GLubyte b, GLubyte a);
	void DrawBox(GLint x, GLint y, GLint w, GLint h, GLint linewidth, GLubyte r, GLubyte g, GLubyte b, GLubyte a);
};

extern CDrawing g_Drawing;