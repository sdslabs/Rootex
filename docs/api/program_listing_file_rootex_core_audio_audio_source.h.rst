
.. _program_listing_file_rootex_core_audio_audio_source.h:

Program Listing for File audio_source.h
=======================================

|exhale_lsh| :ref:`Return to documentation for file <file_rootex_core_audio_audio_source.h>` (``rootex\core\audio\audio_source.h``)

.. |exhale_lsh| unicode:: U+021B0 .. UPWARDS ARROW WITH TIP LEFTWARDS

.. code-block:: cpp

   #pragma once
   
   #include "al.h"
   
   class StreamingAudioBuffer;
   class StaticAudioBuffer;
   
   typedef unsigned int ALuint;
   typedef float ALfloat;
   
   #define MIN_TO_S 60.0f
   
   class AudioSource
   {
   protected:
       ALuint m_SourceID;
   
       bool m_IsStreaming;
       AudioSource(bool isStreaming);
       virtual ~AudioSource();
   
   public:
       enum class AttenuationModel
       {
           Linear = AL_LINEAR_DISTANCE,
           Inverse = AL_INVERSE_DISTANCE,
           Exponential = AL_EXPONENT_DISTANCE,
           LinearClamped = AL_LINEAR_DISTANCE_CLAMPED,
           InverseClamped = AL_INVERSE_DISTANCE_CLAMPED,
           ExponentialClamped = AL_EXPONENT_DISTANCE_CLAMPED
       };
   
       virtual void setLooping(bool enabled);
       virtual void queueNewBuffers();
   
       void play();
       void pause();
       void stop();
   
       bool isPlaying() const;
       bool isPaused() const;
       bool isStopped() const;
       virtual bool isLooping() const;
       ALuint getSourceID() const;
       virtual float getDuration() const = 0;
   
       void setVelocity(const Vector3& velocity);
       void setVolume(float volume);
       void setPosition(Vector3& position);
       void setModel(AttenuationModel distanceModel);
       void setRollOffFactor(ALfloat rolloffFactor);
       void setReferenceDistance(ALfloat referenceDistance);
       void setMaxDistance(ALfloat maxDistance);
   };
   
   class StaticAudioSource : public AudioSource
   {
       Ref<StaticAudioBuffer> m_StaticAudio;
   
   public:
       StaticAudioSource(Ref<StaticAudioBuffer> audio);
       ~StaticAudioSource();
   
       void unqueueBuffers();
   
       float getDuration() const override;
       float getElapsedTimeS();
   };
   
   class StreamingAudioSource : public AudioSource
   {
       Ref<StreamingAudioBuffer> m_StreamingAudio;
   
       bool m_IsLooping;
   
   public:
       StreamingAudioSource(Ref<StreamingAudioBuffer> audio);
       ~StreamingAudioSource();
   
       bool isLooping() const override;
       void setLooping(bool enabled) override;
       void queueNewBuffers() override;
       void unqueueBuffers();
   
       virtual float getDuration() const override;
   };
