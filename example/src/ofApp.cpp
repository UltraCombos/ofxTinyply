#include "ofApp.h"

//--------------------------------------------------------------
void ofApp::setup(){
	ofSetWindowShape(WIDTH, HEIGHT);
	ofSetWindowPosition((ofGetScreenWidth() - ofGetWidth()) / 2, (ofGetScreenHeight() - ofGetHeight()) / 2);

	tinyply = ofxTinyply::create(true);
	string filename = "scene_04_ts0.ply";
	//loadModel(filename);

	camera.setupPerspective(false, 50, 0.01, 30.0f);
	camera.setDistance(5.0f);
}

//--------------------------------------------------------------
void ofApp::update(){
	ofSetWindowTitle("example-ofxTinyply: " + ofToString(ofGetFrameRate(), 1));

}

//--------------------------------------------------------------
void ofApp::draw(){
	ofClear(0);
	auto viewport = ofGetCurrentViewport();
	
	camera.begin();
	if (vbo.getIsAllocated()) vbo.draw(GL_POINTS, 0, vbo.getNumVertices());
	camera.end();

}

//--------------------------------------------------------------
void ofApp::keyPressed(int key){	
}

//--------------------------------------------------------------
void ofApp::dragEvent(ofDragInfo dragInfo){ 
	loadModel(dragInfo.files[0]);
}

void ofApp::loadModel(const string& filename)
{
	tinyply->clear();
	if (tinyply->loadFromFile(filename))
	{
		auto& vertices = tinyply->getVertices();
		auto& normals = tinyply->getNormals();
		auto& colors = tinyply->getColors();

		vbo.clear();
		vbo.setVertexData(&vertices[0].x, 3, vertices.size(), GL_STATIC_DRAW);
		if (normals.size() == vertices.size()) vbo.setNormalData(&normals[0].x, normals.size(), GL_STATIC_DRAW);
		if (colors.size() == vertices.size()) vbo.setColorData(&colors[0].r, colors.size(), GL_STATIC_DRAW);
	}
}