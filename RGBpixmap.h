
class RGB
{
	public:
		unsigned char r, g, b;

		RGB()
		{
			r = 0;
			g = 0;
			b = 0;
		}
};

class RGBpixmap
{
	public:
		int nRows, nCols;
		RGB *pixel;

		RGBpixmap()
		{	
			nRows = 0;
			nCols = 0;
			pixel = NULL;
		}

		~RGBpixmap()
		{	
			if(pixel != NULL)
				delete []pixel;
		}

		int readBMPFile(char * fname);
		void makeCheckBoard();
		void setTexture(GLuint textureName);
};