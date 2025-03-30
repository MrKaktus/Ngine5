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

#include <assert.h>

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
    _display(nullptr),
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

std::shared_ptr<Display> CommonWindow::display(void) const
{
    return _display;
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
   
//Texture* CommonWindow::surface(void)
//{
//    // Should be implemented by specialization class.
//    assert( 0 );
//    return nullptr;
//}
//
//void CommonWindow::present(void)
//{
//    // Should be implemented by specialization class.
//    assert( 0 );
//}

uint32v4 windowBorders(void)
{
    // TODO: Those values are hardcoded based on Windows 10.
    //       They should be queried by engine on application 
    //       start based on backing OS.
    return uint32v4(8, 8, 32, 8);
}

} // en::gpu
} // en
