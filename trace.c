/*
Ryan Lee - 214240196 - drd
Cheng Shao - 214615934 - shaoc2
 */



#include <stddef.h>
#include <stdio.h>
#include <math.h>
#include "artInternal.h"

extern	Ray ShootRay(double, double);
extern	Color GetRadiance(Ray *, double);
extern FILE *OpenTIFF(int, int, char *);
extern void CloseTIFF(FILE *);
extern void WritePixelTIFF(FILE *, int, int, int);

#define INVERSE_GAMMA	(1.0/2.2)

char *
art_Trace(int xRes, int yRes, int numSamples, char *filename)
{
	FILE *fp;
	Ray ray;
	int x, y, red, green, blue;
	double u, v;
	Color sample;

	if(xRes < 1 || yRes < 1 || numSamples < 1 )
		return "art_Trace: domain error";
	if ((fp = OpenTIFF(xRes, yRes, filename)) == NULL)
		return "art_Trace: couldn't open output file";

	Color** colors = (Color**) malloc(xRes * sizeof(Color*));
	for (int i = 0; i < xRes; i++) {
		colors[i] = (Color*) malloc(yRes * sizeof(Color));
	}

	for (int i = 0; i < xRes; i++) {
		for (int j = 0; j < yRes; j++) {
			//printf("%d %d\n", i, j);
			colors[i][j] = (Color){0.0, 0.0, 0.0};
		}
	}
	/* compute image */
	for(y=0; y < yRes * numSamples; y++) {
		for(x= 0; x < xRes * numSamples; x++) {
			u= ((double) x) / (xRes * numSamples);
			v= 1.0 - ((double) y)/ (yRes * numSamples);
			ray= ShootRay(u, v);
			sample = GetRadiance(&ray, 1.000293);

			PLUS(
				colors[x / numSamples][y / numSamples], 
				colors[x / numSamples][y / numSamples], 
				sample);
		}
	}

	for (int i = 0; i < yRes; i++) {
		for (int j = 0; j < xRes; j++) {
			/* convert to bytes and write out */
			red= 255.0*pow(colors[j][i].v[0] / pow(numSamples, 2), INVERSE_GAMMA);
			if(red > 255) {
				red= 255;
				//printf("%f\n", colors[j][i].v[0] / numSamples);
			}
			else if(red < 0)
				red= 0;
			green= 255.0*pow(colors[j][i].v[1] / pow(numSamples, 2), INVERSE_GAMMA);
			if(green > 255)
				green= 255;
			else if(green < 0)
				green= 0;
			blue= 255.0*pow(colors[j][i].v[2] / pow(numSamples, 2), INVERSE_GAMMA);
			if(blue > 255)
				blue= 255;
			else if(blue < 0)
				blue= 0;
			WritePixelTIFF(fp, red, green, blue);
		}
	}

	for (int i = 0; i < xRes; i++) {
		free(colors[i]);
	}
	free(colors);

	CloseTIFF(fp);
	return NULL;
}
