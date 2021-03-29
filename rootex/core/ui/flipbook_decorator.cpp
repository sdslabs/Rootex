#include "flipbook_decorator.h"

#include "RmlUi/Core/Element.h"
#include "RmlUi/Core/Spritesheet.h"

bool FlipbookDecorator::addFrame(const Rml::Sprite* sprite)
{
	if (AddTexture(sprite->sprite_sheet->texture) == -1)
	{
		return false;
	}
	m_SpriteRectangles.push_back(sprite->rectangle);
	return true;
}

void FlipbookDecorator::update(float deltaSeconds)
{
	for (auto& flipbookData : FlipbookElementData::s_DataInstances)
	{
		flipbookData->currentFrame += deltaSeconds * m_FPS;

		const int frameCount = m_SpriteRectangles.size();
		if (flipbookData->currentFrame < 0.0f)
		{
			flipbookData->currentFrame = frameCount + flipbookData->currentFrame;
		}
		if (flipbookData->currentFrame > frameCount)
		{
			float fraction = flipbookData->currentFrame / frameCount;
			fraction -= (int)fraction;
			flipbookData->currentFrame = fraction * frameCount;
		}
		flipbookData->currentFrame = std::clamp(flipbookData->currentFrame, 0.0f, (float)frameCount);
	}
}

Rml::DecoratorDataHandle FlipbookDecorator::GenerateElementData(Rml::Element* element) const
{
	FlipbookElementData* data = new FlipbookElementData();
	data->geometry.SetTexture(GetTexture());
	return (uintptr_t)data;
}

void FlipbookDecorator::ReleaseElementData(Rml::DecoratorDataHandle elementData) const
{
	delete (FlipbookElementData*)elementData;
}

void FlipbookDecorator::RenderElement(Rml::Element* element, Rml::DecoratorDataHandle elementData) const
{
	FlipbookElementData* data = (FlipbookElementData*)elementData;
	Rml::RenderInterface* renderInterface = element->GetRenderInterface();

	Vector<Rml::Vertex>& vertices = data->geometry.GetVertices();
	vertices.resize(4);
	Vector<int>& indices = data->geometry.GetIndices();
	indices.resize(6);

	Rml::Vertex vertex;
	vertex.colour = Rml::Colourb(255, 255, 255, 255);

	const Rml::Texture* texture = GetTexture();
	Rml::Rectangle rect = m_SpriteRectangles[(int)data->currentFrame];
	rect.x /= (float)texture->GetDimensions(renderInterface).x;
	rect.y /= (float)texture->GetDimensions(renderInterface).y;
	rect.width /= (float)texture->GetDimensions(renderInterface).x;
	rect.height /= (float)texture->GetDimensions(renderInterface).y;

	vertex.tex_coord = { rect.x, rect.y };
	vertex.position = Rml::Vector2f(0, 0);
	vertices[0] = vertex;

	const Rml::Vector2f& size = element->GetBox().GetSize(Rml::Box::PADDING);
	vertex.tex_coord = { rect.x, rect.y + rect.height };
	vertex.position = Rml::Vector2f(0, size.y);
	vertices[1] = vertex;

	vertex.tex_coord = { rect.x + rect.width, rect.y + rect.height };
	vertex.position = Rml::Vector2f(size.x, size.y);
	vertices[2] = vertex;

	vertex.tex_coord = { rect.x + rect.width, rect.y };
	vertex.position = Rml::Vector2f(size.x, 0);
	vertices[3] = vertex;

	indices[0] = 0;
	indices[1] = 1;
	indices[2] = 2;
	indices[3] = 0;
	indices[4] = 2;
	indices[5] = 3;

	data->geometry.Release();
	data->geometry.Render(element->GetAbsoluteOffset(Rml::Box::PADDING));
}

FlipbookDecoratorInstancer::FlipbookDecoratorInstancer()
    : Rml::DecoratorInstancer()
{
	m_FPSID = RegisterProperty("fps", "1").AddParser("number").GetId();
	m_SpritesID = RegisterProperty("sprites", "").AddParser("string").GetId();

	RegisterShorthand("decorator", "fps, sprites", Rml::ShorthandType::RecursiveCommaSeparated);
}

Rml::SharedPtr<Rml::Decorator> FlipbookDecoratorInstancer::InstanceDecorator(const Rml::String& name, const Rml::PropertyDictionary& properties, const Rml::DecoratorInstancerInterface& instancerInterface)
{
	const String& sprites = properties.GetProperty(m_SpritesID)->Get<String>();
	const float& fps = properties.GetProperty(m_FPSID)->Get<float>();

	Rml::SharedPtr<FlipbookDecorator> decorator(new FlipbookDecorator());
	Vector<String> spriteNames = Split(sprites, ' ');
	for (auto& sprite : spriteNames)
	{
		if (const Rml::Sprite* frame = instancerInterface.GetSprite(sprite))
		{
			if (!decorator->addFrame(frame))
			{
				Rml::Log::Message(Rml::Log::LT_WARNING, "Could not add sprite named '%s' to flipbook decorator.", sprite.c_str());
				return nullptr;
			}
		}
		else
		{
			Rml::Log::Message(Rml::Log::LT_WARNING, "Could not find sprite named '%s' in flipbook decorator.", sprite.c_str());
			return nullptr;
		}
	}
	decorator->setFPS(fps);

	return decorator;
}

FlipbookDecorator::FlipbookElementData::FlipbookElementData()
{
	s_DataInstances.push_back(this);
}

FlipbookDecorator::FlipbookElementData::~FlipbookElementData()
{
	s_DataInstances.erase(std::find(s_DataInstances.begin(), s_DataInstances.end(), this));
}
