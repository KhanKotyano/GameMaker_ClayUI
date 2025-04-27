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
		_map[? _key] = method_call(_func, _args);
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