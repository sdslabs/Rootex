#include "resource_file.h"

#include <bitset>
#include <sstream>

#include "resource_loader.h"
#include "framework/systems/audio_system.h"
#include "renderer/rendering_device.h"
#include "interpreter.h"

ResourceFile::ResourceFile(const Type& type, ResourceData* resData)
    : m_Type(type)
    , m_ResourceData(resData)
{
	PANIC(resData == nullptr, "Null resource found. Resource of this type has not been loaded correctly: " + std::to_string((int)type));
	m_LastReadTime = OS::s_FileSystemClock.now();
	m_LastChangedTime = OS::GetFileLastChangedTime(getPath().string());
}

bool ResourceFile::isValid()
{
	return m_ResourceData != nullptr;
}

bool ResourceFile::isOpen()
{
	return m_ResourceData == nullptr;
}

FilePath ResourceFile::getPath() const
{
	return m_ResourceData->getPath();
}

ResourceFile::Type ResourceFile::getType() const
{
	return m_Type;
}

ResourceData* ResourceFile::getData()
{
	return m_ResourceData;
}

const FileTimePoint& ResourceFile::getLastChangedTime()
{
	m_LastChangedTime = OS::GetFileLastChangedTime(getPath().string());
	return m_LastChangedTime;
}

bool ResourceFile::isDirty()
{
	return getLastReadTime() < getLastChangedTime();
}

AudioResourceFile::AudioResourceFile(ResourceData* resData)
    : ResourceFile(Type::Wav, resData)
    , m_AudioDataSize(0)
    , m_BitDepth(0)
    , m_Channels(0)
    , m_DecompressedAudioBuffer(nullptr)
    , m_Format(0)
    , m_Frequency(0)
{
}

TextResourceFile::TextResourceFile(const Type& type, ResourceData* resData)
    : ResourceFile(type, resData)
{
}

void TextResourceFile::putString(const String& newData)
{
	*m_ResourceData->getRawData() = FileBuffer(newData.begin(), newData.end());
}

void TextResourceFile::popBack()
{
	m_ResourceData->getRawData()->pop_back();
}

void TextResourceFile::append(const String& add)
{
	m_ResourceData->getRawData()->insert(m_ResourceData->getRawData()->end(), add.begin(), add.end());
}

String TextResourceFile::getString() const
{
	return String(
	    m_ResourceData->getRawData()->begin(),
	    m_ResourceData->getRawData()->end());
}

LuaTextResourceFile::LuaTextResourceFile(ResourceData* resData)
    : TextResourceFile(Type::Lua, resData)
{
}

VisualModelResourceFile::VisualModelResourceFile(Ptr<VertexBuffer> vertexBuffer, Ptr<IndexBuffer> indexBuffer, ResourceData* resData)
    : ResourceFile(Type::Obj, resData)
    , m_VertexBuffer(std::move(vertexBuffer))
    , m_IndexBuffer(std::move(indexBuffer))
{
}

SkeletalAnimationResourceFile::SkeletalAnimationResourceFile(
	const Vector<int>& boneHierarchy,
    const Vector<Matrix>& boneOffsets,
    const Map<String, SkeletalAnimation>& animations,
    Ptr<VertexBuffer> vertexBuffer,
    Ptr<IndexBuffer> indexBuffer,
    ResourceData* resData)
    : ResourceFile(Type::Animation, resData)
    , m_VertexBuffer(std::move(vertexBuffer))
    , m_IndexBuffer(std::move(indexBuffer))
    , m_BoneHierarchy(boneHierarchy)
    , m_BoneOffsets(boneOffsets)
    , m_BoneAnimations(animations)
{
}

void SkeletalAnimationResourceFile::getFinalTransforms(const String& animationName, float timePos, Vector<Matrix>& finalTransforms) const
{
	Vector<Matrix> toParentTransform;
	size_t boneCount = m_BoneOffsets.size();
	toParentTransform.resize(boneCount);

	auto& animation = m_BoneAnimations.find(animationName);
	animation->second.interpolate(toParentTransform, timePos);

	Vector<Matrix> toRootTransforms;
	toRootTransforms.resize(boneCount);

	toRootTransforms[0] = toParentTransform[0];

	for (size_t i = 0; i < boneCount; i++)
	{
		Matrix& toParent = toParentTransform[i];

		int parentIndex = m_BoneHierarchy[i];
		Matrix& parentToRoot = toRootTransforms[parentIndex];

		toRootTransforms[i] = parentToRoot * toParent;
	}

	for (size_t i = 0; i < boneCount; i++)
	{
		finalTransforms[i] = toRootTransforms[i] * m_BoneOffsets[i];
	}
}

unsigned int SkeletalAnimationResourceFile::getBoneCount() const
{
	return m_BoneOffsets.size();
}

float SkeletalAnimationResourceFile::getAnimationStartTime(const String& animationName)
{
	return m_BoneAnimations[animationName].getStartTime();
}

float SkeletalAnimationResourceFile::getAnimationEndTime(const String& animationName)
{
	return m_BoneAnimations[animationName].getEndTime();
}

ImageResourceFile::ImageResourceFile(ResourceData* resData)
    : ResourceFile(Type::Image, resData)
{
}

FontResourceFile::FontResourceFile(ResourceData* resData)
    : ResourceFile(Type::Font, resData)
{
	m_Font = RenderingDevice::GetSingleton()->createFont(resData->getRawData());
	m_Font->SetDefaultCharacter('X');
}
