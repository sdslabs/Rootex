#pragma once

enum class RenderPass : int
{
	Basic = 1 << 0,
	Editor = 1 << 1,
	Alpha = 1 << 2
};
