#include "stdafx.h"
#include "glui.h"
#include "RGBpixmap.h"

FILE *inf;

void RGBpixmap::makeCheckBoard()
{
	int count, i, j, c;

	nRows = nCols = 64;

	if(!(pixel = new RGB[3 * nRows * nCols])) 
		return;

	count = 0;

	for(i = 0; i < nRows; i++)
	{
		for(j = 0; j < nCols; j++)
		{
			c = (((i / 8) + (j / 8)) % 2) * 255;
			pixel[count].r = c;
			pixel[count].g = c;
			pixel[count++].b = 0;
		}
	}
}

void RGBpixmap::setTexture(GLuint textureName)
{
	glBindTexture(GL_TEXTURE_2D,textureName);
	glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_NEAREST);
	glTexImage2D(GL_TEXTURE_2D,0,GL_RGB,nCols,nRows,0,GL_RGB,GL_UNSIGNED_BYTE,pixel);
}

unsigned short getShort()
{
	char ic;
	unsigned short ip;

	ic = getc(inf); ip = ic;
	ic = getc(inf); ip |= ((unsigned short)ic << 8);
	
	return ip;
}

unsigned long getLong()
{
	unsigned long ip=0;
	char ic=0;
	unsigned char uc = ic;

	ic = getc(inf);	uc = ic; ip = uc;
	ic = getc(inf);	uc = ic; ip |= ((unsigned long)uc << 8);
	ic = getc(inf);	uc = ic; ip |= ((unsigned long)uc << 16);
	ic = getc(inf);	uc = ic; ip |= ((unsigned long)uc << 24);

	return ip;
}


int RGBpixmap::readBMPFile(char * fname)
{
	int k, row, col, numPadBytes, nBytesInRow, count;
	char c1, c2, dum, r, g, b;
	unsigned long fileSize, offBits, headerSize, numCols, numRows, compression, imageSize;
	unsigned long xPels, yPels, numLUTentries, impColors;
	unsigned short reserved, planes, bitsPerPixel;

	if((inf = fopen(fname, "r")) == NULL)
		return 0;
	
	c1 = getc(inf);	c2 = getc(inf);

	fileSize = getLong();
	reserved = getShort();
	reserved = getShort();
	offBits = getLong();
	headerSize = getLong();
	numCols = getLong();
	numRows = getLong();
	
	planes = getShort();
	bitsPerPixel = getShort();
	compression = getLong();
	imageSize = getLong();
	xPels = getLong();
	yPels = getLong();
	numLUTentries = getLong();
	impColors = getLong();

	if(bitsPerPixel != 24)
	{	
		printf("A imagem nao e de 24 bits por pixel!\n");
		fclose(inf);
		return 0;
	}

	nBytesInRow = ((3 * numCols + 3) / 4) * 4;
	numPadBytes = nBytesInRow - 3 * numCols;
	nRows = numRows;
	nCols = numCols;

	if (!(pixel = new RGB[3 * nRows * nCols]))
	{	printf("Erro a reservar memoria!");
		return 0;
	}

	count = 0;

	for(row = 0; row < nRows; row++)
	{
		for(col = 0; col < nCols; col++)
		{
			b = getc(inf); g = getc(inf); r = getc(inf);
            pixel[count].r = r;
			pixel[count].g = g;
			pixel[count++].b = b;
		}

		for(k = 0; k < numPadBytes; k++)
			dum = fgetc(inf);
	}

	fclose(inf);

	return 1;	
}