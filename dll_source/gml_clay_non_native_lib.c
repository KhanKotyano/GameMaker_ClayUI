#define NOGDI
#define NOUSER

#include "rlib/uni_util.h"
#include "rlib/raylib.h"
#define CLAY_IMPLEMENTATION
#include "clay.h"
#include "rlib/gml_util.h"

#define RLUNICODE_IMPLEMENTATION
#include "rlib/rlunicode.h"
static s32 render_flags[CLAY_RENDER_COMMAND_TYPE_CUSTOM+1]={1};


#define CLAY_COLOR_TO_UNI_COLOR(color) (UniColor) { .r = (unsigned char)roundf(color.r), .g = (unsigned char)roundf(color.g), .b = (unsigned char)roundf(color.b), .a = (unsigned char)roundf(color.a) }
#define UNI_COLOR_TO_CLAY_COLOR(color) (Clay_Color) { .r = (float)(color.r), .g = (float)(color.g), .b = (float)(color.b), .a = (float)(color.a) }
#define DEBUG_RENDER 1
#define NATIVE_RENDER 1



void HandleClayErrors(Clay_ErrorData errorData) {
    // See the Clay_ErrorData struct for more information
    printf("%s\n", errorData.errorText.chars);
}
typedef struct {
    int data;
    double delta_time;
    Clay_Dimensions screen_size;
    Clay_Vector2 mouse_pos;
    Clay_Vector2 mouse_wheel_pos;
    Clay_Arena memory;
    uint64_t memory_size;
    Clay_ErrorHandler clay_errorh;
    Clay_RenderCommandArray render_commands;
    int counter;
    #if !NATIVE_RENDER
    Image screen_buffer;
    #endif
    bool is_accept_pressed;
  
    int fonts_number;
    Font fonts[100];
}ClayInternalData;

static ClayInternalData clay_data = {0};
typedef struct {
  char *text[1000];
  s32 length;
  s32 counter;
  Clay_Dimensions text_dim[1000];
  bool_s8 request_completed;
} MeasureRequests;
static MeasureRequests m_requests = {0};
static InternalMemoryArena internal_arena = {0};
#if !NATIVE_RENDER
#include "surface_ext.c"
#endif
#include "clay_layout.h"



typedef struct {
  uint32_t *chars;
  uint32_t length;
}StringSliceUTF32;
 
static Clay_Dimensions GmlMeasureText(Clay_StringSlice text, Clay_TextElementConfig *config, void *userData) {
  //double idk = 110000102541E74; 
  
  Clay_Dimensions textSize = { 0 };
    Font fontToUse = clay_data.fonts[config->fontId];
    if (!fontToUse.glyphs) {
        printf("[ClayUI]: Font fail to be recognized\n");
        return textSize;
    }
    Vector2 v =  MeasureTextEx_ext(fontToUse, text.chars, config->fontSize, config->letterSpacing, text.length);
    textSize.width = v.x ;
    textSize.height = v.y;
    //MemoryArenaClear(&internal_arena);
    return textSize;
  }
  
  
DLLEXPORT double gml_clay_update(double width, double height) {
    #if !(NATIVE_RENDER)
    if(width !=  clay_data.screen_size.width && height!=clay_data.screen_size.height){
      SetWindowSize(width, height);
    }
    #endif
    clay_data.screen_size.height = height;
    clay_data.screen_size.width = width;

    ///clay_data.delta_time = GetFrameTime();
    Vector2 scrollDelta = GetMouseWheelMoveV();
    Clay_SetLayoutDimensions((Clay_Dimensions){.height = height, .width = width});
    Clay_SetPointerState(clay_data.mouse_pos, clay_data.is_accept_pressed);
    Clay_UpdateScrollContainers(true, (Clay_Vector2) { scrollDelta.x, scrollDelta.y }, clay_data.delta_time);
    Clay_SetMeasureTextFunction(GmlMeasureText, clay_data.fonts);
    clay_data.render_commands = ClayGetRenderCommands();
    //MemoryArenaClear(&internal_arena);
    //m_requests.request_completed = 0;  
    return (double)clay_data.render_commands.length;
}
DLLEXPORT double gml_clay_clear_arena(){
  MemoryArenaClear(&internal_arena);
  return 1;
}
DLLEXPORT double gml_clay_update_logic(double width, double height) {
  #if !(NATIVE_RENDER)
  if(width !=  clay_data.screen_size.width && height!=clay_data.screen_size.height){
    SetWindowSize(width, height);
  }
  #endif
  clay_data.screen_size.height = height;
  clay_data.screen_size.width = width;

  //clay_data.delta_time = GetFrameTime();
  // IsKeyPressed
  Clay_SetLayoutDimensions((Clay_Dimensions){.height = height, .width = width});
  Clay_SetPointerState(clay_data.mouse_pos,  clay_data.is_accept_pressed);
  Clay_UpdateScrollContainers(true, clay_data.mouse_wheel_pos, clay_data.delta_time);
  Clay_SetMeasureTextFunction(GmlMeasureText, clay_data.fonts);
 // clay_data.render_commands = ClayGetRenderCommands();
  //MemoryArenaClear(&internal_arena);
  //m_requests.request_completed = 0;  
  return 1;
}
DLLEXPORT double gml_clay_debug_mode(double flag) {
  Clay_SetDebugModeEnabled((bool)flag);
  return 1;
}
DLLEXPORT double gml_clay_init(double screen_width, double screen_height) {
    internal_arena = MemoryArenaInit((size_t)(5529600 * 2));
    clay_data.memory_size = Clay_MinMemorySize();
    clay_data.memory.capacity = clay_data.memory_size;
    clay_data.memory.memory = (char*)calloc(clay_data.memory_size,sizeof(char)) ;
    clay_data.clay_errorh.errorHandlerFunction = HandleClayErrors ;
    clay_data.screen_size.width = (float)screen_width;
    clay_data.screen_size.height = (float)screen_height;
    Clay_Initialize(clay_data.memory, clay_data.screen_size, clay_data.clay_errorh);
    #if !NATIVE_RENDER
    SetTraceLogLevel(LOG_WARNING);
    ScreenBufferInit(&clay_data.screen_buffer,(int)screen_width,(int)screen_height);
    u32 flags =  FLAG_WINDOW_HIGHDPI | FLAG_MSAA_4X_HINT | FLAG_VSYNC_HINT | FLAG_WINDOW_HIDDEN | FLAG_WINDOW_RESIZABLE | FLAG_WINDOW_TRANSPARENT;
    Clay_Raylib_Initialize(screen_width, screen_height, "render", flags);
    surface_render = LoadRenderTexture(screen_width, screen_height);
   
    loop(i,8)render_flags[i] = 1;
    #endif 
    return 1;
}
DLLEXPORT double gml_clay_add_font(char *name){
    if(clay_data.fonts_number < 100){
      //clay_data.fonts[clay_data.fonts_number] = LoadFontFromMemory(".ttf", );
      clay_data.fonts[clay_data.fonts_number] = LoadFontEx(TextFormat("fonts/%s", name), 32, 0, 400);
      if(clay_data.fonts_number == 0){
        Clay_SetMeasureTextFunction(GmlMeasureText, clay_data.fonts);
      }
      return (double)clay_data.fonts[clay_data.fonts_number++].baseSize;
    }
    printf("you reacher the maximum number of fonts\n");
    return -1;
  }
#include "native_functions.h"