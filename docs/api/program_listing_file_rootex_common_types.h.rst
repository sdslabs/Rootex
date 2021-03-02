
.. _program_listing_file_rootex_common_types.h:

Program Listing for File types.h
================================

|exhale_lsh| :ref:`Return to documentation for file <file_rootex_common_types.h>` (``rootex\common\types.h``)

.. |exhale_lsh| unicode:: U+021B0 .. UPWARDS ARROW WITH TIP LEFTWARDS

.. code-block:: cpp

   #pragma once
   
   // target Windows 7 or later
   #ifndef _WIN32_WINNT
   #define _WIN32_WINNT 0x0601
   #endif // !_WIN32_WINNT
   #include <sdkddkver.h>
   
   #define NOMINMAX
   #define STRICT
   #include <windows.h>
   
   #define NS_TO_MS 1e-6f
   #define MS_TO_NS 1e+6f
   #define MS_TO_S 1e-3f
   #define S_TO_MS 1e+3f
   
   #include <future>
   template <class T>
   using Future = std::future<T>;
   
   #include <future>
   template <class T>
   using Promise = std::promise<T>;
   
   #include <atomic>
   template <class T>
   using Atomic = std::atomic<T>;
   
   // Smart pointers
   #include <memory>
   template <class T>
   using Ptr = std::unique_ptr<T>;
   template <class T>
   using Ref = std::shared_ptr<T>;
   template <class T>
   using Weak = std::weak_ptr<T>;
   #include <wrl.h> // For using Microsoft::WRL::ComPtr<T>
   
   // Serialization streams
   #include <fstream>
   typedef std::fstream InputOutputFileStream;
   typedef std::ofstream OutputFileStream;
   typedef std::ifstream InputFileStream;
   #include <sstream>
   typedef std::stringstream StringStream;
   
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
   template <typename... P>
   using Tuple = std::tuple<P...>;
   template <class P, class Q>
   using Pair = std::pair<P, Q>;
   
   #include <optional>
   template <class T>
   using Optional = std::optional<T>;
   
   #include <vector>
   template <class T>
   using Vector = std::vector<T>;
   
   #include <stack>
   template <class T>
   using Stack = std::stack<T>;
   
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
   typedef DirectX::SimpleMath::Ray Ray;
   typedef DirectX::BoundingBox BoundingBox;
   typedef DirectX::SimpleMath::Color Color;
   
   #include <DirectXColors.h>
   namespace ColorPresets = DirectX::Colors;
   
   #include <variant>
   typedef Vector<std::variant<bool, int, char, float, String, Vector2, Vector3, Vector4, Matrix>> VariantVector;
   class Scene;
   class Entity;
   using Variant = std::variant<bool, int, char, float, String, Vector2, Vector3, Vector4, Matrix, VariantVector, Scene*, Entity*, Vector<String>>;
   template <typename P, typename Q>
   P Extract(const Q& v)
   {
       return std::get<P>(v);
   }
   
   #include "JSON/json.hpp"
   namespace JSON = nlohmann;
   
   namespace nlohmann
   {
   template <>
   struct adl_serializer<Vector2>
   {
       static void to_json(json& j, const Vector2& v)
       {
           j["x"] = v.x;
           j["y"] = v.y;
       }
   
       static void from_json(const json& j, Vector2& v)
       {
           v.x = j.at("x");
           v.y = j.at("y");
       }
   };
   template <>
   struct adl_serializer<Vector3>
   {
       static void to_json(json& j, const Vector3& v)
       {
           j["x"] = v.x;
           j["y"] = v.y;
           j["z"] = v.z;
       }
   
       static void from_json(const json& j, Vector3& v)
       {
           v.x = j.at("x");
           v.y = j.at("y");
           v.z = j.at("z");
       }
   };
   template <>
   struct adl_serializer<Vector4>
   {
       static void to_json(json& j, const Vector4& v)
       {
           j["x"] = v.x;
           j["y"] = v.y;
           j["z"] = v.z;
           j["w"] = v.w;
       }
   
       static void from_json(const json& j, Vector4& v)
       {
           v.x = j.at("x");
           v.y = j.at("y");
           v.z = j.at("z");
           v.w = j.at("w");
       }
   };
   template <>
   struct adl_serializer<Color>
   {
       static void to_json(json& j, const Color& v)
       {
           j["r"] = v.R();
           j["g"] = v.G();
           j["b"] = v.B();
           j["a"] = v.A();
       }
   
       static void from_json(const json& j, Color& v)
       {
           v.x = j.at("r");
           v.y = j.at("g");
           v.z = j.at("b");
           v.w = j.at("a");
       }
   };
   template <>
   struct adl_serializer<Quaternion>
   {
       static void to_json(json& j, const Quaternion& v)
       {
           j["x"] = v.x;
           j["y"] = v.y;
           j["z"] = v.z;
           j["w"] = v.w;
       }
   
       static void from_json(const json& j, Quaternion& v)
       {
           v.x = j.value("x", 0.0f);
           v.y = j.value("y", 0.0f);
           v.z = j.value("z", 0.0f);
           v.w = j.value("w", 0.0f);
       }
   };
   template <>
   struct adl_serializer<Matrix>
   {
       static void to_json(json& j, const Matrix& v)
       {
           for (int x = 0; x < 4; x++)
           {
               for (int y = 0; y < 4; y++)
               {
                   j.push_back(v.m[x][y]);
               }
           }
       }
   
       static void from_json(const json& j, Matrix& v)
       {
           for (int x = 0; x < 4; x++)
           {
               for (int y = 0; y < 4; y++)
               {
                   v.m[x][y] = j[x * 4 + y];
               }
           }
       }
   };
   template <>
   struct adl_serializer<BoundingBox>
   {
       static void to_json(json& j, const BoundingBox& v)
       {
           j["center"] = (Vector3)v.Center;
           j["extents"] = (Vector3)v.Extents;
       }
   
       static void from_json(const json& j, BoundingBox& v)
       {
           v.Center = (Vector3)j.value("center", Vector3::Zero);
           v.Extents = (Vector3)j.value("extents", Vector3 { 0.5f, 0.5f, 0.5f });
       }
   };
   }
   
   #include <functional>
   template <class T>
   using Function = std::function<T>;
   
   #include "imgui.h"
   #include "utility/imgui_helpers.h"
   #include "editor/editor_events.h"
   #include "event.h"
