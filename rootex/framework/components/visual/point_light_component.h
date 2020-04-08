#include "component.h"
#include "common/common.h"

/// Component to apply point lights to the scene, first 4 created instances of this are used
class PointLightComponent : public Component
{
	static Component* Create(const JSON::json& componentData);
	static Component* CreateDefault();

	friend class EntityFactory;

public:
	static const ComponentID s_ID = (ComponentID)ComponentIDs::PointLightComponent;

	/// attenuation = 1/(attConst + attLin * r + attQuad * r * r) 
	float m_AttConst;
	/// attenuation = 1/(attConst + attLin * r + attQuad * r * r)
	float m_AttLin;
	/// attenuation = 1/(attConst + attLin * r + attQuad * r * r)
	float m_AttQuad;
	/// Lighting effect clipped for distance > range
	float m_Range;
	/// Diffuse intensity of light
	float m_DiffuseIntensity;
	/// Diffuse color of light
	Color m_DiffuseColor;
	/// Ambient color of light
	Color m_AmbientColor;
	
	virtual String getName() const override { return "PointLightComponent"; }
	ComponentID getComponentID() const override { return s_ID; }

	PointLightComponent::PointLightComponent(const float constAtt, const float linAtt, const float quadAtt,
	    const float range, const float diffuseIntensity, const Color& diffuseColor, const Color& ambientColor);
	PointLightComponent(PointLightComponent&) = delete;
	~PointLightComponent();
	virtual JSON::json getJSON() const override;

#ifdef ROOTEX_EDITOR
	void draw() override;
#endif // ROOTEX_EDITOR
};
