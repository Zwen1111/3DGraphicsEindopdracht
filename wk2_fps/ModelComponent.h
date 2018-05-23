//#pragma once
//#include "DrawComponent.h"
//#include <string>
//#include <vector>
//#include <list>
//#include "vector3d.h"
//#include "stb_image.h"
//
//class Texture {
//public:
//	int width, height, bpp;
//	std::string location;
//	stbi_uc* imgData;
//	Texture(std::string location);
//	void bindTexture();
//	void reloadTexture();
//};
//
//class model_component : public draw_component {
//private:
//	class Vertex
//	{
//	public:
//		int position;
//		int normal;
//		int texcoord;
//	};
//
//	class Face
//	{
//	public:
//		std::list<Vertex> vertices;
//	};
//	class MaterialInfo
//	{
//	public:
//		MaterialInfo();
//		std::string name;
//		Texture* texture;
//	};
//
//	class ObjGroup
//	{
//	public:
//		std::string name;
//		int materialIndex;
//		std::list<Face> faces;
//	};
//
//
//	std::vector<Vector3D>	vertices;
//	std::vector<Vector3D>	normals;
//	std::vector<Vector3D>	texcoords;
//	std::vector<ObjGroup*> groups;
//	std::vector<MaterialInfo*> materials;
//
//	void loadMaterialFile(const std::string &fileName, const std::string &dirName);
//public:
//	model_component(const std::string &filename);
//	~model_component(void);
//
//	void draw() override;
//};