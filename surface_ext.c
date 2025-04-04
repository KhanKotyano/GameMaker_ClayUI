
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wunused-function"
#pragma clang diagnostic ignored "-Wunused-variable"
#include "rlib/uni_util.h"
static void ScreenBufferInit(Image *buffer, int w, int h){
  buffer->format = PIXELFORMAT_UNCOMPRESSED_R8G8B8A8;
  buffer->height = h;
  buffer->width = w;
  buffer->mipmaps = 1;
  buffer->data = NULL;
  //buffer->data = (char*)calloc(w*h, sizeof(char) * 4);
}
static void ScreenBufferInitFill(Image *buffer, int w, int h){
  buffer->format = PIXELFORMAT_UNCOMPRESSED_R8G8B8A8;
  buffer->height = h;
  buffer->width = w;
  buffer->mipmaps = 1;
  buffer->data = NULL;
  buffer->data = (char*)calloc(w*h, sizeof(char) * 4);
} 

  
void Clay_Raylib_Render_Surface(RenderTexture2D *surface,Clay_RenderCommandArray renderCommands, Font* fonts){
  ClearBackground(ALPHAZERO_UNI(Color));
  BeginTextureMode(*surface);
  Clay_Raylib_Render( renderCommands,  fonts);
  EndTextureMode();
}
static RenderTexture2D surface_render = {0};
DLLEXPORT double gml_clay_get_gui_surface(char* _GMSBuffPtrStr, double width, double height) {
  char* _GMSBuffer = getGMSBuffAddress(_GMSBuffPtrStr);
  if(surface_render.texture.width != width && surface_render.texture.height != height){
    UnloadRenderTexture(surface_render);
    ZeroMemory_Util(&surface_render, sizeof(RenderTexture2D));
    surface_render = LoadRenderTexture(width, height);
  }

  Clay_Raylib_Render_Surface(&surface_render,clay_data.render_commands, clay_data.fonts );
  clay_data.screen_buffer = LoadImageFromTexture(surface_render.texture);
  //ImageTo()
  ImageFlipVertical(&clay_data.screen_buffer);
  memcpy(_GMSBuffer, clay_data.screen_buffer.data, width * height * 4);
  //UnloadImage(clay_data.screen_buffer);

  free(clay_data.screen_buffer.data);
  ZeroMemory_Util(&clay_data.screen_buffer, sizeof(Image));
  clay_data.screen_buffer.width = width;
  clay_data.screen_buffer.height = height;
  return 1;
}
  #pragma clang diagnostic pop