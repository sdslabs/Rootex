#include "common/common.h"

#include "core/audio/audio_source.h"
#include "core/audio/audio_system.h"
#include "core/audio/static_audio_buffer.h"
#include "core/audio/streaming_audio_buffer.h"

#include "core/input/input_manager.h"

#include "core/renderer/buffer_format.h"
#include "core/renderer/index_buffer.h"
#include "core/renderer/material.h"
#include "core/renderer/renderer.h"
#include "core/renderer/shader_library.h"
#include "core/renderer/vertex_buffer.h"

#include "core/event_manager.h"
#include "core/resource_loader.h"

#include "framework/components/hierarchy_component.h"
#include "framework/components/test_component.h"
#include "framework/components/visual/diffuse_visual_component.h"
#include "framework/components/visual/visual_component.h"
#include "framework/components/visual/visual_component_graph.h"
#include "framework/entity_factory.h"
#include "framework/systems/debug_system.h"
#include "framework/systems/light_system.h"
#include "framework/systems/render_system.h"
#include "framework/systems/test_system.h"

#include "main/window.h"

#include "os/os.h"
#include "os/thread.h"
#include "os/timer.h"

#include "script/interpreter.h"

#include "editor/editor.h"

int main()
{
	OS::Initialize();
	OS::PrintLine("Rootex Editor is starting: Build(" + OS::GetBuildDate() + "|" + OS::GetBuildTime() + ")");
	OS::PrintLine("Starting from path: " + OS::GetAbsolutePath(".").generic_string());

	AudioSystem::GetSingleton()->initialize();

	LuaTextResourceFile* configFile = ResourceLoader::CreateLuaTextResourceFile("editor/config/config.lua");
	LuaInterpreter interpreter;
	interpreter.loadExecuteScript(configFile);

	String projectName = interpreter.getGlobal("project");
	OS::PrintLine("Loading Project: " + projectName);

	LuaVariable windowLua = interpreter.getGlobal("window");
	Vector2 windowPosition = { windowLua["x"], windowLua["y"] };
	Vector2 windowSize = { windowLua["width"], windowLua["height"] };
	bool windowMSAA = windowLua["msaa"];

	Ptr<Window> editorWindow(new Window(
	    windowPosition.x,
	    windowPosition.y,
	    windowSize.x,
	    windowSize.y,
	    projectName,
		true, 
		windowMSAA));

	InputManager::GetSingleton()->initialize(windowSize.x, windowSize.y);

	ShaderLibrary::MakeShaders();

	Ref<VisualComponentGraph> visualGraph(new VisualComponentGraph());
	Ref<RenderSystem> renderSystem(new RenderSystem());
	Ref<LightSystem> lightSystem(new LightSystem());

	Ref<Entity> cube = EntityFactory::GetSingleton()->createEntity(ResourceLoader::CreateLuaTextResourceFile("game/assets/test/cube.lua"));
	cube->getComponent<TransformComponent>()->addTransform(Matrix::CreateTranslation({ 0.0f, 0.0f, -5.0f }));
	visualGraph->addChild(cube);

	OS::PrintLine("Project loaded successfully: " + projectName);
	Editor::GetSingleton()->initialize(editorWindow->getWindowHandle());
	
	AudioResourceFile* w = ResourceLoader::CreateAudioResourceFile("game/assets/hipshop.wav");
	Ref<StreamingAudioBuffer> audio(new StreamingAudioBuffer(w));
	Ref<StreamingAudioSource> source(new StreamingAudioSource(audio.get()));
	source->setLooping(true);
	source->play();

	std::optional<int> ret;
	editorWindow->show();
	while (true)
	{
		if (ret = editorWindow->processMessages())
		{
			break;
		}

		AudioSystem::GetSingleton()->update();
		InputManager::GetSingleton()->update();
		EventManager::GetSingleton()->dispatchDeferred();

		Editor::GetSingleton()->begin(visualGraph.get());
		Editor::GetSingleton()->end(visualGraph.get());

		editorWindow->swapBuffers();
		editorWindow->clear();
	}

	return ret.value();
}
