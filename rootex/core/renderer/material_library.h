#pragma once

#include "common/common.h"

#include "materials/basic_material.h"
#include "materials/sky_material.h"

class MaterialLibrary
{
public:
	typedef HashMap<String, Pair<String, Weak<Material>>> MaterialMap;

private:
	typedef Material* (*MaterialDefaultCreator)();
	typedef Material* (*MaterialCreator)(const JSON::json& materialDescription);
	typedef HashMap<String, Pair<MaterialDefaultCreator, MaterialCreator>> MaterialDatabase;

	static MaterialMap s_Materials;
	static MaterialDatabase s_MaterialDatabase;
	static void PopulateMaterials(const String& path);

public:
	static const String s_DefaultMaterialPath;
	
	static void SaveAll();
	static void LoadMaterials();
	static void CreateNewMaterialFile(const String& materialPath, const String& materialType);
	static bool IsExists(const String& materialPath);

	static Ref<Material> GetMaterial(const String& materialPath);
	static Ref<Material> GetDefaultMaterial();
	static MaterialMap& GetAllMaterials() { return s_Materials; };
	static MaterialDatabase& GetMaterialDatabase() { return s_MaterialDatabase; };
};

void PopulateMaterials();
