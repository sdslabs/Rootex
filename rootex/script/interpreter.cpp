#include "interpreter.h"
#include "core/resource_loader.h"

#include "common/common.h"

#include "scene.h"
#include "scene_loader.h"
#include "ecs_factory.h"
#include "components/transform_component.h"
#include "components/visual/text_ui_component.h"
#include "components/visual/ui_component.h"
#include "components/visual/model_component.h"
#include "components/physics/box_collider_component.h"
#include "core/input/input_manager.h"
#include "core/resource_files/audio_resource_file.h"
#include "core/resource_files/font_resource_file.h"
#include "core/resource_files/image_resource_file.h"
#include "core/resource_files/image_cube_resource_file.h"
#include "core/resource_files/lua_text_resource_file.h"
#include "core/resource_files/model_resource_file.h"
#include "core/resource_files/collision_model_resource_file.h"
#include "core/resource_files/text_resource_file.h"
#include "event_manager.h"

int HandleLuaException(lua_State* L, sol::optional<const std::exception&> maybeException, sol::string_view description)
{
	PRINT("An exception occurred in Lua function:")
	if (maybeException)
	{
		const std::exception& ex = *maybeException;
		ERR(ex.what());
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

	registerTypes();
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

	sol::table dbg = m_Lua.require_file("dbg", "rootex/script/scripts/debugger.lua");
	dbg["auto_where"] = 2;
	
	m_Lua.require_file("class", "rootex/script/scripts/middleclass.lua");
	m_Lua.do_file("rootex/script/scripts/common.lua");
	m_Lua.do_file("rootex/script/scripts/dialogue_node.lua");
	m_Lua.do_file("rootex/script/scripts/question_dialogue.lua");
	m_Lua.do_file("rootex/script/scripts/one_off_dialogue.lua");
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
	
	Event::RegisterAPI(rootex);
	EventManager::RegisterAPI(rootex);
	InputManager::RegisterAPI(rootex);
	SceneLoader::RegisterAPI(rootex);

	ResourceLoader::RegisterAPI(rootex);
	ResourceFile::RegisterAPI(rootex);
	TextResourceFile::RegisterAPI(rootex);
	LuaTextResourceFile::RegisterAPI(rootex);
	AudioResourceFile::RegisterAPI(rootex);
	ModelResourceFile::RegisterAPI(rootex);
	CollisionModelResourceFile::RegisterAPI(rootex);
	ImageResourceFile::RegisterAPI(rootex);
	ImageCubeResourceFile::RegisterAPI(rootex);
	FontResourceFile::RegisterAPI(rootex);
	
	ECSFactory::RegisterAPI(rootex);
	Scene::RegisterAPI(rootex);
	Entity::RegisterAPI(rootex);
	TransformComponent::RegisterAPI(rootex);
	ModelComponent::RegisterAPI(rootex);
	RenderUIComponent::RegisterAPI(rootex);
	TextUIComponent::RegisterAPI(rootex);
	PhysicsColliderComponent::RegisterAPI(rootex);
}
