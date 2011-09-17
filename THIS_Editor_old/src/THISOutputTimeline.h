/*
 *  THISOutputTimeline.h
 *  THIS_Editor
 *
 *  Created by Jim on 9/23/10.
 *  Copyright 2010 FlightPhase. All rights reserved.
 *
 */

#pragma once

#include "ofMain.h"

class THISTimeline;
class THISOutputTimeline
{
  public:
	THISOutputTimeline();
    ~THISOutputTimeline();
    
	THISTimeline* inputTimeline;	
	ofImage* renderOutputFrame(bool thumb);

	int outputFrame;
	
	void draw(int x, int y);
	
  protected:
    ofImage* outputPreviewThumb;
	ofImage* outputPreviewImage;
	
    int bytesPerPixel;

};