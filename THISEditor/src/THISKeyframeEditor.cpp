/*
 *  THISKeyframeEditor.cpp
 *  THIS_Editor
 *
 *  Created by Jim on 6/26/11.
 *  Copyright 2011 FlightPhase. All rights reserved.
 *
 */

#include "THISKeyframeEditor.h"

#include "ofxXmlSettings.h"

static bool isPointInRect(ofVec2f p, ofRectangle r){
	return p.x > r.x && p.y > r.y && p.x < r.x+r.width && p.y < r.y+r.height;
}


bool keyframesort(THISKeyframe* a, THISKeyframe* b){
	return a->position.x < b->position.x;
}

THISKeyframeEditor::THISKeyframeEditor()
{
	zoomBounds = ofRange(0.0, 1.0);
	
	ofAddListener(ofEvents.mouseMoved, this, &THISKeyframeEditor::mouseMoved);
	ofAddListener(ofEvents.mousePressed, this, &THISKeyframeEditor::mousePressed);
	ofAddListener(ofEvents.mouseReleased, this, &THISKeyframeEditor::mouseReleased);
	ofAddListener(ofEvents.mouseDragged, this, &THISKeyframeEditor::mouseDragged);

	ofAddListener(ofEvents.keyPressed, this, &THISKeyframeEditor::keyPressed);

	initializeEasings();
	reset();
	
	selectedKeyframe = NULL;
	hasFocus = false;
	autosave = true;
	enabled = true;
	drawingEasingWindow = false;
	
	xmlFileName = "_keyframes.xml";
}

THISKeyframeEditor::~THISKeyframeEditor()
{
	clear();
}

void THISKeyframeEditor::setXMLFileName(string filename)
{
	xmlFileName = filename;
}

void THISKeyframeEditor::setZoomBounds(ofRange zb) //allows you to zoom in!
{
	zoomBounds = zb;
}

void THISKeyframeEditor::setDrawRect(ofRectangle newBounds)
{
	bounds = newBounds;
}

ofRectangle THISKeyframeEditor::getDrawRect()
{
	return bounds;
}

float THISKeyframeEditor::sampleTimelineAt(float percent)
{
	percent = ofClamp(percent, 0, 1.0);
	
	//edge case
	if(percent == 0.0){
		return firstkey->position.x;
	}
	
	for(int i = 1; i < keyframes.size(); i++){
		if(keyframes[i]->position.x >= percent){
			float percentBetween = ofxTween::map(percent, keyframes[i-1]->position.x, keyframes[i]->position.x, 0.0, 1.0, false, 
												 *keyframes[i-1]->easeFunc->easing, keyframes[i-1]->easeType->type);
			return keyframes[i-1]->position.y * (1.-percentBetween) + keyframes[i]->position.y*percentBetween;
		}
	}
	
	ofLog(OF_LOG_ERROR, "THISKeyframeEditor --- Error condition, couldn't find keyframe for percent " + ofToString(percent, 4));
	return 0;
}

void THISKeyframeEditor::draw()
{
	
	ofPushStyle();
	ofPushMatrix();
	ofEnableSmoothing();
	
	//**** DRAW BORDER
	ofNoFill();
	if(hasFocus){
		ofSetColor(255, 200, 0); //focused outline color
	}
	else{
		ofSetColor(150, 150, 0); //unfocused outline color
	}

	ofRect(bounds.x, bounds.y, bounds.width, bounds.height);

	ofSetColor(100, 0, 0);
	//**** DRAW KEYFRAME LINES
	ofNoFill();
	ofBeginShape();
	for(int p = 0; p <= bounds.width; p++){
		ofVertex(bounds.x + p,  bounds.y + bounds.height - sampleTimelineAt(ofMap(p, 0, bounds.width, zoomBounds.min, zoomBounds.max, true)) * bounds.height);
	}
	ofEndShape(false);
	
	//**** DRAW KEYFRAME DOTS
	ofSetColor(255, 255, 100);
	for(int i = 0; i < keyframes.size(); i++){
		if(!keyframeIsInBounds(keyframes[i])){
			continue;
		}
		ofVec2f screenpoint = coordForKeyframePoint(keyframes[i]->position);
		if(keyframes[i] == selectedKeyframe){
			ofDrawBitmapString(ofToString(selectedKeyframe->position.y*100, 2) + "%", screenpoint.x+5, screenpoint.y+10);
			ofFill();
		}
		else{
			ofNoFill();
		}
		
		ofCircle(screenpoint.x, screenpoint.y, 5);
	}
	
	if(drawingEasingWindow){
		for(int i = 0; i < easingFunctions.size(); i++){
			if(easingFunctions[i] == selectedKeyframe->easeFunc){
				ofSetColor(150, 100, 10);
			}
			else{
				ofSetColor(80, 80, 80);
			}
			ofFill();
			ofRect(easingWindowPosition.x + easingFunctions[i]->bounds.x, easingWindowPosition.y +easingFunctions[i]->bounds.y, 
				   easingFunctions[i]->bounds.width, easingFunctions[i]->bounds.height);
			ofSetColor(200, 200, 200);
			ofDrawBitmapString(easingFunctions[i]->name, 
							   easingWindowPosition.x + easingFunctions[i]->bounds.x+10, 
							   easingWindowPosition.y + easingFunctions[i]->bounds.y+15);			
			
			ofNoFill();
			ofSetColor(40, 40, 40);
			ofRect(easingWindowPosition.x + easingFunctions[i]->bounds.x, easingWindowPosition.y +easingFunctions[i]->bounds.y, 
				   easingFunctions[i]->bounds.width, easingFunctions[i]->bounds.height);
			
		}
		
		for(int i = 0; i < easingTypes.size(); i++){
			if(easingTypes[i] == selectedKeyframe->easeType){
				ofSetColor(150, 100, 10);
			}
			else{
				ofSetColor(80, 80, 80);
			}
			ofFill();
			ofRect(easingWindowPosition.x + easingTypes[i]->bounds.x, easingWindowPosition.y + easingTypes[i]->bounds.y, 
				   easingTypes[i]->bounds.width, easingTypes[i]->bounds.height);
			ofSetColor(200, 200, 200);
			ofDrawBitmapString(easingTypes[i]->name, 
							   easingWindowPosition.x + easingTypes[i]->bounds.x+10, 
							   easingWindowPosition.y + easingTypes[i]->bounds.y+15);			
			ofNoFill();
			ofSetColor(40, 40, 40);
			ofRect(easingWindowPosition.x + easingTypes[i]->bounds.x, 
				   easingWindowPosition.y + easingTypes[i]->bounds.y, 
				   easingTypes[i]->bounds.width, easingTypes[i]->bounds.height);
		}
	}
	
	ofPopMatrix();
	ofPopStyle();
}

void THISKeyframeEditor::enable()
{
	enabled = true;
}

void THISKeyframeEditor::disable()
{
	enabled = false;
	hasFocus = false;
}

void THISKeyframeEditor::loadFromXML()
{
	ofxXmlSettings savedkeyframes;
	if(!savedkeyframes.loadFile(xmlFileName)){
		ofLog(OF_LOG_ERROR, "THISKeyframeEditor --- couldn't load xml file " + xmlFileName);
		reset();
		return;
	}
	
	clear();
	
	savedkeyframes.pushTag("keyframes");
	int numKeyTags = savedkeyframes.getNumTags("key");
	
	for(int i = 0; i < numKeyTags; i++){
		savedkeyframes.pushTag("key", i);
		THISKeyframe* key = newKeyframe(ofVec2f(savedkeyframes.getValue("x", 0.0),
												 savedkeyframes.getValue("y", 0.0)));
								 
		key->easeFunc = easingFunctions[ofClamp(savedkeyframes.getValue("easefunc", 0), 0, easingFunctions.size()-1)];
		key->easeType = easingTypes[ofClamp(savedkeyframes.getValue("easetype", 0), 0, easingTypes.size()-1)];
										
		savedkeyframes.popTag(); //key

	}
	
	savedkeyframes.popTag();//keyframes

	
	updateKeyframeSort();
}

void THISKeyframeEditor::clear()
{
	for(int i = 0; i < keyframes.size(); i++){
		delete keyframes[i];
	}
	keyframes.clear();	
}

void THISKeyframeEditor::reset()
{
	clear();
	//add first and last keyframe always
	firstkey = newKeyframe( ofVec2f(0, .5) );
	lastkey = newKeyframe( ofVec2f(1.0, .5) );
}

void THISKeyframeEditor::saveToXML()
{
	ofxXmlSettings savedkeyframes;
	savedkeyframes.addTag("keyframes");
	savedkeyframes.pushTag("keyframes");
	
	for(int i = 0; i < keyframes.size(); i++){
		savedkeyframes.addTag("key");
		savedkeyframes.pushTag("key", i);
		savedkeyframes.addValue("x", keyframes[i]->position.x);
		savedkeyframes.addValue("y", keyframes[i]->position.y);
		savedkeyframes.addValue("easefunc", keyframes[i]->easeFunc->id);
		savedkeyframes.addValue("easetype", keyframes[i]->easeType->id);
		savedkeyframes.popTag(); //key
	}
	
	savedkeyframes.popTag();//keyframes
	savedkeyframes.saveFile(xmlFileName);
}

void THISKeyframeEditor::mousePressed(ofMouseEventArgs& args)
{
	if(!enabled) return;
	
	ofVec2f screenpoint = ofVec2f(args.x, args.y);
	if(drawingEasingWindow){
		//see if we clicked on an
		drawingEasingWindow = false;

		for(int i = 0; i < easingFunctions.size(); i++){
			if(isPointInRect(screenpoint-easingWindowPosition, easingFunctions[i]->bounds)){
				selectedKeyframe->easeFunc = easingFunctions[i];
				return;
			}
		}
		for(int i = 0; i < easingTypes.size(); i++){
			if(isPointInRect(screenpoint-easingWindowPosition, easingTypes[i]->bounds)){
				selectedKeyframe->easeType = easingTypes[i];
				return;
			}
		}
	}
	
	bool clickIsInRect = screenpointIsInBounds(screenpoint);
	if(!hasFocus){
		hasFocus = clickIsInRect;
		if(!hasFocus){
			selectedKeyframe = NULL;
			drawingEasingWindow = false;
		}
		return;
	}
	
	if(!clickIsInRect){
		hasFocus = false;
		selectedKeyframe = NULL;
		drawingEasingWindow = false;
		return;
	}
	
	selectedKeyframe = keyframeAtScreenpoint(screenpoint, selectedKeyframeIndex);
	//cout << "selected index is " << selectedKeyframeIndex << endl;
	if(selectedKeyframe == NULL){
		//add a new one
		selectedKeyframe = newKeyframe( keyframePointForCoord(screenpoint) );
		
		grabOffset = ofVec2f(0,0);
		updateKeyframeSort();
		//find bounds
		for(int i = 1; i < keyframes.size()-1; i++){
			if(keyframes[i] == selectedKeyframe){
				selectedKeyframeIndex = i;
				minBound = keyframes[selectedKeyframeIndex-1]->position.x;
				maxBound = keyframes[selectedKeyframeIndex+1]->position.x;
			}
		}
	}
	//grabbed a keyframe
	else {
		if(args.button == 0){
			grabOffset = screenpoint - coordForKeyframePoint(selectedKeyframe->position);
			if(selectedKeyframe == firstkey){
				minBound = maxBound = 0.0;
				//cout << "selected first key" << endl;
			}
			else if(selectedKeyframe == lastkey){
				minBound = maxBound = 1.0;
				//cout << "selected last key" << endl;
			}
			else{
				minBound = keyframes[selectedKeyframeIndex-1]->position.x;
				maxBound = keyframes[selectedKeyframeIndex+1]->position.x;
				//cout << "keyframe point is " << selectedKeyframe->position.x << " min bound is " << minBound << " max bound is " << maxBound << endl;
			}
		}
		else if(args.button == 2){
			easingWindowPosition = screenpoint;
			drawingEasingWindow = true;
		}
	}
}

void THISKeyframeEditor::mouseMoved(ofMouseEventArgs& args)
{
	
}

void THISKeyframeEditor::mouseDragged(ofMouseEventArgs& args)
{
	if(!enabled) return;
	
	if(hasFocus && selectedKeyframe != NULL){
		ofVec2f newposition = keyframePointForCoord(ofVec2f(args.x, args.y) - grabOffset);
		newposition.x = ofClamp(newposition.x, minBound, maxBound);
		selectedKeyframe->position = newposition;
	}
}

void THISKeyframeEditor::updateKeyframeSort()
{
	sort(keyframes.begin(), keyframes.end(), keyframesort);
	firstkey = keyframes[0];
	lastkey = keyframes[keyframes.size()-1];	
}

void THISKeyframeEditor::mouseReleased(ofMouseEventArgs& args)
{
	if(autosave){
		saveToXML();
	}
}

void THISKeyframeEditor::keyPressed(ofKeyEventArgs& args)
{
	if(!enabled || !hasFocus) return;
	
	bool modified = false;
	if(args.key == OF_KEY_DEL || args.key == OF_KEY_BACKSPACE){
		if(hasFocus && selectedKeyframe != NULL && selectedKeyframe != firstkey && selectedKeyframe != lastkey){
			for(int i = 1; i < keyframes.size()-1; i++){
				if (keyframes[i] == selectedKeyframe) {
					delete keyframes[i];
					keyframes.erase(keyframes.begin()+i);
					selectedKeyframe = NULL;
					cout << "deleted keyframe at index " << i << endl;
					modified = true;
					break;
				}
			}
		}
	}
	if(hasFocus && selectedKeyframe != NULL){
		
		if(args.key == OF_KEY_UP){
			selectedKeyframe->position.y = MIN( int(100*selectedKeyframe->position.y+1)/100.0, 1.0);
			modified = true;
		}
		
		if(args.key == OF_KEY_DOWN){
			selectedKeyframe->position.y = MAX( int(100*selectedKeyframe->position.y-1)/100.0, 0.0);			
			modified = true;
		}
		
		if(selectedKeyframe != firstkey && selectedKeyframe != lastkey){
			if(args.key == OF_KEY_RIGHT){
				selectedKeyframe->position.x = MIN(selectedKeyframe->position.x+.0001, maxBound);			
				modified = true;
			}
			if(args.key == OF_KEY_LEFT){
				selectedKeyframe->position.x = MAX(selectedKeyframe->position.x-.0001, minBound);			
				modified = true;
			}
		}
	}
	
	if(autosave && modified){
		saveToXML();
	}
	
}

THISKeyframe* THISKeyframeEditor::keyframeAtScreenpoint(ofVec2f p, int& selectedIndex){
	float mindistance = 15;
	for(int i = 0; i < keyframes.size(); i++){
		ofVec2f keyonscreen = coordForKeyframePoint(keyframes[i]->position);
		if(keyonscreen.distance( p ) < mindistance){
			selectedIndex = i;
			return keyframes[i];
		}
	}
	return NULL;

}

bool THISKeyframeEditor::keyframeIsInBounds(THISKeyframe* key)
{
	if(zoomBounds.min == 0.0 && zoomBounds.max == 1.0) return true;
	
	return key->position.x > zoomBounds.min && key->position.x < zoomBounds.max;
}

ofVec2f THISKeyframeEditor::coordForKeyframePoint(ofVec2f keyframePoint)
{
	return ofVec2f(ofMap(keyframePoint.x, zoomBounds.min, zoomBounds.max, bounds.x, bounds.x+bounds.width, true),
				   ofMap(keyframePoint.y, 1.0, 0.0, bounds.y, bounds.y+bounds.height, true));
}

bool THISKeyframeEditor::screenpointIsInBounds(ofVec2f screenpoint)
{
	return isPointInRect(screenpoint, bounds);
}

ofVec2f THISKeyframeEditor::keyframePointForCoord(ofVec2f coord)
{
	return ofVec2f(ofMap(coord.x, bounds.x, bounds.x+bounds.width,  zoomBounds.min, zoomBounds.max, true),
				   ofMap(coord.y, bounds.y, bounds.y+bounds.height, 1.0, 0.0, true));
}

THISKeyframe* THISKeyframeEditor::newKeyframe(ofVec2f point)
{
	THISKeyframe* k = new THISKeyframe();
	k->position = point;
	k->easeFunc = easingFunctions[0];
	k->easeType = easingTypes[0];
	keyframes.push_back( k );
	return k;
}

void THISKeyframeEditor::initializeEasings()
{

	//FUNCTIONS ----
	EasingFunction* ef;
	ef = new EasingFunction();
	ef->easing = new ofxEasingLinear();
	ef->name = "linear";
	easingFunctions.push_back(ef);
	
	ef = new EasingFunction();
	ef->easing = new ofxEasingSine();
	ef->name = "sine";
	easingFunctions.push_back(ef);

	ef = new EasingFunction();
	ef->easing = new ofxEasingCirc();
	ef->name = "circular";
	easingFunctions.push_back(ef);

	ef = new EasingFunction();
	ef->easing = new ofxEasingQuad();
	ef->name = "quadratic";
	easingFunctions.push_back(ef);
	
	ef = new EasingFunction();
	ef->easing = new ofxEasingCubic();
	ef->name = "cubic";
	easingFunctions.push_back(ef);

	ef = new EasingFunction();
	ef->easing = new ofxEasingQuart();
	ef->name = "quartic";
	easingFunctions.push_back(ef);
	
	ef = new EasingFunction();
	ef->easing = new ofxEasingQuint();
	ef->name = "quintic";
	easingFunctions.push_back(ef);

	ef = new EasingFunction();
	ef->easing = new ofxEasingExpo();
	ef->name = "exponential";
	easingFunctions.push_back(ef);
	
	ef = new EasingFunction();
	ef->easing = new ofxEasingBack();
	ef->name = "back";
	easingFunctions.push_back(ef);

	ef = new EasingFunction();
	ef->easing = new ofxEasingBounce();
	ef->name = "bounce";
	easingFunctions.push_back(ef);

	ef = new EasingFunction();
	ef->easing = new ofxEasingElastic();
	ef->name = "elastic";
	easingFunctions.push_back(ef);

	///TYPES -------
	EasingType* et;
	et = new EasingType();
	et->type = ofxTween::easeIn;
	et->name = "ease in";
	easingTypes.push_back(et);

	et = new EasingType();
	et->type = ofxTween::easeOut;
	et->name = "ease out";
	easingTypes.push_back(et);

	et = new EasingType();
	et->type = ofxTween::easeInOut;
	et->name = "ease in-out";
	easingTypes.push_back(et);
	
	//TODO: make configurable
	easingBoxWidth  = 120;
	easingBoxHeight = 20;
	easingWindowSeperatorHeight = 4;
	
	for(int i = 0; i < easingFunctions.size(); i++){
		easingFunctions[i]->bounds = ofRectangle(0, i*easingBoxHeight, easingBoxWidth, easingBoxHeight);
		easingFunctions[i]->id = i;
	}
	
	for(int i = 0; i < easingTypes.size(); i++){
		easingTypes[i]->bounds = ofRectangle(0, (i+easingFunctions.size())*easingBoxHeight + easingWindowSeperatorHeight, easingBoxWidth, easingBoxHeight);
		easingTypes[i]->id = i;
	}
}

