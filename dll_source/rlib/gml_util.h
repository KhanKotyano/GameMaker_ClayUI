#ifndef GML_UTIL_HEADER
#define GML_UTIL_HEADER
#include "uni_util.h"
//For the most part, the remnants of the breakup with raylib framework

int Util_GetCodepointNext(const char *text, int *codepointSize)
{
    const char *ptr = text;
    int codepoint = 0x3f;       // Codepoint (defaults to '?')
    *codepointSize = 1;

    // Get current codepoint and bytes processed
    if (0xf0 == (0xf8 & ptr[0]))
    {
        // 4 byte UTF-8 codepoint
        if (((ptr[1] & 0xC0) ^ 0x80) || ((ptr[2] & 0xC0) ^ 0x80) || ((ptr[3] & 0xC0) ^ 0x80)) { return codepoint; } // 10xxxxxx checks
        codepoint = ((0x07 & ptr[0]) << 18) | ((0x3f & ptr[1]) << 12) | ((0x3f & ptr[2]) << 6) | (0x3f & ptr[3]);
        *codepointSize = 4;
    }
    else if (0xe0 == (0xf0 & ptr[0]))
    {
        // 3 byte UTF-8 codepoint */
        if (((ptr[1] & 0xC0) ^ 0x80) || ((ptr[2] & 0xC0) ^ 0x80)) { return codepoint; } // 10xxxxxx checks
        codepoint = ((0x0f & ptr[0]) << 12) | ((0x3f & ptr[1]) << 6) | (0x3f & ptr[2]);
        *codepointSize = 3;
    }
    else if (0xc0 == (0xe0 & ptr[0]))
    {
        // 2 byte UTF-8 codepoint
        if ((ptr[1] & 0xC0) ^ 0x80) { return codepoint; } // 10xxxxxx checks
        codepoint = ((0x1f & ptr[0]) << 6) | (0x3f & ptr[1]);
        *codepointSize = 2;
    }
    else if (0x00 == (0x80 & ptr[0]))
    {
        // 1 byte UTF-8 codepoint
        codepoint = ptr[0];
        *codepointSize = 1;
    }

    return codepoint;
}
bool IsFontDataValid(FontData font)
{
    return ((font.baseSize > 0) &&    
            (font.glyphCount > 0) &&   
            (font.recs != NULL) &&     
            (font.glyphs != NULL));

    // NOTE: Further validations could be done to verify if recs and glyphs contain valid data (glyphs values, metrics...)
}
int GetGlyphDataIndex(FontData font, int codepoint)
{
    int index = 0;
    if (!IsFontDataValid(font)) return index;
    int fallbackIndex = 0;      // Get index of fallback glyph '?'
    for (int i = 0; i < font.glyphCount; i++){
        if (font.glyphs[i].value == 63) fallbackIndex = i;

        if (font.glyphs[i].value == codepoint)
        {
            index = i;
            break;
        }
    }

    if ((index == 0) && (font.glyphs[0].value != codepoint)) index = fallbackIndex;

    return index;
}
Vector2 MeasureTextEx_ext(FontData font, const char *text, float fontSize, float spacing, int size){
    Vector2 textSize = { 0 };
    int tempByteCounter = 0;        
    int byteCounter = 0;

    float textWidth = 0.0f;
    float tempTextWidth = 0.0f;     

    float textHeight = (fontSize/(float)font.baseSize);
    textHeight = textHeight * 2;
    float scaleFactor = fontSize/(float)font.baseSize;

    int letter = 0;                 
    int index = 0;                  

    for (int i = 0; i < size;)
    {
        byteCounter++;

        int codepointByteCount = 0;
        letter = Util_GetCodepointNext(&text[i], &codepointByteCount);
        index = GetGlyphDataIndex(font, letter);

        i += codepointByteCount;

        if (letter != '\n')
        {
            if (font.glyphs[index].advanceX > 0) textWidth += font.glyphs[index].advanceX;
            else textWidth += (font.recs[index].width + font.glyphs[index].offsetX);
        }
        else
        {
            if (tempTextWidth < textWidth) tempTextWidth = textWidth;
            byteCounter = 0;
            textWidth = 0;

            // NOTE: Line spacing is a global variable, use SetTextLineSpacing() to setup
            textHeight += (fontSize + 2);
        }

        if (tempByteCounter < byteCounter) tempByteCounter = byteCounter;
    }

    if (tempTextWidth < textWidth) tempTextWidth = textWidth;

    textSize.x = tempTextWidth*scaleFactor + (float)((tempByteCounter - 1)*spacing);
    textSize.y = textHeight;

    return textSize;
}
// static char* getGMSBuffAddress(char* _GMSBuffPtrStr) {
//     /*
//         @description    Converts a GMS buffer address string to a usable pointer in C++.
//         @params         {char*} _GMSBuffPtrStr - The ptr to a GMS buffer as a string.
//         @return         {char*} The pointer to the buffer. Now functions like memcpy will work.
//     */
//     char* GMSBuffLongPointer =  (char*)strtoull(_GMSBuffPtrStr, NULL, 16);//Gets the ptr string into and int64_t.
//     return (char*)GMSBuffLongPointer;//Casts the int64_t pointer to char* and returns it so the buffer can be now operated in C++.
//   }
  typedef struct {
    u8 size;
    char *value_ptr;
  }GmlVar;

#endif