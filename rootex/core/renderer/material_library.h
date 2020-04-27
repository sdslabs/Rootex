#pragma once

#include "common/common.h"

#include "materials/color_material.h"
#include "materials/textured_material.h"

class MaterialLibrary
{
public:
	typedef HashMap<String, Pair<String, Weak<Material>>> MaterialMap;
private:
	static MaterialMap s_Materials;

	typedef Material* (*MaterialDefaultCreator)();
	typedef Material* (*MaterialCreator)(const JSON::json& materialDescription);
	typedef HashMap<String, std::pair<MaterialDefaultCreator, MaterialCreator>> MaterialDatabase;
	static MaterialDatabase s_MaterialDatabase;
public:
	static void LoadMaterials();
	static Ref<Material> GetMaterial(const String& materialName);
	static Ref<Material> GetDefaultMaterial();
#ifdef ROOTEX_EDITOR
	static MaterialMap& GetAllMaterials();
	void SaveAll();
	static Ref<Material> CreateNewMaterialFile(String& materialName, String& materialType);
	//static Vector<String> m_MaterialList;
#endif // ROOTEX_EDITOR
};
