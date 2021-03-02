
.. _program_listing_file_rootex_core_random.h:

Program Listing for File random.h
=================================

|exhale_lsh| :ref:`Return to documentation for file <file_rootex_core_random.h>` (``rootex\core\random.h``)

.. |exhale_lsh| unicode:: U+021B0 .. UPWARDS ARROW WITH TIP LEFTWARDS

.. code-block:: cpp

   #pragma once
   
   #include <random>
   
   class Random
   {
       static std::random_device s_RandomDevice;
       static std::uniform_real_distribution<double> s_Distribution;
   
   public:
       static float Float() { return s_Distribution(s_RandomDevice); }
   };
