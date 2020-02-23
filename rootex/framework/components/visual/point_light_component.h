#include "component.h"
#include "common/types.h"

class PointLightComponent : public Component
{
	static Component* Create(const LuaVariable& componentData);

	friend class EntityFactory;

public:
	static const ComponentID s_ID = (ComponentID)ComponentIDs::PointLightComponent;

	float m_constAtt;
	float m_linAtt;
	float m_quadAtt;
	float m_range;
	float m_diffuseIntensity;
	Color m_diffuseColor;
	Color m_ambientColor;
	
	ComponentID getComponentID() const override { return s_ID; }

	PointLightComponent::PointLightComponent(const float constAtt, const float linAtt, const float quadAtt,
	    const float range, const float diffuseIntensity, const Color& diffuseColor, const Color& ambientColor);
	PointLightComponent(PointLightComponent&) = delete;
	~PointLightComponent();
};
