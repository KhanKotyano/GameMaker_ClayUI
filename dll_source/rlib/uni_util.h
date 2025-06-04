#ifndef UNI_UTIL_LIB
#define UNI_UTIL_LIB
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <inttypes.h>
#include <math.h>
typedef int8_t s8;
typedef uint8_t u8;
typedef int16_t s16;
typedef uint16_t u16;
typedef int32_t s32;
typedef uint32_t u32;
typedef int64_t s64;
typedef uint64_t u64;
typedef float f32;
typedef double f64;

typedef int_fast32_t fast_int;
typedef uint_fast32_t fast_uint;
typedef s8 bool_s8;

static uint_fast64_t __loop_cntr64 = 0;
#define repeat64(COUNT) for(__loop_cntr64 = 0; __loop_cntr64 < COUNT; __loop_cntr64++)
#define loop64(name,COUNT) for(uint_fast64_t name = 0; name < COUNT; name++)

#define loop(name, count) for(fast_int name = 0;name < count;name++)
#define loopr(name, count) for(fast_int name = count;name >= count;name--)
static fast_uint __REPEAT_COUNTER__ = 0;
#define repeat(count) for(__REPEAT_COUNTER__ = 0; __REPEAT_COUNTER__< count;__REPEAT_COUNTER__++)

#if defined(__cplusplus)
#define TYPEDEC(type)      type
#define DLLEXPORT extern "C" __declspec(dllexport)
#else
#define TYPEDEC(type)      (type)
#define DLLEXPORT __declspec(dllexport)
#endif
#define ALPHAZERO_UNI(type)  TYPEDEC(type){ 0, 0, 0, 0 }   // Zero alpha
#define LIGHTGRAY_UNI(type)  TYPEDEC(type){ 200, 200, 200, 255 }   // Light Gray
#define GRAY_UNI(type)       TYPEDEC(type){ 130, 130, 130, 255 }   // Gray
#define DARKGRAY_UNI(type)   TYPEDEC(type){ 80, 80, 80, 255 }      // Dark Gray
#define YELLOW_UNI(type)     TYPEDEC(type){ 253, 249, 0, 255 }     // Yellow
#define GOLD_UNI(type)       TYPEDEC(type){ 255, 203, 0, 255 }     // Gold
#define ORANGE_UNI(type)     TYPEDEC(type){ 255, 161, 0, 255 }     // Orange
#define PINK_UNI(type)       TYPEDEC(type){ 255, 109, 194, 255 }   // Pink
#define RED_UNI(type)        TYPEDEC(type){ 230, 41, 55, 255 }     // Red
#define MAROON_UNI(type)     TYPEDEC(type){ 190, 33, 55, 255 }     // Maroon
#define GREEN_UNI(type)      TYPEDEC(type){ 0, 228, 48, 255 }      // Green
#define LIME_UNI(type)       TYPEDEC(type){ 0, 158, 47, 255 }      // Lime
#define DARKGREEN_UNI(type)  TYPEDEC(type){ 0, 117, 44, 255 }      // Dark Green
#define SKYBLUE_UNI(type)    TYPEDEC(type){ 102, 191, 255, 255 }   // Sky Blue
#define BLUE_UNI(type)       TYPEDEC(type){ 0, 121, 241, 255 }     // Blue
#define DARKBLUE_UNI(type)   TYPEDEC(type){ 0, 82, 172, 255 }      // Dark Blue
#define PURPLE_UNI(type)     TYPEDEC(type){ 200, 122, 255, 255 }   // Purple
#define VIOLET_UNI(type)     TYPEDEC(type){ 135, 60, 190, 255 }    // Violet
#define DARKPURPLE_UNI(type) TYPEDEC(type){ 112, 31, 126, 255 }    // Dark Purple
#define BEIGE_UNI(type)      TYPEDEC(type){ 211, 176, 131, 255 }   // Beige
#define BROWN_UNI(type)      TYPEDEC(type){ 127, 106, 79, 255 }    // Brown
#define DARKBROWN_UNI(type)  TYPEDEC(type){ 76, 63, 47, 255 }      // Dark Brown

#define WHITE_UNI(type)      TYPEDEC(type){ 255, 255, 255, 255 }   // White
#define BLACK_UNI(type)      TYPEDEC(type){ 0, 0, 0, 255 }         // Black
#define BLANK_UNI(type)      TYPEDEC(type){ 0, 0, 0, 0 }           // Blank (Transparent)
#define MAGENTA_UNI(type)    TYPEDEC(type){ 255, 0, 255, 255 }     // Magenta

#define EPSILON 0.000001f

#define EqualMemory_Util(Destination,Source,Length) (!memcmp((Destination),(Source),(Length)))
#define MoveMemory_Util(Destination,Source,Length) memmove((Destination),(Source),(Length))
#define CopyMemory_Util(Destination,Source,Length) memcpy((Destination),(Source),(Length))
#define FillMemory_Util(Destination,Length,Fill) memset((Destination),(Fill),(Length))
#define ZeroMemory_Util(Destination,Length) memset((Destination),0,(Length))

typedef struct {
  char *buffer;
  size_t size;
  size_t alloc_size;

}InternalMemoryArena;
//5529600


#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wunused-function"
static InternalMemoryArena MemoryArenaInit(size_t allocated_size){
  InternalMemoryArena arena = {0};
  arena.alloc_size = allocated_size;
  arena.buffer = (char *)calloc(allocated_size, sizeof(char));
  return arena;
}
static void* MemoryRequest(size_t byte_size, InternalMemoryArena *arena){
  char *_ret = (char *)(arena->buffer + arena->size);
  arena->size +=(size_t)(byte_size+(size_t)1);
  arena->buffer[byte_size] = '\0';
  return (void*)_ret;
}
static void MemoryArenaClear(InternalMemoryArena *arena){
  ZeroMemory_Util(arena->buffer, arena->size);
  arena->size = 0;
}


static void __memset64(void* dest, uint64_t value, uintptr_t size) {
  uintptr_t i;
  for (i = 0; i < (size & (~7)); i += 8)memcpy(((char*)dest) + i, &value, 8);
  for (; i < size; i++) ((char*)dest)[i] = ((char*)&value)[i & 7];
}
#ifndef max
#define max(a,b) (((a) > (b)) ? (a) : (b))
#endif

#ifndef min
#define min(a,b) (((a) < (b)) ? (a) : (b))
#endif

#define MAX max
#define memsetv __memset64
typedef union {
  u64 num;
  u32 au32[2];
  u16 au16[4];
  u8  au8[8];

}UniNum64;

typedef union {
  struct {
    s32 x;
    s32 y;
  };
  s32 element[2];
  s64 xy;
}GridVector2D;
#define iV2 GridVector2D

typedef struct {
  u32* data;
  u32 w;
  u32 h;
}Grid2D;

typedef struct {
  iV2* data;
  u32 w;
  u32 h;
}GridV2;
typedef union {
    u8 array[4];
    u8 array_u24[3];
    struct {u8 r;u8 g;u8 b;u8 a;};
    u32 uni;
}UniColor;



static inline int F32Equals(float x, float y){
    return (fabsf(x - y)) <= (EPSILON*fmaxf(1.0f, fmaxf(fabsf(x), fabsf(y))));;
}

static inline float SignF32(float a) {
  float  result = 0.0f;
  if (a > 0.0f) {result = 1.0f;}
  else if (a < 0.0f) {result = -1.0f;}
  return result;
}
static inline float ClampF32(float Min, float Value, float Max) {
  float Result = Value;
  if (Result < Min)Result = Min;
  if (Result > Max)Result = Max;
  return Result;
}

static iV2 GetGridOnHover(float _mouse_world_pos_x, float _mouse_world_pos_y, unsigned int grid_width, unsigned int grid_height,
  float grid_world_offset_x, float grid_world_offset_y, int pixel_size) {
  iV2 on_hover_grid = {0};
  on_hover_grid.x = (((unsigned int)_mouse_world_pos_x - (unsigned int)grid_world_offset_x) / pixel_size) * (_mouse_world_pos_x >= 0);
  on_hover_grid.y = (((unsigned int)_mouse_world_pos_y -(unsigned int)grid_world_offset_y) / pixel_size) * (_mouse_world_pos_y >= 0);
  if (on_hover_grid.x >= grid_width) on_hover_grid.x = grid_width - 1;
  if (on_hover_grid.y >= grid_height) on_hover_grid.y = grid_height - 1;
  return on_hover_grid;
}
static bool_s8 InRangeiV2(iV2 value, s32 min, s32 max) {
  bool_s8 ret = 0;
  ret = (bool_s8)(((value.x >= min) && (value.y >= min)) && ((value.x <= max) && (value.y <= max)));
  return ret;
}
static bool_s8 InRange(s64 value, s64 min, s64 max) {
  bool_s8 ret = 0;
  ret = (bool_s8)(((value >= min)  && (value <= max)));
  return ret;
}


///-------------------TEMP------------------------//

uintmax_t ConvertStringToNumber(char* s) {
  uintmax_t num = strtoumax(s, NULL, 10);
  if (num == UINTMAX_MAX && errno == ERANGE) {printf("Conversion error has occured\n");}
  return num;
}
uintmax_t ConvertHexStringToNumber(char* s) {
  uintmax_t num = strtoumax(s, NULL, 16);
  if (num == UINTMAX_MAX && errno == ERANGE) {printf("Conversion error has occured\n");}
  return num;
}

#endif