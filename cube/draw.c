#include <stdlib.h>
#include "draw.h"
#include "string.h"

// Set a single voxel to ON
void setvoxel(int x, int y, int z)
{
	if (inrange(x,y,z))
		cube[z][y] |= (1 << x);
}

// Set a single voxel in the temporary cube buffer to ON
void tmpsetvoxel(int x, int y, int z)
{
	if (inrange(x,y,z))
		fb[z][y] |= (1 << x);
}

// Set a single voxel to OFF
void clrvoxel(int x, int y, int z)
{
	if (inrange(x,y,z))
		cube[z][y] &= ~(1 << x);
}

// Set a single voxel to OFF
void tmpclrvoxel(int x, int y, int z)
{
	if (inrange(x,y,z))
		fb[z][y] &= ~(1 << x);
}

// This function validates that we are drawing inside the cube.
unsigned char inrange(int x, int y, int z)
{
	if (x >= 0 && x < CUBE_SIZE && y >= 0 && y < CUBE_SIZE && z >= 0 && z < CUBE_SIZE)
	{
		return 1;
	} else
	{
		// One of the coordinates was outside the cube.
		return 0;
	}
}

// Get the current status of a voxel
unsigned char getvoxel(int x, int y, int z)
{
	if (inrange(x,y,z))
	{
		if (cube[z][y] & (1 << x))
		{
			return 1;
		} else
		{
			return 0;
		}
	} else
	{
		return 0;
	}
}

// In some effect we want to just take bool and write it to a voxel
// this function calls the apropriate voxel manipulation function.
void altervoxel(int x, int y, int z, int state)
{
	if (state == 1)
	{
		setvoxel(x,y,z);
	} else
	{
		clrvoxel(x,y,z);
	}
}

// Flip the state of a voxel.
// If the voxel is 1, its turned into a 0, and vice versa.
void flpvoxel(int x, int y, int z)
{
	if (inrange(x, y, z))
		cube[z][y] ^= (1 << x);
}

// Makes sure x1 is alwas smaller than x2
// This is usefull for functions that uses for loops,
// to avoid infinite loops
void argorder(int ix1, int ix2, int *ox1, int *ox2)
{
	if (ix1>ix2)
	{
		int tmp;
		tmp = ix1;
		ix1= ix2;
		ix2 = tmp;
	}
	*ox1 = ix1;
	*ox2 = ix2;
}

// Sets all voxels along a X/Y plane at a given point
// on axis Z
void setplane_z (int z)
{
	int i;
	if (z>=0 && z<CUBE_SIZE)
	{
		for (i=0;i<CUBE_SIZE;i++)
			cube[z][i] = 0xff;
	}
}

// Clears voxels in the same manner as above
void clrplane_z (int z)
{
	int i;
	if (z>=0 && z<CUBE_SIZE)
	{
		for (i=0;i<CUBE_SIZE;i++)
			cube[z][i] = 0x00;
	}
}

void setplane_x (int x)
{
	int z;
	int y;
	if (x>=0 && x<CUBE_SIZE)
	{
		for (z=0;z<CUBE_SIZE;z++)
		{
			for (y=0;y<CUBE_SIZE;y++)
			{
				cube[z][y] |= (1 << x);
			}
		}
	}
}

void clrplane_x (int x)
{
	int z;
	int y;
	if (x>=0 && x<CUBE_SIZE)
	{
		for (z=0;z<CUBE_SIZE;z++)
		{
			for (y=0;y<CUBE_SIZE;y++)
			{
				cube[z][y] &= ~(1 << x);
			}
		}
	}
}

void setplane_y (int y)
{
	int z;
	if (y>=0 && y<CUBE_SIZE)
	{
		for (z=0;z<CUBE_SIZE;z++)
			cube[z][y] = 0xff;
	} 
}

void clrplane_y (int y)
{
	int z;
	if (y>=0 && y<CUBE_SIZE)
	{
		for (z=0;z<CUBE_SIZE;z++)
			cube[z][y] = 0x00; 
	}
}

void setplane (char axis, unsigned char i)
{
    switch (axis)
    {
        case AXIS_X:
            setplane_x(i);
            break;
        
       case AXIS_Y:
            setplane_y(i);
            break;

       case AXIS_Z:
            setplane_z(i);
            break;
    }
}

void clrplane (char axis, unsigned char i)
{
    switch (axis)
    {
        case AXIS_X:
            clrplane_x(i);
            break;
        
       case AXIS_Y:
            clrplane_y(i);
            break;

       case AXIS_Z:
            clrplane_z(i);
            break;
    }
}

// Fill a value into all 64 byts of the cube buffer
// Mostly used for clearing. fill(0x00)
// or setting all on. fill(0xff)
void fill (unsigned char pattern)
{
	int z;
	int y;
	for (z=0;z<CUBE_SIZE;z++)
	{
		for (y=0;y<CUBE_SIZE;y++)
		{
			cube[z][y] = pattern;
		}
	}
}

void tmpfill (unsigned char pattern)
{
	int z;
	int y;
	for (z=0;z<CUBE_SIZE;z++)
	{
		for (y=0;y<CUBE_SIZE;y++)
		{
			fb[z][y] = pattern;
		}
	}
}

// Draw a box with all walls drawn and all voxels inside set
void box_filled(int x1, int y1, int z1, int x2, int y2, int z2)
{
	int iy;
	int iz;

	argorder(x1, x2, &x1, &x2);
	argorder(y1, y2, &y1, &y2);
	argorder(z1, z2, &z1, &z2);

	for (iz=z1;iz<=z2;iz++)
	{
		for (iy=y1;iy<=y2;iy++)
		{
			cube[iz][iy] |= byteline(x1,x2);
		}
	}

}

// Darw a hollow box with side walls.
void box_walls(int x1, int y1, int z1, int x2, int y2, int z2)
{
	int iy;
	int iz;
	
	argorder(x1, x2, &x1, &x2);
	argorder(y1, y2, &y1, &y2);
	argorder(z1, z2, &z1, &z2);

	for (iz=z1;iz<=z2;iz++)
	{
		for (iy=y1;iy<=y2;iy++)
		{	
			if (iy == y1 || iy == y2 || iz == z1 || iz == z2)
			{
				cube[iz][iy] = byteline(x1,x2);
			} else
			{
				cube[iz][iy] |= ((0x01 << x1) | (0x01 << x2));
			}
		}
	}

}

// Draw a wireframe box. This only draws the corners and edges,
// no walls.
void box_wireframe(int x1, int y1, int z1, int x2, int y2, int z2)
{
	int iy;
	int iz;

	argorder(x1, x2, &x1, &x2);
	argorder(y1, y2, &y1, &y2);
	argorder(z1, z2, &z1, &z2);

	// Lines along X axis
	cube[z1][y1] = byteline(x1,x2);
	cube[z1][y2] = byteline(x1,x2);
	cube[z2][y1] = byteline(x1,x2);
	cube[z2][y2] = byteline(x1,x2);

	// Lines along Y axis
	for (iy=y1;iy<=y2;iy++)
	{
		setvoxel(x1,iy,z1);
		setvoxel(x1,iy,z2);
		setvoxel(x2,iy,z1);
		setvoxel(x2,iy,z2);
	}

	// Lines along Z axis
	for (iz=z1;iz<=z2;iz++)
	{
		setvoxel(x1,y1,iz);
		setvoxel(x1,y2,iz);
		setvoxel(x2,y1,iz);
		setvoxel(x2,y2,iz);
	}

}

// Returns a byte with a row of 1's drawn in it.
// byteline(2,5) gives 0b00111100
char byteline (int start, int end)
{
	return ((0xff<<start) & ~(0xff<<(end+1)));
}

// Flips a byte 180 degrees.
// MSB becomes LSB, LSB becomes MSB.
char flipbyte (char byte)
{
	char flop = 0x00;

	flop = (flop & 0b11111110) | (0b00000001 & (byte >> 7));
	flop = (flop & 0b11111101) | (0b00000010 & (byte >> 5));
	flop = (flop & 0b11111011) | (0b00000100 & (byte >> 3));
	flop = (flop & 0b11110111) | (0b00001000 & (byte >> 1));
	flop = (flop & 0b11101111) | (0b00010000 & (byte << 1));
	flop = (flop & 0b11011111) | (0b00100000 & (byte << 3));
	flop = (flop & 0b10111111) | (0b01000000 & (byte << 5));
	flop = (flop & 0b01111111) | (0b10000000 & (byte << 7));
	return flop;
}

// Draw a line between any coordinates in 3d space.
// Uses integer values for input, so dont expect smooth animations.
void line(int x1,int y1,int z1,int x2,int y2,int z2) {
    int i, dx, dy, dz, l, m, n, x_inc, y_inc, z_inc, err_1, err_2, dx2, dy2, dz2;
    int pixel[3];

    pixel[0] = x1;
    pixel[1] = y1;
    pixel[2] = z1;
    dx = x2 - x1;
    dy = y2 - y1;
    dz = z2 - z1;
    x_inc = (dx < 0) ? -1 : 1;
    l = abs(dx);
    y_inc = (dy < 0) ? -1 : 1;
    m = abs(dy);
    z_inc = (dz < 0) ? -1 : 1;
    n = abs(dz);
    dx2 = l << 1;
    dy2 = m << 1;
    dz2 = n << 1;

    if ((l >= m) && (l >= n)) {
        err_1 = dy2 - l;
        err_2 = dz2 - l;
        for (i = 0; i < l; i++) {
			setvoxel(pixel[0],pixel[1],pixel[2]);
            if (err_1 > 0) {
                pixel[1] += y_inc;
                err_1 -= dx2;
            }
            if (err_2 > 0) {
                pixel[2] += z_inc;
                err_2 -= dx2;
            }
            err_1 += dy2;
            err_2 += dz2;
            pixel[0] += x_inc;
        }
    } else if ((m >= l) && (m >= n)) {
        err_1 = dx2 - m;
        err_2 = dz2 - m;
        for (i = 0; i < m; i++) {
			setvoxel(pixel[0],pixel[1],pixel[2]);
            if (err_1 > 0) {
                pixel[0] += x_inc;
                err_1 -= dy2;
            }
            if (err_2 > 0) {
                pixel[2] += z_inc;
                err_2 -= dy2;
            }
            err_1 += dx2;
            err_2 += dz2;
            pixel[1] += y_inc;
        }
    } else {
        err_1 = dy2 - n;
        err_2 = dx2 - n;
        for (i = 0; i < n; i++) {
			setvoxel(pixel[0],pixel[1],pixel[2]);
            if (err_1 > 0) {
                pixel[1] += y_inc;
                err_1 -= dz2;
            }
            if (err_2 > 0) {
                pixel[0] += x_inc;
                err_2 -= dz2;
            }
            err_1 += dy2;
            err_2 += dx2;
            pixel[2] += z_inc;
        }
    }
	setvoxel(pixel[0],pixel[1],pixel[2]);
}

// Delay loop.
// This is not calibrated to milliseconds,
// but we had allready made to many effects using this
// calibration when we figured it might be a good idea
// to calibrate it.
void delay_ms(uint16_t x)
{
  uint8_t y, z;
  for ( ; x > 0 ; x--){
    for ( y = 0 ; y < 90 ; y++){
      for ( z = 0 ; z < 6 ; z++){
        asm volatile ("nop");
      }
    }
  }
}

// Copies the contents of fb (temp cube buffer) into the rendering buffer
void tmp2cube (void)
{	
    memcpy(cube, fb, 64); // copy the current cube into a buffer.
}

// Shift the entire contents of the cube along an axis
// This is great for effects where you want to draw something
// on one side of the cube and have it flow towards the other
// side. Like rain flowing down the Z axiz.
void shift (char axis, int direction)
{
	int i, x ,y;
	int ii, iii;
	int state;

	for (i = 0; i < CUBE_SIZE; i++)
	{
		if (direction == -1)
		{
			ii = i;
		} else
		{
			ii = (7-i);
		}	
	
	
		for (x = 0; x < CUBE_SIZE; x++)
		{
			for (y = 0; y < CUBE_SIZE; y++)
			{
				if (direction == -1)
				{
					iii = ii+1;
				} else
				{
					iii = ii-1;
				}
				
				if (axis == AXIS_Z)
				{
					state = getvoxel(x,y,iii);
					altervoxel(x,y,ii,state);
				}
				
				if (axis == AXIS_Y)
				{
					state = getvoxel(x,iii,y);
					altervoxel(x,ii,y,state);
				}
				
				if (axis == AXIS_X)
				{
					state = getvoxel(iii,y,x);
					altervoxel(ii,y,x,state);
				}
			}
		}
	}
	
	if (direction == -1)
	{
		i = 7;
	} else
	{
		i = 0;
	}	
	
	for (x = 0; x < CUBE_SIZE; x++)
	{
		for (y = 0; y < CUBE_SIZE; y++)
		{
			if (axis == AXIS_Z)
				clrvoxel(x,y,i);
				
			if (axis == AXIS_Y)
				clrvoxel(x,i,y);
			
			if (axis == AXIS_X)
				clrvoxel(i,y,x);
		}
	}
}

// Flip the cube 180 degrees along the y axis.
void mirror_y (void)
{
    unsigned char buffer[CUBE_SIZE][CUBE_SIZE];
    unsigned char x,y,z;

    memcpy(buffer, cube, CUBE_BYTES); // copy the current cube into a buffer.

    fill(0x00);
    for (z=0; z<CUBE_SIZE; z++)
    {
        for (y=0; y<CUBE_SIZE; y++)
        {
            for (x=0; x<CUBE_SIZE; x++)
            {
                if (buffer[z][y] & (0x01 << x))
                    setvoxel(x,CUBE_SIZE-1-y,z);
            }
        }
    }

}

// Flip the cube 180 degrees along the x axis
void mirror_x (void)
{
    unsigned char buffer[CUBE_SIZE][CUBE_SIZE];
    unsigned char y,z;

    memcpy(buffer, cube, CUBE_BYTES); // copy the current cube into a buffer.

    fill(0x00);
    
    for (z=0; z<CUBE_SIZE; z++)
    {
        for (y=0; y<CUBE_SIZE; y++)
        {
            // This will break with different buffer sizes..
            cube[z][y] = flipbyte(buffer[z][y]);
        }
    }
}

// flip the cube 180 degrees along the z axis
void mirror_z (void)
{
    unsigned char buffer[CUBE_SIZE][CUBE_SIZE];
    unsigned char z, y;

    memcpy(buffer, cube, CUBE_BYTES); // copy the current cube into a buffer.

    for (y=0; y<CUBE_SIZE; y++)
    {
        for (z=0; z<CUBE_SIZE; z++)
        {
            cube[CUBE_SIZE-1-z][y] = buffer[z][y];
        }
    }
}

//Circle Draw
void drawCircle(int x0, int y0, int z0, int radius){
  int r = radius, t = 0;
  int radiusError = 1-r;
 
  while(r >= t){
    setvoxel(r + x0, t + y0,z0);
    setvoxel(t + x0, r + y0,z0);
    setvoxel(-r + x0, t + y0,z0);
    setvoxel(-t + x0, r + y0,z0);
    setvoxel(-r + x0, -t + y0,z0);
    setvoxel(-t + x0, -r + y0,z0);
    setvoxel(r + x0, -t + y0,z0);
    setvoxel(t + x0, -r + y0,z0);
    t++;
    if (radiusError<0){
      radiusError += 2 * t + 1;
    }
    else{
      r--;
      radiusError += 2 * (t - r + 1);
    }
  }
}

//3D Sphere
void draw3DSphere(int x0, int y0, int z0, int radius){
	
	
	
/*	int i,k=1,r;
	r=radius;
	
	//r=3, radius=3
	for(i=0;i<radius*2;i++){
		if(i<radius){
			drawCircle(x0,y0,z0-r,k);	//i=0,1,2	r=3,2,1		k=1,2,3
			r--;
			k++;	//2,3,4
		}
		else{
			k--;	//3,2,1
			drawCircle(x0,y0,z0+r,k);	//i=3,4,5	k=3,2,1
			r++;
			
		}
		
		
	}*/
	
	
	
//	int x = radius, y = 0;
//	int radiusError = 1-x;

/*	while(x >= y){
		// pass in base point (x0,y0,z0), this algorithm's y as y1,
		// this algorithm's x as the radius, and pass along radius error.
		DrawCircle(x0, y0, z0, y, x, radiusError);
		y++;
		if(radiusError<0){
		  radiusError+=2*y+1;
		}
		else{
		  x--;
		  radiusError+=2*(y-x+1);
		}
	}*/
}

//3D Sphere
/*void drawCircle(int x0, int y0, int z0, int y1, int radius, int error0){
	int x = radius, z = 0;
	int radiusError = error0; // Initial error state passed in, NOT 1-x

	while(x >= z){
		// draw the 32 points here.
		z++;
		if(radiusError<0){
		  radiusError+=2*z+1;
		}
		else{
		  x--;
		  radiusError+=2*(z-x+1);
		}
	}
}*/

/*void drawSphere(int x0, int y0, int z0, int radius){
	int x = radius, y = 0;
	int radiusError = 1-x;

	while(x >= y){
		// pass in base point (x0,y0,z0), this algorithm's y as y1,
		// this algorithm's x as the radius, and pass along radius error.
		DrawCircle(x0, y0, z0, y, x, radiusError);
		y++;
		if(radiusError<0){
		  radiusError+=2*y+1;
		}
		else{
		  x--;
		  radiusError+=2*(y-x+1);
		}
	}
}*/

void arrayCopy(int arrayTo[], int arrayFrom[], int arrSize){
	int i;
	
	for(i=0; i<arrSize; i++){
		arrayTo[i] = arrayFrom[i];
	}

}