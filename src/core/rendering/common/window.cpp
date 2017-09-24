/*
 
 Ngine v5.0
 
 Module      : Common Window.
 Requirements: none
 Description : Rendering context supports window
               creation and management of graphics
               resources. It allows programmer to
               use easy abstraction layer that
               removes from him platform dependent
               implementation of graphic routines.
 
 */

#include "core/rendering/common/window.h"

namespace en
{
   namespace gpu
   {
   WindowSettings::WindowSettings() :
      mode(BorderlessWindow),
      display(nullptr),
      position(0u, 0u),
      size(0u, 0u),
      format(Format::RGBA_8),
      resolution(0u, 0u),
      verticalSync(true)
   {
   }

   CommonWindow::CommonWindow() :
      _mode(WindowMode::Windowed),
      _frame(0u),
      needNewSurface(true),
      verticalSync(true),
      Window()
   {
   }

   CommonWindow::~CommonWindow()
   {
   _display = nullptr;
   }

   Ptr<Display> CommonWindow::display(void) const
   {
   return ptr_reinterpret_cast<Display>(&_display);
   }

   uint32v2 CommonWindow::position(void) const
   {
   return _position;
   }

   uint32v2 CommonWindow::size(void) const
   {
   return _size;
   }
   
   uint32v2 CommonWindow::resolution(void) const
   {
   return _resolution;
   }

   uint32 CommonWindow::frame(void) const
   {
   return _frame;
   }

   void CommonWindow::transparent(const float opacity)
   {
   // Should be implemented by specialization class.
   assert( 0 );
   }
   
   void CommonWindow::opaque(void)
   {
   // Should be implemented by specialization class.
   assert( 0 );
   }
   
   //Ptr<Texture> CommonWindow::surface(void)
   //{
   //// Should be implemented by specialization class.
   //assert( 0 );
   //return Ptr<Texture>(nullptr);
   //}
   //
   //void CommonWindow::present(void)
   //{
   //// Should be implemented by specialization class.
   //assert( 0 );
   //}

   }
}