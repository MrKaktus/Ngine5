/*

 Ngine v5.0
 
 Module      : Sampler.
 Requirements: none
 Description : Rendering context supports window
               creation and management of graphics
               resources. It allows programmer to
               use easy abstraction layer that 
               removes from him platform dependent
               implementation of graphic routines.

*/

#include "core/rendering/common/sampler.h"

namespace en
{
namespace gpu
{

// Metal support table for Warping modes:
//
// Repeat
// RepeatMirrored
// ClampToEdge
// ClampToBorder     (macOS 10.12+)
// MirrorClampToEdge (macOS 10.11+)
   
SamplerState::SamplerState() :
    minification(SamplerFilter::Linear),
    magnification(SamplerFilter::Linear),
    mipmap(SamplerMipMapMode::Linear),
    anisotropy(1),
    coordU(SamplerAdressing::ClampToEdge),
    coordV(SamplerAdressing::ClampToEdge),
    coordW(SamplerAdressing::ClampToEdge),
    borderColor(SamplerBorder::OpaqueBlack),
    compare(CompareOperation::Always),   // Disabled depth test, "LessOrEqual" typical depth test
    LodBias(0.0f),
    minLod(-1000.0f),
    maxLod(1000.0f) 
{
}

Sampler::~Sampler()
{
}

} // en::gpu
} // en
