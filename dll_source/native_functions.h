



// DLLEXPORT char *gml_clay_get_next_text_signal(double id){
//   return m_requests.text[(int)id];
// }
// DLLEXPORT double gml_clay_get_text_signal_left(){
//   m_requests.length--;
//   return (double)(m_requests.length);
// }
// DLLEXPORT double gml_clay_set_text_signal_dimentions(double width, double height){
//   m_requests.text_dim[m_requests.length].height = (float)height;
//   m_requests.text_dim[m_requests.length].width = (float)width;
//   return 1;
// }
// DLLEXPORT double gml_clay_end_text_signal_process(){
//   //m_requests.counter= 0;
//   m_requests.length = 0;
//   m_requests.request_completed = 1;
//   return 1;
// }

DLLEXPORT double gml_clay_get_on_hover(char *name){
  return (double)Clay_PointerOver(Clay_GetElementId((Clay_String){.chars = name, .length = strlen(name)}));
}
DLLEXPORT double gml_clay_get_on_hover_id(double id){
  return (double)Clay_PointerOver((Clay_ElementId){.id = id});
}
DLLEXPORT double gml_clay_set_mouse_pos(double x, double y){
  clay_data.mouse_pos.x = x;
  clay_data.mouse_pos.y = y;
  return 1;
}
DLLEXPORT double gml_clay_set_is_assept_down(double assept_state){
  clay_data.is_accept_pressed = assept_state;
  return 1;
}
DLLEXPORT double gml_clay_set_delta_time(double delta_time){
  clay_data.delta_time = delta_time;
  return 1;
}

DLLEXPORT double gml_clay_set_render_flag(double flag, double value){
  u32 _lflag = (u32)flag;
  if(_lflag <  CLAY_RENDER_COMMAND_TYPE_CUSTOM && _lflag > 0){
    render_flags[_lflag] = (s32)value;
    printf("[ClayUI]: Flag [id:%d] changed to value [%d]", _lflag, (s32)value);
  }
  
  return 1;
}
DLLEXPORT double gml_clay_get_render_flag(double flag){
  u32 _lflag = (u32)flag;
  if(_lflag <  CLAY_RENDER_COMMAND_TYPE_CUSTOM && _lflag > 0){
    return render_flags[_lflag];
  }
  return -1;
}

DLLEXPORT double gml_clay_set_mousewheel_vector(double x, double y) {
  clay_data.mouse_wheel_pos.x =  (float)x;
  clay_data.mouse_wheel_pos.y = (float)y;

  return 1;
}
DLLEXPORT double gml_clay_get_command_buffer_length(){
    return (double)clay_data.render_commands.length;
}
DLLEXPORT double gml_clay_get_command_buffer_size() {
    return (double)((clay_data.render_commands.length * sizeof(Clay_RenderCommand)));
}

#define local_cmd clay_data.render_commands.internalArray[clay_data.counter]



DLLEXPORT double gml_clay_get_on_image_id(){
  return  (double)local_cmd.renderData.image.imageData;
}
DLLEXPORT double gml_clay_get_cmd_type(){
  return (double)local_cmd.commandType;
}
DLLEXPORT double gml_clay_get_cmd_id(){
  return (double)local_cmd.id;
}
DLLEXPORT double gml_clay_get_box_x(){
  return (double)local_cmd.boundingBox.x;
}
DLLEXPORT double gml_clay_get_box_y(){
  return (double)local_cmd.boundingBox.y;
}
DLLEXPORT double gml_clay_get_box_w(){
  return (double)local_cmd.boundingBox.width;
}
DLLEXPORT double gml_clay_get_box_h(){
  return (double)local_cmd.boundingBox.height;
}
DLLEXPORT double gml_clay_get_color(){
  UniColor color= CLAY_COLOR_TO_UNI_COLOR(local_cmd.renderData.border.color); 
  return  (double)color.uni;
}
DLLEXPORT double gml_clay_get_roundiness(){
  return local_cmd.renderData.rectangle.cornerRadius.bottomLeft * 2;
}
DLLEXPORT double gml_clay_get_font_id(){
  return local_cmd.renderData.text.fontId;
}
DLLEXPORT double gml_clay_get_font_size(){
  return local_cmd.renderData.text.fontSize;
}
DLLEXPORT double gml_clay_get_border_w(){
  UniNum64 num = {0};
  memcpy(num.au8,&local_cmd.renderData.border.width, sizeof(UniNum64));
  return (double)num.num;
}
DLLEXPORT double gml_clay_get_is_brect_full(){
  UniNum64 num = {0};
  memcpy(num.au8,&local_cmd.renderData.border.width, sizeof(UniNum64));
  return (double)(num.au16[0] && num.au16[1] && num.au16[2] && num.au16[3]);
}

static char *cloned = NULL;
DLLEXPORT char* gml_clay_get_string(){
  cloned = (char *)MemoryRequest(local_cmd.renderData.text.stringContents.length + 1, &internal_arena);
  //(char *)malloc(local_cmd.renderData.text.stringContents.length + 1);
  memcpy(cloned, local_cmd.renderData.text.stringContents.chars, local_cmd.renderData.text.stringContents.length);
  cloned[local_cmd.renderData.text.stringContents.length] = '\0';
  return cloned;
}
DLLEXPORT double gml_clay_free_string(){
  //if (cloned != NULL) free(cloned);
  return internal_arena.size;
}
// s32 RemoveSpacesAtTheEnd(char *gml_string, s32 length){
//   s32 count = 0;
//   loopr(i,length-1){
//     if(gml_string[i] == ' '){
//       count++;
//       gml_string[i] = 0;
//     } else {
//       break;
//     }
//   }
//   return length-count;
// }
DLLEXPORT char *gml_clay_save_string(char *gml_string){
  s32 length = strlen(gml_string);

  cloned = (char *)MemoryRequest(length + 1, &internal_arena);
  memcpy(cloned, gml_string, length);
  //length = RemoveSpacesAtTheEnd(cloned, length);
  //if (cloned != NULL) free(cloned);
  return cloned;
}
DLLEXPORT double gml_clay_dispatch_command(){
  if(clay_data.counter+1 < clay_data.render_commands.length) {
    clay_data.counter++;
    return 1;
  }
  clay_data.counter =-1;
  return 0;
}