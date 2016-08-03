#include "ofxTinyply.h"

using namespace tinyply;

namespace
{
	typedef std::chrono::time_point<std::chrono::high_resolution_clock> timepoint;
	std::chrono::high_resolution_clock c;

	inline std::chrono::time_point<std::chrono::high_resolution_clock> now()
	{
		return c.now();
	}

	inline double difference_micros(timepoint start, timepoint end)
	{
		return (double)std::chrono::duration_cast<std::chrono::microseconds>(end - start).count();
	}
}

bool ofxTinyply::loadFromFile(const string& filename)
{
	// Tinyply can and will throw exceptions at you!
	try
	{
		// Read the file and create a std::istringstream suitable
		// for the lib -- tinyply does not perform any file i/o.
		string path = ofToDataPath(filename);
		std::ifstream ss(path, std::ios::binary);
		
		// Parse the ASCII header fields
		PlyFile file(ss);
		
		if (is_verbose)
		{
			for (auto e : file.get_elements())
			{
				std::cout << "element - " << e.name << " (" << e.size << ")" << std::endl;
				for (auto p : e.properties)
				{
					std::cout << "\tproperty - " << p.name << " (" << PropertyTable[p.propertyType].str << ")" << std::endl;
				}
			}
			std::cout << std::endl;
		}

		// Define containers to hold the extracted data. The type must match
		// the property type given in the header. Tinyply will interally allocate the
		// the appropriate amount of memory.
		std::vector<float> verts;
		std::vector<float> norms;
		std::vector<uint8_t> colors;

		std::vector<uint32_t> faces;
		std::vector<float> uvCoords;
		std::vector<uint8_t> faceColors;

		uint32_t vertexCount, normalCount, colorCount, faceCount, faceTexcoordCount, faceColorCount;
		vertexCount = normalCount = colorCount = faceCount = faceTexcoordCount = faceColorCount = 0;

		// The count returns the number of instances of the property group. The vectors
		// above will be resized into a multiple of the property group size as
		// they are "flattened"... i.e. verts = {x, y, z, x, y, z, ...}
		vertexCount = file.request_properties_from_element("vertex", { "x", "y", "z" }, verts);
		normalCount = file.request_properties_from_element("vertex", { "nx", "ny", "nz" }, norms);
		colorCount = file.request_properties_from_element("vertex", { "red", "green", "blue", "alpha" }, colors);

		// For properties that are list types, it is possibly to specify the expected count (ideal if a
		// consumer of this library knows the layout of their format a-priori). Otherwise, tinyply
		// defers allocation of memory until the first instance of the property has been found
		// as implemented in file.read(ss)
		faceCount = file.request_properties_from_element("face", { "vertex_indices" }, faces, 3);
		faceTexcoordCount = file.request_properties_from_element("face", { "texcoord" }, uvCoords, 6);
		faceColorCount = file.request_properties_from_element("face", { "red", "green", "blue", "alpha" }, faceColors);

		// Now populate the vectors...
		timepoint before = now();
		file.read(ss);
		timepoint after = now();

		// Good place to put a breakpoint!
		if (is_verbose)
		{
			std::cout << "Parsing took " << difference_micros(before, after) << "μs: " << std::endl;
			std::cout << "\tRead " << verts.size() << " total vertices (" << vertexCount << " properties)." << std::endl;
			std::cout << "\tRead " << norms.size() << " total normals (" << normalCount << " properties)." << std::endl;
			std::cout << "\tRead " << colors.size() << " total vertex colors (" << colorCount << " properties)." << std::endl;
			std::cout << "\tRead " << faces.size() << " total faces (triangles) (" << faceCount << " properties)." << std::endl;
			std::cout << "\tRead " << uvCoords.size() << " total texcoords (" << faceTexcoordCount << " properties)." << std::endl;
			std::cout << "\tRead " << faceColors.size() << " total face colors (" << faceColorCount << " properties)." << std::endl;
		}

		// copy to vector
		if (vertexCount > 0)
		{
			ply_vertices.resize(vertexCount);
			memcpy(&ply_vertices[0].x, &verts[0], sizeof(ofVec3f) * vertexCount);
		}
		if (normalCount > 0)
		{
			ply_normals.resize(normalCount);
			memcpy(&ply_normals[0].x, &norms[0], sizeof(ofVec3f) * normalCount);
		}
		if (colorCount > 0)
		{
			for (size_t i = 0; i < colors.size(); i+=3)
				ply_colors.push_back(ofColor(colors[i], colors[i+1], colors[i+2]));
		}

		return true;
	}
	catch (const std::exception & e)
	{
		std::cerr << "Caught exception: " << e.what() << std::endl;
		return false;
	}
}

void ofxTinyply::saveToFile(const string& filename, const vector<ofVec3f>& vertices, const vector<ofVec3f>& normals, const vector<ofFloatColor>& color, bool isBinary)
{
	std::vector<float> verts;
	std::vector<float> norms;
	std::vector<uint8_t> colors;

	std::vector<uint32_t> faces;
	std::vector<float> uvCoords;
	std::vector<uint8_t> faceColors;

	if (vertices.size() > 0)
	{
		int num = vertices.size();
		verts.resize(num * 3);
		memcpy(&verts[0], &vertices[0].x, sizeof(float) * 3 * num);
	}

	if (normals.size() > 0)
	{
		int num = normals.size();
		norms.resize(num * 3);
		memcpy(&norms[0], &normals[0].x, sizeof(float) * 3 * num);
	}

	if (color.size() > 0)
	{
		for (auto& c : color)
		{
			ofColor cc = c;
			colors.push_back(cc.r);
			colors.push_back(cc.g);
			colors.push_back(cc.b);
			colors.push_back(cc.a);
		}
	}

	// Tinyply does not perform any file i/o internally
	string path = ofToDataPath(filename);
	std::ofstream outputFile(path);
	std::ostringstream outputStream;

	PlyFile myFile;

	if (verts.size() > 0) myFile.add_properties_to_element("vertex", { "x", "y", "z" }, verts);
	if (norms.size() > 0) myFile.add_properties_to_element("vertex", { "nx", "ny", "nz" }, norms);
	if (colors.size() > 0) myFile.add_properties_to_element("vertex", {"red", "green", "blue", "alpha"}, colors);

	// List property types must also be created with a count and type.
	if (faces.size() > 0) myFile.add_properties_to_element("face", { "vertex_indices" }, faces, 3, PlyProperty::Type::INT8);
	if (uvCoords.size() > 0) myFile.add_properties_to_element("face", { "texcoord" }, uvCoords, 6, PlyProperty::Type::INT8);
	if (faceColors.size() > 0) myFile.add_properties_to_element("face", { "red", "green", "blue", "alpha" }, faceColors);

	myFile.comments.push_back("generated by tinyply");
	myFile.write(outputStream, isBinary);

	outputFile << outputStream.str();
	outputFile.close();
}

void ofxTinyply::clear()
{
	ply_vertices.clear();
	ply_normals.clear();
	ply_colors.clear();
}