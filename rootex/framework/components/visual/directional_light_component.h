#include "component.h"
#include "common/types.h"

class DirectionalLightComponent : public Component
{
	static Component* Create(const JSON::json& componentData);
	static Component* CreateDefault();

	friend class EntityFactory;

public:
	static const ComponentID s_ID = (ComponentID)ComponentIDs::DirectionalLightComponent;

	Vector3 m_direction;
	float m_diffuseIntensity;
	Color m_diffuseColor;
	Color m_ambientColor;
	
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
