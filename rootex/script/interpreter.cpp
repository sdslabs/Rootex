#include "interpreter.h"
#include "core/resource_loader.h"

#include "common/common.h"

#include "scene.h"
#include "scene_loader.h"
#include "ecs_factory.h"
#include "script.h"
#include "components/transform_component.h"
#include "components/visual/text_ui_component.h"
#include "components/visual/ui_component.h"
#include "components/visual/model_component.h"
#include "components/visual/animated_model_component.h"
#include "components/physics/box_collider_component.h"
#include "components/physics/capsule_collider_component.h"
#include "components/physics/sphere_collider_component.h"
#include "components/visual/ui_component.h"
#include "systems/input_system.h"
#include "core/resource_files/audio_resource_file.h"
#include "core/resource_files/font_resource_file.h"
#include "core/resource_files/image_resource_file.h"
#include "core/resource_files/image_cube_resource_file.h"
#include "core/resource_files/lua_text_resource_file.h"
#include "core/resource_files/model_resource_file.h"
#include "core/resource_files/collision_model_resource_file.h"
#include "core/resource_files/text_resource_file.h"
#include "event_manager.h"

extern "C" int luaopen_lpeg(lua_State* L);

int HandleLuaException(lua_State* L, sol::optional<const std::exception&> maybeException, sol::string_view description)
{
	PRINT("An exception occurred in Lua function:")
	if (maybeException)
	{
		const std::exception& ex = *maybeException;
		WARN(ex.what());
	}
	else
	{
		PRINT(description.data());
	}
	return sol::stack::push(L, description);
}

LuaInterpreter::LuaInterpreter()
{
	m_Lua.set_exception_handler(&HandleLuaException);
	m_Lua.open_libraries(sol::lib::base);
	m_Lua.open_libraries(sol::lib::io);
	m_Lua.open_libraries(sol::lib::math);
	m_Lua.open_libraries(sol::lib::os);
	m_Lua.open_libraries(sol::lib::string);
	m_Lua.open_libraries(sol::lib::table);
	m_Lua.open_libraries(sol::lib::coroutine);
	m_Lua.open_libraries(sol::lib::package);
	m_Lua.open_libraries(sol::lib::debug);

	luaL_requiref(m_Lua.lua_state(), "lpeg", luaopen_lpeg, 1);
	lua_pop(m_Lua.lua_state(), 1);

	registerTypes();
	runScripts();
}

LuaInterpreter* LuaInterpreter::GetSingleton()
{
	static LuaInterpreter singleton;
	return &singleton;
}

void LuaInterpreter::runScripts()
{
	static bool called = false;
	if (called)
	{
		return;
	}
	called = true;

	sol::table middleclass = m_Lua.require_file("class", "rootex/vendor/Middleclass/middleclass.lua");
	sol::table dbg = m_Lua.require_file("dbg", "rootex/vendor/Debugger/debugger.lua");
	dbg["auto_where"] = 2;

	m_Lua.do_string("package.path = package.path .. ';rootex/vendor/Narrator/?.lua'");
}

void LuaInterpreter::registerTypes()
{
	sol::table& rootex = m_Lua.create_named_table("RTX");
	{
		sol::usertype<Vector2> vector2 = rootex.new_usertype<Vector2>(
		    "Vector2",
		    sol::constructors<Vector2(), Vector2(float, float)>(),
		    sol::meta_function::addition, [](Vector2& l, Vector2& r) { return l + r; },
		    sol::meta_function::subtraction, [](Vector2& l, Vector2& r) { return l - r; });
		vector2["dot"] = &Vector2::Dot;
		vector2["cross"] = [](const Vector2& l, const Vector2& r) { return l.Cross(r); };
		vector2["x"] = &Vector2::x;
		vector2["y"] = &Vector2::y;
	}
	{
		sol::usertype<Vector3> vector3 = rootex.new_usertype<Vector3>(
		    "Vector3",
		    sol::constructors<Vector3(), Vector3(float, float, float)>(),
		    sol::meta_function::addition, [](Vector3& l, Vector3& r) { return l + r; },
		    sol::meta_function::subtraction, [](Vector3& l, Vector3& r) { return l - r; });
		vector3["dot"] = &Vector3::Dot;
		vector3["cross"] = [](const Vector3& l, const Vector3& r) { return l.Cross(r); };
		vector3["x"] = &Vector3::x;
		vector3["y"] = &Vector3::y;
		vector3["z"] = &Vector3::z;
	}
	{
		sol::usertype<Vector4> vector4 = rootex.new_usertype<Vector4>(
		    "Vector4", sol::constructors<Vector4(), Vector4(float, float, float, float)>(),
		    sol::meta_function::addition, [](Vector4& l, Vector4& r) { return l + r; },
		    sol::meta_function::subtraction, [](Vector4& l, Vector4& r) { return l - r; });
		vector4["dot"] = &Vector4::Dot;
		vector4["cross"] = [](const Vector4& l, const Vector4& r) { return l.Cross(l, r); };
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
		sol::usertype<Matrix> matrix = rootex.new_usertype<Matrix>(
		    "Matrix",
		    sol::constructors<Matrix()>(),
		    sol::meta_function::addition, [](Matrix& l, Matrix& r) { return l + r; },
		    sol::meta_function::subtraction, [](Matrix& l, Matrix& r) { return l - r; },
		    sol::meta_function::multiplication, [](Matrix& l, Matrix& r) { return l * r; });
		matrix["Identity"] = sol::var(Matrix::Identity);
	}
	{
		sol::usertype<Event> event = rootex.new_usertype<Event>("Event", sol::constructors<Event(const Event::Type, const Variant)>());
		event["getType"] = &Event::getType;
		event["getData"] = &Event::getData;
	}
	{
		rootex["AddEvent"] = [](const String& eventType) { EventManager::GetSingleton()->addEvent(eventType); };
		rootex["RemoveEvent"] = [](const String& eventType) { EventManager::GetSingleton()->removeEvent(eventType); };
		rootex["CallEvent"] = [](const Event& event) { EventManager::GetSingleton()->call(event); };
		rootex["DeferredCallEvent"] = [](const Ref<Event>& event) { EventManager::GetSingleton()->deferredCall(event); };
		rootex["ReturnCallEvent"] = [](const Event& event) { return EventManager::GetSingleton()->returnCall(event); };
		rootex["BindFunction"] = [](const Function<Variant(const Event*)>& function, const String& eventName) { BIND_EVENT_FUNCTION(eventName, function); };
		rootex["BindMemberFunction"] = [](const sol::object& self, const Function<Variant(const sol::object&, const Event*)>& function, const String& eventName) {
			BIND_EVENT_FUNCTION(eventName, [=](const Event* e) -> Variant { return function(self, e); });
		};
	}
	{
		sol::usertype<Atomic<int>> atomicInt = rootex.new_usertype<Atomic<int>>("AtomicInt", sol::constructors<Atomic<int>(), Atomic<int>(int)>());
		atomicInt["load"] = [](Atomic<int>* a) { return a->load(); };

		rootex["LoadScene"] = [](const String& sceneFile, const sol::table& arguments) { SceneLoader::GetSingleton()->loadScene(sceneFile, arguments.as<Vector<String>>()); };
		rootex["PreloadScene"] = [](const String& sceneFile, Atomic<int>& progress) { return SceneLoader::GetSingleton()->preloadScene(sceneFile, progress); };
		rootex["LoadPreloadedScene"] = [](const String& sceneFile, const sol::nested<Vector<String>>& arguments) { SceneLoader::GetSingleton()->loadPreloadedScene(sceneFile, arguments.value()); };
		rootex["GetSceneArguments"] = []() { return SceneLoader::GetSingleton()->getArguments(); };
		rootex["GetCurrentScene"] = []() { return SceneLoader::GetSingleton()->getCurrentScene(); };
	}
	{
		sol::usertype<InputManager> inputManager = rootex.new_usertype<InputManager>("Input");
		inputManager["SetEnabled"] = &InputManager::SetEnabled;
		inputManager["MapBool"] = &InputManager::MapBool;
		inputManager["MapFloat"] = &InputManager::MapFloat;
		inputManager["IsPressed"] = &InputManager::IsPressed;
		inputManager["WasPressed"] = &InputManager::WasPressed;
		inputManager["GetFloat"] = &InputManager::GetFloat;
		inputManager["GetFloatDelta"] = &InputManager::GetFloatDelta;
		inputManager["Unmap"] = &InputManager::Unmap;
	}
	{
		sol::usertype<ResourceLoader> resourceLoader = rootex.new_usertype<ResourceLoader>("ResourceLoader");
		resourceLoader["CreateAudio"] = &ResourceLoader::CreateAudioResourceFile;
		resourceLoader["CreateFont"] = &ResourceLoader::CreateFontResourceFile;
		resourceLoader["CreateImage"] = &ResourceLoader::CreateImageResourceFile;
		resourceLoader["CreateLua"] = &ResourceLoader::CreateLuaTextResourceFile;
		resourceLoader["CreateText"] = &ResourceLoader::CreateTextResourceFile;
		resourceLoader["CreateNewText"] = &ResourceLoader::CreateNewTextResourceFile;
		resourceLoader["CreateVisualModel"] = &ResourceLoader::CreateModelResourceFile;
		resourceLoader["CreateAnimatedModel"] = &ResourceLoader::CreateAnimatedModelResourceFile;
		resourceLoader["CreateCollisionModel"] = &ResourceLoader::CreateAnimatedModelResourceFile;
	}
	{
		sol::usertype<ResourceFile> resourceFile = rootex.new_usertype<ResourceFile>("ResourceFile");
		resourceFile["isDirty"] = &ResourceFile::isDirty;
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
		sol::usertype<ModelResourceFile> modelResourceFile = rootex.new_usertype<ModelResourceFile>(
		    "ModelResourceFile",
		    sol::base_classes, sol::bases<ResourceFile>());
	}
	{
		sol::usertype<AnimatedModelResourceFile> animatedModelResourceFile = rootex.new_usertype<AnimatedModelResourceFile>(
		    "AnimatedModelResourceFile",
		    sol::base_classes, sol::bases<ResourceFile>());
	}
	{
		sol::usertype<CollisionModelResourceFile> collisionModelResourceFile = rootex.new_usertype<CollisionModelResourceFile>(
		    "CollisionModelResourceFile",
		    sol::base_classes, sol::bases<ResourceFile>());
	}
	{
		sol::usertype<ImageResourceFile> imageResourceFile = rootex.new_usertype<ImageResourceFile>(
		    "ImageResourceFile",
		    sol::base_classes, sol::bases<ResourceFile>());
	}
	{
		sol::usertype<ImageCubeResourceFile> imageCubeResourceFile = rootex.new_usertype<ImageCubeResourceFile>(
		    "ImageCubeResourceFile",
		    sol::base_classes, sol::bases<ResourceFile>());
	}
	{
		sol::usertype<FontResourceFile> fontResourceFile = rootex.new_usertype<FontResourceFile>(
		    "FontResourceFile",
		    sol::base_classes, sol::bases<ResourceFile>());
	}
	{
		sol::usertype<ECSFactory> ecsFactory = rootex.new_usertype<ECSFactory>("ECSFactory");
	}
	{
		sol::usertype<Scene> scene = rootex.new_usertype<Scene>("Scene");
		scene["CreateEmpty"] = &Scene::CreateEmpty;
		scene["CreateEmptyWithEntity"] = &Scene::CreateEmptyWithEntity;
		scene["CreateFromFile"] = &Scene::CreateFromFile;
		scene["FindScenesByName"] = &Scene::FindScenesByName;
		scene["addChild"] = &Scene::addChild;
		scene["removeChild"] = &Scene::removeChild;
		scene["snatchChild"] = &Scene::snatchChild;
		scene["setName"] = &Scene::setName;
		scene["setEntity"] = &Scene::setEntity;
		scene["getID"] = &Scene::getID;
		scene["getParent"] = &Scene::getParent;
		scene["getChildren"] = &Scene::getChildren;
		scene["getEntity"] = &Scene::getEntity;
		scene["getName"] = &Scene::getName;
		scene["getFullName"] = &Scene::getFullName;
	}
	{
		sol::usertype<Entity> entity = rootex.new_usertype<Entity>("Entity");
		entity["addComponent"] = &Entity::addComponent;
		entity["addDefaultComponent"] = &Entity::addDefaultComponent;
		entity["removeComponent"] = &Entity::removeComponent;
		entity["destroy"] = &Entity::destroy;
		entity["hasComponent"] = &Entity::hasComponent;
		entity["getScene"] = &Entity::getScene;
		entity["getName"] = &Entity::getName;
		entity["setScript"] = &Entity::setScript;
		entity[sol::meta_function::new_index] = [](Entity* e, const String& newIndex) -> sol::object {
			if (Script* s = e->getScript())
			{
				return s->getScriptInstance()[newIndex];
			};
			return sol::nil;
		};
	}
	{
		sol::usertype<Component> component = rootex.new_usertype<Component>("Component");
		component["getOwner"] = &Component::getOwner;
		component["getComponentID"] = &Component::getComponentID;
		component["getName"] = &Component::getName;
	}
	{
		sol::usertype<TransformComponent> transformComponent = rootex.new_usertype<TransformComponent>(
		    "TransformComponent",
		    sol::base_classes, sol::bases<Component>());

		rootex["Entity"]["getTransform"] = &Entity::getComponent<TransformComponent>;
		transformComponent["setPosition"] = &TransformComponent::setPosition;
		transformComponent["setRotation"] = &TransformComponent::setRotation;
		transformComponent["setScale"] = &TransformComponent::setScale;
		transformComponent["setTransform"] = &TransformComponent::setTransform;
		transformComponent["addTransform"] = &TransformComponent::addTransform;
		transformComponent["getPosition"] = &TransformComponent::getPosition;
		transformComponent["getRotation"] = &TransformComponent::getRotation;
		transformComponent["getScale"] = &TransformComponent::getScale;
		transformComponent["getLocalTransform"] = &TransformComponent::getLocalTransform;
		transformComponent["getParentAbsoluteTransform"] = &TransformComponent::getParentAbsoluteTransform;
		transformComponent["getComponentID"] = &TransformComponent::getComponentID;
		transformComponent["getName"] = &TransformComponent::getName;
	}
	{
		sol::usertype<RenderableComponent> renderableComponent = rootex.new_usertype<RenderableComponent>(
		    "RenderableComponent",
		    sol::base_classes, sol::bases<Component>());
		renderableComponent["isVisible"] = &RenderableComponent::isVisible;
		renderableComponent["setVisibility"] = &RenderableComponent::setVisibility;
	}
	{
		sol::usertype<ModelComponent> modelComponent = rootex.new_usertype<ModelComponent>(
		    "ModelComponent",
		    sol::base_classes, sol::bases<Component, RenderableComponent>());
		rootex["Entity"]["getModel"] = &Entity::getComponent<ModelComponent>;
	}
	{
		sol::usertype<AnimatedModelComponent> animatedModelComponent = rootex.new_usertype<AnimatedModelComponent>(
		    "AnimatedModelComponent",
		    sol::base_classes, sol::bases<Component, RenderableComponent>());
		rootex["Entity"]["getAnimatedModel"] = &Entity::getComponent<AnimatedModelComponent>;
	}
	{
		sol::usertype<RenderUIComponent> renderUIComponent = rootex.new_usertype<RenderUIComponent>(
		    "RenderUIComponent",
		    sol::base_classes, sol::bases<Component>());
	}
	{
		sol::usertype<TextUIComponent> textUIComponent = rootex.new_usertype<TextUIComponent>(
		    "TextUIComponent",
		    sol::base_classes, sol::bases<Component, RenderUIComponent>());
		rootex["Entity"]["getTextUI"] = &Entity::getComponent<TextUIComponent>;
		textUIComponent["setFont"] = &TextUIComponent::setFont;
		textUIComponent["setText"] = &TextUIComponent::setText;
	}
	{
		sol::usertype<UIComponent> ui = rootex.new_usertype<UIComponent>(
		    "UIComponent",
		    sol::base_classes, sol::bases<Component>());
		rootex["Entity"]["getUI"] = &Entity::getComponent<UIComponent>;
		ui["getDocumentID"] = [](UIComponent* ui) { return ui->getDocument()->GetId(); };
	}
	{
		sol::usertype<PhysicsColliderComponent> physicsColliderComponent = rootex.new_usertype<PhysicsColliderComponent>(
		    "PhysicsColliderComponent",
		    sol::base_classes, sol::bases<Component>());
		rootex["Entity"]["getPhysicsCollider"] = &Entity::getComponent<PhysicsColliderComponent>;
		physicsColliderComponent["getVelocity"] = &PhysicsColliderComponent::getVelocity;
		physicsColliderComponent["setVelocity"] = &PhysicsColliderComponent::setVelocity;
		physicsColliderComponent["applyForce"] = &PhysicsColliderComponent::applyForce;
	}
	{
		sol::usertype<BoxColliderComponent> bcc = rootex.new_usertype<BoxColliderComponent>(
		    "BoxColliderComponent",
		    sol::base_classes, sol::bases<PhysicsColliderComponent, Component>());
		rootex["Entity"]["getBoxCollider"] = &Entity::getComponent<BoxColliderComponent>;
	}
	{
		sol::usertype<SphereColliderComponent> scc = rootex.new_usertype<SphereColliderComponent>(
		    "SphereColliderComponent",
		    sol::base_classes, sol::bases<PhysicsColliderComponent, Component>());
		rootex["Entity"]["getSphereCollider"] = &Entity::getComponent<SphereColliderComponent>;
	}
	{
		sol::usertype<CapsuleColliderComponent> ccc = rootex.new_usertype<CapsuleColliderComponent>(
		    "CapsuleColliderComponent",
		    sol::base_classes, sol::bases<PhysicsColliderComponent, Component>());
		rootex["Entity"]["getCapsuleCollider"] = &Entity::getComponent<CapsuleColliderComponent>;
	}
}
