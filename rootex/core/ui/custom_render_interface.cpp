#include "custom_render_interface.h"

#include "core/resource_loader.h"
#include "renderer/rendering_device.h"
#include "renderer/shaders/register_locations_vertex_shader.h"

unsigned int CustomRenderInterface::s_TextureCount = 0;

CustomRenderInterface::CustomRenderInterface(int width, int height)
    : m_Width(width)
    , m_Height(height)
{
	BufferFormat format;
	format.push(VertexBufferElement::Type::FloatFloat, "POSITION");
	format.push(VertexBufferElement::Type::ByteByteByteByte, "COLOR");
	format.push(VertexBufferElement::Type::FloatFloat, "TEXCOORD");

	m_UIShader.reset(new BasicShader(L"rootex/assets/shaders/ui_vertex_shader.cso", L"rootex/assets/shaders/ui_pixel_shader.cso", format));

	m_Textures[0].reset(new Texture(ResourceLoader::CreateImageResourceFile("rootex/assets/white.png")));
}

void CustomRenderInterface::RenderGeometry(Rml::Core::Vertex* vertices, int numVertices, int* indices, int numIndices, Rml::Core::TextureHandle texture, const Rml::Core::Vector2f& translation) 
{
	Vector<UIVertexData> vertexData;
	vertexData.assign((UIVertexData*)vertices, (UIVertexData*)vertices + numVertices);
	VertexBuffer vb(vertexData);
	Vector<int> indicesBuffer;
	indicesBuffer.assign(indices, indices + numIndices);
	IndexBuffer ib(indicesBuffer);

	vb.bind();
	ib.bind();
	m_UIShader->bind();

	Material::SetVSConstantBuffer(
	    VSSolidConstantBuffer(Matrix::CreateTranslation(translation.x - m_Width / 2.0, translation.y, 0.0f) * Matrix::CreateOrthographic(m_Width, m_Height, 0.0f, 1.0f)), 
		m_ModelMatrixBuffer,
		PER_OBJECT_VS_CPP);

	RenderingDevice::GetSingleton()->setInPixelShader(0, 1, m_Textures[texture]->getTextureResourceView());

	RenderingDevice::GetSingleton()->drawIndexed(ib.getCount());
}

Rml::Core::CompiledGeometryHandle CustomRenderInterface::CompileGeometry(Rml::Core::Vertex* vertices, int numVertices, int* indices, int numIndices, Rml::Core::TextureHandle texture)
{
	return (Rml::Core::CompiledGeometryHandle) nullptr;
}

void CustomRenderInterface::RenderCompiledGeometry(Rml::Core::CompiledGeometryHandle geometry, const Rml::Core::Vector2f& translation)
{
}

void CustomRenderInterface::ReleaseCompiledGeometry(Rml::Core::CompiledGeometryHandle geometry)
{
}

bool CustomRenderInterface::LoadTexture(Rml::Core::TextureHandle& textureHandle, Rml::Core::Vector2i& textureDimensions, const String& source)
{
	ImageResourceFile* image = ResourceLoader::CreateImageResourceFile(source);

	if (image)
	{
		textureHandle = s_TextureCount;
		m_Textures[textureHandle].reset(new Texture(image));
		s_TextureCount++;

		return true;
	}
	return false;
}

bool CustomRenderInterface::GenerateTexture(Rml::Core::TextureHandle& textureHandle, const byte* source, const Rml::Core::Vector2i& sourceDimensions)
{
	textureHandle = s_TextureCount;
	m_Textures[textureHandle].reset(new Texture((const char*)source, sourceDimensions.x, sourceDimensions.y));
	s_TextureCount++;
	return m_Textures[textureHandle] != nullptr;
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
	static Rml::Core::Matrix4f identity = Rml::Core::Matrix4f::Identity();

	if (!transform)
	{
		transform = &identity;
	}

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
