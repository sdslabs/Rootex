#include "component.h"
#include "common/common.h"

/// Component to apply directional light to the scene, only the first created instance is used in case of multiple such components
class DirectionalLightComponent : public Component
{
	static Component* Create(const JSON::json& componentData);
	static Component* CreateDefault();

	friend class EntityFactory;

public:
	static const ComponentID s_ID = (ComponentID)ComponentIDs::DirectionalLightComponent;

	/// Direction of light
	Vector3 m_Direction;
	/// Diffuse intensity of light
	float m_DiffuseIntensity;
	/// Diffuse color of light
	Color m_DiffuseColor;
	/// Ambient color of light
	Color m_AmbientColor;
	
	virtual String getName() const override { return "DirectionalLightComponent"; }
	ComponentID getComponentID() const override { return s_ID; }

	DirectionalLightComponent(const Vector3& direction, const float diffuseIntensity,
	    const Color& diffuseColor, const Color& ambientColor);
	DirectionalLightComponent(DirectionalLightComponent&) = delete;
	~DirectionalLightComponent();
	virtual JSON::json getJSON() const override;

#ifdef ROOTEX_EDITOR
	void draw() override;
#endif // ROOTEX_EDITOR
};
