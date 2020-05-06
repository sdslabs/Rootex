#include "material_library.h"

#include "core/resource_loader.h"

MaterialLibrary::MaterialMap MaterialLibrary::s_Materials;

const String ColorMaterial::s_MaterialName = "ColoredMaterial";
const String AnimationMaterial::s_MaterialName = "AnimationMaterial";
const String TexturedMaterial::s_MaterialName = "TexturedMaterial";

MaterialLibrary::MaterialDatabase MaterialLibrary::s_MaterialDatabase = {
	{ ColorMaterial::s_MaterialName, { ColorMaterial::CreateDefault, ColorMaterial::Create } },
	{ TexturedMaterial::s_MaterialName, { TexturedMaterial::CreateDefault, TexturedMaterial::Create } }
};

void MaterialLibrary::LoadMaterials()
{
	for (auto& material : OS::GetFilesInDirectory("game/assets/materials/"))
	{
		if (material.extension() == ".rmat")
		{
			String materialName = material.filename().string();
			TextResourceFile* materialFile = ResourceLoader::CreateNewTextResourceFile("game/assets/materials/" + materialName);
			const JSON::json materialJSON = JSON::json::parse(materialFile->getString());
			s_Materials[materialName] = { (String)materialJSON["type"], {} };
		}
	}
	s_Materials["Default"] = { ColorMaterial::s_MaterialName, {} };
}

Ref<Material> MaterialLibrary::GetMaterial(const String& materialName)
{
	if (s_Materials.find(materialName) == s_Materials.end())
	{
		WARN("Material file not found, returning stand-in Material for: " + materialName);
		return GetDefaultMaterial();
	}
	else
	{
		if (Ref<Material> lockedMaterial = s_Materials[materialName].second.lock())
		{
			return lockedMaterial;
		}
		else
		{
			TextResourceFile* materialFile = ResourceLoader::CreateNewTextResourceFile("game/assets/materials/" + materialName);
			const JSON::json materialJSON = JSON::json::parse(materialFile->getString());
			Ref<Material> material(s_MaterialDatabase[materialJSON["type"]].second(materialJSON));
			material->setFileName(materialName);
			s_Materials[materialName].second = material;
			return material;
		}
	}
}

Ref<Material> MaterialLibrary::GetDefaultMaterial()
{
	if (Ref<Material> lockedMaterial = s_Materials["Default"].second.lock())
	{
		return lockedMaterial;
	}
	else
	{
		Ref<Material> material(ColorMaterial::CreateDefault());
		material->setFileName("Default");
		s_Materials["Default"].second = material;
		return material;
	}
}

void MaterialLibrary::SaveAll()
{
	for (auto& [materialName, materialInfo] : s_Materials)
	{
		if (materialName == "Default")
		{
			continue;
		}
		if (Ref<Material> lockedMaterial = materialInfo.second.lock())
		{
			TextResourceFile* materialFile = ResourceLoader::CreateTextResourceFile("game/assets/materials/" + materialName);
			materialFile->putString(lockedMaterial->getJSON().dump(4));
			ResourceLoader::SaveResourceFile(materialFile);
		}
	}
}

#ifdef ROOTEX_EDITOR
MaterialLibrary::MaterialMap& MaterialLibrary::GetAllMaterials()
{
	return s_Materials;
}

void MaterialLibrary::CreateNewMaterialFile(const String& materialName, const String& materialType)
{
	if (materialName == "Default")
	{
		WARN("Cannot create another Default material");
		return;
	}
	String materialFileName = materialName + ".rmat";
	if (s_Materials.find(materialName) == s_Materials.end())
	{
		TextResourceFile* materialFile = ResourceLoader::CreateNewTextResourceFile("game/assets/materials/" + materialFileName);
		Ref<Material> material(s_MaterialDatabase[materialType].first());
		materialFile->putString(material->getJSON().dump(4));
		ResourceLoader::SaveResourceFile(materialFile);
		s_Materials[materialFileName] = { materialType, {} };
		PRINT("Created Material- " + materialFileName + " - " + materialType);
		return;
	}
	WARN("Material already exists- " + materialName);
}

#endif // ROOTEX_EDITOR
