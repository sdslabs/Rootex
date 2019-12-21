#include "viewport.h"

Viewport::Viewport(float width, float height, float minDepth, float maxDepth, float topLeftX, float topLeftY)
    : m_Viewport({ topLeftX, topLeftY, width, height, minDepth, maxDepth })
{
}

Viewport::~Viewport()
{
}
