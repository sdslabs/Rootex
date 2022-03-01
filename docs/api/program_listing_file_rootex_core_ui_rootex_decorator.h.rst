
.. _program_listing_file_rootex_core_ui_rootex_decorator.h:

Program Listing for File rootex_decorator.h
===========================================

|exhale_lsh| :ref:`Return to documentation for file <file_rootex_core_ui_rootex_decorator.h>` (``rootex\core\ui\rootex_decorator.h``)

.. |exhale_lsh| unicode:: U+021B0 .. UPWARDS ARROW WITH TIP LEFTWARDS

.. code-block:: cpp

   #pragma once
   
   #include "common/common.h"
   
   #include "RmlUi/Core/Decorator.h"
   
   class RootexDecorator : public Rml::Decorator
   {
       static inline Vector<RootexDecorator*> s_DecoratorInstances;
   
   public:
       static void UpdateAll(float deltaSeconds);
   
       RootexDecorator();
       virtual ~RootexDecorator();
   
       virtual void update(float deltaSeconds) = 0;
   };
