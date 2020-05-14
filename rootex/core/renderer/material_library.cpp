#include "material_library.h"

#include "core/resource_loader.h"

MaterialLibrary::MaterialMap MaterialLibrary::s_Materials;

//const String ColorMaterial::s_MaterialName = "ColoredMaterial";
const String BasicMaterial::s_MaterialName = "BasicMaterial";

MaterialLibrary::MaterialDatabase MaterialLibrary::s_MaterialDatabase = {
	//{ ColorMaterial::s_MaterialName, { ColorMaterial::CreateDefault, ColorMaterial::Create } },
	{ BasicMaterial::s_MaterialName, { BasicMaterial::CreateDefault, BasicMaterial::Create } }
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
	s_Materials["Default"] = { BasicMaterial::s_MaterialName, {} };
}

Ref<Material> MaterialLibrary::GetMaterial(const String& materialName)
{
	if (s_Materials.find(materialName) == s_Materials.end())
	{
		WARN("Material file not found, returning stand-in Material instead of: " + materialName);
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
			TextResourceFile* materialFile = ResourceLoader::CreateTextResourceFile("game/assets/materials/" + materialName);
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
	if (Ref<Material> lockedMaterial = s_Materials["DefaultMaterial"].second.lock())
	{
		return lockedMaterial;
	}
	else
	{
		Ref<Material> material(BasicMaterial::CreateDefault());
		material->setFileName("DefaultMaterial");
		s_Materials["DefaultMaterial"].second = material;
		return material;
	}
}

void MaterialLibrary::SaveAll()
{
	for (auto& [materialName, materialInfo] : s_Materials)
	{
		if (materialName == "DefaultMaterial")
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

void MaterialLibrary::CreateNewMaterialFile(const String& materialName, const String& materialType)
{
	if (materialName == "DefaultMaterial")
	{
		WARN("Cannot create another DefaultMaterial");
		return;
	}
	String materialFileName = materialName + ".rmat";
	if (s_Materials.find(materialName) == s_Materials.end())
	{
		if (!OS::IsExists("game/assets/materials/" + materialFileName))
		{
			TextResourceFile* materialFile = ResourceLoader::CreateNewTextResourceFile("game/assets/materials/" + materialFileName);
			Ref<Material> material(s_MaterialDatabase[materialType].first());
			materialFile->putString(material->getJSON().dump(4));
			ResourceLoader::SaveResourceFile(materialFile);
			s_Materials[materialFileName] = { materialType, {} };
			PRINT("Created Material- " + materialFileName + " - " + materialType);
			return;
		}
	}
	WARN("Material already exists- " + materialName);
}

#endif // ROOTEX_EDITOR
