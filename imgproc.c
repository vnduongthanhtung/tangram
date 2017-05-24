
#include <stdio.h>
#include <assert.h>
#include <png.h>
#include <math.h>


#define __DEBUG__

#include "/media/mint/9CFC-DB7F/C/array.h"
#include "/media/mint/9CFC-DB7F/C/list.h"
#include "/media/mint/9CFC-DB7F/C/map.h"

typedef unsigned char bool;
typedef unsigned char byte;

#define false 0
#define true 1

float blur_matrix[] = {
	1.0f/4, 1.0f/4,
	1.0f/4, 1.0f/4,
};

float GX[] = {
	-1, 0, 1
};

float GY[] = {
	-1, 0, 1
};

typedef struct{
	int x;
	int y;
}point;

typedef struct{
	int left;
	int top;
	int right;
	int bottom;	
}rect;

typedef struct{
	float r;
	float g;
	float b;
}color;

typedef list(point) list_point;
typedef list(list_point) list_list_point;
typedef array2(color) array2_color;

void filter(array2_float* pkernel, array2_color* psrcImg, array2_color* pdstImg)
{
	assert(psrcImg->rows == pdstImg->rows && psrcImg->cols == pdstImg->cols);	
	int rows = psrcImg->rows;
	int cols = psrcImg->cols;
	
	for(int y = 0; y < rows; y++)
	{
		for(int x = 0; x < cols; x++)
		{
			float r = 0.0f, g = 0.0f, b = 0.0f;
			for(int dy = 0; dy < pkernel->rows; dy++)
			{
				for(int dx = 0; dx < pkernel->cols; dx++)
				{
					int idx = x + dx - pkernel->cols/2, idy = y + dy - pkernel->rows/2;					
					if(idx >= 0 && idy >= 0 && idx < cols && idy < rows)
					{
						color col = at_parray2(psrcImg, idy, idx);
						float f = at_parray2(pkernel, dy, dx);
						r += f * col.r;
						g += f * col.g;
						b += f * col.b;
					}
				}
			}
			at_parray2(pdstImg, y, x) = (color){r,g,b};
		}
	}	
}

array2_float calc_canny_image(array2_color* psrcImg)
{
	array2_float dstImg	= new_array2(float, psrcImg->rows, psrcImg->cols);
	
	array2_float blur_kernel = {blur_matrix, 2, 2};
	array2_float gx_kernel = {GX, 1, 3};
	array2_float gy_kernel = {GY, 3, 1};
	
	array2_color filteredImg = new_array2(color, psrcImg->rows, psrcImg->cols);
	array2_color gImg = new_array2(color, psrcImg->rows, psrcImg->cols);
	
	filter(&blur_kernel, psrcImg, &filteredImg);
	
	filter(&gx_kernel, &filteredImg, &gImg);
	
	for(int y = 0; y < dstImg.rows; y++)
	{
		for(int x = 0; x < dstImg.cols; x++)
		{
			color gxColor = at_array2(gImg, y, x);
			at_array2(dstImg, y, x) = gxColor.r * gxColor.r + gxColor.g * gxColor.g + gxColor.b * gxColor.b;
		}
	}
	
	filter(&gy_kernel, &filteredImg, &gImg);
	
	for(int y = 0; y < dstImg.rows; y++)
	{
		for(int x = 0; x < dstImg.cols; x++)
		{
			color gyColor = at_array2(gImg, y, x);
			at_array2(dstImg, y, x) += gyColor.r * gyColor.r + gyColor.g * gyColor.g + gyColor.b * gyColor.b;
		}
	}
		
	free_array2(filteredImg);
	free_array2(gImg);
	
	return dstImg;
}

array2_float get_borders_image(array2_float* pcannyImg, float min_thresh, float max_thresh, int expansion){

	int cols = pcannyImg->cols, rows = pcannyImg->rows;
	array2_float bordersImg = new_array2(float, rows, cols);
	
	float max_thresh2 = max_thresh * max_thresh;
	float min_thresh2 = min_thresh * min_thresh;
	int marginSize = 1;
	
	for(int y = 0; y < rows; y++)
	{
		for(int x = 0; x < cols; x++)
		{
			at_array2(bordersImg, y, x) = 0.0f;
			if(x >= marginSize && y >= marginSize && x + marginSize < cols && y + marginSize < rows)
			{
				bool is_border = false;
				float val = at_parray2(pcannyImg, y, x);
				if(val > max_thresh2)
				{
					is_border = true;
				}
				else if(val > min_thresh2)
				{					
					is_border |= at_parray2(pcannyImg, y-1, x-1) > max_thresh2;
					is_border |= at_parray2(pcannyImg, y-1, x) > max_thresh2;
					is_border |= at_parray2(pcannyImg, y-1, x+1) > max_thresh2;
					is_border |= at_parray2(pcannyImg, y, x-1) > max_thresh2;
					is_border |= at_parray2(pcannyImg, y, x+1) > max_thresh2;
					is_border |= at_parray2(pcannyImg, y+1, x-1) > max_thresh2;
					is_border |= at_parray2(pcannyImg, y+1, x) > max_thresh2;
					is_border |= at_parray2(pcannyImg, y+1, x+1) > max_thresh2;					
				}
				
				if(is_border)
				{
					for(int dy = -expansion; dy <= expansion; dy++)
					{
						for(int dx = -expansion; dx <= expansion; dx++)
						{
							int x1 = x + dx, y1 = y + dy;
							if(x1 < 0 || y1 < 0 || x1 >= cols || y1 >= rows) continue;
							at_array2(bordersImg, y1, x1) = 1.0f;
						}
					}						
				}
			}
		}
	}
	
	return bordersImg;
}

list_point read_from_file(char* infile)
{	
	list_point lp = new_list(point);
	FILE* f = fopen(infile, "rt");
	
	point last_point = {-1, -1};
	point first_point = {-1, -1};
	while(!feof(f))
	{
		int x = -1, y = -1;
		fscanf(f, "%d %d", &x, &y);
		if(x >= 0 && y >= 0)
		{
			if(first_point.x < 0)
			{
				first_point = (point){x,y};
			}
			
			add_to_list(lp, ((point){x,y}));
			
			if(last_point.x >= 0)
			{
				add_to_list(lp, ((point){(x + last_point.x)/2, (y + last_point.y)/2}));
			}
			last_point = (point){x,y};
		}
	}
	add_to_list(lp, ((point){(first_point.x + last_point.x)/2, (first_point.y + last_point.y)/2}));
	return lp;
}

list_point copy_list_point(list_point* plp)
{
	list_point lp1 = new_list(point);
	for(int i = 0; i < plist_size(plp); i++)
	{
		add_to_list(lp1, at_plist(plp, i));
	}
	return lp1;
}

void translate(list_point* pcurve, int dx, int dy)
{
	for(int i = 0; i < plist_size(pcurve); i++)
	{
		point* pt = &at_plist(pcurve, i);
		pt->x += dx;
		pt->y += dy;
	}
}

void scale(list_point* pcurve, float f)
{
	for(int i = 0; i < plist_size(pcurve); i++)
	{
		point* pt = &at_plist(pcurve, i);
		pt->x = (int) (pt->x * f);
		pt->y = (int) (pt->y * f);
	}
}

rect get_boundary(list_point* plp)
{
	int xmin = 0, xmax = 0, ymin = 0, ymax = 0;
	for(int i = 0; i < plist_size(plp); i++)
	{
		point pt = at_plist(plp, i);
		if(i == 0 || xmin > pt.x)
		{
			xmin = pt.x;
		}
		if(i == 0 || xmax < pt.x)
		{
			xmax = pt.x;
		}
		if(i == 0 || ymin > pt.y)
		{
			ymin = pt.y;
		}
		if(i == 0 || ymax < pt.y)
		{
			ymax = pt.y;
		}
	}
	return (rect){xmin, ymin, xmax, ymax};
}


#define THRESH 32.0f/255
#define EXPANSION_RATE 0.03f
#define Y_INC 1
#define X_INC 1
#define SCALE_INC 0.01f

array2_float get_background_image(array2_color* pimage)
{
	int diag = (int) sqrtf(pimage->rows * pimage->rows + pimage->cols * pimage->cols);
	
	array2_float cannyImg = calc_canny_image(pimage);
	array2_float borderImg = get_borders_image(&cannyImg, THRESH, 2*THRESH, (int) (diag * EXPANSION_RATE));
	
	free_array2(cannyImg);
	return borderImg;
}

bool fit(array2_float* pbkgImg, list_point* plp)
{
	for(int i = 0; i < plist_size(plp); i++)
	{
		point pt = at_plist(plp, i);
		if(pt.x < 0 || pt.y < 0 || pt.x >= pbkgImg->cols || pt.y >= pbkgImg->rows || at_parray2(pbkgImg, pt.y, pt.x) != 1.0f)
		{
			return false;
		}		
	}
	return true;
}

int main(int argc, char *argv[])
{
	
	int w2, h2;
	char border_file_name[256];
	
	list_point lp = read_from_file(argv[1]);
	
	int w1 = atoi(argv[3]);
	int h1 = atoi(argv[4]);
	array2_color img = new_array2(color, h1, w1);
	
	FILE* rawfile = fopen(argv[2], "rb");
	byte* data = malloc(4*w1*h1);
	fread(data, 1, 4*h1*w1, rawfile);
	fclose(rawfile);
	
	for(int y = 0; y < h1; y++)
	{
		for(int x = 0; x < w1; x++)
		{
			int index = y * w1 + x;
			at_array2(img, y, x) = (color){data[4*index+1]/255.0f, data[4*index+2]/255.0f, data[4*index+3]/255.0f};
		}
	}
	
	free(data);
	
	rect bound = get_boundary(&lp);
	translate(&lp, -bound.left, -bound.top);
	w2 = bound.right - bound.left + 1;
	h2 = bound.bottom - bound.top + 1;
	
	
	array2_float bkgImg = get_background_image(&img);
	
	float fw = ((float)w1)/w2;
	float fh = ((float)h1)/h2;
	float f = fw < fh? fw : fh;
	w2 = (int)(w2 * f);
	h2 = (int)(h2 * f);
	
	//printf("w2=%d,h2=%d,f=%f\n", w2, h2, f);
	scale(&lp, f);
	
	bool ok = false;
	for(float f = 1.0f; !ok && f >= 0.7f; f -= SCALE_INC)
	{
		list_point scale_lp = copy_list_point(&lp);
		scale(&scale_lp, f);		
		for(int dy = 0; !ok && dy < h1 - (int)(h2*f); dy += Y_INC)
		{
			for(int dx = 0; !ok && dx < w1 - (int)(w2*f); dx += X_INC)
			{
				list_point trans_lp = copy_list_point(&scale_lp);
				translate(&trans_lp, dx, dy);
				if(fit(&bkgImg, &trans_lp))
				{
					ok = true;
					//printf("f=%f,dx=%d,dy=%d\n", f, dx, dy);
				}
				free_list(trans_lp);
			}
		}
		free_list(scale_lp);
	}
		
	free_list(lp);
	free_array2(bkgImg);
	free_array2(img);
	printf("%d", ok);
	return 0;
}
