#include "material_library.h"

#include "core/resource_loader.h"

MaterialLibrary::MaterialMap MaterialLibrary::s_Materials;

MaterialLibrary::MaterialDatabase MaterialLibrary::s_MaterialDatabase = {
	{ "Color Material", { ColorMaterial::CreateDefault, ColorMaterial::Create } },
	{ "Textured Material", { TexturedMaterial::CreateDefault, TexturedMaterial::Create } }
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

			//#ifdef ROOTEX_EDITOR
			//			m_MaterialList.push_back(material.stem().string());
			//#endif // ROOTEX_EDITOR
		}
	}
	s_Materials["DEFAULT"] = { "Color Material", {} };
}

Ref<Material> MaterialLibrary::GetMaterial(String& materialName)
{
	if (s_Materials.find(materialName) == s_Materials.end())
	{
		WARN("Material file not found, returning stand-in Material");
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
	if (Ref<Material> lockedMaterial = s_Materials["DEFAULT"].second.lock())
	{
		return lockedMaterial;
	}
	else
	{
		Ref<Material> material(ColorMaterial::CreateDefault());
		material->setFileName(String("DEFAULT"));
		s_Materials["DEFAULT"].second = material;
		return material;
	}
}

#ifdef ROOTEX_EDITOR
MaterialLibrary::MaterialMap& MaterialLibrary::GetAllMaterials()
{
	return s_Materials;
}
void MaterialLibrary::SaveAll()
{
	for (auto& [materialName, materialInfo] : s_Materials)
	{
		if (Ref<Material> lockedMaterial = s_Materials["DEFAULT"].second.lock())
		{
			TextResourceFile* materialFile = ResourceLoader::CreateNewTextResourceFile("game/assets/materials/" + materialName);
			materialFile->putString(lockedMaterial->getJSON());
		}
	}
}

Ref<Material> MaterialLibrary::CreateNewMaterialFile(String& materialName, String& materialType)
{
	if (materialName == "DEFAULT")
	{
		return nullptr;
	}
	materialName += ".rmat";
	if (s_Materials.find(materialName) == s_Materials.end())
	{
		TextResourceFile* materialFile = ResourceLoader::CreateNewTextResourceFile("game/assets/materials/" + materialName);
		Ref<Material> material(s_MaterialDatabase[materialType].first());
		materialFile->putString(material->getJSON());
		s_Materials[materialName] = { materialType, material };
		//m_MaterialList.push_back(materialName);
		return Ref<Material>(material);
	}
	else
	{
		return nullptr;
	}
}

#endif // ROOTEX_EDITOR
