#include "custom_render_interface.h"

#include "core/resource_loader.h"
#include "core/resource_files/image_resource_file.h"
#include "renderer/rendering_device.h"
#include "renderer/shaders/register_locations_vertex_shader.h"

unsigned int CustomRenderInterface::s_TextureCount = 1; // 0 is reserved for white texture

struct PerUIVSCB
{
	Matrix Model;
	PerUIVSCB() = default;
	PerUIVSCB(const Matrix& model)
	{
		Model = model.Transpose();
	}
};

void CustomRenderInterface::render(GeometryData* geometry, const Rml::Vector2f& translation)
{
	geometry->vertexBuffer.bind();
	geometry->indexBuffer.bind();
	m_Shader->bind();

	RenderingDevice::GetSingleton()->editBuffer(PerUIVSCB(Matrix::CreateTranslation(translation.x, translation.y, 0.0f) * m_UITransform * Matrix::CreateOrthographic(m_Width, m_Height, 0.0f, 10000.0f)), m_ModelMatrixBuffer.Get());
	RenderingDevice::GetSingleton()->setVSCB(PER_OBJECT_VS_CPP, 1, m_ModelMatrixBuffer.GetAddressOf());

	ID3D11ShaderResourceView* textures[] = { m_Textures[geometry->textureHandle]->getTextureResourceView() };
	RenderingDevice::GetSingleton()->setPSSRV(DIFFUSE_PS_CPP, 1, textures);
	RenderingDevice::GetSingleton()->drawIndexed(geometry->indexBuffer.getCount());
}

Variant CustomRenderInterface::windowResized(const Event* event)
{
	const Vector2& newSize = Extract<Vector2>(event->getData());
	m_Width = newSize.x;
	m_Height = newSize.y;
	return true;
}

CustomRenderInterface::CustomRenderInterface(int width, int height)
    : m_Width(width)
    , m_Height(height)
    , m_Binder(this)
{
	m_Binder.bind(RootexEvents::WindowResized, &CustomRenderInterface::windowResized);

	BufferFormat format;
	format.push(VertexBufferElement::Type::FloatFloat, "POSITION", D3D11_INPUT_PER_VERTEX_DATA, 0, false, 0);
	format.push(VertexBufferElement::Type::ByteByteByteByte, "COLOR", D3D11_INPUT_PER_VERTEX_DATA, 0, false, 0);
	format.push(VertexBufferElement::Type::FloatFloat, "TEXCOORD", D3D11_INPUT_PER_VERTEX_DATA, 0, false, 0);
	m_Shader.reset(new Shader("rootex/core/renderer/shaders/ui_vertex_shader.hlsl", "rootex/core/renderer/shaders/ui_pixel_shader.hlsl", format));

	m_Textures[0] = ResourceLoader::CreateImageResourceFile("rootex/assets/white.png")->getTexture();

	m_ModelMatrixBuffer = RenderingDevice::GetSingleton()->createBuffer<PerUIVSCB>(PerUIVSCB(), D3D11_BIND_CONSTANT_BUFFER, D3D11_USAGE_DYNAMIC, D3D11_CPU_ACCESS_WRITE);
}

void CustomRenderInterface::RenderGeometry(Rml::Vertex* vertices, int numVertices, int* indices, int numIndices, Rml::TextureHandle texture, const Rml::Vector2f& translation)
{
	GeometryData geometry((UIVertexData*)vertices, numVertices, (int*)indices, numIndices, texture);
	render(&geometry, translation);
}

Rml::CompiledGeometryHandle CustomRenderInterface::CompileGeometry(Rml::Vertex* vertices, int numVertices, int* indices, int numIndices, Rml::TextureHandle texture)
{
	return (Rml::CompiledGeometryHandle) new GeometryData((UIVertexData*)vertices, numVertices, (int*)indices, numIndices, texture);
}

void CustomRenderInterface::RenderCompiledGeometry(Rml::CompiledGeometryHandle geometry, const Rml::Vector2f& translation)
{
	render((GeometryData*)geometry, translation);
}

void CustomRenderInterface::ReleaseCompiledGeometry(Rml::CompiledGeometryHandle geometry)
{
	delete (GeometryData*)geometry;
}

bool CustomRenderInterface::LoadTexture(Rml::TextureHandle& textureHandle, Rml::Vector2i& textureDimensions, const String& source)
{
	Ref<ImageResourceFile> image = ResourceLoader::CreateImageResourceFile(source);

	if (image)
	{
		textureHandle = s_TextureCount;
		m_Textures[textureHandle] = image->getTexture();
		textureDimensions.x = image->getWidth();
		textureDimensions.y = image->getHeight();
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

CustomRenderInterface::GeometryData::GeometryData(const UIVertexData* vertices, size_t verticesSize, int* indices, size_t indicesSize, Rml::TextureHandle texture)
    : vertexBuffer((const char*)vertices, verticesSize, sizeof(UIVertexData), D3D11_USAGE_IMMUTABLE, 0)
    , indexBuffer(indices, indicesSize)
    , textureHandle(texture)
{
}
