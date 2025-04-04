#ifndef GML_UTIL_HEADER
#define GML_UTIL_HEADER
#include "uni_util.h"
Vector2 MeasureTextEx_ext(Font font, const char *text, float fontSize, float spacing, int size){
    Vector2 textSize = { 0 };
    int tempByteCounter = 0;        // Used to count longer text line num chars
    int byteCounter = 0;

    float textWidth = 0.0f;
    float tempTextWidth = 0.0f;     // Used to count longer text line width

    float textHeight = fontSize;
    float scaleFactor = fontSize/(float)font.baseSize;

    int letter = 0;                 // Current character
    int index = 0;                  // Index position in sprite font

    for (int i = 0; i < size;)
    {
        byteCounter++;

        int codepointByteCount = 0;
        letter = GetCodepointNext(&text[i], &codepointByteCount);
        index = GetGlyphIndex(font, letter);

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