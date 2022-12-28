#include "interpreter.h"
#include "core/resource_loader.h"

#include "common/common.h"

#include "scene.h"
#include "scene_loader.h"
#include "ecs_factory.h"
#include "system.h"
#include "script.h"
#include "systems/animation_system.h"
#include "systems/audio_system.h"
#include "systems/input_system.h"
#include "systems/light_system.h"
#include "systems/particle_system.h"
#include "systems/pause_system.h"
#include "systems/physics_system.h"
#include "systems/player_system.h"
#include "systems/post_process_system.h"
#include "systems/render_system.h"
#include "systems/render_ui_system.h"
#include "systems/script_system.h"
#include "systems/transform_system.h"
#include "systems/transform_animation_system.h"
#include "systems/trigger_system.h"
#include "systems/ui_system.h"
#include "components/audio/audio_component.h"
#include "components/audio/short_music_component.h"
#include "components/audio/music_component.h"
#include "components/space/transform_component.h"
#include "components/visual/ui/text_ui_component.h"
#include "components/visual/ui/ui_component.h"
#include "components/visual/model/model_component.h"
#include "components/visual/model/animated_model_component.h"
#include "components/physics/collision_component.h"
#include "components/physics/rigid_body_component.h"
#include "components/physics/box_collider_component.h"
#include "components/physics/capsule_collider_component.h"
#include "components/physics/sphere_collider_component.h"
#include "components/physics/static_mesh_collider_component.h"
#include "components/physics/trigger_component.h"
#include "components/visual/ui/ui_component.h"
#include "components/visual/effect/particle_effect_component.h"
#include "core/resource_files/audio_resource_file.h"
#include "core/resource_files/font_resource_file.h"
#include "core/resource_files/image_resource_file.h"
#include "core/resource_files/image_cube_resource_file.h"
#include "core/resource_files/lua_text_resource_file.h"
#include "core/resource_files/model_resource_file.h"
#include "core/resource_files/collision_model_resource_file.h"
#include "core/resource_files/text_resource_file.h"
#include "core/resource_files/particle_effect_resource_file.h"
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

LuaInterpreter::~LuaInterpreter()
{
	m_Binder.unbindAll();
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
	m_Lua.do_string("package.path = package.path .. ';rootex/vendor/Flux/?.lua'");
}

void LuaInterpreter::registerTypes()
{
	sol::table& rootex = m_Lua.create_named_table("RTX");
	{
		sol::usertype<Vector2> vector2 = rootex.new_usertype<Vector2>(
		    "Vector2",
		    sol::constructors<Vector2(), Vector2(float, float)>(),
		    sol::meta_function::addition, [](Vector2& l, Vector2& r) { return l + r; },
		    sol::meta_function::subtraction, [](Vector2& l, Vector2& r) { return l - r; },
		    sol::meta_function::multiplication, [](Vector2& l, Vector2& r) { return l * r; },
		    sol::meta_function::multiplication, [](Vector2& l, float r) { return l * r; },
		    sol::meta_function::division, [](Vector2& l, float r) { return l / r; },
		    sol::meta_function::division, [](Vector2& l, Vector2& r) { return l / r; });
		vector2["dot"] = [](const Vector2& l, const Vector2& r) { return l.Dot(r); };
		vector2["cross"] = [](const Vector2& l, const Vector2& r) { return l.Cross(r); };
		vector2["x"] = &Vector2::x;
		vector2["y"] = &Vector2::y;
	}
	{
		sol::usertype<Vector3> vector3 = rootex.new_usertype<Vector3>(
		    "Vector3",
		    sol::constructors<Vector3(), Vector3(float, float, float)>(),
		    sol::meta_function::addition, [](Vector3& l, Vector3& r) { return l + r; },
		    sol::meta_function::subtraction, [](Vector3& l, Vector3& r) { return l - r; },
		    sol::meta_function::multiplication, [](float l, Vector3& r) { return l * r; },
		    sol::meta_function::multiplication, [](Vector3& l, float r) { return l * r; },
		    sol::meta_function::division, [](Vector3& l, float r) { return l / r; },
		    sol::meta_function::division, [](Vector3& l, Vector3& r) { return l / r; });
		vector3["dot"] = [](const Vector3& l, const Vector3& r) { return l.Dot(r); };
		vector3["cross"] = [](const Vector3& l, const Vector3& r) { return l.Cross(r); };
		vector3["x"] = &Vector3::x;
		vector3["y"] = &Vector3::y;
		vector3["z"] = &Vector3::z;
	}
	{
		sol::usertype<Vector4> vector4 = rootex.new_usertype<Vector4>(
		    "Vector4", sol::constructors<Vector4(), Vector4(float, float, float, float)>(),
		    sol::meta_function::addition, [](Vector4& l, Vector4& r) { return l + r; },
		    sol::meta_function::subtraction, [](Vector4& l, Vector4& r) { return l - r; },
		    sol::meta_function::multiplication, [](Vector4& l, Vector4& r) { return l * r; },
		    sol::meta_function::multiplication, [](Vector4& l, float r) { return l * r; },
		    sol::meta_function::division, [](Vector4& l, Vector4& r) { return l / r; },
		    sol::meta_function::division, [](Vector4& l, float r) { return l / r; });
		vector4["dot"] = [](const Vector4& l, const Vector4& r) { return l.Dot(r); };
		vector4["cross"] = [](const Vector4& l, const Vector4& r) { return l.Cross(l, r); };
		vector4["x"] = &Vector4::x;
		vector4["y"] = &Vector4::y;
		vector4["z"] = &Vector4::z;
		vector4["w"] = &Vector4::w;
	}
	{
		sol::usertype<Color> color = rootex.new_usertype<Color>(
		    "Color", sol::constructors<Color(), Color(float, float, float, float)>(),
		    sol::meta_function::addition, [](Color& l, Color& r) { return l + r; },
		    sol::meta_function::subtraction, [](Color& l, Color& r) { return l - r; },
		    sol::meta_function::multiplication, [](Color& l, Color& r) { return l * r; });
		color["x"] = &Color::x;
		color["y"] = &Color::y;
		color["z"] = &Color::z;
		color["w"] = &Color::w;
		color["r"] = &Color::x;
		color["g"] = &Color::y;
		color["b"] = &Color::z;
		color["a"] = &Color::w;
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
		matrix["_11"] = &Matrix::_11;
		matrix["_12"] = &Matrix::_12;
		matrix["_13"] = &Matrix::_13;
		matrix["_14"] = &Matrix::_14;
		matrix["_21"] = &Matrix::_21;
		matrix["_22"] = &Matrix::_22;
		matrix["_23"] = &Matrix::_23;
		matrix["_24"] = &Matrix::_24;
		matrix["_31"] = &Matrix::_31;
		matrix["_32"] = &Matrix::_32;
		matrix["_33"] = &Matrix::_33;
		matrix["_34"] = &Matrix::_34;
		matrix["_41"] = &Matrix::_41;
		matrix["_42"] = &Matrix::_42;
		matrix["_43"] = &Matrix::_43;
		matrix["_44"] = &Matrix::_44;
	}
	{
		sol::usertype<Event> event = rootex.new_usertype<Event>("Event", sol::constructors<Event(const Event::Type, const Variant)>());
		event["getType"] = &Event::getType;
		event["getData"] = &Event::getData;
	}
	{
		rootex["CallEvent"] = [](const Event& event) { EventManager::GetSingleton()->call(event); };
		rootex["Call"] = [](const Event::Type& type, const Variant& data) { EventManager::GetSingleton()->call(type, data); };
		rootex["DeferredCallEvent"] = [](const Ref<Event>& event) { EventManager::GetSingleton()->deferredCall(event); };
		rootex["ReturnCallEvent"] = [](const Event& event) { return EventManager::GetSingleton()->returnCall(event); };
		rootex["Bind"] = [this](const Event::Type& event, sol::function function) { m_Binder.bind(event, function); };
		rootex["Unbind"] = [this](const Event::Type& event) { m_Binder.unbind(event); };
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
		inputManager["GetMousePosition"] = &InputManager::GetMousePosition;
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
		resourceLoader["CreateParticleEffectResourceFile"] = &ResourceLoader::CreateParticleEffectResourceFile;
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
		audioResourceFile["audioDataSize"] = &AudioResourceFile::getAudioDataSize;
		audioResourceFile["getFormat"] = &AudioResourceFile::getFormat;
		audioResourceFile["getFrequency"] = &AudioResourceFile::getFrequency;
		audioResourceFile["getBitDepth"] = &AudioResourceFile::getBitDepth;
		audioResourceFile["getChannels"] = &AudioResourceFile::getChannels;
		audioResourceFile["getDuration"] = &AudioResourceFile::getDuration;
	}
	{
		sol::usertype<MaterialResourceFile> materialResourceFile = rootex.new_usertype<MaterialResourceFile>(
		    "MaterialResourceFile",
		    sol::base_classes, sol::bases<ResourceFile>(),
		    "alpha", sol::property(&MaterialResourceFile::isAlpha, &MaterialResourceFile::setAlpha));
		materialResourceFile["isAlpha"] = &MaterialResourceFile::isAlpha;
		materialResourceFile["setAlpha"] = &MaterialResourceFile::setAlpha;
		materialResourceFile["asBasic"] = &MaterialResourceFile::as<BasicMaterialResourceFile>;
		materialResourceFile["asCustom"] = &MaterialResourceFile::as<CustomMaterialResourceFile>;
	}
	{
		sol::usertype<BasicMaterialResourceFile> basicMaterialResourceFile = rootex.new_usertype<BasicMaterialResourceFile>(
		    "BasicMaterialResourceFile",
		    sol::base_classes, sol::bases<ResourceFile, MaterialResourceFile>());
		basicMaterialResourceFile["setColor"] = &BasicMaterialResourceFile::setColor;
		basicMaterialResourceFile["setDiffuse"] = &BasicMaterialResourceFile::setDiffuse;
		basicMaterialResourceFile["setNormal"] = &BasicMaterialResourceFile::setNormal;
		basicMaterialResourceFile["setSpecular"] = &BasicMaterialResourceFile::setSpecular;
		basicMaterialResourceFile["setLightmap"] = &BasicMaterialResourceFile::setLightmap;
		basicMaterialResourceFile["setAffectedBySky"] = &BasicMaterialResourceFile::setAffectedBySky;
		basicMaterialResourceFile["setAffectedByLight"] = &BasicMaterialResourceFile::setAffectedByLight;
		basicMaterialResourceFile["getColor"] = &BasicMaterialResourceFile::getColor;
		basicMaterialResourceFile["getDiffuse"] = &BasicMaterialResourceFile::getDiffuse;
		basicMaterialResourceFile["getNormal"] = &BasicMaterialResourceFile::getNormal;
		basicMaterialResourceFile["getSpecular"] = &BasicMaterialResourceFile::getSpecular;
		basicMaterialResourceFile["getLightmap"] = &BasicMaterialResourceFile::getLightmap;
	}
	{
		sol::usertype<CustomMaterialResourceFile> customMaterialResourceFile = rootex.new_usertype<CustomMaterialResourceFile>(
		    "CustomMaterialResourceFile",
		    sol::base_classes, sol::bases<ResourceFile, MaterialResourceFile>());
		customMaterialResourceFile["setFloat"] = &CustomMaterialResourceFile::setFloat;
		customMaterialResourceFile["setFloat3"] = &CustomMaterialResourceFile::setFloat3;
		customMaterialResourceFile["setColor"] = &CustomMaterialResourceFile::setColor;
		customMaterialResourceFile["getFloat"] = &CustomMaterialResourceFile::getFloat;
		customMaterialResourceFile["getFloat3"] = &CustomMaterialResourceFile::getFloat3;
		customMaterialResourceFile["getColor"] = &CustomMaterialResourceFile::getColor;
	}
	{
		sol::usertype<Mesh> mesh = rootex.new_usertype<Mesh>("Mesh");
		mesh["getBoundingBox"] = &Mesh::getBoundingBox;
		mesh["addLOD"] = &Mesh::addLOD;
	}
	{
		sol::usertype<ModelResourceFile> modelResourceFile = rootex.new_usertype<ModelResourceFile>(
		    "ModelResourceFile",
		    sol::base_classes, sol::bases<ResourceFile>());
		modelResourceFile["getMeshesOfMaterialAt"] = &ModelResourceFile::getMeshesOfMaterialAt;
		modelResourceFile["getMaterialCount"] = &ModelResourceFile::getMaterialCount;
		modelResourceFile["getMaterialAt"] = &ModelResourceFile::getMaterialAt;
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
		sol::usertype<CPUTexture> cpuTexture = rootex.new_usertype<CPUTexture>("CPUTexture");
		cpuTexture["getPixel"] = &CPUTexture::getPixel;
		cpuTexture["setPixel"] = &CPUTexture::setPixel;
		cpuTexture["getWidth"] = &CPUTexture::getWidth;
		cpuTexture["getHeight"] = &CPUTexture::getHeight;
	}
	{
		sol::usertype<ImageResourceFile> imageResourceFile = rootex.new_usertype<ImageResourceFile>(
		    "ImageResourceFile",
		    sol::base_classes, sol::bases<ResourceFile>(),
		    "cpuAccess", sol::property(&ImageResourceFile::isCPUAccess, &ImageResourceFile::setCPUAccess));
		imageResourceFile["getCPUTexture"] = &ImageResourceFile::getCPUTexture;
		imageResourceFile["uploadCPUTexturetoGPU"] = &ImageResourceFile::uploadCPUTexturetoGPU;
		imageResourceFile["setCPUAccess"] = &ImageResourceFile::setCPUAccess;
		imageResourceFile["isCPUAccess"] = &ImageResourceFile::isCPUAccess;
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
		sol::usertype<ParticleEffectResourceFile> particleEffectResourceFile = rootex.new_usertype<ParticleEffectResourceFile>(
		    "ParticleEffectResourceFile",
		    sol::base_classes, sol::bases<ResourceFile>());
		particleEffectResourceFile["getEffect"] = &ParticleEffectResourceFile::getEffect;
	}
	{
		sol::table& ecs = rootex.create_named("ECS");
		ecs["AddComponent"] = &ECSFactory::AddComponent;
	}
	{
		sol::usertype<Scene> scene = rootex.new_usertype<Scene>("Scene",
		    "name", sol::property(&Scene::getName, &Scene::setName),
		    "parent", sol::property(&Scene::getParent),
		    "children", sol::property(&Scene::getChildren),
		    "entity", sol::property(&Scene::getEntity));
		scene["CreateEmpty"] = &Scene::CreateEmpty;
		scene["CreateFromFile"] = &Scene::CreateFromFile;
		scene["FindScenesByName"] = &Scene::FindScenesByName;
		scene["FindSceneByID"] = &Scene::FindSceneByID;
		scene["addChild"] = &Scene::addChild;
		scene["removeChild"] = &Scene::removeChild;
		scene["snatchChild"] = &Scene::snatchChild;
		scene["getID"] = &Scene::getID;
		scene["getFullName"] = &Scene::getFullName;
	}
	{
		sol::usertype<System> system = rootex.new_usertype<System>("System");
		system["pause"] = &System::pause;
		system["unPause"] = &System::unPause;

		sol::table& systems = rootex.create_named("Systems");
		{
			sol::usertype<AnimationSystem> animationSystem = systems.new_usertype<AnimationSystem>("AnimationSystem", sol::base_classes, sol::bases<System>());
			sol::usertype<AudioSystem> audioSystem = systems.new_usertype<AudioSystem>("AudioSystem", sol::base_classes, sol::bases<System>());
			sol::usertype<InputSystem> inputSystem = systems.new_usertype<InputSystem>("InputSystem", sol::base_classes, sol::bases<System>());
			sol::usertype<LightSystem> lightSystem = systems.new_usertype<LightSystem>("LightSystem", sol::base_classes, sol::bases<System>());
			sol::usertype<ParticleSystem> particleSystem = systems.new_usertype<ParticleSystem>("ParticleSystem", sol::base_classes, sol::bases<System>());
			sol::usertype<PauseSystem> pauseSystem = systems.new_usertype<PauseSystem>("PauseSystem", sol::base_classes, sol::bases<System>());
			sol::usertype<PhysicsSystem> physicsSystem = systems.new_usertype<PhysicsSystem>("PhysicsSystem", sol::base_classes, sol::bases<System>());
			sol::usertype<PlayerSystem> playerSystem = systems.new_usertype<PlayerSystem>("PlayerSystem", sol::base_classes, sol::bases<System>());
			sol::usertype<PostProcessSystem> postProcessSystem = systems.new_usertype<PostProcessSystem>("PostProcessSystem", sol::base_classes, sol::bases<System>());
			sol::usertype<RenderSystem> renderSystem = systems.new_usertype<RenderSystem>("RenderSystem", sol::base_classes, sol::bases<System>());
			sol::usertype<RenderUISystem> renderUISystem = systems.new_usertype<RenderUISystem>("RenderUISystem", sol::base_classes, sol::bases<System>());
			sol::usertype<ScriptSystem> scriptSystem = systems.new_usertype<ScriptSystem>("ScriptSystem", sol::base_classes, sol::bases<System>());
			sol::usertype<TransformSystem> transformSystem = systems.new_usertype<TransformSystem>("TransformSystem", sol::base_classes, sol::bases<System>());
			sol::usertype<TransformAnimationSystem> transformAnimationSystem = systems.new_usertype<TransformAnimationSystem>("TransformAnimationSystem", sol::base_classes, sol::bases<System>());
			sol::usertype<TriggerSystem> triggerSystem = systems.new_usertype<TriggerSystem>("TriggerSystem", sol::base_classes, sol::bases<System>());
			sol::usertype<UISystem> uiSystem = systems.new_usertype<UISystem>("UISystem", sol::base_classes, sol::bases<System>());

			systems["Animation"] = sol::property(&AnimationSystem::GetSingleton);
			systems["Audio"] = sol::property(&AudioSystem::GetSingleton);
			systems["Input"] = sol::property(&InputSystem::GetSingleton);
			systems["Light"] = sol::property(&LightSystem::GetSingleton);
			systems["Particle"] = sol::property(&ParticleSystem::GetSingleton);
			systems["Pause"] = sol::property(&PauseSystem::GetSingleton);
			systems["Physics"] = sol::property(&PhysicsSystem::GetSingleton);
			systems["Player"] = sol::property(&PlayerSystem::GetSingleton);
			systems["PostProcess"] = sol::property(&PostProcessSystem::GetSingleton);
			systems["Render"] = sol::property(&RenderSystem::GetSingleton);
			systems["RenderUI"] = sol::property(&RenderUISystem::GetSingleton);
			systems["Script"] = sol::property(&ScriptSystem::GetSingleton);
			systems["Transform"] = sol::property(&TransformSystem::GetSingleton);
			systems["TransformAnimation"] = sol::property(&TransformAnimationSystem::GetSingleton);
			systems["Trigger"] = sol::property(&TriggerSystem::GetSingleton);
			systems["UI"] = sol::property(&UISystem::GetSingleton);

			pauseSystem["getIsPausingEnabled"] = &PauseSystem::getIsPausingEnabled;
			pauseSystem["setIsPausingEnabled"] = &PauseSystem::setIsPausingEnabled;
		}
	}
	{
		sol::usertype<Entity> entity = rootex.new_usertype<Entity>("Entity",
		    "transform", sol::property(&Entity::getComponent<TransformComponent>),
		    "model", sol::property(&Entity::getComponent<ModelComponent>),
		    "animatedModel", sol::property(&Entity::getComponent<AnimatedModelComponent>),
		    "particleEffect", sol::property(&Entity::getComponent<ParticleEffectComponent>),
		    "textUI", sol::property(&Entity::getComponent<TextUIComponent>),
		    "ui", sol::property(&Entity::getComponent<UIComponent>),
		    "shortMusic", sol::property(&Entity::getComponent<ShortMusicComponent>),
		    "music", sol::property(&Entity::getComponent<MusicComponent>),
		    "boxCollider", sol::property(&Entity::getComponent<BoxColliderComponent>),
		    "sphereCollider", sol::property(&Entity::getComponent<SphereColliderComponent>),
		    "capsuleCollider", sol::property(&Entity::getComponent<CapsuleColliderComponent>),
		    "staticMeshCollider", sol::property(&Entity::getComponent<StaticMeshColliderComponent>),
		    "trigger", sol::property(&Entity::getComponent<TriggerComponent>),
		    "script", sol::property(&Entity::getScript),
		    "scene", sol::property(&Entity::getScene));
		entity["addComponent"] = &Entity::addComponent;
		entity["addDefaultComponent"] = &Entity::addDefaultComponent;
		entity["removeComponent"] = &Entity::removeComponent;
		entity["destroy"] = &Entity::destroy;
		entity["hasComponent"] = &Entity::hasComponent;
		entity["getScene"] = &Entity::getScene;
		entity["getName"] = &Entity::getName;
		entity["setScript"] = &Entity::setScript;
		entity["bind"] = &Entity::bind;
	}
	{
		sol::usertype<Component> component = rootex.new_usertype<Component>("Component",
		    "owner", sol::property(&Component::getOwner),
		    "ID", sol::property(&Component::getComponentID),
		    "name", sol::property(&Component::getName));
	}
	{
		sol::usertype<TransformComponent> transformComponent = rootex.new_usertype<TransformComponent>(
		    "TransformComponent",
		    sol::base_classes, sol::bases<Component>(),
		    "position", sol::property(&TransformComponent::getPosition, &TransformComponent::setPosition),
		    "scale", sol::property(&TransformComponent::getScale, &TransformComponent::setScale),
		    "rotation", sol::property(&TransformComponent::getRotation, &TransformComponent::setRotation),
		    "localTransform", sol::property(&TransformComponent::getLocalTransform, &TransformComponent::setLocalTransform),
		    "absoluteTransform", sol::property(&TransformComponent::getAbsoluteTransform, &TransformComponent::setAbsoluteTransform));

		transformComponent["getPosition"] = &TransformComponent::getPosition;
		transformComponent["setPosition"] = &TransformComponent::setPosition;

		transformComponent["getScale"] = &TransformComponent::getScale;
		transformComponent["setScale"] = &TransformComponent::setScale;

		transformComponent["getRotation"] = &TransformComponent::getRotation;
		transformComponent["setRotation"] = &TransformComponent::setRotation;
		transformComponent["addQuaternion"] = &TransformComponent::addQuaternion;
		transformComponent["addRotation"] = &TransformComponent::addRotation;

		transformComponent["setLocalTransform"] = &TransformComponent::setLocalTransform;
		transformComponent["getLocalTransform"] = &TransformComponent::getLocalTransform;
		transformComponent["addLocalTransform"] = &TransformComponent::addLocalTransform;

		transformComponent["getAbsoluteTransform"] = &TransformComponent::getAbsoluteTransform;
		transformComponent["setAbsoluteTransform"] = &TransformComponent::setAbsoluteTransform;
	}
	{
		sol::usertype<RenderableComponent> renderableComponent = rootex.new_usertype<RenderableComponent>(
		    "RenderableComponent",
		    sol::base_classes, sol::bases<Component>(),
		    "visible", sol::property(&RenderableComponent::isVisible, &RenderableComponent::setVisible));
		renderableComponent["isVisible"] = &RenderableComponent::isVisible;
		renderableComponent["setVisible"] = &RenderableComponent::setVisible;
		renderableComponent["getMaterialOverride"] = &RenderableComponent::getMaterialOverride;
	}
	{
		sol::usertype<ModelComponent> modelComponent = rootex.new_usertype<ModelComponent>(
		    "ModelComponent",
		    sol::base_classes, sol::bases<Component, RenderableComponent>());
		modelComponent["getModelResourceFile"] = &ModelComponent::getModelResourceFile;
	}
	{
		sol::usertype<AnimatedModelComponent> animatedModelComponent = rootex.new_usertype<AnimatedModelComponent>(
		    "AnimatedModelComponent",
		    sol::base_classes, sol::bases<Component, RenderableComponent>(),
		    "animatedResourceFile", sol::property(&AnimatedModelComponent::getAnimatedResourceFile, &AnimatedModelComponent::setAnimatedResourceFile));
		animatedModelComponent["getAnimatedResourceFile"] = &AnimatedModelComponent::getAnimatedResourceFile;
		animatedModelComponent["setAnimatedResourceFile"] = &AnimatedModelComponent::setAnimatedResourceFile;

		animatedModelComponent["getCurrentAnimation"] = &AnimatedModelComponent::getCurrentAnimationName;
		animatedModelComponent["setAnimation"] = &AnimatedModelComponent::setAnimation;

		animatedModelComponent["transition"] = &AnimatedModelComponent::transition;
		animatedModelComponent["play"] = &AnimatedModelComponent::play;
		animatedModelComponent["stop"] = &AnimatedModelComponent::stop;
		animatedModelComponent["setPlaying"] = &AnimatedModelComponent::setPlaying;
	}
	{
		sol::usertype<ParticleEffectComponent> particleEffectComponent = rootex.new_usertype<ParticleEffectComponent>(
		    "ParticleEffectComponent",
		    sol::base_classes, sol::bases<Component>());
		particleEffectComponent["setPlaying"] = &ParticleEffectComponent::setPlaying;
		particleEffectComponent["setEffect"] = &ParticleEffectComponent::setEffect;
		particleEffectComponent["getEffectResource"] = &ParticleEffectComponent::getEffectResource;
		particleEffectComponent["setPlaying"] = &ParticleEffectComponent::setPlaying;
		particleEffectComponent["isPaused"] = &ParticleEffectComponent::isPaused;
		particleEffectComponent["isMoving"] = &ParticleEffectComponent::isMoving;
		particleEffectComponent["setMoving"] = &ParticleEffectComponent::setMoving;
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
		textUIComponent["setFont"] = &TextUIComponent::setFont;
		textUIComponent["setText"] = &TextUIComponent::setText;
	}
	{
		sol::usertype<UIComponent> ui = rootex.new_usertype<UIComponent>(
		    "UIComponent",
		    sol::base_classes, sol::bases<Component>(),
		    "document", sol::property(&UIComponent::getDocument));
		ui["getDocumentID"] = [](UIComponent* ui) { return ui->getDocument()->GetId(); };
	}
	{
		sol::usertype<AudioComponent> audioComponent = rootex.new_usertype<AudioComponent>(
		    "AudioComponent",
		    sol::base_classes, sol::bases<Component>());
		audioComponent["play"] = &AudioComponent::play;
		audioComponent["stop"] = &AudioComponent::stop;
		audioComponent["setPlaying"] = &AudioComponent::setPlaying;
	}
	{
		sol::usertype<ShortMusicComponent> shortMusicComponent = rootex.new_usertype<ShortMusicComponent>(
		    "ShortMusicComponent",
		    sol::base_classes, sol::bases<Component, AudioComponent>());
	}
	{
		sol::usertype<MusicComponent> musicComponent = rootex.new_usertype<MusicComponent>(
		    "MusicComponent",
		    sol::base_classes, sol::bases<Component, AudioComponent>());
	}
	{
		sol::usertype<Hit> hit = rootex.new_usertype<Hit>("Hit");
		hit["this"] = &Hit::thisOne;
		hit["that"] = &Hit::thatOne;
	}
	{
		sol::usertype<CollisionComponent> collisionComponent = rootex.new_usertype<CollisionComponent>(
		    "CollisionComponent",
		    sol::base_classes, sol::bases<Component>());
	}
	{
		sol::usertype<RigidBodyComponent> rigidBodyComponent = rootex.new_usertype<RigidBodyComponent>(
		    "RigidBodyComponent",
		    sol::base_classes, sol::bases<CollisionComponent, Component>(),
		    "velocity", sol::property(&RigidBodyComponent::getVelocity, &RigidBodyComponent::setVelocity),
		    "gravity", sol::property(&RigidBodyComponent::getGravity, &RigidBodyComponent::setGravity),
		    "offset", sol::property(&RigidBodyComponent::getOffset, &RigidBodyComponent::setOffset),
		    "angularFactor", sol::property(&RigidBodyComponent::getAngularFactor, &RigidBodyComponent::setAngularFactor),
		    "angularVelocity", sol::property(&RigidBodyComponent::getAngularVelocity, &RigidBodyComponent::setAngularVelocity),
		    "material", sol::property(&RigidBodyComponent::getMaterial),
		    "generatesHitEvents", sol::property(&RigidBodyComponent::isGeneratesHitEvents, &RigidBodyComponent::setGeneratedHitEvents),
		    "ccd", sol::property(&RigidBodyComponent::isCCD, &RigidBodyComponent::setCCD),
		    "moveable", sol::property(&RigidBodyComponent::isMoveable, &RigidBodyComponent::setMoveable),
		    "kinematic", sol::property(&RigidBodyComponent::isKinematic, &RigidBodyComponent::setKinematic),
		    "sleepable", sol::property(&RigidBodyComponent::canSleep, &RigidBodyComponent::setSleepable));

		rigidBodyComponent["getVelocity"] = &RigidBodyComponent::getVelocity;
		rigidBodyComponent["setVelocity"] = &RigidBodyComponent::setVelocity;

		rigidBodyComponent["applyForce"] = &RigidBodyComponent::applyForce;
		rigidBodyComponent["applyTorque"] = &RigidBodyComponent::applyTorque;
		rigidBodyComponent["setAxisLock"] = &RigidBodyComponent::setAxisLock;
		rigidBodyComponent["translate"] = &RigidBodyComponent::translate;
	}
	{
		sol::usertype<BoxColliderComponent> bcc = rootex.new_usertype<BoxColliderComponent>(
		    "BoxColliderComponent",
		    sol::base_classes, sol::bases<Component, CollisionComponent, RigidBodyComponent>());
	}
	{
		sol::usertype<SphereColliderComponent> scc = rootex.new_usertype<SphereColliderComponent>(
		    "SphereColliderComponent",
		    sol::base_classes, sol::bases<Component, CollisionComponent, RigidBodyComponent>());
	}
	{
		sol::usertype<CapsuleColliderComponent> ccc = rootex.new_usertype<CapsuleColliderComponent>(
		    "CapsuleColliderComponent",
		    sol::base_classes, sol::bases<Component, CollisionComponent, RigidBodyComponent>());
	}
	{
		sol::usertype<StaticMeshColliderComponent> smc = rootex.new_usertype<StaticMeshColliderComponent>(
		    "StaticMeshColliderComponent",
		    sol::base_classes, sol::bases<Component, CollisionComponent, RigidBodyComponent>());
		smc["setCollisionModel"] = &StaticMeshColliderComponent::setCollisionModel;
	}
	{
		sol::usertype<TriggerComponent> trigger = rootex.new_usertype<TriggerComponent>(
		    "TriggerComponent",
		    sol::base_classes, sol::bases<Component, CollisionComponent>());
		trigger["addEntryTarget"] = &TriggerComponent::addEntryTarget;
		trigger["removeEntryTarget"] = &TriggerComponent::removeEntryTarget;
		trigger["addExitTarget"] = &TriggerComponent::addEntryTarget;
		trigger["removeExitTarget"] = &TriggerComponent::removeEntryTarget;
		trigger["isEntryRepeat"] = &TriggerComponent::isEntryRepeat;
		trigger["isExitRepeat"] = &TriggerComponent::isExitRepeat;
		trigger["setDimensions"] = &TriggerComponent::setDimensions;
	}
}
