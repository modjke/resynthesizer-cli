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

	int width;
	int height;

	read(0, &width, sizeof(width));
	read(0, &height, sizeof(height));

	int imageLength = 3 * width * height;
	int maskLength = width * height;

	unsigned char *image = malloc(imageLength);
	int bytesTotal = 0;
	int bytes = 0;

	while (bytesTotal < imageLength) {
		unsigned char *imagePtr = image;
		imagePtr += bytesTotal;
		bytes = read(0, imagePtr, imageLength - bytesTotal);
		bytesTotal += bytes;
	}
	
	unsigned char *mask = malloc(maskLength);	
	bytesTotal = 0;
	bytes = 0;	
	while (bytesTotal < maskLength) {
		unsigned char *maskPtr = mask;
		maskPtr += bytesTotal;
		bytes = read(0, maskPtr, maskLength - bytesTotal);
		bytesTotal += bytes;
	}

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
