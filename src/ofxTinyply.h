#pragma once

#include "ofMain.h"

using ofxTinyplyRef = std::shared_ptr<class ofxTinyply>;

class ofxTinyply
{
public:
	static ofxTinyplyRef create(bool bVerbose = false)
	{
		return ofxTinyplyRef(new ofxTinyply(bVerbose));
	}

	bool loadFromFile(const string& filename);
	void saveToFile(const string& filename, const std::vector<ofVec3f>& vertices, const std::vector<ofVec3f>& normals, const std::vector<ofFloatColor>& colors, bool isBinary = false);
	void clear();

	const std::vector<ofVec3f>& getVertices() { return ply_vertices; }
	const std::vector<ofVec3f>& getNormals() { return ply_normals; }
	const std::vector<ofFloatColor>& getColors() { return ply_colors; }

private:
	ofxTinyply(bool bVerbose)
		:is_verbose(bVerbose)
	{

	}

	bool is_verbose;
	std::vector<ofVec3f> ply_vertices;
	std::vector<ofVec3f> ply_normals;
	std::vector<ofFloatColor> ply_colors;
};