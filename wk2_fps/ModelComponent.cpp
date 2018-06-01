#include "ModelComponent.h"
#include <fstream>
#include <iostream>
#include <vector>
#include <algorithm>
GLuint textureId;
/**
* Replaces a substring in a string
*/
static std::string replace(std::string str, const std::string &toReplace, const std::string &replacement)
{
	size_t index = 0;
	while (true)
	{
		index = str.find(toReplace, index);
		if (index == std::string::npos)
			break;
		str.replace(index, toReplace.length(), replacement);
		++index;
	}
	return str;
}

/**
* Splits a string into substrings, based on a seperator
*/
static std::vector<std::string> split(std::string str, const std::string &seperator)
{
	std::vector<std::string> ret;
	size_t index;
	while (true)
	{
		index = str.find(seperator);
		if (index == std::string::npos)
			break;
		ret.push_back(str.substr(0, index));
		str = str.substr(index + 1);
	}
	ret.push_back(str);
	return ret;
}

/**
* Turns a string to lowercase
*/
static inline std::string toLower(std::string data)
{
	std::transform(data.begin(), data.end(), data.begin(), ::tolower);
	return data;
}


/**
* Cleans up a line for processing
*/
static inline std::string cleanLine(std::string line)
{
	line = replace(line, "\t", " ");
	while (line.find("  ") != std::string::npos)
		line = replace(line, "  ", " ");
	if (line == "")
		return "";
	if (line[0] == ' ')
		line = line.substr(1);
	if (line == "")
		return "";
	if (line[line.length() - 1] == ' ')
		line = line.substr(0, line.length() - 1);
	return line;
}

Texture::Texture(std::string _location) {
	std::cout << "Text Location: " << location << "\n";
	location = _location;
	glGenTextures(1, &textureId);
	glBindTexture(GL_TEXTURE_2D, textureId);
	stbi_set_flip_vertically_on_load(true);
	imgData = stbi_load(location.c_str(), &width, &height, &bpp, 4);
	glTexImage2D(GL_TEXTURE_2D,
		0,		//level
		GL_RGBA,		//internal format
		width,		//width
		height,		//height
		0,		//border
		GL_RGBA,		//data format
		GL_UNSIGNED_BYTE,	//data type
		imgData);		//data
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	stbi_image_free(imgData);
}

void Texture::reloadTexture() {
	glGenTextures(1, &textureId);
	glBindTexture(GL_TEXTURE_2D, textureId);
	stbi_set_flip_vertically_on_load(true);
	imgData = stbi_load(location.c_str(), &width, &height, &bpp, 4);
	glTexImage2D(GL_TEXTURE_2D,
		0,		//level
		GL_RGBA,		//internal format
		width,		//width
		height,		//height
		0,		//border
		GL_RGBA,		//data format
		GL_UNSIGNED_BYTE,	//data type
		imgData);		//data
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	stbi_image_free(imgData);
}

void Texture::bindTexture() {
	glBindTexture(GL_TEXTURE_2D, textureId);
}




/**
* Loads an object model
*/
ModelComponent::ModelComponent(const std::string &fileName)
{
	std::cout << "Loading " << fileName << std::endl;
	std::string dirName = fileName;
	if (dirName.rfind("/") != std::string::npos)
		dirName = dirName.substr(0, dirName.rfind("/"));
	if (dirName.rfind("\\") != std::string::npos)
		dirName = dirName.substr(0, dirName.rfind("\\"));
	if (fileName == dirName)
		dirName = "";


	std::ifstream pFile(fileName.c_str());

	if (!pFile.is_open())
	{
		std::cout << "Could not open file " << fileName << std::endl;
		return;
	}


	ObjGroup* currentGroup = new ObjGroup();
	currentGroup->materialIndex = -1;


	while (!pFile.eof())
	{
		std::string line;
		std::getline(pFile, line);
		line = cleanLine(line);
		if (line == "" || line[0] == '#') //skip empty or commented line
			continue;

		std::vector<std::string> params = split(line, " ");
		params[0] = toLower(params[0]);

		if (params[0] == "v")
			vertices.push_back(Vector3D((float)atof(params[1].c_str()), (float)atof(params[2].c_str()), (float)atof(params[3].c_str())));
		else if (params[0] == "vn")
			normals.push_back(Vector3D((float)atof(params[1].c_str()), (float)atof(params[2].c_str()), (float)atof(params[3].c_str())));
		else if (params[0] == "vt")
			texcoords.push_back(Vector3D((float)atof(params[1].c_str()), (float)atof(params[2].c_str()), 0));
		else if (params[0] == "f")
		{
			for (size_t ii = 4; ii <= params.size(); ii++)
			{
				Face face;

				for (size_t i = ii - 3; i < ii; i++)	//magische forlus om van quads triangles te maken ;)
				{
					Vertex vertex;
					std::vector<std::string> indices = split(params[i == (ii - 3) ? 1 : i], "/");
					if (indices.size() >= 1)	//er is een positie
						vertex.position = atoi(indices[0].c_str()) - 1;
					if (indices.size() == 2)		//alleen texture
						vertex.texcoord = atoi(indices[1].c_str()) - 1;
					if (indices.size() == 3)		//v/t/n of v//n
					{
						if (indices[1] != "")
							vertex.texcoord = atoi(indices[1].c_str()) - 1;
						vertex.normal = atoi(indices[2].c_str()) - 1;
					}
					face.vertices.push_back(vertex);
				}
				currentGroup->faces.push_back(face);
			}
		}
		else if (params[0] == "s")
		{//smoothing groups
		}
		else if (params[0] == "mtllib")
		{
			loadMaterialFile(dirName + "/" + params[1], dirName);
		}
		else if (params[0] == "usemtl")
		{
			if (currentGroup->faces.size() != 0)
				groups.push_back(currentGroup);
			currentGroup = new ObjGroup();
			currentGroup->materialIndex = -1;

			for (size_t i = 0; i < materials.size(); i++)
			{
				MaterialInfo* info = materials[i];
				if (info->name == params[1])
				{
					currentGroup->materialIndex = (int)i;
					break;
				}
			}
			if (currentGroup->materialIndex == -1)
				std::cout << "Could not find material name " << params[1] << std::endl;
		}
	}
	groups.push_back(currentGroup);

	float x = 20;
	float y = 10;
	float z = 20;

	auto firstVertex = groups[0]->faces.front().vertices.front();
	lowestX = vertices[firstVertex.position].x - fmod((vertices[firstVertex.position].x), 0.25);
	highestX = lowestX;
	lowestY = vertices[firstVertex.position].y - fmod((vertices[firstVertex.position].y), 0.25);
	highestY = lowestY;
	lowestZ = vertices[firstVertex.position].z - fmod((vertices[firstVertex.position].z), 0.25);
	highestZ = lowestZ;

	for (auto& group : groups) {
		for (auto& face : group->faces) {
			for (auto& vertex : face.vertices) {
				if (vertices[vertex.position].x - fmod((vertices[vertex.position].x), 0.25) < lowestX)
					lowestX =	vertices[vertex.position].x - fmod((vertices[vertex.position].x),0.25);
				if (vertices[vertex.position].x - fmod((vertices[vertex.position].x), 0.25) > highestX)
					highestX =	vertices[vertex.position].x - fmod((vertices[vertex.position].x), 0.25);
				if (vertices[vertex.position].y - fmod((vertices[vertex.position].y), 0.25) < lowestY)
					lowestY =	vertices[vertex.position].y - fmod((vertices[vertex.position].y), 0.25);
				if (vertices[vertex.position].y - fmod((vertices[vertex.position].y), 0.25) > highestY)
					highestY =	vertices[vertex.position].y - fmod((vertices[vertex.position].y), 0.25);
				if (vertices[vertex.position].z - fmod((vertices[vertex.position].z), 0.25) < lowestZ)
					lowestZ =	vertices[vertex.position].z - fmod((vertices[vertex.position].z), 0.25);
				if (vertices[vertex.position].z - fmod((vertices[vertex.position].z), 0.25) > highestZ)
					highestZ =	vertices[vertex.position].z - fmod((vertices[vertex.position].z), 0.25);
			}
		}
	}
}


ModelComponent::~ModelComponent(void)
{
}


void ModelComponent::draw()
{
	for (auto& group : groups) {
		materials[group->materialIndex]->texture->bindTexture();
		for (auto& face : group->faces) {
			glBegin(GL_TRIANGLES);
			for (auto& vertex : face.vertices) {
				Vector3D vectexture = texcoords[vertex.texcoord];
				if (normals.size() > vertex.normal) {
					Vector3D vecNorm = normals[vertex.normal];
				}
				glTexCoord2f(vectexture.x, vectexture.y);

				glVertex3f(vertices[vertex.position].x, vertices[vertex.position].y, vertices[vertex.position].z);

			}
			glEnd();
		}
	}

	glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	glBegin(GL_QUADS);
	glColor3f(255, 0, 0);
	glVertex3f(lowestX, lowestY, lowestZ);
	glVertex3f(lowestX, lowestY, highestZ);
	glVertex3f(lowestX, highestY, highestZ);
	glVertex3f(lowestX, highestY, lowestZ);

	glVertex3f(highestX, highestY, lowestZ);
	glVertex3f(highestX, lowestY, lowestZ);
	glVertex3f(lowestX, lowestY, lowestZ);
	glVertex3f(lowestX, highestY, lowestZ);

	glVertex3f(highestX, highestY, highestZ);
	glVertex3f(highestX, lowestY, highestZ);
	glVertex3f(highestX, lowestY, lowestZ);
	glVertex3f(highestX, highestY, lowestZ);

	glVertex3f(highestX, highestY, highestZ);
	glVertex3f(lowestX, highestY, highestZ);
	glVertex3f(lowestX, lowestY, highestZ);
	glVertex3f(highestX, lowestY, highestZ);
	glEnd();
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
}

bool ModelComponent::collision(Vec3f otherPosition) {
	if ((otherPosition.x >= lowestX * 0.1f + 20) && (otherPosition.x <= highestX * 0.1f + 20) && (otherPosition.y >= lowestY * 0.1f + 10) && (otherPosition.y <= highestY * 0.1f + 10) && (otherPosition.z >= lowestZ * 0.1f + 20) && (otherPosition.z <= highestZ * 0.1f + 20))
		return true;
	else
		return false;
}

void ModelComponent::loadMaterialFile(const std::string &fileName, const std::string &dirName)
{
	std::cout << "Loading " << fileName << std::endl;
	std::ifstream pFile(fileName.c_str());
	if (!pFile.is_open())
	{
		std::cout << "Could not open file " << fileName << std::endl;
		return;
	}

	MaterialInfo* currentMaterial = NULL;

	while (!pFile.eof())
	{
		std::string line;
		std::getline(pFile, line);
		line = cleanLine(line);
		if (line == "" || line[0] == '#')
			continue;

		std::vector<std::string> params = split(line, " ");
		params[0] = toLower(params[0]);

		if (params[0] == "newmtl")
		{
			if (currentMaterial != NULL)
			{
				materials.push_back(currentMaterial);
			}
			currentMaterial = new MaterialInfo();
			currentMaterial->name = params[1];
		}
		else if (params[0] == "map_kd")
		{
			std::string tex = params[1];
			if (tex.find("/"))
				tex = tex.substr(tex.rfind("/") + 1);
			if (tex.find("\\"))
				tex = tex.substr(tex.rfind("\\") + 1);
			//TODO
			currentMaterial->texture = new Texture(dirName + "/" + tex);
		}
		else if (params[0] == "kd")
		{//TODO, diffuse color
		}
		else if (params[0] == "ka")
		{//TODO, ambient color
		}
		else if (params[0] == "ks")
		{//TODO, specular color
		}
		else if (
			params[0] == "illum" ||
			params[0] == "map_bump" ||
			params[0] == "map_ke" ||
			params[0] == "map_ka" ||
			params[0] == "map_d" ||
			params[0] == "d" ||
			params[0] == "ke" ||
			params[0] == "ns" ||
			params[0] == "ni" ||
			params[0] == "td" ||
			params[0] == "tf" ||
			params[0] == "tr" ||
			false)
		{
			//these values are usually not used for rendering at this time, so ignore them
		}
		else
			std::cout << "Didn't parse " << params[0] << " in material file" << std::endl;
	}
	if (currentMaterial != NULL)
		materials.push_back(currentMaterial);

}

ModelComponent::MaterialInfo::MaterialInfo()
{
	texture = NULL;
}