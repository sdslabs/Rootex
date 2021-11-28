//#include "heightmap_component.h"
//
//#include "core/resource_loader.h"
//
//HeightmapComponent::HeightmapComponent(Entity& owner, const JSON::json& data)
//    : Component(owner)
//    , m_CollisionGroup(collisionGroup)
//    , m_CollisionMask(collisionMask)
//{
//}
//
//JSON::json HeightmapComponent::getJSON() const
//{
//	JSON::json j;
//
//	return j;
//}
//
//void HeightmapComponent::generate()
//{
//	static Ref<ImageResourceFile> heightmapImage = ResourceLoader::CreateImageResourceFile(imageFilePath);
//	float strideX = float(heightmapImage->getWidth()) / x;
//	float strideY = float(heightmapImage->getHeight()) / y;
//
//	for (int i = 0; i <= x; i++)
//	{
//		for (int j = 0; j <= y; j++)
//		{
//			float currX = i * strideX;
//			float currY = j * strideY;
//
//		}
//	}
//}
//
//void HeightmapComponent::draw()
//{
//	//if (ImGui::TreeNodeEx("Collision Group"))
//	//{
//	//	displayCollisionLayers(m_CollisionGroup);
//	//	ImGui::TreePop();
//	//}
//
//	//if (ImGui::TreeNodeEx("Collision Mask"))
//	//{
//	//	displayCollisionLayers(m_CollisionMask);
//	//	ImGui::TreePop();
//	//}
//}
