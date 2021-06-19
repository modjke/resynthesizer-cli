#include <stddef.h> // size_t
#include <stdio.h>	// printf
#include <stdlib.h>
#include <unistd.h>

#include "glibProxy.h"
#include "map.h"
#include "engineTypes.h"
#include "imageSynth.h"

void voidProgressCallback(int time, void* some) {} 

int main()
{

	int width;
	int height;

	read(0, &width, sizeof(width));
	read(0, &height, sizeof(height));

	int imageLength = 3 * width * height;
	int maskLength = width * height;

	unsigned char *image = malloc(imageLength);
	read(0, image, imageLength);
	unsigned char *mask = malloc(maskLength);
	read(0, mask, maskLength);

	

	ImageBuffer *imageBuffer = malloc(sizeof(ImageBuffer));
	imageBuffer->data = image;
	imageBuffer->width = width;
	imageBuffer->height = height;
	imageBuffer->rowBytes = width * 3;

	ImageBuffer *maskBuffer = malloc(sizeof(ImageBuffer));
	maskBuffer->data = mask;
	maskBuffer->width = width;
	maskBuffer->height = height;
	maskBuffer->rowBytes = width;

	
	//int status;
	// int error = imageSynth(imageBuffer, maskBuffer, T_RGB, NULL, voidProgressCallback, NULL, &status);

	//if (error == 0) {
	//	write(1, image, imageLength);
	//}
	write(1, image, imageLength);

	return 0;
}
