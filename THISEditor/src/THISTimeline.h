/*
 *  THISTimeline.h
 *  THIS_Editor
 *
 *  Created by Jim on 9/23/10.
 *  Copyright 2010 FlightPhase. All rights reserved.
 *
 */

#ifndef THIS_TIMELINE
#define THIS_TIMELINE

#include "ofMain.h"
#include "ofxMSAInteractiveObjectDelegate.h"
#include "THISSequence.h"
#include "ofxXmlSettings.h"
#include "THISOutputTimeline.h"
#include "THISExporter.h"
#include "THISKeyframeEditor.h"
#include "THISTimelineZoom.h"

typedef struct {
	bool loaded;
	float* pixels;
	float distortionWidth;
	float lastUsedTime;
	int index;
} BlendedDistortionFrame;


class THISTimeline : public ofxMSAInteractiveObjectDelegate
{
  public:
	THISTimeline();
	~THISTimeline();

	int maxDistortionWidth;


	void setup();
	bool loadComposition(string compFolder);

	void draw();

    ofxXmlSettings settings;
    ofxXmlSettings defaults;

    ofPoint position;
	ofPoint uiposition;

	float width;
    bool allSequencesLoaded();

	THISSequence* sourceA;
	THISSequence* sourceB;
	THISSequence* distortion;
    THISOutputTimeline* output;

	THISKeyframeEditor* maxwidthkeys;
	THISKeyframeEditor* minwidthkeys;
	THISKeyframeEditor* sourcekeys;
	THISKeyframeEditor* blendkeys;
	THISTimelineZoom* zoomer;

	int getWindowStartFrame(float percent);
	int getWindowEndFrame(float percent);

	int getWindowStartFrame(int baseFrame);
	int getWindowEndFrame(int baseFrame);

	int getDistortionWidth(int baseFrame);
	int getDistortionWidth(float percent);
	int getMinOffsetAtPercent(float percent);

	int getFirstFrameInView();
	int getLastFrameInView();

	int getCurrentWindowStartFrame();
	int getCurrentWindowEndFrame();
	int getCurrentDistortionWidth();

	ofTexture* getCurrentPreviewStartSourceAFrame();
	ofTexture* getCurrentPreviewEndSourceAFrame();

	ofTexture* getCurrentPreviewStartSourceBFrame();
	ofTexture* getCurrentPreviewEndSourceBFrame();

	ofTexture* getCurrentPreviewStartDistortionFrame();
	ofTexture* getCurrentPreviewEndDistortionFrame();

	ofTexture* getCurrentPreviewBlendedDistortionFrame();
	ofTexture* getCurrentPreviewOutputFrame();

	unsigned char* getSourcePixelsForFrame(int offset, bool thumb);
	float* getCurrentBlendedDistortionFrame(bool thumb);
	float* getBlendedDistortionFrame(float percent, bool thumb);
	float* getBlendedDistortionFrame(int index, bool thumb);

    unsigned char* distortionPreviewPixelsChar;

	ofxMSAInteractiveObjectWithDelegate* loadCompButton;
	ofxMSAInteractiveObjectWithDelegate* loadSourceAButton;
	ofxMSAInteractiveObjectWithDelegate* loadSourceBButton;
	ofxMSAInteractiveObjectWithDelegate* loadDistortionButton;
	ofxMSAInteractiveObjectWithDelegate* setOutputDirectoryButton;
	ofxMSAInteractiveObjectWithDelegate* playheadBar;

	ofxMSAInteractiveObjectWithDelegate* exportCurrentViewButton;
	ofxMSAInteractiveObjectWithDelegate* exportEntireSequenceButton;
	ofxMSAInteractiveObjectWithDelegate* cancelExportButton;

	vector<BlendedDistortionFrame> blendedDistortion;
	vector<BlendedDistortionFrame> blendedDistortionThumbnails;

	void objectDidRollOver(ofxMSAInteractiveObject* object, int x, int y);
    void objectDidRollOut(ofxMSAInteractiveObject* object, int x, int y);

	void objectDidPress(ofxMSAInteractiveObject* object, int x, int y, int button);
	void objectDidRelease(ofxMSAInteractiveObject* object, int x, int y, int button);
	void objectDidMouseMove(ofxMSAInteractiveObject* object, int x, int y);

	float inPercent;
	float outPercent;

    string settingsFileName;

	void purgeMemoryForExport();

	int getSequenceLength();

    bool rollingOverPlayhead;
    float playheadPosition;
    float playheadFloatPosition;
	float zoomedPlayheadPosition;
	float zoomedPlayheadFloatPosition;

	void togglePlayback();

	THISExporter* exporter;
	void exportEntireSequence();
	void exportCurrentView();


  private:
	bool isPlayingBack;
	float playbackStartTime;
	float percentPerFrame;

	bool exporting;
	string currentCompFolder;

	//textures for previewing frames
	ofTexture* getCurrentPreviewFrame(THISSequence* source, ofTexture* previewFrame, int baseFrame);
	ofTexture* previewStartSourceAFrame;
	int currentPreviewStartSourceAFrame;

	ofTexture* previewEndSourceAFrame;
	int currentPreviewEndSourceAFrame;

	ofTexture* previewStartSourceBFrame;
	int currentPreviewStartSourceBFrame;

	ofTexture* previewEndSourceBFrame;
	int currentPreviewEndSourceBFrame;

	ofTexture* previewStartDistortionFrame;
	int currentPreviewStartDistortionFrame;

	ofTexture* previewEndDistortionFrame;
    int currentPreviewEndDistortionFrame;

	ofTexture* previewBlendedDistortionFrame;
	int currentPreviewBlendedDistortionFrame;

	ofTexture* previewOutputFrame;
    int currentPreviewOutputFrame;

	void createDistortionCaches();
	void clearDistortionCaches();
	void startExport();
};

#endif
