/*
 
 Ngine v5.0
 
 Module      : Metal API
 Requirements: none
 Description : Rendering context supports window
               creation and management of graphics
               resources. It allows programmer to
               use easy abstraction layer that
               removes from him platform dependent
               implementation of graphic routines.
               This module implements Metal backend.
 
 */

#ifndef ENG_CORE_RENDERING_METAL
#define ENG_CORE_RENDERING_METAL

#pragma push_macro("aligned")
#undef aligned
#import <Metal/Metal.h>
#import <QuartzCore/CAMetalLayer.h>
#pragma pop_macro("aligned")

#endif
