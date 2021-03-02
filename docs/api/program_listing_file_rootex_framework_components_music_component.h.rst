
.. _program_listing_file_rootex_framework_components_music_component.h:

Program Listing for File music_component.h
==========================================

|exhale_lsh| :ref:`Return to documentation for file <file_rootex_framework_components_music_component.h>` (``rootex\framework\components\music_component.h``)

.. |exhale_lsh| unicode:: U+021B0 .. UPWARDS ARROW WITH TIP LEFTWARDS

.. code-block:: cpp

   #pragma once
   
   #include "audio_component.h"
   
   #include "core/audio/audio_source.h"
   #include "core/audio/streaming_audio_buffer.h"
   #include "core/event_manager.h"
   #include "core/resource_loader.h"
   
   class MusicComponent : public AudioComponent
   {
       static Component* Create(const JSON::json& componentData);
   
       Ref<StreamingAudioSource> m_StreamingAudioSource;
       Ref<StreamingAudioBuffer> m_StreamingAudioBuffer;
       AudioResourceFile* m_AudioFile;
   
       MusicComponent(AudioResourceFile* audioFile, bool playOnStart, bool isLooping, bool attenuation, AudioSource::AttenuationModel model, ALfloat rolloffFactor, ALfloat referenceDistance, ALfloat maxDistance);
       virtual ~MusicComponent();
   
       friend class ECSFactory;
   
   public:
       static const ComponentID s_ID = (ComponentID)ComponentIDs::MusicComponent;
   
       virtual bool setupData() override;
   
       AudioResourceFile* getAudioFile() const { return m_AudioFile; }
       void setAudioFile(AudioResourceFile* audioFile);
   
       virtual const char* getName() const override { return "MusicComponent"; };
       virtual ComponentID getComponentID() const override { return s_ID; };
       virtual JSON::json getJSON() const override;
   
       void draw() override;
   };
