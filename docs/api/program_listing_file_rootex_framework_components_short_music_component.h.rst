
.. _program_listing_file_rootex_framework_components_short_music_component.h:

Program Listing for File short_music_component.h
================================================

|exhale_lsh| :ref:`Return to documentation for file <file_rootex_framework_components_short_music_component.h>` (``rootex\framework\components\short_music_component.h``)

.. |exhale_lsh| unicode:: U+021B0 .. UPWARDS ARROW WITH TIP LEFTWARDS

.. code-block:: cpp

   #pragma once
   
   #include "audio_component.h"
   
   #include "core/audio/audio_source.h"
   #include "core/audio/static_audio_buffer.h"
   #include "core/event_manager.h"
   #include "core/resource_loader.h"
   
   class ShortMusicComponent : public AudioComponent
   {
       static Component* Create(const JSON::json& componentData);
   
       Ref<StaticAudioSource> m_StaticAudioSource;
       Ref<StaticAudioBuffer> m_StaticAudioBuffer;
       AudioResourceFile* m_AudioFile;
   
       ShortMusicComponent(AudioResourceFile* audioFile, bool playOnStart, bool isLooping, bool attenuation, AudioSource::AttenuationModel model, ALfloat rolloffFactor, ALfloat referenceDistance, ALfloat maxDistance);
       virtual ~ShortMusicComponent();
   
       friend class ECSFactory;
   
   public:
       static const ComponentID s_ID = (ComponentID)ComponentIDs::ShortMusicComponent;
   
       virtual bool setupData() override;
   
       AudioResourceFile* getAudioFile() const { return m_AudioFile; }
       void setAudioFile(AudioResourceFile* audioFile);
   
       virtual ComponentID getComponentID() const override { return s_ID; };
       virtual const char* getName() const override { return "ShortMusicComponent"; };
       virtual JSON::json getJSON() const override;
   
       void draw() override;
   };
