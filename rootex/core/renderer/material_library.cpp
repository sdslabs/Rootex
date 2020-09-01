#include "material_library.h"

#include "core/resource_loader.h"

MaterialLibrary::MaterialMap MaterialLibrary::s_Materials;

MaterialLibrary::MaterialDatabase MaterialLibrary::s_MaterialDatabase = {
	{ BasicMaterial::s_MaterialName, { BasicMaterial::CreateDefault, BasicMaterial::Create } },
	{ SkyMaterial::s_MaterialName, { SkyMaterial::CreateDefault, SkyMaterial::Create } }
};

void MaterialLibrary::PopulateMaterials(const String& path)
{
	for (auto& materialFile : OS::GetAllInDirectory(path))
	{
		if (OS::IsFile(materialFile.generic_string()) && materialFile.extension() == ".rmat")
		{
			TextResourceFile* materialResourceFile = ResourceLoader::CreateTextResourceFile(materialFile.generic_string());
			const JSON::json& materialJSON = JSON::json::parse(materialResourceFile->getString());
			s_Materials[materialFile.generic_string()] = { (String)materialJSON["type"], {} };
		}
	}
}

void MaterialLibrary::LoadMaterials()
{
	PopulateMaterials("game/assets/");
	PopulateMaterials("rootex/assets/");
	s_Materials["DefaultMaterial"] = { BasicMaterial::s_MaterialName, {} };
}

Ref<Material> MaterialLibrary::GetMaterial(const String& materialPath)
{
	if (s_Materials.find(materialPath) == s_Materials.end())
	{
		WARN("Material file not found, returning default material instead of: " + materialPath);
		return GetDefaultMaterial();
	}
	else
	{
		if (Ref<Material> lockedMaterial = s_Materials[materialPath].second.lock())
		{
			return lockedMaterial;
		}
		else
		{
			TextResourceFile* materialFile = ResourceLoader::CreateTextResourceFile(materialPath);
			const JSON::json materialJSON = JSON::json::parse(materialFile->getString());
			Ref<Material> material(s_MaterialDatabase[materialJSON["type"]].second(materialJSON));
			material->setFileName(materialPath);
			s_Materials[materialPath].second = material;
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
	for (auto& [materialPath, materialInfo] : s_Materials)
	{
		if (materialPath == "DefaultMaterial")
		{
			continue;
		}
		if (Ref<Material> lockedMaterial = materialInfo.second.lock())
		{
			TextResourceFile* materialFile = ResourceLoader::CreateNewTextResourceFile(materialPath);
			materialFile->putString(lockedMaterial->getJSON().dump(4));
			ResourceLoader::SaveResourceFile(materialFile);
		}
	}
}

void MaterialLibrary::CreateNewMaterialFile(const String& materialPath, const String& materialType)
{
	if (materialPath == "DefaultMaterial")
	{
		return;
	}

	if (s_Materials.find(materialPath) == s_Materials.end())
	{
		if (!OS::IsExists(materialPath))
		{
			TextResourceFile* materialFile = ResourceLoader::CreateNewTextResourceFile(materialPath);
			Ref<Material> material(s_MaterialDatabase[materialType].first());
			materialFile->putString(material->getJSON().dump(4));
			ResourceLoader::SaveResourceFile(materialFile);
			s_Materials[materialPath] = { materialType, {} };
			PRINT("Created material: " + materialPath + " of type " + materialType);
			
			return;
		}
	}
}

bool MaterialLibrary::IsExists(const String& materialPath)
{
	return OS::IsExists(materialPath);
}
