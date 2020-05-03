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
			s_Materials[materialName] = { (String)materialJSON["type"] , nullptr};

//#ifdef ROOTEX_EDITOR
//			m_MaterialList.push_back(material.stem().string());
//#endif // ROOTEX_EDITOR
		}
	}
	s_Materials["DEFAULT"] = { "Color Material", nullptr };
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
		if (s_Materials[materialName].second != nullptr)
		{
			return s_Materials[materialName].second;
		}
		else
		{
			TextResourceFile* materialFile = ResourceLoader::CreateNewTextResourceFile("game/assets/materials/" + materialName);
			const JSON::json materialJSON = JSON::json::parse(materialFile->getString());
			Ref<Material> material(s_MaterialDatabase[materialJSON["type"]].second(materialJSON));
			material->setFileName(materialName);
			s_Materials[materialName].second = material;
			return Ref<Material>(material);
		}
	}
}

Ref<Material> MaterialLibrary::GetDefaultMaterial()
{
	if (s_Materials["DEFAULT"].second == nullptr)
	{
		Ref<Material> material(ColorMaterial::CreateDefault());
		material->setFileName(String("DEFAULT"));
		s_Materials["DEFAULT"].second = material;
	}
	return s_Materials["DEFAULT"].second;
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
		TextResourceFile* materialFile = ResourceLoader::CreateNewTextResourceFile("game/assets/materials/" + materialName);
		materialFile->putString(materialInfo.second->getJSON());
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
