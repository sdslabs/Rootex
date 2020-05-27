#include "custom_render_interface.h"

#include "core/resource_loader.h"
#include "renderer/rendering_device.h"
#include "renderer/shaders/register_locations_vertex_shader.h"

CustomRenderInterface::CustomRenderInterface()
{
	BufferFormat format;
	format.push(VertexBufferElement::Type::TEXCOORD, "POSITION");
	format.push(VertexBufferElement::Type::TEXCOORD, "TEXCOORD");
	format.push(VertexBufferElement::Type::POSITION, "COLOR");

	m_UIShader.reset(new BasicShader(L"rootex/assets/shaders/ui_vertex_shader.cso", L"rootex/assets/shaders/ui_pixel_shader.cso", format));
}

void CustomRenderInterface::RenderGeometry(Rml::Core::Vertex* vertices, int num_vertices, int* indices, int num_indices, Rml::Core::TextureHandle texture, const Rml::Core::Vector2f& translation) 
{
	Vector<UIVertexData> vertexData;
	vertexData.assign((UIVertexData*)vertices, (UIVertexData*)vertices + num_vertices);
	VertexBuffer vb(vertexData);
	Vector<int> indicesBuffer;
	indicesBuffer.assign(indices, indices + num_indices);
	IndexBuffer ib(indicesBuffer);

	vb.bind();
	ib.bind();
	m_UIShader->bind();
	RenderingDevice::GetSingleton()->setInPixelShader(0, 1, m_Textures[texture]->getTextureResourceView());

	RenderingDevice::GetSingleton()->drawIndexed(ib.getCount());
}

Rml::Core::CompiledGeometryHandle CustomRenderInterface::CompileGeometry(Rml::Core::Vertex* vertices, int num_vertices, int* indices, int num_indices, Rml::Core::TextureHandle texture)
{
	return (Rml::Core::CompiledGeometryHandle) nullptr;
}

void CustomRenderInterface::RenderCompiledGeometry(Rml::Core::CompiledGeometryHandle geometry, const Rml::Core::Vector2f& translation)
{
}

void CustomRenderInterface::ReleaseCompiledGeometry(Rml::Core::CompiledGeometryHandle geometry)
{
}

bool CustomRenderInterface::LoadTexture(Rml::Core::TextureHandle& texture_handle, Rml::Core::Vector2i& texture_dimensions, const String& source)
{
	ImageResourceFile* image = ResourceLoader::CreateImageResourceFile(source);

	if (image)
	{
		m_Textures[texture_handle].reset(new Texture(image));
		return true;
	}
	return false;
}

bool CustomRenderInterface::GenerateTexture(Rml::Core::TextureHandle& texture_handle, const byte* source, const Rml::Core::Vector2i& source_dimensions)
{
	m_Textures[texture_handle].reset(new Texture((uint8_t*)source, source_dimensions.x * source_dimensions.y * 4));
	return m_Textures[texture_handle] != nullptr;
}

void CustomRenderInterface::ReleaseTexture(Rml::Core::TextureHandle texture)
{
	m_Textures[texture].reset();
}

void CustomRenderInterface::EnableScissorRegion(bool enable)
{
	if (enable)
	{
		RenderingDevice::GetSingleton()->setRasterizerState(RenderingDevice::RasterizerState::DefaultScissor);
	}
	else
	{
		RenderingDevice::GetSingleton()->setRasterizerState(RenderingDevice::RasterizerState::Default);	
	}
}

void CustomRenderInterface::SetScissorRegion(int x, int y, int width, int height)
{
	RenderingDevice::GetSingleton()->setScissorRectangle(x, y, width, height);
}

void CustomRenderInterface::SetTransform(const Rml::Core::Matrix4f* transform)
{
	D3D11_BUFFER_DESC cbd = { 0 };
	cbd.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	cbd.Usage = D3D11_USAGE_DYNAMIC;
	cbd.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	cbd.MiscFlags = 0u;
	cbd.ByteWidth = sizeof(Rml::Core::Matrix4f);
	cbd.StructureByteStride = 0u;
	D3D11_SUBRESOURCE_DATA csd = { 0 };
	csd.pSysMem = &transform;

	RenderingDevice::GetSingleton()->setVSConstantBuffer(
		RenderingDevice::GetSingleton()->createVSConstantBuffer(&cbd, &csd).Get(), 
		PER_OBJECT_VS_CPP);
}
