#pragma once

#include "common/common.h"
#include "resource_files/image_resource_file.h"
#include "resource_files/image_cube_resource_file.h"

#include "ImGuiFileDialog.h"

void RootexSelectableImage(const char* name, ImageResourceFile* image, Function<void(const String&)> onSelected);
void RootexSelectableImageCube(const char* name, ImageCubeResourceFile* image, Function<void(const String&)> onSelected);
