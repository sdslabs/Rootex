#pragma once

// Smart pointers
#include <memory>
template <class T>
using Ptr = std::unique_ptr<T>;
template <class T>
using Ref = std::shared_ptr<T>;
template <class T>
using Weak = std::weak_ptr<T>;

// Containers
#include <string>
typedef std::string String;

#include <map>
template <class P, class Q>
using Map = std::map<P, Q>;

#include <unordered_map>
template <class P, class Q>
using HashMap = std::unordered_map<P, Q>;

#include <vector>
template <class T>
using Vector = std::vector<T>;

#include <filesystem>
using FilePath = std::filesystem::path;

// Math Containers
#include <d3d11.h>
#include "vendor/SimpleMath/SimpleMath.h"
typedef DirectX::SimpleMath::Matrix Matrix;
typedef DirectX::SimpleMath::Vector2 Vector2;
typedef DirectX::SimpleMath::Vector3 Vector3;
typedef DirectX::SimpleMath::Vector4 Vector4;
typedef DirectX::SimpleMath::Color Color;