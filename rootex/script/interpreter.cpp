#include "interpreter.h"
#include "core/resource_loader.h"

#include "common/common.h"
#include "components/transform_component.h"
#include "entity_factory.h"
#include "script/interpreter.h"

void LuaInterpreter::registerTypes()
{
	sol::table rootex = m_Lua.create_named_table("Rootex");
	{
		sol::usertype<Vector2> vector2 = rootex.new_usertype<Vector2>("Vector2", sol::constructors<Vector2(), Vector2(float, float)>());
		vector2["x"] = &Vector2::x;
		vector2["y"] = &Vector2::y;
	}
	{
		sol::usertype<Vector3> vector3 = rootex.new_usertype<Vector3>("Vector3", sol::constructors<Vector3(), Vector3(float, float, float)>());
		vector3["x"] = &Vector3::x;
		vector3["y"] = &Vector3::y;
		vector3["z"] = &Vector3::z;
	}
	{
		sol::usertype<Vector4> vector4 = rootex.new_usertype<Vector4>("Vector4", sol::constructors<Vector4(), Vector4(float, float, float, float)>());
		vector4["x"] = &Vector4::x;
		vector4["y"] = &Vector4::y;
		vector4["z"] = &Vector4::z;
		vector4["w"] = &Vector4::w;
	}
	{
		sol::usertype<Color> color = rootex.new_usertype<Color>("Color", sol::constructors<Color(), Color(float, float, float, float)>());
		color["x"] = &Vector4::x;
		color["y"] = &Vector4::y;
		color["z"] = &Vector4::z;
		color["w"] = &Vector4::w;
	}
	{
		sol::usertype<Quaternion> quaternion = rootex.new_usertype<Quaternion>("Quaternion", sol::constructors<Quaternion(), Quaternion(float, float, float, float)>());
		quaternion["x"] = &Quaternion::x;
		quaternion["y"] = &Quaternion::y;
		quaternion["z"] = &Quaternion::z;
		quaternion["w"] = &Quaternion::w;
	}
	{
		sol::usertype<Matrix> matrix = rootex.new_usertype<Matrix>("Matrix", sol::constructors<Matrix()>());
		matrix["Identity"] = sol::var(Matrix::Identity);
	}
	{
		sol::usertype<ResourceLoader> resourceLoader = rootex.new_usertype<ResourceLoader>("ResourceLoader");
		resourceLoader["CreateAudio"] = &ResourceLoader::CreateAudioResourceFile;
		resourceLoader["CreateFont"] = &ResourceLoader::CreateFontResourceFile;
		resourceLoader["CreateImage"] = &ResourceLoader::CreateImageResourceFile;
		resourceLoader["CreateLua"] = &ResourceLoader::CreateLuaTextResourceFile;
		resourceLoader["CreateText"] = &ResourceLoader::CreateTextResourceFile;
		resourceLoader["CreateNewText"] = &ResourceLoader::CreateNewTextResourceFile;
		resourceLoader["CreateVisualModel"] = &ResourceLoader::CreateVisualModelResourceFile;
	}
	{
		sol::usertype<ResourceFile> resourceFile = rootex.new_usertype<ResourceFile>("ResourceFile");
		resourceFile["isValid"] = &ResourceFile::isValid;
		resourceFile["isDirty"] = &ResourceFile::isDirty;
		resourceFile["isOpen"] = &ResourceFile::isOpen;
		resourceFile["reload"] = &ResourceFile::reload;
		resourceFile["getPath"] = [](ResourceFile& f) { return f.getPath().string(); };
		resourceFile["getType"] = &ResourceFile::getType;
	}
	{
		sol::usertype<TextResourceFile> textResourceFile = rootex.new_usertype<TextResourceFile>(
		    "TextResourceFile",
		    sol::base_classes, sol::bases<ResourceFile>());
		textResourceFile["getString"] = &TextResourceFile::getString;
	}
	{
		sol::usertype<LuaTextResourceFile> luaTextResourceFile = rootex.new_usertype<LuaTextResourceFile>(
		    "LuaTextResourceFile",
		    sol::base_classes, sol::bases<ResourceFile, TextResourceFile>());
	}
	{
		sol::usertype<AudioResourceFile> audioResourceFile = rootex.new_usertype<AudioResourceFile>(
		    "AudioResourceFile",
		    sol::base_classes, sol::bases<ResourceFile>());
		audioResourceFile["getAudioDataSize"] = &AudioResourceFile::getAudioDataSize;
		audioResourceFile["getFormat"] = &AudioResourceFile::getFormat;
		audioResourceFile["getFrequency"] = &AudioResourceFile::getFrequency;
		audioResourceFile["getBitDepth"] = &AudioResourceFile::getBitDepth;
		audioResourceFile["getChannels"] = &AudioResourceFile::getChannels;
		audioResourceFile["getDuration"] = &AudioResourceFile::getDuration;
	}
	{
		sol::usertype<VisualModelResourceFile> visualModelResourceFile = rootex.new_usertype<VisualModelResourceFile>(
		    "VisualModelResourceFile",
		    sol::base_classes, sol::bases<ResourceFile>());
	}
	{
		sol::usertype<ImageResourceFile> imageResourceFile = rootex.new_usertype<ImageResourceFile>(
		    "ImageResourceFile",
		    sol::base_classes, sol::bases<ResourceFile>());
	}
	{
		sol::usertype<FontResourceFile> fontResourceFile = rootex.new_usertype<FontResourceFile>(
		    "FontResourceFile",
		    sol::base_classes, sol::bases<ResourceFile>());
		fontResourceFile["getFontName"] = &FontResourceFile::getFontName;
	}
	{
		sol::usertype<EntityFactory> entityFactory = rootex.new_usertype<EntityFactory>("EntityFactory");
		entityFactory["Get"] = &EntityFactory::GetSingleton;
		entityFactory["create"] = &EntityFactory::createEntity;
		entityFactory["find"] = &EntityFactory::findEntity;
	}
	{
		sol::usertype<Entity> entity = rootex.new_usertype<Entity>("Entity");
		entity["addChild"] = &Entity::addChild;
		entity["removeComponent"] = &Entity::removeComponent;
		entity["destroy"] = &Entity::destroy;
		entity["hasComponent"] = &Entity::hasComponent;
		entity["getID"] = &Entity::getID;
		entity["getName"] = &Entity::getName;
		entity["setName"] = &Entity::setName;

		sol::usertype<Component> component = rootex.new_usertype<Component>("Component");
		component["getOwner"] = &Component::getOwner;
		component["getComponentID"] = &Component::getComponentID;
		component["getName"] = &Component::getName;

		sol::usertype<TransformComponent> transformComponent = rootex.new_usertype<TransformComponent>(
		    "TransformComponent",
		    sol::base_classes, sol::bases<Component>());
		entity["getTransform"] = [](Entity* e) { return e->getComponent<TransformComponent>(); };
		transformComponent["setPosition"] = &TransformComponent::setPosition;
		transformComponent["setRotation"] = &TransformComponent::setRotation;
		transformComponent["setScale"] = &TransformComponent::setScale;
		transformComponent["setTransform"] = &TransformComponent::setTransform;
		transformComponent["addTransform"] = &TransformComponent::addTransform;
		transformComponent["getPosition"] = &TransformComponent::getPosition;
		transformComponent["getRotation"] = &TransformComponent::getRotation;
		transformComponent["getScale"] = &TransformComponent::getScale;
		transformComponent["getLocalTransform"] = &TransformComponent::getLocalTransform;
		transformComponent["getAbsoluteTransform"] = &TransformComponent::getAbsoluteTransform;
		transformComponent["getComponentID"] = &TransformComponent::getComponentID;
		transformComponent["getName"] = &TransformComponent::getName;
	}
}

LuaInterpreter::LuaInterpreter()
{
	m_Lua.open_libraries(sol::lib::base);
	m_Lua.open_libraries(sol::lib::io);
	m_Lua.open_libraries(sol::lib::math);
	m_Lua.open_libraries(sol::lib::os);
	m_Lua.open_libraries(sol::lib::string);
	m_Lua.open_libraries(sol::lib::table);

	registerTypes();
}

LuaInterpreter* LuaInterpreter::GetSingleton()
{
	static LuaInterpreter singleton;
	return &singleton;
}
