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
BMAP*	sphere 		= "../public/BBSphere.bmp";
BMAP*	displace 	= "../public/o1.bmp";
BMAP*	background 	= "../public/backdrop.bmp";
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
	obj_scale 	= 150;
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

		// THE NEXT LINES AREN'T NECESSARY
		// THESE JUST CREATING AN ANIMATED
		// WATERSURFACE, BUT YOU CAN DELETE
		// THESE IF YOU LIKE
		count += (int)4*time_step;
		if(count > 10){count %= 10;frm++;}
		if(frm >= 7)frm = 0;

		switch (frm)
		{
			case 0: bmap_purge(source);
			case 1: bmap_load(source, "../public/o2.bmp", 0);
			case 2: bmap_load(source, "../public/o3.bmp", 0);
			case 3: bmap_load(source, "../public/o4.bmp", 0);
			case 4: bmap_load(source, "../public/o5.bmp", 0);
			case 5: bmap_load(source, "../public/o6.bmp", 0);
			case 6: bmap_load(source, "../public/o7.bmp", 0);
			case 7: bmap_load(source, "../public/o8.bmp", 0);
		}

		wait(1);
	}
}

function mode_7_sprite(PANEL *sprite, int worldX, int worldY)
{
    float rot_x		= (worldX - xOffset) / m7.scale;
    float rot_y		= (worldY - yOffset) / m7.scale;
    float space_x	=  rot_x * cos(angle) + rot_y * sin(angle);
    float space_y	= -rot_x * sin(angle) + rot_y * cos(angle);

    float y			= (-m7.foclength - space_y * m7.horizon) / (space_y + 1);
    float pz		= y + m7.horizon;
    float dx_screen = space_x*pz + xres/2;
    float dy_screen = y + yres/2;

    // COMPUTE SPRITE SCALE ON DISTANCE
	// EQUATIONS STANDS IN EVERY FORMULARY
    float distance	= sqrt((worldX - xOffset) * (worldX - xOffset) + (worldY - yOffset) * (worldY - yOffset));
    float scale		= m7.obj_scale / distance;
    sprite.scale_x	= scale;
    sprite.scale_y	= scale;
    sprite.pos_x	= dx_screen - (sprite.size_x * scale / 2);
    sprite.pos_y	= dy_screen - (sprite.size_y * scale / 2);

    // JUST A QUICKFIX SOLUTION FOR CULLING
    if (distance > 0 && pz > 0)	sprite.flags |= VISIBLE;
    else						sprite.flags &= ~VISIBLE;
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
