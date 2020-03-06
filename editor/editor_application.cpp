#include "editor_application.h"

Ref<Application> CreateRootexApplication()
{
	return Ref<Application>(new EditorApplication());
}

EditorApplication::EditorApplication()
    : Application("editor/project.lua")
{
}

EditorApplication::~EditorApplication()
{
}
