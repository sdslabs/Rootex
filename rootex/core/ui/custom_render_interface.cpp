#include "custom_render_interface.h"

#include "core/resource_loader.h"
#include "core/resource_files/image_resource_file.h"
#include "renderer/rendering_device.h"
#include "renderer/shaders/register_locations_vertex_shader.h"
#include "renderer/vertex_data.h"
#include "renderer/vertex_buffer.h"
#include "renderer/index_buffer.h"

unsigned int CustomRenderInterface::s_TextureCount = 1; // 0 is reserved for white texture

Variant CustomRenderInterface::windowResized(const Event* event)
{
	const Vector2& newSize = Extract(Vector2, event->getData());
	m_Width = newSize.x;
	m_Height = newSize.y;
	return true;
}

CustomRenderInterface::CustomRenderInterface(int width, int height)
    : m_Width(width)
    , m_Height(height)
{
	BufferFormat format;
	format.push(VertexBufferElement::Type::FloatFloat, "POSITION");
	format.push(VertexBufferElement::Type::ByteByteByteByte, "COLOR");
	format.push(VertexBufferElement::Type::FloatFloat, "TEXCOORD");

	m_UIShader.reset(new BasicShader(L"rootex/assets/shaders/ui_vertex_shader.cso", L"rootex/assets/shaders/ui_pixel_shader.cso", format));

	m_Textures[0] = ResourceLoader::CreateImageResourceFile("rootex/assets/white.png")->getTexture();
}

void CustomRenderInterface::RenderGeometry(Rml::Vertex* vertices, int numVertices, int* indices, int numIndices, Rml::TextureHandle texture, const Rml::Vector2f& translation) 
{
	Vector<UIVertexData> vertexData;
	vertexData.assign((UIVertexData*)vertices, (UIVertexData*)vertices + numVertices);
	for (auto& vertex : vertexData)
	{
		vertex.m_Position.x += translation.x;
		vertex.m_Position.y += translation.y;
	}
	VertexBuffer vb(vertexData);
	Vector<int> indicesBuffer;
	indicesBuffer.assign(indices, indices + numIndices);
	IndexBuffer ib(indicesBuffer);

	vb.bind();
	ib.bind();
	m_UIShader->bind();

	Material::SetVSConstantBuffer(
	    VSSolidConstantBuffer(m_UITransform * Matrix::CreateOrthographic(m_Width, m_Height, 0.0f, 10000.0f)), 
		m_ModelMatrixBuffer,
		PER_OBJECT_VS_CPP);

	RenderingDevice::GetSingleton()->setInPixelShader(0, 1, m_Textures[texture]->getTextureResourceView());
	RenderingDevice::GetSingleton()->drawIndexed(ib.getCount());
}

Rml::CompiledGeometryHandle CustomRenderInterface::CompileGeometry(Rml::Vertex* vertices, int numVertices, int* indices, int numIndices, Rml::TextureHandle texture)
{
	return (Rml::CompiledGeometryHandle) nullptr;
}

void CustomRenderInterface::RenderCompiledGeometry(Rml::CompiledGeometryHandle geometry, const Rml::Vector2f& translation)
{
}

void CustomRenderInterface::ReleaseCompiledGeometry(Rml::CompiledGeometryHandle geometry)
{
}

bool CustomRenderInterface::LoadTexture(Rml::TextureHandle& textureHandle, Rml::Vector2i& textureDimensions, const String& source)
{
	ImageResourceFile* image = ResourceLoader::CreateImageResourceFile(source);

	if (image)
	{
		textureHandle = s_TextureCount;
		m_Textures[textureHandle] = image->getTexture();
		s_TextureCount++;

		return true;
	}
	return false;
}

bool CustomRenderInterface::GenerateTexture(Rml::TextureHandle& textureHandle, const byte* source, const Rml::Vector2i& sourceDimensions)
{
	textureHandle = s_TextureCount;
	m_Textures[textureHandle].reset(new Texture((const char*)source, sourceDimensions.x, sourceDimensions.y));
	s_TextureCount++;
	return m_Textures[textureHandle] != nullptr;
}

void CustomRenderInterface::ReleaseTexture(Rml::TextureHandle texture)
{
	m_Textures[texture].reset();
}

void CustomRenderInterface::EnableScissorRegion(bool enable)
{
	if (enable)
	{
		RenderingDevice::GetSingleton()->setTemporaryUIScissoredRS();
	}
	else
	{
		RenderingDevice::GetSingleton()->setTemporaryUIRS();
	}
}

void CustomRenderInterface::SetScissorRegion(int x, int y, int width, int height)
{
	RenderingDevice::GetSingleton()->setScissorRectangle(x, y, width, height);
}

void CustomRenderInterface::SetTransform(const Rml::Matrix4f* transform)
{
	if (!transform)
	{
		m_UITransform = Matrix::Identity;
		return;
	}
	m_UITransform = Matrix(transform->data());
}
