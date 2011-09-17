/*
 *  THISExporter.h
 *  THIS_Editor
 *
 *  Created by Jim on 6/26/11.
 *  Copyright 2011 FlightPhase. All rights reserved.
 *
 */

#pragma once

#include "ofMain.h"

#include "THISOutputTimeline.h"

//TODO: extend to support mulitiple exporters

class THISTimeline;
class THISExporter : public ofThread
{
  public:
	THISExporter();
	bool isExporting();
	float getPercentDone();
	void cancelExport();
	
	THISTimeline* inputTimeline;
	THISOutputTimeline* outputTimeline;

	int startFrame;
	int endFrame;
	int frameStep;
	
	string pathPrefix;
	
	int currentFrame;
	
	
  protected:
	bool exporting;
	bool shouldCancelExport;
	virtual void threadedFunction();

};