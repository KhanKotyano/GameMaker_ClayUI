#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <inttypes.h>
#include <math.h>
#define STB_TRUETYPE_IMPLEMENTATION
#include "stb_truetype.h"
typedef struct GlyphData {
    int value;              // Character value (Unicode)
    int offsetX;            // Character offset X when drawing
    int offsetY;            // Character offset Y when drawing
    int advanceX;           // Character advance position X
} GlyphData;
typedef struct  {
    int width;
    int height;
}fRect;
typedef struct {
    int baseSize;           // Base size (default chars height)
    int glyphCount;         // Number of glyph characters
    int glyphPadding;       // Padding around the glyph characters
    fRect *recs;
    GlyphData *glyphs;      // Glyphs info data
} FontData;
unsigned char *Util_LoadFileData(const char *fileName, int *dataSize){
    unsigned char *data = NULL;
    *dataSize = 0;

    if (fileName != NULL){
        FILE *file = fopen(fileName, "rb");
        if (file != NULL){
            fseek(file, 0, SEEK_END);
            int size = ftell(file); 
            fseek(file, 0, SEEK_SET);

            if (size > 0){
                data = (unsigned char *)malloc(size*sizeof(unsigned char));
                if (data != NULL){
                    size_t count = fread(data, sizeof(unsigned char), size, file);
                    if (count > 2147483647){
                        free(data);
                        data = NULL;
                    }else{
                        *dataSize = (int)count;
                    }
                }
            }
            fclose(file);
        }
    }

    return data;
}

void Util_UnloadFileData(unsigned char *data){
    free(data);
}
bool Util_TextIsEqual(const char *text1, const char *text2)
{
    bool result = false;

    if ((text1 != NULL) && (text2 != NULL))
    {
        if (strcmp(text1, text2) == 0) result = true;
    }

    return result;
}
#define SUPPORT_FILEFORMAT_TTF
    // Default values for ttf font generation
#ifndef FONT_TTF_DEFAULT_SIZE
    #define FONT_TTF_DEFAULT_SIZE           32      // TTF font generation default char size (char-height)
#endif
#ifndef FONT_TTF_DEFAULT_NUMCHARS
    #define FONT_TTF_DEFAULT_NUMCHARS       95      // TTF font generation default charset: 95 glyphs (ASCII 32..126)
#endif
#ifndef FONT_TTF_DEFAULT_FIRST_CHAR
    #define FONT_TTF_DEFAULT_FIRST_CHAR     32      // TTF font generation default first char for image sprite font (32-Space)
#endif
#ifndef FONT_TTF_DEFAULT_CHARS_PADDING
    #define FONT_TTF_DEFAULT_CHARS_PADDING   4      // TTF font generation default chars padding
#endif
GlyphData *Util_LoadFontData(const unsigned char *fileData, int dataSize, int fontSize, int *codepoints, int codepointCount, int type)
{
    // NOTE: Using some SDF generation default values,
    // trades off precision with ability to handle *smaller* sizes
#ifndef FONT_SDF_CHAR_PADDING
    #define FONT_SDF_CHAR_PADDING            4      // SDF font generation char padding
#endif
#ifndef FONT_SDF_ON_EDGE_VALUE
    #define FONT_SDF_ON_EDGE_VALUE         128      // SDF font generation on edge value
#endif
#ifndef FONT_SDF_PIXEL_DIST_SCALE
    #define FONT_SDF_PIXEL_DIST_SCALE     64.0f     // SDF font generation pixel distance scale
#endif
#ifndef FONT_BITMAP_ALPHA_THRESHOLD
    #define FONT_BITMAP_ALPHA_THRESHOLD     80      // Bitmap (B&W) font generation alpha threshold
#endif

    GlyphData *chars = NULL;

#if defined(SUPPORT_FILEFORMAT_TTF)
    // Load font data (including pixel data) from TTF memory file
    // NOTE: Loaded information should be enough to generate font image atlas, using any packaging method
    if (fileData != NULL)
    {
        int genFontChars = 0;
        stbtt_fontinfo fontInfo = { 0 };

        if (stbtt_InitFont(&fontInfo, (unsigned char *)fileData, 0))     // Initialize font for data reading
        {
            // Calculate font scale factor
            float scaleFactor = stbtt_ScaleForPixelHeight(&fontInfo, (float)fontSize);

            // Calculate font basic metrics
            // NOTE: ascent is equivalent to font baseline
            int ascent, descent, lineGap;
            stbtt_GetFontVMetrics(&fontInfo, &ascent, &descent, &lineGap);

            // In case no chars count provided, default to 95
            codepointCount = (codepointCount > 0)? codepointCount : 95;

            // Fill fontChars in case not provided externally
            // NOTE: By default we fill glyphCount consecutively, starting at 32 (Space)
            if (codepoints == NULL)
            {
                codepoints = (int *)malloc(codepointCount*sizeof(int));
                for (int i = 0; i < codepointCount; i++) codepoints[i] = i + 32;
                genFontChars = true;
            }

            chars = (GlyphData *)calloc(codepointCount, sizeof(GlyphData));

            // NOTE: Using simple packaging, one char after another
            for (int i = 0; i < codepointCount; i++)
            {
                //int chw = 0, chh = 0;   // Character width and height (on generation)
                int ch = codepoints[i];  // Character value to get info for
                chars[i].value = ch;
                int index = stbtt_FindGlyphIndex(&fontInfo, ch);

                if (index > 0)
                {
                    if (chars[i].value != 0)    // Glyph data has been found in the font
                    {
                        stbtt_GetCodepointHMetrics(&fontInfo, ch, &chars[i].advanceX, NULL);
                        chars[i].advanceX = (int)((float)chars[i].advanceX*scaleFactor);
                        chars[i].offsetY += (int)((float)ascent*scaleFactor);
                    }

                    // NOTE: We create an empty image for space character,
                    // it could be further required for atlas packing
                    if (ch == 32)
                    {
                        stbtt_GetCodepointHMetrics(&fontInfo, ch, &chars[i].advanceX, NULL);
                        chars[i].advanceX = (int)((float)chars[i].advanceX*scaleFactor);
                    }
                }
                else
                {
                    // TODO: Use some fallback glyph for codepoints not found in the font
                }
            }
        }

        if (genFontChars) free(codepoints);
    }
#endif

    return chars;
}
#define MAX_TEXT_BUFFER_LENGTH 1024
char *Util_TextToLower(const char *text)
{
    static char buffer[MAX_TEXT_BUFFER_LENGTH] = { 0 };
    memset(buffer, 0, MAX_TEXT_BUFFER_LENGTH);

    if (text != NULL)
    {
        for (int i = 0; (i < MAX_TEXT_BUFFER_LENGTH - 1) && (text[i] != '\0'); i++)
        {
            if ((text[i] >= 'A') && (text[i] <= 'Z')) buffer[i] = text[i] + 32;
            else buffer[i] = text[i];
        }
    }

    return buffer;
}
FontData Util_LoadFontFromMemory(const char *fileType, const unsigned char *fileData, int dataSize, int fontSize, int *codepoints, int codepointCount)
{
    FontData font = { 0 };

    char fileExtLower[16] = { 0 };
    strncpy(fileExtLower, Util_TextToLower(fileType), 16 - 1);

    font.baseSize = fontSize;
    font.glyphCount = (codepointCount > 0)? codepointCount : 95;
    font.glyphPadding = 0;

#if defined(SUPPORT_FILEFORMAT_TTF)
    if (Util_TextIsEqual(fileExtLower, ".ttf") ||
        Util_TextIsEqual(fileExtLower, ".otf"))
    {
        font.glyphs = Util_LoadFontData(fileData, dataSize, font.baseSize, codepoints, font.glyphCount, 0);
    }
    else
#endif
#if defined(SUPPORT_FILEFORMAT_BDF)
    if (TextIsEqual(fileExtLower, ".bdf"))
    {
        font.glyphs = LoadFontDataBDF(fileData, dataSize, codepoints, font.glyphCount, &font.baseSize);
    }
    else
#endif
    {
        font.glyphs = NULL;
    }

#if defined(SUPPORT_FILEFORMAT_TTF) || defined(SUPPORT_FILEFORMAT_BDF)
    if (font.glyphs != NULL)   {
        font.glyphPadding = FONT_TTF_DEFAULT_CHARS_PADDING;
    }
#else
    font = GetFontDefault();
#endif

    return font;
}
const char *Util_GetFileExtension(const char *fileName){
    const char *dot = strrchr(fileName, '.');

    if (!dot || dot == fileName) return NULL;

    return dot;
}
FontData Util_LoadFontEx(const char *fileName, int fontSize, int *codepoints, int codepointCount){
    FontData font = { 0 };

    // Loading file to memory
    int dataSize = 0;
    unsigned char *fileData = Util_LoadFileData(fileName, &dataSize);

    if (fileData != NULL){
        // Loading font from memory data
        font = Util_LoadFontFromMemory(Util_GetFileExtension(fileName), fileData, dataSize, fontSize, codepoints, codepointCount);

        Util_UnloadFileData(fileData);
    }

    return font;
}
#pragma clang diagnostic pop
