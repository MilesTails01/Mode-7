/////////////////////////////////////////////////////////////////////
// Default key and debug functions
// (c) jcl / Conitec  2008
/////////////////////////////////////////////////////////////////////
#ifndef default_c
#define default_c
#include <acknex.h> // standard engine objects

var def_dfps,def_dtps,def_dtlv,def_dtcs,def_dtac,def_dtrf,def_dsnd;
ANGLE def_cang;
var uu1,uu2; // user debug variables

VIEW* viewMap = 
{
	flags = WIREFRAME | NOCULL; 
	layer = 1; 
	roll = 270; 
	tilt = -90; 
	arc = 10; 
}

PANEL* def_debug_pan = 
{
   pos_x = 5; 
	pos_y = 5; 
	layer = 998; 

	digits(0,2,"%4.0fx",*,1,screen_size.x);
	digits(30,2,"%.0f",*,1,screen_size.y);
   digits(0,12,"fps%5.0f",*,16,def_dfps);
   digits(0,22,"pps%5.0fk",*,0.016,def_dtps);
	digits(0,32,"x%7.0f",*,1,camera.x);
	digits(0,42,"y%7.0f",*,1,camera.y);
	digits(0,52,"z%7.0f",*,1,camera.z);
	digits(0,62,"pan%5.0f",*,1,def_cang.pan);
	digits(0,72,"til%5.0f",*,1,def_cang.tilt);
	digits(0,82,"rol%5.0f",*,1,def_cang.roll);
	//    digits(0,92,"ml %5.f",*,1,mouse_right);

	digits(65,2,"ms/frame",*,0,0);
	digits(65,12,"geo%5.1f",*,1,def_dtlv);
	digits(65,22,"ent%5.1f",*,1,time_entities);
	digits(65,32,"par%5.1f",*,1,time_effects);
	digits(65,42,"drw%5.1f",*,1,time_draw);
	digits(65,52,"pan%5.1f",*,1,time_panels);
	digits(65,62,"ref%5.1f",*,1,def_dtrf);
	digits(65,72,"srv%5.1f",*,1,def_dtcs);
	digits(65,82,"fnc%5.1f",*,1,def_dtac);
	digits(65,92,"phy%5.1f",*,1,time_physics);
	//    digits(65,106,"sco%5.1f",*,1,time_scope);

	digits(130,2,"count of",*,0,0;
	digits(130,12,"ent%5.0f",*,1,num_entities);
	digits(130,22,"vis%5.0f",*,1,num_visents);
	digits(130,32,"tri%5.0fk",*,0.001,num_vistriangles);
	digits(130,42,"par%5.0f",*,1,num_particles);
	digits(130,52,"lit%5.0f",*,1,num_lights);
	digits(130,62,"snd%5.0f",*,1,def_dsnd);
	digits(130,72,"fnc%5.0f",*,1,num_actions);
	digits(130,82,"bdy%5.0f",*,1,ph_num_bodies);
	digits(130,92,"con%5.0f",*,1,ph_num_contacts);

	digits(200,2,"memory kb",*,0,0);
	digits(200,12,"nx%7.0f",*,1,nexus);
	digits(200,22,"m%8.0f",*,1,sys_memory);
	digits(200,32,"geo%6.0f",*,1,d3d_texsurfs);
	digits(200,42,"shd%6.0f",*,1,d3d_texsmaps);
	digits(200,52,"ent%6.0f",*,1,d3d_texskins);
	digits(200,62,"v%8.0f",*,1,d3d_texfree);
	digits(200,82,"uu1%6.0f",*,1,uu1);
	digits(200,92,"uu2%6.0f",*,1,uu2);

	digits(270,2,"network",*,0,0);
	digits(270,12,"lat%4.0f",*,1,dplay_latency);
	digits(270,22,"bps%4.0f",*,1,dplay_bps);
	digits(270,32,"rel%4.0f",*,1,dplay_bpsreliable);
	digits(270,42,"unr%4.0f",*,1,dplay_bpsunreliable);
	digits(270,52,"com%4.0f",*,1,dplay_compression);
	digits(270,62,"num%4.0f",*,1,num_clients);
}

void def_box(var x1,var y1,var x2,var y2,VECTOR* color)
{
    draw_line(vector(x1,y1,0),NULL,100);
    draw_line(vector(x2,y1,0),color,100);
    draw_line(vector(x2,y2,0),color,100);
    draw_line(vector(x1,y2,0),color,100);
    draw_line(vector(x1,y1,0),color,100);
}

var def_oldmouse = 0;

void def_debug() 
{
    beep();
    if(key_alt) 
    { 
        diag_status(); 
        return; 
    }
    
    if (key_shift) 
	{
        diag_status();
        freeze_mode = !freeze_mode;
        if (freeze_mode) {
            def_oldmouse = mouse_mode;
            mouse_mode = 4;
            mouse_pointer = 2;
        } else {
	        mouse_mode = def_oldmouse;
        }
        while (freeze_mode) {
            if (mouse_left)
				watched = mouse_ent;
            wait(1);
        }
        return;
    }

    if (key_ctrl) {
		toggle(viewMap,SHOW);
		while is(viewMap,SHOW) {
			vec_set(&viewMap.x,&camera.x);
			viewMap.z += 10000;
			wait(1);
		}
		return;
    }

    if (d3d_lines) {
            reset(def_debug_pan,SHOW);
            d3d_lines = 0;
			diag_mode &= ~1;
    } else {
        if is(def_debug_pan,SHOW) {
                d3d_lines = 3;
        } else {
            set(def_debug_pan,SHOW);
			diag_mode |= 1;
            while is(def_debug_pan,SHOW) {
				def_dfps = 0.9*def_dfps+0.1/time_frame;
				def_dtps = 0.9*def_dtps+0.1*num_vistriangles/time_frame;
				def_dtlv = time_level+time_sky;
				def_dtcs = time_client+time_server;
				def_dtrf = time_update+time_idle;
				def_dtac = time_actions+time_pfuncs;
                def_dsnd = num_mediastreams + num_sounds;
				def_cang.pan = cycle(camera.pan,0,360); 
				def_cang.tilt = cycle(camera.tilt,-180,180); 
				def_cang.roll = cycle(camera.roll,-180,180); 
                def_box(def_debug_pan.pos_x-3,def_debug_pan.pos_y-2,
                    def_debug_pan.pos_x+314,def_debug_pan.pos_y+103,
                    vector(255,255,255));
				wait(1);
            }
        }
    }
}

void def_exit() 
{
	if (!key_f4 || key_alt)
		sys_exit(NULL); 
}

var def_shot_num = 0;
void def_shot() 
{ 
	file_for_screen("shot_",def_shot_num); 
	beep(); 
	def_shot_num++; 
}

void def_save() 
{ 
	game_save(app_name,1,SV_ALL-SV_INFO); 
	beep(); 
}

void def_load() 
{ 
	game_load(app_name,1); 
}

void def_video() 
{
	var mode = video_mode;
	while(1) {
		if (!key_shift) 
			mode++; 
		else 
			mode--;
		mode = cycle(mode,6,12); 
		if (video_switch(mode,0,0)) 
			break;
   }
}

void def_screen() 
{
	if (!key_alt) 
		return;
	var mode = video_screen + 1;
	if (mode > 2) 
		mode = 1;
	if (video_switch(0,0,mode) == 0) 
		beep();
}

var def_mvol = 50;
var def_svol = 50;
void def_sound() 
{
    if (sound_vol==0) { 
		sound_vol = def_svol; 
		midi_vol = def_mvol; 
		return; 
	}
    if (midi_vol==0) { 
		def_svol = sound_vol; 
		sound_vol = 0; 
		return; 
	}
    def_mvol = midi_vol; 
	midi_vol = 0;
}


var def_camera = 0;
VECTOR* def_cam_dist = { x=100; y=0; z=100; }

// call this function from a level to enable the free camera movement
void def_move()
{
	VECTOR force,speed,dist;
	ANGLE aforce,aspeed; 

// initialize speed and distance
	vec_zero(speed);
	vec_zero(aspeed);
	vec_zero(dist);

  if (1 > def_camera)
     def_camera = 1;
	if (1 < run_mode && run_mode < 5) 
		def_camera = 2;	// prevent player movement in entity viewer mode

	while (def_camera) 
	{
		aforce.tilt = 5*(key_pgup - key_pgdn + mouse_right*mouse_force.y);
		if (key_alt==0) {
		    aforce.pan = -5*(key_force.x + mouse_right*mouse_force.x + joy_force.x);
		    aforce.roll = 0;
		} else {
		    aforce.pan = 0;
		    aforce.roll = 5*(key_force.x + mouse_right*mouse_force.x + joy_force.x);
		}

		vec_add(camera.pan,vec_accelerate(dist,aspeed,aforce,0.8));

		force.x = 7*(key_force.y + key_w - key_s + joy_force.y);
		force.y = 3*(key_comma - key_period + key_a - key_d);
		force.z = 3*(key_home - key_end);
		vec_accelerate(dist,speed,force,0.5);
		
		if (NULL != player && 1 == def_camera) {
		    c_move(player,dist,nullvector,IGNORE_PASSABLE|IGNORE_PASSENTS|GLIDE);
		    camera.genius = player;
		    vec_set(player.pan,camera.pan);
		    vec_set(camera.x,def_cam_dist);
		    vec_rotate(camera.x,camera.pan);
		    vec_add(camera.x,player.x);
		} else {
		    camera.genius = NULL;
		    vec_add(camera.x,vec_rotate(dist,camera.pan));
		}
		wait(1);
  }
}

void def_moveset() 
{
    def_camera += 1;
	if (NULL != player)
		def_camera = cycle(def_camera,0,3);	// 0-1-2
	else
		def_camera = cycle(def_camera,0,2);	// 0-1
    if (!key_shift && def_camera > 0) {
        beep();
		def_move();
    }
}

TEXT* def_ctxt = { string("Enter var or STRING","#80"); layer = 999; }

void def_console() // Tab
{
   def_ctxt.pos_x = 2;
   def_ctxt.pos_y = screen_size.y-30;
   toggle(def_ctxt,SHOW);
   while is(def_ctxt,SHOW) {
     beep();
     inkey((def_ctxt.pstring)[1]);
	 if (13 == result) {
		void* found = var_for_name((def_ctxt.pstring)[1]);
		if (!found) execute((def_ctxt.pstring)[1]);
	 } else
		reset(def_ctxt,SHOW);
   }
}

TEXT* def_dtxt = {
   pos_x = 2; pos_y = 2; layer = 999;
   string = debug_str,watch_str;
   flags = SHOW;
}

void def_startup() 
{ 
	if (!on_f2) on_f2 = def_save;
	if (!on_f3) on_f3 = def_load;
	if (!on_f5) on_f5 = def_video;
	if (!on_f4) on_f4 = def_exit;
	if (!on_f6) on_f6 = def_shot;
	if (!on_f11) on_f11 = def_debug;
	if (!on_f12) on_f12 = def_sound;

	if (!on_0) on_0 = def_moveset;
	if (!on_enter) on_enter = def_screen;
	if (!on_tab) on_tab = def_console;
	if (!on_esc) on_esc = def_exit;
	if (!on_close) on_close = def_exit;
}

#endif
//test