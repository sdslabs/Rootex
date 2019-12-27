#include "Cube.h"

Cube::Cube()
{
	std::cout << "lol aaya";
	vertexBuffer.reset(new VertexBuffer({ { -1.0f, -1.0f, -1.0f },
	    { +1.0f, -1.0f, -1.0f },
	    { -1.0f, +1.0f, -1.0f },
	    { +1.0f, +1.0f, -1.0f },
	    { -1.0f, -1.0f, +1.0f },
	    { +1.0f, -1.0f, +1.0f },
	    { -1.0f, +1.0f, +1.0f },
	    { +1.0f, +1.0f, +1.0f } }));

	indexBuffer.reset( new IndexBuffer({ 0, 2, 1,
	    2, 3, 1,
	    1, 3, 5,
	    3, 7, 5,
	    2, 6, 3,
	    3, 6, 7,
	    4, 5, 7,
	    4, 7, 6,
	    0, 4, 2,
	    2, 4, 6,
	    0, 1, 4,
	    1, 5, 4 }));
	BufferFormat bufferFormat;
	bufferFormat.push(VertexBufferElement::Type::POSITION, "POSITION");
	vsConstantBuffer.reset( new VSConstantBuffer());
	psConstantBuffer.reset( new PSConstantBuffer());
	psConstantBuffer->m_Colors[0] = { 1.0f, 0.0f, 0.0f, 1.0f };
	psConstantBuffer->m_Colors[1] = { 0.0f, 1.0f, 0.0f, 1.0f };
	psConstantBuffer->m_Colors[2] = { 0.0f, 0.0f, 1.0f, 1.0f };
	psConstantBuffer->m_Colors[3] = { 0.0f, 0.0f, 1.0f, 1.0f };
	psConstantBuffer->m_Colors[4] = { 1.0f, 1.0f, 0.0f, 1.0f };
	psConstantBuffer->m_Colors[5] = { 0.0f, 1.0f, 1.0f, 1.0f };
	shader.reset(new Shader(L"VertexShader.cso", L"PixelShader.cso", bufferFormat));
	std::cout << "lol yaha bhi aaya";
}

//TODO- remove the reference to Projection matrix part
void Cube::GetSpatialData(float u, float l, float roll, float yaw, float pitch, DirectX::XMMATRIX& projection)
{
	DirectX::XMMATRIX model = DirectX::XMMatrixRotationRollPitchYaw(roll, pitch, yaw) * DirectX::XMMatrixTranslation(u, l, 0.0f);
	DirectX::XMMATRIX view = DirectX::XMMatrixLookAtLH({ 0.0f, 0.0f, 4.0f }, { 0.0f, 0.0f, 0.0f }, { 0.0f, 1.0f, 0.0f });
	vsConstantBuffer->m_M = model;
	vsConstantBuffer->m_V = view;
	vsConstantBuffer->m_P = projection;
}

void Cube::Update(const AlignedMatrix& transform)
{
	shader->setConstantBuffer(Shader::ConstantBufferType::Model, transform);
	shader->setConstantBuffer(*psConstantBuffer);
}