%module Rootex
%{
#include <memory>
#include <wrl.h>
#include <string>
#include <map>
#include <unordered_map>
#include <vector>
#include <filesystem>
#include <d3d11.h>
#include "vendor/DirectXTK/Inc/SimpleMath.h"
#include <DirectXColors.h>
#include <variant>
#include <functional>

#include "common/common.h"
#include "common/types.h"
#include "core/audio/audio_buffer.h"
#include "core/audio/audio_source.h"
#include "core/audio/audio_system.h"
#include "core/audio/static_audio_buffer.h"
#include "core/audio/streaming_audio_buffer.h"
#include "core/event.h"
#include "core/event_manager.h"
#include "core/resource_data.h"
#include "core/resource_file.h"
#include "core/resource_loader.h"
#include "framework/components/visual/visual_component.h"
#include "framework/components/visual/camera_visual_component.h"
#include "framework/components/visual/diffuse_visual_component.h"
#include "framework/components/visual/directional_light_component.h"
#include "framework/components/visual/point_light_component.h"
#include "framework/components/visual/spot_light_component.h"
#include "framework/components/component_ids.h"
#include "framework/components/debug_component.h"
#include "framework/components/hierarchy_component.h"
#include "framework/components/root_hierarchy_component.h"
#include "framework/components/test_component.h"
#include "framework/components/transform_component.h"
#include "framework/system.h"
#include "framework/systems/debug_system.h"
#include "framework/systems/test_system.h"
#include "framework/entity.h"
#include "framework/entity_factory.h"
#include "main/window.h"
#include "os/os.h"
#include "os/timer.h"
#include "os/thread.h"
%}
namespace DirectX {
    namespace Colors {
    }
}

%include "common/common.h"
%include "common/types.h"
%include "core/audio/audio_buffer.h"
%include "core/audio/audio_source.h"
%include "core/audio/audio_system.h"
%include "core/audio/static_audio_buffer.h"
%include "core/audio/streaming_audio_buffer.h"
%include "core/event.h"
%include "core/event_manager.h"
%include "core/resource_data.h"
%include "core/resource_file.h"
%include "core/resource_loader.h"
%include "framework/components/visual/visual_component.h"
%include "framework/components/visual/camera_visual_component.h"
%include "framework/components/visual/diffuse_visual_component.h"
%include "framework/components/visual/directional_light_component.h"
%include "framework/components/visual/point_light_component.h"
%include "framework/components/visual/spot_light_component.h"
%include "framework/components/component_ids.h"
%include "framework/components/debug_component.h"
%include "framework/components/hierarchy_component.h"
%include "framework/components/root_hierarchy_component.h"
%include "framework/components/test_component.h"
%include "framework/components/transform_component.h"
%include "framework/system.h"
%include "framework/systems/debug_system.h"
%include "framework/systems/test_system.h"
%include "framework/entity.h"
%include "framework/entity_factory.h"
%include "os/os.h"
%include "os/timer.h"
