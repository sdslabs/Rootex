Audio
=====

Audio in Rootex has been implemented using OpenAL 1.1. Rootex supports both stereo and mono sound effects, as well as longer duration music.

Rootex is aware of the delay that might occur while trying to play large length audio pieces at once. To rectify this, Rootex implements Audio Streaming with :ref:`Class MusicComponent` and shorter sound effects that need to be loaded and played as fast as possible are implemented as :ref:`Class ShortMusicComponent`.

Rootex also supports audio attenuation models like Linear, Exponential and their respective clamped versions, as offered by OpenAL. However, audio attenuation works only with mono channel audio pieces.
