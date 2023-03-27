#include "audio_bus.h"
#include "audio_listener_component.h"
#include "audio_component.h"
#include "core/audio/audio_source.h"
#include "music_component.h"

Vector<String> AudioBusCollection::all_buses = { "Speakers" };
Vector<String> AudioBusCollection::all_buses_parent = { "Master" };
Vector<float> AudioBusCollection::all_buses_volume = { 100.0 };

AudioBusCollection::AudioBusCollection(Entity& owner, const JSON::json& data)
    : Component(owner)
{
	//all_buses_parent.push_back("Speakers");
	//all_buses.push_back("Master");
	//all_buses_volume.push_back(100.0);
	count = 1;
}

void AudioBusCollection::to_json(JSON::json& j, const AudioBusCollection& a)
{
	//write the code
	j["audioBusName"] = JSON::json::array();
	for (auto bus : a.all_buses)
	{
		j["audioBusName"].push_back(bus);
	}
	j["parentAudioBusName"] = JSON::json::array();
	for (auto bus : a.all_buses_parent)
	{
		j["parentAudioBusName"].push_back(bus);
	}
	j["audioBusVolume"] = JSON::json::array();
	for (auto vol : a.all_buses_volume)
	{
		j["audioBusVolume"].push_back(vol);
	}
}

void AudioBusCollection::from_json(const JSON::json& j, AudioBusCollection& a)
{
	//for (auto& index : j.value("pixelShaderTexturesMapping", Vector<int>()))
	//{
	//	s.pixelShaderTexturesMapping.push_back(index);
	//}
	for (auto& bus : j.value("audioBusName", Vector<String>()))
	{
		a.all_buses.push_back(bus);
	}
	for (auto& parent : j.value("parentAudioBusName", Vector<String>()))
	{
		a.all_buses_parent.push_back(parent);
	}
	for (auto& volume : j.value("audioBusVolume", Vector<float>()))
	{
		a.all_buses_volume.push_back(volume);
	}
}

void AudioBusCollection::volumeChange(String name, float volume)
{
	int j = getIndexByName(name);
	float val = all_buses_volume[j];
	all_buses_volume[j] = volume;
	float ratio = volume/val;
	float temp;
	String parent = all_buses_parent[j];
	int y = all_buses_parent.size();
	ALfloat finvol;
	auto itr = MusicComponent::music_comp_audio_bus.begin();
	auto itr2 = MusicComponent::music_comp_volume.begin();
	while (itr != MusicComponent::music_comp_audio_bus.end())
	{
		if (itr->second == name)
		{
			finvol = itr2->second;
			itr2->second = finvol * ratio;
			AL_CHECK(alSourcef(itr->first, AL_GAIN, finvol * ratio));
		}
		itr++;
		itr2++;
	}
	for ( int i = 0; i < all_buses_parent.size(); i++)
	{
		if( all_buses_parent[i] == name ) 
		{
			temp = ratio*(all_buses_volume[i]);
			volumeChange(all_buses[i],temp);
		}
	}
}

void AudioBusCollection::removeAudioBus(String name)
{
	int j = getIndexByName(name);
	String parent = all_buses_parent[j]; //can't remove without setting the parent
	for ( int i = 0; i < all_buses_parent.size(); i++)
	{
		if(all_buses_parent[i]==name) 
		{
			all_buses_parent[i] = parent;
		}
	}
	int t = j;
	auto it = all_buses.begin();
	while(t--)
		it++;
	all_buses.erase(it);
	it = all_buses_parent.begin();
	t = j;
	while (t--)
		it++; 
	all_buses_parent.erase(it);
	auto it2 = all_buses_volume.begin();
	t = j;
	while (t--)
		it2++;
	all_buses_volume.erase(it2);
	auto itr = MusicComponent::music_comp_audio_bus.begin();
	while (itr != MusicComponent::music_comp_audio_bus.end())
	{
		if (itr->second == name)
		{
			itr->second = parent;
		}
        itr++;
	}
}

void AudioBusCollection::setAudioBusName(int ind, String name)
{
	if(ind<all_buses.size()) all_buses[ind] = name;
	else all_buses.push_back(name);
}

void AudioBusCollection::setAudioParentBusName(int ind, String name)
{
	if (ind < all_buses_parent.size())
		all_buses_parent[ind] = name;
	else
		all_buses_parent.push_back(name);
}

void AudioBusCollection::setAudioBusVolume(int ind, float volume)
{
	if (ind < all_buses_volume.size())
		all_buses_volume[ind] = volume;
	else
		all_buses_volume.push_back(volume);
}

int AudioBusCollection::getIndexByName(String name)
{
	for (int i = 0; i < all_buses.size(); i++)
	{
		if(name==all_buses[i]) return i;
	}
	return -1;
}

void AudioBusCollection::draw()
{
	// master bus
	all_buses_parent[0]="Speakers";
	all_buses[0]="Master";
	all_buses_volume[0]=100.0;
	for (int i = 1; i < count; i++)
	{
		//if (i == 0)
			//continue;
		ImGui::PushID(i);
		String name = "temp";
		String par = "Master";
		float volume = 100;
		String s1 = "Name" + std::to_string(i);
		String s2 = "Par" + std::to_string(i);
		String filepath = "";
		int par_id = 0;
		bool bl = false,bl2=true,bl3=false;
		if(i>=all_buses.size()) bl2=false;
		if (bl2)
		{
			name = all_buses[i];
			volume = all_buses_volume[i];
			par = all_buses_parent[i];
			par_id = getIndexByName(par);
		}
		setAudioBusName(i, name);
		setAudioParentBusName(i, par);
		if (!bl2)
			all_buses_volume.push_back(volume);
		if (ImGui::InputTextWithHint(s1.c_str(), "Name of audio bus",&name, ImGuiInputTextFlags_EnterReturnsTrue))
		{
			setAudioBusName(i, name);
		}
		//only for debugging
		if (ImGui::ListBoxHeader("Set Parent of Audio Bus"))
		{
			for (int j = 0; j < all_buses.size(); j++)
			{
				if (i == j)
					continue;
				if (ImGui::RadioButton(all_buses[j].c_str(), bl))
				{
					{
						setAudioParentBusName(i, all_buses[j]);
					}
				}
			}
			ImGui::ListBoxFooter();
		}
		/*if (ImGui::InputTextWithHint(s2.c_str(), "Set Parent of Audio Bus", &par, ImGuiInputTextFlags_EnterReturnsTrue))
		//{
			//bl3 = true;
			//for (int j = 0; j < count; j++)
		{
				if (i == j)
					continue;
				if (ImGui::RadioButton(all_buses[j].c_str(), bl))
				{
					{
						setAudioParentBusName(i, all_buses[j]);
					}
				}
			}
		}*/
		//while (bl3)
		//{
			//bl3 = !bl;
		//}
		if (ImGui::DragFloat("Volume", &volume, 0.01f, 0.001f, FLT_MAX))
		{
			if (bl2)
				volumeChange(name, volume);
			else
				setAudioBusVolume(i, volume);
			//all_buses_volume.push_back(volume);
		}
		if (ImGui::Button("Remove Audio Bus"))
		{
			removeAudioBus(name);
			count--;
		}
		ImGui::PopID();
	}

	if (ImGui::Button("Add Audio Bus"))
	{
		count++;
	}
}

float AudioSourceChild::getDuration() const
{
	//
	return 1.0;
}

/*
#include "audio_bus.h"
#include "audio_listener_component.h"

static int count = 0;

void AudioBusCollection::to_json(JSON::json& j, const AudioBusCollection& a)
{
	//write the code
	j["audioBusName"] = JSON::json::array();
	for (auto& bus : a.all_buses)
	{
		j["audioBusName"].push_back(bus);
	}
	j["parentAudioBusName"] = JSON::json::array();
	for (auto& bus : a.all_buses_parent)
	{
		j["parentAudioBusName"].push_back(bus);
	}
	j["audioBusVolume"] = JSON::json::array();
	for (auto& vol : a.all_buses_volume)
	{
		j["audioBusVolume"].push_back(vol);
	}
}

void AudioBusCollection::from_json(JSON::json& j, const AudioBusCollection& a)
{
	//write the code
	
	for (auto& index : j.value("pixelShaderTexturesMapping", Vector<int>()))
	{
		s.pixelShaderTexturesMapping.push_back(index);
	}
}

/*
void to_json(JSON::json& j, const CustomMaterialData& s)
{
	j["pixelShader"] = s.pixelShaderPath;
	j["pixelShaderCount"] = s.pixelShaderCount;
	j["vertexShader"] = s.vertexShaderPath;
	j["vertexShaderTextures"] = JSON::json::array();
	for (auto& texture : s.vertexShaderTextures)
	{
		j["vertexShaderTextures"].push_back(texture->getPath().generic_string());
	}
	j["pixelShaderTextures"] = JSON::json::array();
	j["pixelShaderTexturesMapping"] = JSON::json::array();
	for (auto& texture : s.pixelShaderTextures)
	{
		j["pixelShaderTextures"].push_back(texture->getPath().generic_string());
	}
	for (auto& index : s.pixelShaderTexturesMapping)
	{
		j["pixelShaderTexturesMapping"].push_back(index);
	}
	for (auto& customConstantBuffers : s.customConstantBuffers)
	{
		j["customConstantBuffers"].push_back(customConstantBuffers);
	}
	for (auto& typeOfCustomConstantBuffers : s.typeOfCustomConstantBuffers)
	{
		j["typeOfCustomConstantBuffers"].push_back(typeOfCustomConstantBuffers);
	}
	j["vertexShaderPath"] = s.dummyVertexShaderPath;
}

void from_json(const JSON::json& j, CustomMaterialData& s)
{
	s.vertexShaderPath = j.value("vertexShader", CustomMaterialResourceFile::s_DefaultCustomVSPath);
	s.pixelShaderPath = j.value("pixelShader", CustomMaterialResourceFile::s_DefaultCustomPSPath);
	s.pixelShaderCount = j.value("pixelShaderCount", 1);
	s.dummyVertexShaderPath = j.value("vertexShaderPath", CustomMaterialResourceFile::s_DefaultCustomVSPath);
	for (auto& texturePath : j.value("vertexShaderTextures", Vector<String>()))
	{
		if (Ref<ImageResourceFile> texture = ResourceLoader::CreateImageResourceFile(texturePath))
		{
			s.vertexShaderTextures.push_back(texture);
		}
	}
	for (auto& texturePath : j.value("pixelShaderTextures", Vector<String>({ "rootex/assets/rootex.png" })))
	{
		if (Ref<ImageResourceFile> texture = ResourceLoader::CreateImageResourceFile(texturePath))
		{
			s.pixelShaderTextures.push_back(texture);
		}
	}
	for (auto& index : j.value("pixelShaderTexturesMapping", Vector<int>()))
	{
		s.pixelShaderTexturesMapping.push_back(index);
	}
	for (auto& customConstantBuffers : j.value("customConstantBuffers", Vector<float>()))
	{
		s.customConstantBuffers.push_back(customConstantBuffers);
	}
	for (auto& typeOfCustomConstantBuffers : j.value("typeOfCustomConstantBuffers", Vector<TYPES_OF_BUFFERS>()))
	{
		s.typeOfCustomConstantBuffers.push_back(typeOfCustomConstantBuffers);
	}
}

AudioBus* AudioBusCollection::getAudioBusByName(String name, AudioBus* master)
{
	AudioBus* ans = nullptr;
	for (auto& child : master->children)
	{
		if (child->name == name)
			return child;
		else
			ans = (getAudioBusByName(name, child) == nullptr) ? ans : getAudioBusByName(name, child);
	}
	return ans;
}

int AudioBusCollection::getIndexByName(String name)
{
	
	for ( int i = 0; i < all_buses.size(); i++)
	{
		if(all_buses[i]==bus.name) break;
	}
	return i;
}

AudioBus* AudioBusCollection::addAudioBus(String name, bool isMaster, float volume, AudioBus* parentBus)
{
	AudioBus* newBus;
	newBus->isMaster = isMaster;
	newBus->volume = volume;
	all_buses_volume.push_back(volume);
	newBus->parent = parentBus;
	newBus->name = name;
	parentBus->children.push_back(newBus);
	all_buses.push_back(name);
	all_buses_parent.push_back(parentBus.name);
	return newBus;
}

void AudioBusCollection::volumeChange(AudioBus* bus, float val)
{
	if (val > 100)
	{
		// error
	}
	else
	{
		int i = getIndexByName(bus.name);
		float t = val / (bus->volume);
		bus->volume = val;
		all_buses_volume[i] = val;
		float temp;
		for (auto& child : bus->children)
		{
			temp = t * (child->volume);
			volumeChange(child, temp);
		}
	}
}

void AudioBusCollection::setParentOfAudioBus(AudioBus* bus, AudioBus* parentBus)
{
	bus->parent = parentBus;
	int i = getIndexByName(bus);
	all_buses_parent[i] = parentBus.name;
}

void AudioBusCollection::removeAudioBus(AudioBus* bus)
{
	for (auto& child : bus->children)
	{
		child->parent = bus->parent;
	}
	bus->parent = nullptr;
}

void AudioBusCollection::draw()
{
	// master bus
	AudioBus master;
	master.name = "Master";
	master.isMaster = true;
	master.volume = 50.0;
	// master.parent = "Speakers";
	all_buses.push_back("Master");
	for (int i = 0; i < count; i++)
	{
		AudioBus *currentBus, *parentBus;
		AudioBus* bus;
		String name, *parentname;
		float volume = 100;
		bool bl = false;
		if (ImGui::DragFloat("Volume", &volume, 0.01f, 0.001f, FLT_MAX))
		{
			;
		}
		if (ImGui::InputTextWithHint("Name", "Name of audio bus", &name, ImGuiInputTextFlags_EnterReturnsTrue))
		{
			for (int j = 0; j <= count; j++)
			{
				if (i == j)
					continue;
				// if (ImGui::RadioButton())
				parentname = &all_buses[j];
				if (ImGui::RadioButton("", bl))
				{
					{
						parentBus = getAudioBusByName(all_buses[j], &master);
						bus = addAudioBus(name, false, volume, parentBus);
					}
				}
			}
		}
		currentBus = getAudioBusByName(name, &master);
		volumeChange(currentBus, volume);
		setParentOfAudioBus(currentBus, parentBus);
		if (ImGui::Button("Remove Audio Bus"))
		{
			removeAudioBus(currentBus);
			count--;
		}
	}

	if (ImGui::Button("Add Audio Bus"))
	{
		count++;
	}
}

*/
