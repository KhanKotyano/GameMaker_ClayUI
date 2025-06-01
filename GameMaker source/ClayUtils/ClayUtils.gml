function DSMapCreateWithKeys(_key_array, _default_key = 0){
	var _size = array_length(_key_array);
	var _ds_map = ds_map_create();
	for(var i =0;i < _size;i++){
		ds_map_add(_ds_map, _key_array[i], _default_key);
	}
	return _ds_map;
}

function MapPopulateByFunction(_map, _func, _args){
	var _size = ds_map_size(_map);
	var _key = ds_map_find_first(_map)
	for(var i =0;i<_size;i++){
		//_map[? _key] = method_call(_func, _args);
		ds_map_replace_map(_map, _key, method_call(_func, _args));
		_key = ds_map_find_next(_map, _key);
	}
}
function AddKeyWithFunction(_map, _key,_func, _args){
	var _size = ds_map_size(_map);
	_map[? _key] = method_call(_func, _args);
	_key = ds_map_find_next(_map, _key);
}
function PushInEachMapKey(_map, _val){
	var _size = ds_map_size(_map);
	var _key = ds_map_find_first(_map)
	for(var i =0;i<_size;i++){
		_map[? _key] = _val;
		_key = ds_map_find_next(_map, _key);
	}
}
function DSMapPushInEachKeyStruct(_map, _struct_name,_val){
	var _size = ds_map_size(_map);
	var _key = ds_map_find_first(_map)
	for(var i =0;i<_size;i++){
		_map[? _key][$ _struct_name] = _val;
		_key = ds_map_find_next(_map, _key);
	}
}
function Room_x_to_gui_x(_x, _camera ) {
	
    var p = _x - camera_get_view_x(_camera);
    p /= camera_get_view_width(_camera);
	return p * display_get_gui_width();
}

function Gui_x_to_room_x(_x, _camera ) {
    var p = _x / display_get_gui_width();
    p *= camera_get_view_width(_camera);
    return p + camera_get_view_x(_camera);
}

function Room_y_to_gui_y(_y, _camera ) {
    var p = _y - camera_get_view_y(_camera);
    p /= camera_get_view_height(_camera);
	return p * display_get_gui_height();
}

function Gui_y_to_room_y(_y, _camera ) {
    var p = _y / display_get_gui_height();
    p *= camera_get_view_height(_camera);
    return p + camera_get_view_y(_camera);
}
function IsIndexInArray(_array, _index){
	var _size = array_length(_array)
	if(_index >= 0 && _index < _size)return true;
	return false;
}
function Range(_a, _b){
	return  abs( _a - _b);
}
function MakeCharList(_string, _list){
	var _size = string_length(_string)
	for(var i =0;i<_size;i++){
		_list[| i] = string_char_at(_string, i+1);
	}
	return _list;
}
function MakeStringFromCharList(_list){
	var _size = ds_list_size(_list);
	var _string = "";
	for(var i =0;i<_size;i++){
		_string+= string(_list[| i]);
	}
	return _string;
}
function DsMapGetNextKey(ds_map, _key){
	if(_key == ds_map_find_last(ds_map)){
		_key = ds_map_find_first(ds_map);
	}else {
		_key = ds_map_find_next(ds_map,_key);
	}
	return _key
}
function GetKeysFromString(_string, _div_char = ";", _preserve_digits = false){
	var _size = string_length(_string);
	var _keys_array = [];
	var _key = "";
	var _c = ""
	for(var i =1;i<=_size;i++){
		_c = string_char_at(_string, i)
		while(_c != _div_char && i <=_size){
			_key += _c;
			 i++;
			 _c = string_char_at(_string, i);
		}
		if(!_preserve_digits){
			if(_key != "" && string_length(_key) == string_length( string_digits(_key) )){
				_key = real(_key);
			}
		}
		if(_key !="") array_push(_keys_array, _key);
		_key = "";
	}
	return _keys_array;
}