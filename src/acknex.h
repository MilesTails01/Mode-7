//////////////////////////////////////////////////////////////////////
// Acknex header file for lite-C
//////////////////////////////////////////////////////////////////////
#ifndef acknex_h	// don't include twice
#define acknex_h

#include <litec.h>
#include <compat.h>

typedef fixed var;
typedef fixed function;
typedef void action;	// parsed by WED for entity functions

#include <atypes.h>

/////////////////////////////////////////////////////////////////////
#define EXT
#define EXT_var var
#define F(f) f
#define afuncs_dll_h	// exclude DLL functions
#include <afuncs.h>

//overloaded functions
var abs(var);
var acos(var);
var asin(var);
var atan(var);
var atan2(var,var);
var cos(var);
var sin(var);
var tan(var);
STRING* txt_str(TEXT*);

// special functions not listed in afuncs.h
var wait(var);
var inchar(STRING*);
STRING* inkey(STRING*);

/////////////////////////////////////////////////////////////////////
#define ENGINE_VAR		extern var
#define ENGINE_VECTOR	extern VECTOR
#define ENGINE_COLOR	extern COLOR
#define ENGINE_ANGLE	extern ANGLE
#define ENGINE_EVENT	extern EVENT
#define ENGINE_ENTITY	extern ENTITY*
#define ENGINE_STRING	extern STRING*
#define ENGINE_BMAP		extern BMAP*
#define ENGINE_VIEW		extern VIEW*
#define ENGINE_MATERIAL	extern MATERIAL*
#define ENGINE_CONTACT  extern CONTACT*
#define ENGINE_PANEL    extern PANEL*
#define ENGINE_			extern

int ackFirst;

#include <avars.h>

//dummy function to preset external structs for CS_GetVar()
void ackStructs()
{
	VECTOR* v; v->x = 0;
	COLOR*	c; c->blue = 0;
	COLOR4*	c4; c4->blue = 0;
	ANGLE*	a; a->pan = 0;
	C_LINK*	cl; cl->index = 0;
	STRING*	s; s->length = 0;
	SOUND*	o; o->length = 0;
	BMAP*	b; b->width = 0;
	D3DVERTEX* d3dx; d3dx->x = 0;
	CONTACT*	x; x->x = 0;
	MATERIAL* m; m->power = 0;
	ENTITY*	e; e->x = 0;
	PARTICLE* p; p->x = 0;
	FONT*	f; f->dx = 0;
	TEXT*	t; t->layer = 0;
	PANEL*	pa; pa->layer = 0;
	VIEW*	v; v->layer = 0;
}

//////////////////////////////////////////////////////////////
//convenience functions and definitions
ENGINE_ENTITY my;
ENGINE_ENTITY your;

VECTOR* nullvector = { x=0; y=0; z=0; }

#define set(obj,flag) obj.flags |= (flag)
#define reset(obj,flag) obj.flags &= ~(flag)
#define toggle(obj,flag) obj.flags ^= (flag)
#define is(obj,flag) (obj.flags & (flag))

void reset_emask() { my.emask &= ~(1<<event_type); }

#define wait_for(function) while (proc_status(function)) wait(1)
#define zero(ptr) memset((void*)&ptr,0,sizeof(ptr))
#define vec_zero(vec) ((var*)&(vec))[0] = 0; ((var*)&(vec))[1] = 0; ((var*)&(vec))[2] = 0
#define set_zero(ptr,len) memset((void*)&ptr,0,len)
#define _VAR(i) ((i)*1024)
#define _INT(i) ((i)/1024)

#define COLOR_WHITE vector(255,255,255)
#define COLOR_BLUE vector(255,0,0)
#define COLOR_GREEN vector(0,255,0)
#define COLOR_RED vector(0,0,255)
#define COLOR_GREY vector(128,128,128)
#define COLOR_BLACK vector(1,1,1)
#define NULLVECTOR nullvector

#define VAR_MAX  999999.999
#define VAR_MIN -999999.999

#define SPHERE_MDL "_sphere.mdl"
#define CUBE_MDL "_cube.mdl"
#define SHADOW_DDS "_shadow.dds"

// initialize a skill to a default value when zero
#define SKILL_DEFAULT(skill,def) if(!my.skill) my.skill = def

// create a static engine object inside a function, only once
#define SND_CREATE_STATIC(name,file) static SOUND* name = NULL; if (!name) name = snd_create(file)
#define BMAP_CREATE_STATIC(name,file) static BMAP* name = NULL; if (!name) name = bmap_create(file)

// display a variable for debugging purposes
#define DEBUG_VAR(vv,pos_y) draw_text(str_for_num(NULL,vv),10,pos_y,COLOR_RED)

// display a bitmap, f.i. a view target for debugging purposes
#define DEBUG_BMAP(bmap,pos_x,scale) draw_quad(bmap,vector(pos_x,10,0),NULL,NULL,vector(scale,scale,0),NULL,100,0)

#endif