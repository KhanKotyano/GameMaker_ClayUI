#include "raylib.h"
#define RLGL_IMPLEMENTATION
#define GRAPHICS_API_OPENGL_11
#include "rlgl.h"
#if defined(_WIN32)
    #define APIENTRY __stdcall
#else
    #define APIENTRY
#endif
#include "GL/GL.h"
//#include "rlgl.h"
//rlReadTexturePixels implementation without malloc
void rlWriteTexturePixels(unsigned int id, int width, int height, int format, void *pixels)
{
    if(pixels == NULL){
        TRACELOG(RL_LOG_WARNING, "TEXTURE: [ID %i] Attempt to write texture data to a null pointer", id, format);
        return;
    }
#if defined(GRAPHICS_API_OPENGL_11) || defined(GRAPHICS_API_OPENGL_33)
    glBindTexture(GL_TEXTURE_2D, id);
    glPixelStorei(GL_PACK_ALIGNMENT, 1);

    unsigned int glInternalFormat, glFormat, glType;
    rlGetGlTextureFormats(format, &glInternalFormat, &glFormat, &glType);
    unsigned int size = rlGetPixelDataSize(width, height, format);

    if ((glInternalFormat != 0) && (format < RL_PIXELFORMAT_COMPRESSED_DXT1_RGB))
    {
        glGetTexImage(GL_TEXTURE_2D, 0, glFormat, glType, pixels);
    }
    else TRACELOG(RL_LOG_WARNING, "TEXTURE: [ID %i] Data retrieval not suported for pixel format (%i)", id, format);

    glBindTexture(GL_TEXTURE_2D, 0);
#endif

#if defined(GRAPHICS_API_OPENGL_ES2)

 
    unsigned int fboId = rlLoadFramebuffer();

    glBindFramebuffer(GL_FRAMEBUFFER, fboId);
    glBindTexture(GL_TEXTURE_2D, 0);

 
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, id, 0);

    glReadPixels(0, 0, width, height, GL_RGBA, GL_UNSIGNED_BYTE, (unsigned char *)pixels);

    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    rlUnloadFramebuffer(fboId);
#endif
}
void UpdateImage(Image image,Texture2D texture ){
    if (texture.format < PIXELFORMAT_COMPRESSED_DXT1_RGB)
    {
        if(image.width == texture.width && image.height == texture.height){
          rlWriteTexturePixels(texture.id, texture.width, texture.height, texture.format,image.data);
        }else TRACELOG(LOG_WARNING, "TEXTURE: [ID %i] Cant assert width and hegiht are equal between texture and image", texture.id);
    }
    else TRACELOG(LOG_WARNING, "TEXTURE: [ID %i] Failed to retrieve compressed pixel data", texture.id);
  }
  