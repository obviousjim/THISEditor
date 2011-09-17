/*
 *  THISOutputTimeline.cpp
 *  THIS_Editor
 *
 *  Created by Jim on 9/23/10.
 *  Copyright 2010 FlightPhase. All rights reserved.
 *
 */

#include "THISOutputTimeline.h"
#include "THISTimeline.h"

THISOutputTimeline::THISOutputTimeline()
{
    outputPreviewThumb = NULL;
	outputPreviewImage = NULL;
}

ofImage* THISOutputTimeline::renderOutputFrame(bool thumb)
{
    if(inputTimeline == NULL){
        return NULL;
    }
	
	if(outputPreviewImage == NULL){
		int type = inputTimeline->sourceA->getFrame(0, true)->type;
		outputPreviewImage = new ofImage();
		outputPreviewImage->setUseTexture(false);
		outputPreviewImage->allocate(inputTimeline->sourceA->getImageWidth(), inputTimeline->sourceA->getImageHeight(), type);
		
		outputPreviewThumb = new ofImage();
		outputPreviewThumb->setUseTexture(false);
		outputPreviewThumb->allocate(inputTimeline->sourceA->getThumbWidth(), inputTimeline->sourceA->getThumbHeight(), type);
		
		if(type == OF_IMAGE_COLOR){
            bytesPerPixel = 3;
        }
        else if(type == OF_IMAGE_COLOR_ALPHA){
            bytesPerPixel = 4;
        }
	}
	
	ofImage* outputDestination = thumb ? outputPreviewThumb : outputPreviewImage;
	int imagewidth  = outputDestination->getWidth();
	int imageheight = outputDestination->getHeight();
	
	//calculate the new distorted image
	unsigned char* writebuffer = outputDestination->getPixels();
	unsigned char* outbuffer = writebuffer;
	int mapMin;
	int mapMax;
	float* delayMapPixels;

	//	cout << "current output frame " << outputFrame << endl;
	
	if(thumb){
		delayMapPixels = inputTimeline->getCurrentBlendedDistortionFrame(true);
		mapMin = inputTimeline->getCurrentWindowStartFrame();
		mapMax = inputTimeline->getCurrentWindowEndFrame();
	}
	else{
		delayMapPixels = inputTimeline->getBlendedDistortionFrame(outputFrame, false);
		mapMin = inputTimeline->getWindowStartFrame(outputFrame);
		mapMax = inputTimeline->getWindowEndFrame(outputFrame);
	}
	
	if(mapMax <= mapMin+1){
		memcpy(outbuffer, inputTimeline->getSourcePixelsForFrame(mapMin, thumb), imagewidth*imageheight*bytesPerPixel);
	}
	else{
		int x, y, offset, lower_offset, upper_offset, pixelIndex;
		float precise, alpha, invalpha;	
		
		int bytesPerRow = imagewidth * bytesPerPixel;
				
		for(y = 0; y < imageheight; y++){
			for(x = 0; x < imagewidth; x++){
				
				//find pixel point in local reference
				pixelIndex = bytesPerRow*y + x*bytesPerPixel;
				precise = ofMap(delayMapPixels[imagewidth*y+x], 0.0, 1.0, mapMin, mapMax);
				
				//cast it to an integer
				offset = int(precise);
				
				//calculate alpha
				alpha = precise - offset;
				invalpha = 1.0 - alpha;
				
				unsigned char *a = inputTimeline->getSourcePixelsForFrame(offset  , thumb)+pixelIndex;
				unsigned char *b = inputTimeline->getSourcePixelsForFrame(offset+1, thumb)+pixelIndex;
				
				//interpolate and set values;
				for(int c = 0; c < bytesPerPixel; c++){
					*outbuffer++ = (a[c]*invalpha)+(b[c]*alpha);
				}
			}
		}
	}
	
	outputDestination->setFromPixels(writebuffer, imagewidth, imageheight, outputDestination->type);
	return outputDestination;
}
