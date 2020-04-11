#include "component.h"
#include "common/common.h"

/// Component to apply point lights to the scene, first 4 created instances of this are used
class SpotLightComponent : public Component
{
	static Component* Create(const JSON::json& componentData);
	static Component* CreateDefault();

	friend class EntityFactory;

public:
	static const ComponentID s_ID = (ComponentID)ComponentIDs::SpotLightComponent;

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
	/// Increasing spot increases the angular attenuation wrt axis
	float m_Spot;
	/// Lighting effect clipped for angle > angleRange
	float m_AngleRange;
	
	virtual String getName() const override { return "SpotLightComponent"; }
	ComponentID getComponentID() const override { return s_ID; }

	SpotLightComponent::SpotLightComponent(const float constAtt, const float linAtt, const float quadAtt,
	    const float range, const float diffuseIntensity, const Color& diffuseColor, const Color& ambientColor,
		float spot, float angleRange);
	SpotLightComponent(SpotLightComponent&) = delete;
	~SpotLightComponent();
	virtual JSON::json getJSON() const override;

#ifdef ROOTEX_EDITOR
	void draw() override;
#endif // ROOTEX_EDITOR
};
