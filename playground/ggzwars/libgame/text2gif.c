#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "libgame.h"

#define PROGRAM_NAME		"LibGame"		  

static unsigned int
    RedColor = 255,
    GreenColor = 255,
    BlueColor = 255;

static void QuitGifError(GifFileType *GifFile);
static void GenRasterTextLine(GifRowType *RasterBuffer, char *TextLine, int BufferWidth, int ForeGroundIndex);

void text2gif (char *text, char *filename)
{
    	int i, j, l, ImageWidth, ImageHeight, LogNumLevels, NumLevels;
    	GifRowType RasterBuffer[GIF_FONT_HEIGHT];
    	ColorMapObject *ColorMap;
    	GifFileType *GifFile;

	ImageHeight = GIF_FONT_HEIGHT;
	ImageWidth = GIF_FONT_WIDTH * strlen (text);

    	for (i = 0; i < GIF_FONT_HEIGHT; i++)
		if ((RasterBuffer[i] = (GifRowType) malloc(
				sizeof(GifPixelType) * ImageWidth)) == NULL)
			GIF_EXIT("Failed to allocate memory required, aborted.");

    	if ((GifFile = EGifOpenFileName(filename, TRUE)) == NULL)
		QuitGifError(GifFile);

    	for (LogNumLevels = 1, NumLevels = 2; NumLevels < 1;
			LogNumLevels++, NumLevels <<= 1);
    		if (NumLevels < (1 << 1)) 
		{
    			NumLevels = (1 << 1);
			LogNumLevels = 1;
		}

    	if ((ColorMap = MakeMapObject(NumLevels, NULL)) == NULL)
		GIF_EXIT("Failed to allocate memory required, aborted.");

    	for (i = 0; i < NumLevels; i++)
		ColorMap->Colors[i].Red = ColorMap->Colors[i].Green = 
			ColorMap->Colors[i].Blue = 0;
    	ColorMap->Colors[1].Red = RedColor;
    	ColorMap->Colors[1].Green = GreenColor;
    	ColorMap->Colors[1].Blue = BlueColor;

    	if (EGifPutScreenDesc(GifFile, ImageWidth, ImageHeight, LogNumLevels,
				0, ColorMap) == GIF_ERROR)
		QuitGifError(GifFile);

    	if (EGifPutImageDesc(GifFile, 0, 0, ImageWidth, ImageHeight, FALSE,
				NULL) == GIF_ERROR)
		QuitGifError(GifFile);

    	GifQprintf("\n%s: Image 1 at (%d, %d) [%dx%d]:     ", 
			PROGRAM_NAME, GifFile->Image.Left, GifFile->Image.Top,
			GifFile->Image.Width, GifFile->Image.Height);

    
	GenRasterTextLine(RasterBuffer, text, ImageWidth, 1);
		for (j = 0; j < GIF_FONT_HEIGHT; j++) 
		{
	    		if (EGifPutLine(GifFile, RasterBuffer[j],
						ImageWidth) == GIF_ERROR)
				QuitGifError(GifFile);
	    		GifQprintf("\b\b\b\b%-4d", l++);
		}

    	if (EGifCloseFile(GifFile) == GIF_ERROR)
		QuitGifError(GifFile);
}

static void GenRasterTextLine(GifRowType *RasterBuffer, char *TextLine,
					int BufferWidth, int ForeGroundIndex)
{
	char c;
    	unsigned char Byte, Mask;
    	int i, j, k, CharPosX, Len = strlen(TextLine);

    	for (i = 0; i < BufferWidth; i++)
        	for (j = 0; j < GIF_FONT_HEIGHT; j++) RasterBuffer[j][i] = 0;

    	for (i = CharPosX = 0; i < Len; i++, CharPosX += GIF_FONT_WIDTH) 
	{
		c = TextLine[i];
		for (j = 0; j < GIF_FONT_HEIGHT; j++) 
		{
	    		Byte = AsciiTable[(int)c][j];
	    		for (k = 0, Mask = 128; k < GIF_FONT_WIDTH; k++,
					Mask >>= 1)
				if (Byte & Mask)
		    			RasterBuffer[j][CharPosX + k] = 
						ForeGroundIndex;
		}
    	}
}

static void QuitGifError(GifFileType *GifFile)
{
    	PrintGifError();
    	if (GifFile != NULL) EGifCloseFile(GifFile);
    	exit(1);
}
