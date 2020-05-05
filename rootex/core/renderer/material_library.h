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
	typedef HashMap<String, Pair<MaterialDefaultCreator, MaterialCreator>> MaterialDatabase;
	static MaterialDatabase s_MaterialDatabase;
public:
	static void LoadMaterials();
	static Ref<Material> GetMaterial(const String& materialName);
	static Ref<Material> GetDefaultMaterial();
#ifdef ROOTEX_EDITOR
	static MaterialMap& GetAllMaterials() { return s_Materials; };
	static MaterialDatabase& GetMaterialDatabase() { return s_MaterialDatabase; };
	static void SaveAll();
	static void CreateNewMaterialFile(const String& materialName, const String& materialType);

#endif // ROOTEX_EDITOR
};
