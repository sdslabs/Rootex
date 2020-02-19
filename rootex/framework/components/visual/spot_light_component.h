#include "component.h"
#include "common/types.h"

class SpotLightComponent : public Component
{
	static Component* Create(const LuaVariable& componentData);

	friend class EntityFactory;

public:
	static const ComponentID s_ID = (ComponentID)ComponentIDs::SpotLightComponent;

	float m_constAtt;
	float m_linAtt;
	float m_quadAtt;
	float m_range;
	float m_diffuseIntensity;
	Color m_diffuseColor;
	Color m_ambientColor;
	float m_spot;
	
	ComponentID getComponentID() const override { return s_ID; }

	SpotLightComponent::SpotLightComponent(const float constAtt, const float linAtt, const float quadAtt,
	    const float range, const float diffuseIntensity, const Color& diffuseColor, const Color& ambientColor,
		float spot);
	SpotLightComponent(SpotLightComponent&) = delete;
	~SpotLightComponent();
};
