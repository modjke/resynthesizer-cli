#include <stddef.h> // size_t
#include <stdio.h>	// printf
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

#include "glibProxy.h"
#include "map.h"
#include "engineTypes.h"
#include "imageSynth.h"

void voidProgressCallback(int time, void* some) {} 

int main()
{

	int width = 600;
	int height = 400;

	// read(0, &width, sizeof(width));
	// read(0, &height, sizeof(height));

	int imageLength = 3 * width * height;
	int maskLength = width * height;

	unsigned char *image = malloc(imageLength);
	int imageFile = open("image.bin", O_RDONLY);
	if (read(imageFile, image, imageLength) != imageLength) {		
		return 1;
	}
	close(imageFile);
	unsigned char *mask = malloc(maskLength);
	int maskFile = open("mask.bin", O_RDONLY);	
	if (read(maskFile, mask, maskLength) != maskLength) {		
		return 1;
	}
	close(maskFile);

	

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

	TImageSynthParameters parameters;
	setDefaultParams(&parameters);
	
	int cancel = 0;
	int error = imageSynth(imageBuffer, maskBuffer, T_RGB, &parameters, voidProgressCallback, (void *) 0, &cancel);

	if (error == 0) {
		write(1, image, imageLength);
	}


	return 0;
}
