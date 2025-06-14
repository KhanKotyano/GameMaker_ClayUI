

enum CLAY_CMD_TYPE {
	NONE,
    RECTANGLE,
    BORDER,
	TEXT,
    IMAGE,
    START,
	END,
    CUSTOM,
};
enum CLAY_LAYOUT_SIZING{
    GROW = 0, // Grow width of height until fill the empty space of a parent
    FIT, // Grow width of height until fit all children unside (recommended)
	PERCENT,//percentage of total parent width of height
    FIXED //Fixed width of height
};
enum CLAY_LAYOUT_ALLIGMENT_X {
	LEFT = 0,   //Allign X to the left corner of the parent
    RIGHT,      //Allign X to the right corner of the parent
    CENTER,     //Allign X to the center of the parent
};
enum CLAY_LAYOUT_ALLIGMENT_Y {
	TOP = 0,    //Allign Y to the top side of the parent
    BOTTOM,     //Allign Y to the bottom side of the parent
    CENTER,     //Allign Y to the center of the parent
};
enum CLAY_LAYOUT_DIRECTION {
	RIGHT = 0,
	BOTTOM,
};
enum CLAY_ATTACH_POINT {
	LEFT_TOP = 0,
    LEFT_CENTER,
    LEFT_BOTTOM,
    CENTER_TOP,
    CENTER_CENTER,
    CENTER_BOTTOM,
    RIGHT_TOP,
    RIGHT_CENTER,
    RIGHT_BOTTOM,
};

function ClayInit(_width, height){
	static ret =0;
	if(!ret){
		ret = gml_clay_init(_width, height);
		ClayOnInit();
		if(ret !=1){
			show_error("[ClayUI]:Clay was not able to init itself", true);
			/*
				If an error message lead you here, this means clay.dll was not 
				be able to load it self. Please check you settings of clay in the ClayUI pluggin
				and set nlib.dll target to Windows, and export target to Windows.
				If nothing helps, message to "Khan Kotyan (˶•̀⩊•́˶)" on discord,
				or in developer chat to try to solve this issue
			*/
		} else {
			show_debug_message("[ClayUI]:Init is done");	
		}
	} else {
		show_debug_message("[ClayUI]:Clay already initialized, stop calling this function");	
	}
	return ret;
}
function CreateASCII(){
	var _map = ds_map_create();
	for(var i =0;i< 255;i++){
		_map[? chr(i)] = chr(i);
	}
	return _map;
}
global.__clay_render_stack__ = ds_stack_create();
global.__clay_render_args__ = ds_map_create();
function ClayUpdateSceduleArgs(_method, _args = []){
	global.__clay_render_args__[? _method] = _args;
	
}
function ClaySceduleLayout(_method, _args, _gui = true){
	if(_gui){
		ds_stack_push(global.__clay_render_stack__, _method);
		global.__clay_render_args__[? _method] = _args;
	}
}
global.__ASCII_table__ = CreateASCII();
#macro ASCII global.__ASCII_table__

global.win_w = 1280;
global.win_h = 720;
#macro WIN_WIDTH global.win_w 
#macro WIN_HEIGHT global.win_h 
global.__font_array__ = array_create(100);
global.__font_array_size = 0;
global.__sprite_map__ = ds_map_create();
global.__reverse_sprite_map__ = ds_map_create();
global.__last_sprite_id__ = 1;

global.__ui_string_table__ = ds_map_create();
global.__ui_string_table_last_free__ =0;


//function SetDynamicString(_string){
//	static _dstring_size = 0;
//	_dstring_size  = gml_clay_free_string();
//	gml_clay_save_string(_string);
//	return _dstring_size;
//}
//function SaveAndGetString(_string){
//	var _d = gml_clay_free_string();
//	_string = gml_clay_save_string(_string);
//	_d = gml_clay_free_string();
//	return _string
//}

function SetUIStringUnderName(_string, _name){
	if(global.__ui_string_table__[? _name] != _string){
		global.__ui_string_table__[? _name] = _string;
	}
	return global.__ui_string_table__[? _name];
}
function AddUIString(_string){
	global.__ui_string_table_last_free__++;
	global.__ui_string_table__[? global.__ui_string_table_last_free__] = _string;
	return global.__ui_string_table_last_free__;
}
function GetUIString(_id){
	return global.__ui_string_table__[? _id] ?? ASCII[? ""];
}

function GetNumAsStrings(_index){
	return string(_index);
}
global.__free_names__ = array_create_ext(10000,GetNumAsStrings);
global.__last_free_name_id__= 0;
global.__layout_number__ =0;
#macro GET_FREE_NAME global.__free_names__[(global.__layout_number__*1000)+(global.__last_free_name_id__++)]
function GetFreeName(){
	global.__last_free_name_id__++;
	return global.__free_names__[global.__last_free_name_id__];
}

function ResetFreeNames(){
	global.__last_free_name_id__=0;
	global.__layout_number__ =0;
}

function AddUISprite(_sprite){
	global.__sprite_map__[? global.__last_sprite_id__] = _sprite;
	global.__reverse_sprite_map__[? _sprite] = global.__last_sprite_id__;
	global.__last_sprite_id__++;
}
function GetUISpriteID(_sprite){
	return global.__reverse_sprite_map__[? _sprite];
}
function __FontArray(){
	//static font_array = array_create(0);
	return global.__font_array__;
}
function GetClayFont(ID){
	if(ID>= 100 || ID <0){
		show_debug_message("[ClayUI]: Font ID is incorrent");
		return -1;
	}
	return  global.__font_talble__[? ID] ;
}

function LoadFont(_name,_fist_char =32 , _last_char =128, _defsize = 8,_bold = false, _italic = false){
	static _clay_font_id = -1;
	_clay_font_id++;
	var _size = gml_clay_add_font(_name);
	var _new_font = -1;
	if(_size > 0){
		_new_font = font_add($"fonts/{_name}", _defsize, _bold, _italic, _fist_char, _last_char);
		//font_enable_sdf(_new_font, true);
	}
	global.__font_talble__[? _clay_font_id] = _new_font;
	//global.__font_array__[global.__font_array_size] = _new_font;
	//global.__font_array_size++;
	if(_new_font !=-1) show_debug_message($"[ClayUI]: Font {_name} loaded");
	if(_new_font ==-1) show_debug_message($"[ClayUI]: Font {_name} IS NOT loaded!");
	return _new_font;
}
global.__font_talble__ = ds_map_create();
function GetFontID(_gml_font){
	return global.__font_talble__[? _gml_font] ?? 0;
}

function RenderClayCommandsOnPing(_surface_id, _ping){
	static _surface_map = ds_map_create();
	if(_surface_map[? _surface_id] == undefined){
		_surface_map[? _surface_id] = 0;
	}
	var _surface = _surface_map[? _surface_id];
	if(!surface_exists(_surface)) _ping = true;
	if(_ping){
		_surface = RenderClayCommands(_surface);
	} else {
		ClayEmptyDispatch();	
	}
	gml_clay_clear_arena();
	_surface = ResizeSurfToWin(_surface)
	_surface_map[? _surface_id] = _surface;
	return _surface;
}


function ResizeSurfToWin(_surface){
	if(display_get_gui_width() != WIN_WIDTH || display_get_gui_height() != WIN_HEIGHT){
		if((display_get_gui_width() * display_get_gui_height()) > 0){
			WIN_WIDTH = display_get_gui_width();
			WIN_HEIGHT = display_get_gui_height();
			surface_resize(_surface, WIN_WIDTH, WIN_HEIGHT);
		}
	}
	return _surface
}

function RenderClayCommands(_surface){
	var _dbg_surface = _surface;
	if(!surface_exists(_surface)) _surface = surface_create(WIN_WIDTH, WIN_HEIGHT);
	RenderClayCommandsNDispatch(0,0,_surface);
	_surface = ResizeSurfToWin(_surface);
	
	return _surface;
}
function RednerClaySurface(){
	static _buffer = buffer_create(WIN_WIDTH * WIN_HEIGHT * 4,buffer_fixed, 1);
	static _surface = surface_create(WIN_WIDTH, WIN_HEIGHT, surface_rgba8unorm)
	static _timer = 0;
	var _win_w =WIN_WIDTH;
	var _win_h = WIN_HEIGHT;
	
	var _length =gml_clay_update(_win_w, _win_h);
	if((_win_h+_win_w) <=0){
		return;
	}
	if(!surface_exists(_surface)){
		_surface = surface_create(_win_w, _win_h, surface_rgba8unorm);
	}
	
	if(surface_get_height(_surface) != _win_h && surface_get_width(_surface) != _win_w){
		surface_resize(_surface, _win_w, _win_h);
	}
	if(buffer_get_size(_buffer) != (_win_w * _win_h * 4) ){
		buffer_resize(_buffer, (_win_w * _win_h * 4));
	}
	var _addres = string( buffer_get_address(_buffer) );
	if (_timer%2){
		gml_clay_get_gui_surface(_addres,  _win_w, _win_h);
		buffer_set_surface(_buffer, _surface, 0);
	}
	draw_surface(_surface,0,0);
	_timer++;
}



function IsBrectFull(_brect){
	return ((_brect & 65535 ) > 0) && ((_brect>>16 & 65535 ) > 0) && ((_brect>>32 & 65535 ) > 0) &&((_brect>>48 & 65535 ) > 0);
}
function ClayEmptyDispatch(){
	var _win_h =  WIN_HEIGHT;
	var _win_w =WIN_WIDTH ;
	var _length =gml_clay_update(_win_w, _win_h);
	while(gml_clay_dispatch_command()){}
}
function RenderClayCommandsNDispatch(_x_offset = 0, _y_offset = 0, _surface = noone){
	//static vform_settup = true;
	//static vertex_format = 0;
	//static vertex_buffer = 0;
	var _win_h =  WIN_HEIGHT;
	var _win_w =WIN_WIDTH ;
	
	var _length =gml_clay_update(_win_w, _win_h);
	var _scissor = gpu_get_scissor();
	var _type = 0;
	var _x = 0;
	var _y = 0;
	var _w = 0;		
	var _h = 0;
	var _x2 = 0;
	var _y2 = 0;
	var _col_t = 0;
	var _color = c_white;
	var _alpha = 1;
	var _asset_id = noone;
	draw_set_halign(fa_left);
	draw_set_valign(fa_top);
	if(_surface != noone && surface_exists(_surface)){
		surface_set_target(_surface);
		draw_clear_alpha(c_black, 0);
		gpu_set_alphatestenable(true); 
		gpu_set_blendmode(bm_add)
		gpu_set_blendmode_ext_sepalpha(bm_src_alpha, bm_inv_src_alpha, bm_src_alpha, bm_one);
	}
	while(gml_clay_dispatch_command()){
		_type = gml_clay_get_cmd_type();
		_col_t = gml_clay_get_color();
		_color = make_color_rgb(
		(_col_t) & 255,
		(_col_t >> 8) & 255,
		(_col_t >> 16) & 255
		);
		_alpha = ((_col_t >> 24) & 255) /255;
		if(_alpha <=0 && _type <= CLAY_CMD_TYPE.IMAGE)continue;
		_x = _x_offset+gml_clay_get_box_x();
		_y = _y_offset+gml_clay_get_box_y();
		_w = gml_clay_get_box_w();		
		_h = gml_clay_get_box_h();
		_x2 = _x+ _w;
		_y2 = _y+ _h;
	
		
		draw_set_color(_color);
		draw_set_alpha(_alpha);
		switch(_type){
			case CLAY_CMD_TYPE.NONE:{
			
			}break;
		    case CLAY_CMD_TYPE.RECTANGLE:{
				var _corner_rad = gml_clay_get_roundiness();
				if(_corner_rad > 0){
					draw_roundrect_ext(_x,  _y, _x2, _y2, _corner_rad, _corner_rad, false);
				}else {
					draw_rectangle( _x,  _y,  _x2,   _y2, false);
				}
			}break;
		    case CLAY_CMD_TYPE.BORDER:{
				var _corner_rad = gml_clay_get_roundiness();
				var _border_rect = gml_clay_get_border_w();
				var _left = (_border_rect & 65535 );
				var _right = (_border_rect>>16 )& 65535 ;
				var _bottom = (_border_rect>>32 )& 65535 ;
				var _top = (_border_rect>> 48) & 65535 ;
				
				
				if(_left > 0&& _right > 0&& _bottom > 0&& _top > 0){
					if(_corner_rad > 1){
						draw_roundrect_ext(_x,  _y, _x2, _y2, _corner_rad, _corner_rad, true);
						break;
					}else {
						draw_rectangle( _x,  _y,  _x2,   _y2, true);
					}
						
				}
				if(_left > 0 ){
					draw_line_width(_x, _y, _x, _y2,_left);
					//draw_rectangle(_x,  _y,  _x + (_left ),  _y2, false);
				}
				if(_right > 0){
					draw_line_width(_x2, _y2, _x2, _y,_right);
					//draw_rectangle(_x2,  _y2,  _x2 ,  _y- _right, false);
				}
				if(_bottom > 0){
					draw_line_width(_x, _y, _x2, _y,_bottom);
					//draw_rectangle(_x,  _y,  _x2,  _y + _bottom, false);
					
				}
				if(_top > 0){
					draw_line_width(_x2, _y2, _x, _y2,_top);
					//draw_rectangle(_x2,  _y2,  _x - _top,  _y2, false);
				}
				
			}break;
			case CLAY_CMD_TYPE.TEXT:{
				//continue;
				var _def = draw_get_font();
				var _fid = gml_clay_get_font_id();
				var _lfont = GetClayFont(_fid) ;
				var _fsize = gml_clay_get_font_size();
				var _def_fsize = font_get_size(_lfont);
				var _string = gml_clay_get_string();
				
				_fsize = (_fsize * 0.60) / _def_fsize;
				draw_set_font(_lfont);
				_fsize = _fsize * 100;
				_fsize = round (_fsize) / 100
				draw_text_transformed( _x,  _y, _string,  round (_fsize), round(_fsize),0);
				draw_set_font(_def);
				gml_clay_free_string();
			}break;
		    case CLAY_CMD_TYPE.IMAGE:{
				_asset_id = global.__sprite_map__[? gml_clay_get_image_id() ];
				//if(sprite_exists(_asset_id)){
				draw_sprite_ext(_asset_id, 0, _x,  _y,
					_w / sprite_get_width(_asset_id),
					_h / sprite_get_height(_asset_id),
					0,_color, _alpha);
			}break;
		    case CLAY_CMD_TYPE.START:{
				gpu_set_scissor( _x,  _y,  _w,  _h);
			}break;
			case CLAY_CMD_TYPE.END:{
				gpu_set_scissor(_scissor);
			}break;
			case CLAY_CMD_TYPE.CUSTOM:{
			
			}break;
			default:{
				
			}break;
		}
	}
	var _s = gml_clay_free_string();
	//if(_s>0) show_debug_message($"size{_s}");
	gml_clay_clear_arena();
	if(_surface != noone&& surface_exists(_surface)){surface_reset_target();}
	gpu_set_scissor(_scissor);
	draw_set_color(c_white);
	draw_set_alpha(1);
	gpu_set_blendmode(bm_normal);
	//gpu_set_alphatestenable(false); 
	//gpu_set_colorwriteenable(true, true, true, true);
	//gpu_set_ztestenable(false);
}
//function InitVertexFormat(){
//		vertex_format_begin();
//		vertex_format_add_position_3d();
//		vertex_format_add_color();
//		vertex_format_add_texcoord();
//		return vertex_format_end();
//}

//function VertexSpriteInit(_sprite, _format, _x, _y, _w, _h){
//	var _uv_data = sprite_get_uvs(_sprite, 0);
//	var _umin = _uv_data[0], _vmin = _uv_data[1], _umax = _uv_data[2], _vmax = _uv_data[3];
//	var _x2 = _x+_w;
//	var _y2 = _y+_h;
//	var vb = vertex_create_buffer();

//	vertex_begin(vb, _format);

//	vertex_position_3d(vb,   _x,   _y, -5000); vertex_color(vb, c_white, 1); vertex_texcoord(vb, _umin, _vmin);
//	vertex_position_3d(vb, _x2,   _y, -5000); vertex_color(vb, c_white, 1); vertex_texcoord(vb, _umax, _vmin);
//	vertex_position_3d(vb,   _x, _y2, -5000); vertex_color(vb, c_white, 1); vertex_texcoord(vb, _umin, _vmax);
//	vertex_position_3d(vb, _x2, _y2, -5000); vertex_color(vb, c_white, 1); vertex_texcoord(vb, _umax, _vmax);

//	vertex_end(vb);
//	return vb;
//}
//function DrawVertexSprite(_sprite, _format,_x, _y, _w, _h, _color = c_white, _alpha = 1){
//	static _map = ds_map_create();
//	var _dbg_map = _map;
//	var v_buf = VertexSpriteInit(_sprite,  _format,_x, _y, _w, _h);
	
//	var _tex = sprite_get_texture(_sprite, 0);
//	vertex_submit(v_buf, pr_trianglestrip, _tex);
//	vertex_delete_buffer(v_buf);
//	//todo
//}


