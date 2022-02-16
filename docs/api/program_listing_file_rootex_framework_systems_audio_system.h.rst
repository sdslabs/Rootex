
.. _program_listing_file_rootex_framework_systems_audio_system.h:

Program Listing for File audio_system.h
=======================================

|exhale_lsh| :ref:`Return to documentation for file <file_rootex_framework_systems_audio_system.h>` (``rootex\framework\systems\audio_system.h``)

.. |exhale_lsh| unicode:: U+021B0 .. UPWARDS ARROW WITH TIP LEFTWARDS

.. code-block:: cpp

   #pragma once
   
   #include "common/common.h"
   
   #include "components/space/transform_component.h"
   #include "components/audio/audio_listener_component.h"
   
   #include "al.h"
   #include "alc.h"
   #include "alut.h"
   
   #include "system.h"
   
   #ifndef ALUT_CHECK
   #ifdef _DEBUG
   #define ALUT_CHECK(alutFunction)                                        \
       do                                                                  \
       {                                                                   \
           alutFunction;                                                   \
           AudioSystem::CheckALUTError(#alutFunction, __FILE__, __LINE__); \
       } while (0)
   #else
   #define ALUT_CHECK(alutFunction) alutFunction
   #endif
   #endif
   
   #ifndef AL_CHECK
   #ifdef _DEBUG
   #define AL_CHECK(alFunction)                                        \
       do                                                              \
       {                                                               \
           alFunction;                                                 \
           AudioSystem::CheckALError(#alFunction, __FILE__, __LINE__); \
       } while (0)
   #else
   #define AL_CHECK(alFunction) alFunction
   #endif
   #endif
   
   class AudioBuffer;
   class StaticAudioBuffer;
   class StreamingAudioBuffer;
   
   class AudioSource;
   class StreamingAudioSource;
   
   class ResourceFile;
   
   class AudioSystem : public System
   {
       ALCdevice* m_Device = nullptr;
       ALCcontext* m_Context = nullptr;
   
       AudioListenerComponent* m_Listener = nullptr;
   
       AudioSystem();
       AudioSystem(AudioSystem&) = delete;
       virtual ~AudioSystem() = default;
   
   public:
       static AudioSystem* GetSingleton();
   
       static String GetALErrorString(int errID);
       static String GetALCErrorString(int errID);
       static void CheckALError(const char* msg, const char* fname, int line);
       static void CheckALCError(const char* msg, const char* fname, int line);
       static void CheckALUTError(const char* msg, const char* fname, int line);
   
       AudioListenerComponent* getListener() const { return m_Listener; }
       void setListener(AudioListenerComponent* listenerComponent);
   
       void restoreListener();
   
       bool initialize(const JSON::json& systemData) override;
       void setConfig(const SceneSettings& sceneSettings) override;
       void shutDown();
   
       void update(float deltaMilliseconds) override;
       void begin() override;
       void end() override;
   };
