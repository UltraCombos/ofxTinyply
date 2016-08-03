#pragma once

#include "ofMain.h"
#include "ofxTinyply.h"

class ofApp : public ofBaseApp{

public:
	void setup();
	void update();
	void draw();

	void keyPressed(int key);
	void dragEvent(ofDragInfo dragInfo);

	void loadModel(const string& filename);

private:
	enum {
		WIDTH = 1280,
		HEIGHT = 720
	};
	
	ofEasyCam camera;
	ofVbo vbo;
	ofxTinyplyRef tinyply;
};


