// Pseudo-Mode-7!
/* Programm still need some enhancement */
/* written in August 2012 by Tommy - tommy.draeger@googlemail.com */

#include <acknex.h>
#include <litec.h>
#include <default.c>

#define xres 320
#define yres 240
#define PI 3.14159265
	
BMAP* blank_16;
BMAP* blank_32;
	
BMAP* sphere 		= "../public/object.bmp";
BMAP* displace 		= "../public/groundPink.bmp";
BMAP* background 	= "../public/backdropPink.bmp";
	
PANEL* framebuffer =
{
	layer = 10;
	flags = VISIBLE;
}
	
PANEL* framedepth =
{
	layer = 11;
	flags = VISIBLE;
}
	
PANEL* obj_01 =
{
	layer 	= 12;
	flags 	= VISIBLE | OVERLAY;
	bmap 	= sphere;
}
	
function mode_7_level (BMAP* screen, BMAP* source, BMAP* distfog, BMAP* backdrop);
function mode_7_sprite (PANEL* sprite, int px, int py);
	
// mode 7 settings
typedef struct
{
	int foclength; // focal length
	int horizon;
	int scale; // level ground scale factor
	int obj_scale; // scale factor for sprites
} M7_OPT;
	
	
// these are values that work fine at a resolution of 320 x 240
M7_OPT* m7 =
{
	foclength = 400;
	horizon = 80;
	scale = 60;
	obj_scale = 200;
}
	
float angle = 0;
	
float xOffset = 0;
float yOffset = 0;
	
// renders the mode 7 screen image; screen and distfog are the (cpu) target bitmaps,
// distfog and backdrop are the source images
function mode_7_level (BMAP* screen, BMAP* source, BMAP* distfog, BMAP* backdrop)
{
	fixed count;
	DWORD blend;
	
	int x,y;
	
	int frm; // frame
	
	int dx, dy;
	int mask_x, mask_y;
	int mask_x_backdrop;

	float space_x, space_y, space_z;
	float screen_x, screen_y;
	float px, py, pz;
	float sx, sy;
	
	mask_x = bmap_width(source) - 1;
	mask_y = bmap_height(source) - 1;
	mask_x_backdrop = bmap_width(backdrop) - 1;
	
	while (1)
	{
		// steering
		angle += (key_cur - key_cul) * time_step / 15;
		
		xOffset  += (key_d - key_a) * time_step * 4;//SHIFTING
		m7.scale += (key_w - key_s) * time_step * 4;//SHIFTING
		
		xOffset += (key_cuu - key_cud) * sin(angle) * time_step * 8;
		yOffset -= (key_cuu - key_cud) * cos(angle) * time_step * 8;
		
		var formatScreen 		= bmap_lock(screen, 0);
		var formatSource 		= bmap_lock(source, 0);
		var formatDistFog 	= bmap_lock(distfog, 0);
		var formatBackdrop 	= bmap_lock(backdrop, 0);
	
		for(y = -yres/2; y < yres/2; y++)
		{
			dy = y+yres/2;
			
			// blend is the alpha of the fog
			blend = ((dy)-60) * -100/110 - 10;
			blend = clamp(blend, -100, 0);
			
			// just to create a non-linear effect (magic number)
			blend &= 505;
			
			// make a slice at 70 to render the background:
			
			// render ground
			if (y >= (-yres/2)+70)
			{
				for (x = -xres/2; x < xres/2; x++)
				{
					dx = x+xres/2;
					
					// actual mode 7 code
					
					// assign some tempvars for a better overview
					px = x; 
					py = y + m7.foclength;
					pz = y + m7.horizon;
					
					// projection from a 3d point to a 2d screenpixel; y stands for z (depth)
					space_x = px / pz;
					space_y = py / pz * -1;
					
					// invert the y direction so that everthing points in a correct way
					
					// a trigonomic solution to be able to rotate the bmap so you can look around 360�
					screen_x = space_x * cos(angle) - space_y * sin(angle);
					screen_y = space_x * sin(angle) + space_y * cos(angle);
					
					// final transformation and scaling
					sx = screen_x * m7.scale + xOffset;
					sy = screen_y * m7.scale + yOffset;
					
					// use the and-operator to create an infintife pattern
					
					var pixel = pixel_for_bmap(source, (int)sx & mask_x, (int)sy & mask_y);
					
					// transfer source pixel to target pixel format					
					COLOR color;
					pixel_to_vec(&color, NULL, formatSource, pixel);
					pixel = pixel_for_vec(&color, 100, formatScreen);			
					
					// write source to screen render target
					pixel_to_bmap(screen,dx,dy, pixel);
					
					// write fog
					pixel = pixel_for_vec(vector(255,255,255) , blend, formatDistFog);
					pixel_to_bmap(distfog, dx, dy, pixel);
				}
			}
			else // render sky
			{
				for(x = 0; x < xres; x++)
				{
					// read sky pixel
					var pixel = pixel_for_bmap(backdrop, (int)(x+angle*200) & mask_x_backdrop, dy);
					
					// transfer sky pixel to screen format
					COLOR color;
					pixel_to_vec(&color, NULL, formatBackdrop, pixel);
					pixel = pixel_for_vec(&color, 100, formatScreen);
					
					// draw sky to screen render target
					pixel_to_bmap(screen, x, dy, pixel);
				}
			}
		}
	
		// everytime you want to add a object you've to place a new mode_7_sprite function here
		mode_7_sprite(obj_01, 0, -200);
		
		bmap_unlock(backdrop);
		bmap_unlock(distfog);
		bmap_unlock(source);
		bmap_unlock(screen);
		
		wait(1);
	}
}
	
// draws an object
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
	
	// create two blank bmaps for the screen and the distance fogging
	blank_16 = bmap_createblack(xres, yres, 16);
	blank_32 = bmap_createblack(xres, yres, 32);
	
	framebuffer.bmap = blank_16;
	framedepth.bmap = blank_32;
	
	mode_7_level(blank_16, displace, blank_32, background);
}