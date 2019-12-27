#include "renderable_object.h"

class Cube : public RenderableObject
{
	std::unique_ptr<VSConstantBuffer> vsConstantBuffer;

public :
	Cube();
	void Update(const AlignedMatrix& transform) override;
	void GetSpatialData(float u, float l, float roll, float yaw, float pitch, DirectX::XMMATRIX& projection);
};