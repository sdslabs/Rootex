#pragma once

// Smart pointers
#include <memory>
template <class T>
using Ptr = std::unique_ptr<T>;
template <class T>
using Ref = std::shared_ptr<T>;
template <class T>
using Weak = std::weak_ptr<T>;
#include <wrl.h> // For using Microsoft::WRL::ComPtr<T>

// Containers
#include <string>
typedef std::string String;

#include <map>
template <class P, class Q>
using Map = std::map<P, Q>;

#include <unordered_map>
template <class P, class Q>
using HashMap = std::unordered_map<P, Q>;

#include <utility>
template <typename...P>
using Tuple = std::tuple<P...>;
template <class P, class Q>
using Pair = std::pair<P, Q>;

#include <vector>
template <class T>
using Vector = std::vector<T>;

#include <filesystem>
using FilePath = std::filesystem::path;

// Math Containers
#include <d3d11.h>
#include "vendor/DirectXTK/Inc/SimpleMath.h"
typedef DirectX::SimpleMath::Matrix Matrix;
typedef DirectX::SimpleMath::Vector2 Vector2;
typedef DirectX::SimpleMath::Vector3 Vector3;
typedef DirectX::SimpleMath::Vector4 Vector4;
typedef DirectX::SimpleMath::Quaternion Quaternion;
typedef DirectX::SimpleMath::Color Color;

#include <DirectXColors.h>
namespace ColorPresets = DirectX::Colors;

#include <variant>
typedef Vector<std::variant<bool, int, char, float, String, Vector2, Vector3, Vector4, Matrix>> VariantVector;
class Entity;
using Variant = std::variant<bool, int, char, float, String, Vector2, Vector3, Vector4, Matrix, VariantVector, Ref<Entity>, FilePath>;
#define Extract(TypeName, variant) std::get<TypeName>((variant))

#include <functional>
template <class T>
using Function = std::function<T>;
