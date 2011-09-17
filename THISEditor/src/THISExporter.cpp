/*
 *  THISExporter.cpp
 *  THIS_Editor
 *
 *  Created by Jim on 6/26/11.
 *  Copyright 2011 FlightPhase. All rights reserved.
 *
 */

#include "THISExporter.h"
#include "THISTimeline.h"
#include "ofFileUtils.h"
THISExporter::THISExporter()
{
	inputTimeline = NULL;
	outputTimeline = NULL;
	currentFrame = 0;
	exporting = false;
	shouldCancelExport = false;
}

bool THISExporter::isExporting()
{
	return exporting;
}

float THISExporter::getPercentDone()
{
	return ofMap(currentFrame, startFrame, endFrame, 0, 1.0, true);
}
 
void THISExporter::cancelExport()
{
	shouldCancelExport = true;
}

void THISExporter::threadedFunction()
{
	if(inputTimeline == NULL){
		ofLog(OF_LOG_ERROR, "THISExporter -- no input timeline");
		return;
	}
	
	if(outputTimeline == NULL){
		ofLog(OF_LOG_ERROR, "THISExporter -- no output timeline");
		return;
	}
	
	currentFrame = startFrame;
	if(!ofDirectory::doesDirectoryExist(pathPrefix+"/output/", false)){
		if(!ofDirectory::createDirectory(pathPrefix+"/output/")){
			ofLog(OF_LOG_ERROR, "THISExporter -- couldn't make output directory: " + pathPrefix+"/output/");
			return;
		}
	}
	
	exporting = true;
	shouldCancelExport = false;
	
	char filename[1024];
	while(isThreadRunning() && !shouldCancelExport){
		
		outputTimeline->outputFrame = currentFrame;
		ofImage* outputFrame = outputTimeline->renderOutputFrame(false);
		if(outputFrame != NULL){
			sprintf(filename, "%s/output/output_%05d.png", pathPrefix.c_str(), currentFrame);
			outputFrame->saveImage(string(filename));
		}
		else{
			ofLog(OF_LOG_ERROR, "Couldn't export frame %d ", currentFrame);
		}
		
		inputTimeline->purgeMemoryForExport();
		
		currentFrame++;
		if(currentFrame > endFrame){
			break;
		}
		
		ofSleepMillis(5);
	}
	
	shouldCancelExport = false;
	exporting = false;
}