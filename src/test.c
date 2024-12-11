// Pseudo-Mode-7!
/* Programm still need some enhancement */
/* written in August 2012 by Tommy - tommy.draeger@googlemail.com */

#include <acknex.h>
#include <litec.h>
#include <default.c>

#define xres 320
#define yres 240

BMAP*	blank_16;
BMAP*	blank_32;
//BMAP*	sphere 		= "../public/BBSphere.bmp";
//BMAP*	displace 	= "../public/o1.bmp";
//BMAP*	background 	= "../public/backdrop.bmp";

BMAP* 	sphere		= "../public/object.bmp";
BMAP* 	displace	= "../public/groundPink.bmp";
BMAP* 	background	= "../public/backdropPink.bmp";

PANEL*	framebuffer = {layer = 10; flags = VISIBLE; }
PANEL*	framedepth 	= {layer = 11; flags = VISIBLE; }
PANEL*	obj_01 		= {layer = 12; flags = VISIBLE | OVERLAY; bmap = sphere; }

function mode_7_level(BMAP *screen, BMAP *source, BMAP *distfog, BMAP *backdrop);
function mode_7_sprite(PANEL *sprite, int px, int py);

typedef struct /* MODE 7 SETTINGS */
{
	int foclength; // FOCALLENGTH
	int horizon;
	int scale;	   // SCALEFACTOR FOR LEVELGROUND
	int obj_scale; // SCALEFACTOR FOR SPRITES
} M7_OPT;

// THESE ARE VALUES THAT WORKS FINE FOR
// ME AT A RESOLUTION OF 320 X 240
M7_OPT *m7 =
{
	foclength 	= 400;
	horizon 	= 80;
	scale 		= 60;
	obj_scale 	= 60;
}

float angle 	= 0;
float xOffset 	= 0;
float yOffset 	= 0;

// SCREEN AND DISTFOG ARE BMAPS TO PAINT ON
// THE OTHERS ARE USES FOR LEVELGROUND AND
// SKYKBACKDROP
function mode_7_level(BMAP *screen, BMAP *source, BMAP *distfog, BMAP *backdrop)
{
	fixed count;
	DWORD blend;
	int x, y;
	int frm;
	int dx, dy;
	int mask_x, mask_y;
	int mask_x_backdrop;
	float space_x, space_y, space_z;
	float screen_x, screen_y;
	float px, py, pz;
	float sx, sy;

	mask_x 			= bmap_width(source)	- 1;
	mask_y 			= bmap_height(source)	- 1;
	mask_x_backdrop = bmap_width(backdrop)	- 1;

	while (1)
	{
		
		angle		+= (key_cur - key_cul) 				* time_step / 15;	// STEERING
		xOffset		+= (key_d - key_a) 					* time_step * 4;	// SHIFTING
		m7.scale 	+= (key_w - key_s) 					* time_step * 4; 	// SHIFTING
		xOffset 	+= (key_cuu - key_cud) * sin(angle) * time_step * 8;
		yOffset		-= (key_cuu - key_cud) * cos(angle) * time_step * 8;

		var format_screen 	= bmap_lock(screen, 0);
		var format_source 	= bmap_lock(source, 0);
		var format_distfog 	= bmap_lock(distfog, 0);
		var format_backdrop = bmap_lock(backdrop, 0);

		for (y = -yres / 2; y < yres / 2; y++)
		{
			dy = y + yres / 2;

			// BLEND HOLDS THE ALPHAVALUE
			// FOR THE DISTANTSFOGGING
			blend = ((dy)-60) * -100 / 110 - 10;
			blend = clamp(blend, -100, 0);
			// JUST TO CREATE A NON-LINEAR EFFECT
			// 505 WAS DETERMINE BY ME
			blend &= 505;

			// MAKE A SLICE AT 70
			// TO RENDER THE BACKGROUND
			if (y >= (-yres / 2) + 70)
			{
				for (x = -xres / 2; x < xres / 2; x++)
				{
					dx = x + xres / 2;

					/******* ACTUAL MODE 7 ********/

					// ASIGN SOME TEMPVARS FOR
					// A BETTER OVERVIEW
					px = x;
					py = y + m7.foclength;
					pz = y + m7.horizon;

					// MATHIMATICAL PROJECTION
					// FOR TURNING A 3D POINT TO
					// A 2D SCREENPIXEL
					// Y STANDS FOR Z RESP. THE DEPTH
					space_x = px / pz;
					space_y = py / pz * -1;
					// INVERT THE Y DIRECTION
					// SO THAT EVERTHING POINT
					// IN A CORRECT WAY

					// A TRIGONOMIC SOLUTION TO BE ABLE
					// TO ROTATE THE BMAP SO YOU CAN
					// LOOK AROUND 360DEGREE
					screen_x = space_x * cos(angle) - space_y * sin(angle);
					screen_y = space_x * sin(angle) + space_y * cos(angle);

					// FINAL TRANSFORMATION AND SCALING
					sx = screen_x * m7.scale + xOffset;
					sy = screen_y * m7.scale + yOffset;

					// USE THE AND-OPERATOR TO CREATE AN INFINTIY-PATTERN
					VECTOR color;
					pixel_to_vec(&color, NULL, format_source, pixel_for_bmap(source, (int)sx & mask_x, (int)sy & mask_y) + 0x12);

					pixel_to_bmap(screen, dx, dy, pixel_for_vec(&color, 100, format_screen));
					pixel_to_bmap(distfog, dx, dy, pixel_for_vec(vector(240, 254, 160), blend, format_distfog));

					//	pixel_to_bmap(screen,dx,dy,pixel_for_bmap(source,(int)sx & mask_x,(int)sy & mask_y)+0x001200);
					//	pixel_to_bmap(distfog,dx,dy,pixel_for_vec(vector(240,254,160),blend,0));

					/*******************************/
				}
			}
			else
			{
				for (x = 0; x < xres; x++)
				{
					COLOR color;
					pixel_to_vec(&color, NULL, format_backdrop, pixel_for_bmap(backdrop, (int)(x + angle * 200) & mask_x_backdrop, dy));
					pixel_to_bmap(screen, x, dy, pixel_for_vec(&color, 100, format_screen));
				}
			}
		}

		// EVERYTIME YOU WANT TO ADD A OBJECT YOU'VE
		// TO PLACE A NEW MODE_7_SPRITE FUNCTION HERE
		// IT'S PLACED HERE BECAUSE OF THE PERFORMANCE
		mode_7_sprite(obj_01, 0, -200);

		bmap_unlock(backdrop);
		bmap_unlock(distfog);
		bmap_unlock(source);
		bmap_unlock(screen);

		wait(1);
	}
}

function mode_7_sprite(PANEL *sprite, int worldX, int worldY)
{
    // Compute intermediate values
    float rot_x = (worldX - xOffset) / m7.scale;
    float rot_y = (worldY - yOffset) / m7.scale;

    float space_x = rot_x*cos(angle) + rot_y*sin(angle);
    float space_y = -rot_x*sin(angle) + rot_y*cos(angle);

    // Solve for screen line y
    float y = (-m7.foclength - space_y*m7.horizon) / (space_y + 1);
    float pz = y + m7.horizon;

    // dx = space_x * pz
    // dy = y
    float dx_screen = space_x*pz + xres/2;
    float dy_screen = y + yres/2;

    // Compute sprite scale based on distance
    float distance = sqrt((worldX - xOffset)*(worldX - xOffset) + (worldY - yOffset)*(worldY - yOffset));
    float scale = m7.obj_scale / distance;

    sprite.scale_x = scale;
    sprite.scale_y = scale;
    sprite.pos_x = dx_screen - (sprite.size_x*scale/2);
    sprite.pos_y = dy_screen - (sprite.size_y*scale/2);

    // Make the sprite visible if in front
    if (distance > 0 && pz > 0) // Simple check: sprite in front
        sprite.flags |= VISIBLE;
    else
        sprite.flags &= ~VISIBLE;
}

function mode_7_sprite2(PANEL *sprite, int px, int py)
{

	float width, height;
	float space_x, space_y;
	float screen_x, screen_y;
	float obj_dir, obj_org;

	float obj_x = px + xOffset;
	float obj_y = py - yOffset;

	// EQUATIONS STANDS IN EVERY FORMULARY
	float distance = sqrt(pow(obj_x, 2) + pow(obj_y, 2));

	space_x = obj_x * cos(angle) - obj_y * sin(angle);
	space_y = obj_x * sin(angle) + obj_y * cos(angle);

	// SPACE_Y IS THE DEPTH
	// IF YOU WANT TO REPRODUCE THESE EQUATION JUST LOOK
	// FOR "PROJECTION" RESP. "SIMILAR TRIANGLES"
	screen_x = xres / 2 + (space_x * m7.foclength) / space_y;
	screen_y = (m7.foclength / space_y) + m7.horizon - 10;

	// CALCULATE THE NEW HEIGHT DEPEND
	// INDIRECT PROPORTIONAL TO THE
	// DISTANCE
	height	= (m7.obj_scale / distance);
	width	= (m7.obj_scale / distance);

	sprite.scale_x = width;
	sprite.scale_y = height;

	sprite.pos_x = screen_x - (width * sprite.size_x / 2);
	sprite.pos_y = screen_y + (height * sprite.size_y / 2);

	// JUST A SMALL SOLUTION FOR CULLING
	// SO YOU DOESN'T SEE THE SPRITE TWICE

	// CONVERT VECTOR TO +-180 ANGLE
	vec_to_angle(obj_dir, vector(space_x, space_y, 0));
	vec_to_angle(obj_org, vector(px, py, 0));

	// CONVERT +-180 ANGLE TO 0...360 ANGLE
	obj_dir = cycle(obj_dir, 0, 360);
	obj_org = cycle(obj_org, 0, 360);

	// DETERMINE WHETER THE OBJECT IS IN EYESIGHT
	if (obj_dir > (obj_org - 90) && obj_dir < (obj_org + 90))
	{
		sprite.flags |= VISIBLE;
	}
	else
	{
		sprite.flags &= ~VISIBLE;
	}
	return;
}



function main()
{
	level_load("");
	wait(2);

	video_set(xres, yres, 16, 0);

	// CREATE TWO BLANK BMAPS FOR THE SCREEN
	// AND THE DISTANCE FOGGING (BECAUSE OF
	// OF THE TRANSPERANTY OF THE LAYER)
	blank_16 = bmap_createblack(xres, yres, 16);
	blank_32 = bmap_createblack(xres, yres, 32);

	framebuffer.bmap = blank_16;
	framedepth.bmap = blank_32;

	// fps_max = 20;
	mode_7_level(blank_16, displace, blank_32, background);
}
