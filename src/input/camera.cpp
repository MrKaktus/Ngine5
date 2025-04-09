/*

Ngine v5.0

Module      : Input controllers and devices.
Requirements: none
Description : Captures and holds signals from
              input devices and interfaces like
              keyboard, mouse or touchscreen.

*/

#include "core/log/log.h"
#include "input/context.h"

#include "utilities/strings.h"

#include "utilities/timer.h"

#ifdef EN_PLATFORM_WINDOWS

   #if MICROSOFT_KINECT
   #include "NuiApi.h"                  // Kinect ( SDK installs in: C:\Program Files\Microsoft SDKs\Kinect\v1.8 )
   #endif

#endif

namespace en
{
namespace input
{

#if 0 // Old Interface

//# INTERFACE
//##########################################################################

uint8 Interface::Camera::available(void) const
{
    return InputContext.camera.device.size();
}

std::shared_ptr<input::Camera> Interface::Camera::get(uint8 index) const
{
    if (index < InputContext.camera.device.size())
    {
        return InputContext.camera.device[index];
    }

    return std::shared_ptr<input::Camera>(nullptr);
}

//# IMPLEMENTATION
//##########################################################################

#ifdef EN_PLATFORM_WINDOWS
#if INTEL_PERCEPTUAL_COMPUTING_2014
class CreativeDepthCamera : public Camera
{
    public:
    virtual bool        on(void);           // Turns camera on
    virtual bool        off(void);          // Turns camera off
    virtual CameraState state(void) const;  // Returns camera state (on/off)
    virtual CameraType  type(void) const;   // Returns camera type
    virtual bool        support(CameraStream type) const; // Returns true if camera supports given stream type
    virtual CameraInfo  info(void) const;   // Returns all camera properties
    virtual void        update(void);       // Update camera stream events

    CreativeDepthCamera(uint32 id,
                        PXCCapture::Device* ptr,
                        PXCCapture::DeviceInfo info);
    virtual ~CreativeDepthCamera();        

    private: 
    uint32                 id;           // Camera id on the devices list
    CameraState            currentState; // Is it turned on
    std::string            name;         // Device name
    CameraInfo             settings;     // Camera properties
    PXCCapture::DeviceInfo deviceInfo;   // Device properties
    PXCCapture::Device*    device;       // Device handle

    uint32                 active;       // Bitfield of active streams
    StreamSettings         stream[8];    // Configuration of initiated stream

    // API specyfic streams description
    PXCCapture::Device::StreamProfileSet set; // Configuration of initiated streams 
    PXCCapture::Sample     sample[8];         // Sample from streams
    PXCSyncPoint*          sync[8];           // Sync points for streams
};

CreativeDepthCamera::CreativeDepthCamera(uint32 _id, PXCCapture::Device* ptr, PXCCapture::DeviceInfo _info) :
    id(_id),
    currentState(Initializing),
    name(stringFromWchar((wchar_t*)&_info.name, 1024)),
    deviceInfo(_info),
    device(ptr),
    active(0)
{
    memset(&settings, 0, sizeof(settings));
    memset(&stream[0], 0, sizeof(StreamSettings) * 8);
    //memset(&set, 0, sizeof(PXCCapture::Device::StreamProfileSet));
    for(uint8 i=0; i<8; ++i)
    {
        sync[i] = nullptr;
    }

    // Determine device type
    if (_info.model == PXCCapture::DEVICE_MODEL_IVCAM)
    {
        settings.type = CreativeRealSense;
    }
    else
    if ( (_info.model == PXCCapture::DEVICE_MODEL_GENERIC) &&
         (device->QueryDepthSensorRange().min == 152.4f) &&
         (device->QueryDepthSensorRange().max == 990.6f) )
    {
        settings.type = CreativeSenz3D;
    }
    else
    {
        settings.type = Default;
        currentState  = Disconnected;
        Log << "Error: Device type unknown!\n"; 
        return;
    }

    // Check which streams are supported by the device
    if (checkBit(_info.streams, PXCCapture::STREAM_TYPE_COLOR))
    {
        setBit(settings.streams, Color);
    }
    if (checkBit(_info.streams, PXCCapture::STREAM_TYPE_DEPTH))
    {
        setBit(settings.streams, Depth);
    }
    if (checkBit(_info.streams, PXCCapture::STREAM_TYPE_IR))
    {
        setBit(settings.streams, IR);
    }
    if (checkBit(_info.streams, PXCCapture::STREAM_TYPE_LEFT))
    {
        setBit(settings.streams, IntensityLeft);
    }
    if (checkBit(_info.streams, PXCCapture::STREAM_TYPE_RIGHT))
    {
        setBit(settings.streams, IntensityRight);
    }

    stream[Color].width  = 1920;
    stream[Color].height = 1080;
    stream[Color].format = gpu::FormatBGR_8;
    stream[Color].hz     = 30;

    stream[Depth].width  = 640;
    stream[Depth].height = 480;
    stream[Depth].format = gpu::FormatR_16_u;
    stream[Depth].hz     = 60;


    // Save device information
    PXCPointF32 vec2;
    PXCRangeF32 vec3;
    PXCCapture::Device::PowerLineFrequency hz = device->QueryColorPowerLineFrequency();
    settings.color.fps           = (hz == PXCCapture::Device::PowerLineFrequency::POWER_LINE_FREQUENCY_60HZ) ? 60 : ( (hz == PXCCapture::Device::PowerLineFrequency::POWER_LINE_FREQUENCY_50HZ) ? 50 : 0);
    vec2 = device->QueryColorFieldOfView();
    settings.color.hFOV          = vec2.x;
    settings.color.vFOV          = vec2.y;
    vec2 = device->QueryDepthFieldOfView();
    settings.depth.hFOV          = vec2.x;
    settings.depth.vFOV          = vec2.y;
    vec3 = device->QueryDepthSensorRange();
    settings.depth.minDistance   = vec3.min / 1000.0f; //0.1524f;  0.2f;
    settings.depth.maxDistance   = vec3.max / 1000.0f; //0.9906f;  1.2f;
    settings.depth.scale         = device->QueryDepthUnit() / 1000000.0f;
    settings.depth.lowConfidence = uint16(device->QueryDepthLowConfidenceValue());
    
    if (settings.type == CreativeSenz3D)
    {
        settings.color.width         = 1280;
        settings.color.height        = 720;
        settings.depth.width         = 320;
        settings.depth.height        = 240;
        settings.depth.fps           = 60;
    }
    if (settings.type == CreativeRealSense)
    {
        settings.color.width         = stream[Color].width;
        settings.color.height        = stream[Color].height;
        settings.color.fps           = stream[Color].hz;
        settings.depth.width         = stream[Depth].width;
        settings.depth.height        = stream[Depth].height;
        settings.depth.fps           = stream[Depth].hz;
    }

  /* ! */   settings.depth.saturate      = 0; 




    currentState = Ready;
}

CreativeDepthCamera::~CreativeDepthCamera()
{
    off();
    device->Release();
    device = nullptr;
}

// TODO: Clean formatting from here down

   gpu::TextureFormat textureFormat(PXCImage::PixelFormat format)
   {
   if (format == PXCImage::PixelFormat::PIXEL_FORMAT_ANY)            return gpu::FormatUnsupported; // Unknown/undefined 
   /* STREAM_TYPE_COLOR */
   if (format == PXCImage::PixelFormat::PIXEL_FORMAT_YUY2)           return gpu::FormatUnsupported; // Not supported yet
   if (format == PXCImage::PixelFormat::PIXEL_FORMAT_NV12)           return gpu::FormatUnsupported; // Not supported yet
   if (format == PXCImage::PixelFormat::PIXEL_FORMAT_RGB32)          return gpu::FormatBGRA_8;   // BGRA layout on a little-endian machine 
   if (format == PXCImage::PixelFormat::PIXEL_FORMAT_RGB24)          return gpu::FormatBGR_8;    // BGR layout on a little-endian machine 
   if (format == PXCImage::PixelFormat::PIXEL_FORMAT_Y8)             return gpu::FormatR_8_u;    // 8-Bit Gray Image, or IR 8-bit
   /* STREAM_TYPE_DEPTH */
   if (format == PXCImage::PixelFormat::PIXEL_FORMAT_DEPTH)          return gpu::FormatR_16_u;   // 16-bit unsigned integer with precision mm.
   if (format == PXCImage::PixelFormat::PIXEL_FORMAT_DEPTH_RAW)      return gpu::FormatR_16_u;   // 16-bit unsigned integer with device specific precision (call device->QueryDepthUnit())
   if (format == PXCImage::PixelFormat::PIXEL_FORMAT_DEPTH_F32)      return gpu::FormatR_32_f;   // 32-bit float-point with precision mm.
   /* STREAM_TYPE_IR */
   if (format == PXCImage::PixelFormat::PIXEL_FORMAT_Y16)            return gpu::FormatR_16_s;   // 16-Bit Gray Image
   if (format == PXCImage::PixelFormat::PIXEL_FORMAT_Y8_IR_RELATIVE) return gpu::FormatR_8_s;    // Relative IR Image

   return gpu::FormatUnsupported;
   };

   PXCImage::PixelFormat creativeFormat(gpu::TextureFormat format)
   {
   if (format == gpu::FormatBGRA_8) return PXCImage::PixelFormat::PIXEL_FORMAT_RGB32;
   if (format == gpu::FormatBGR_8)  return PXCImage::PixelFormat::PIXEL_FORMAT_RGB24;
   if (format == gpu::FormatR_16_u) return PXCImage::PixelFormat::PIXEL_FORMAT_DEPTH;
   if (format == gpu::FormatR_32_f) return PXCImage::PixelFormat::PIXEL_FORMAT_DEPTH_F32;
   if (format == gpu::FormatR_8_u)  return PXCImage::PixelFormat::PIXEL_FORMAT_Y8;
   if (format == gpu::FormatR_8_s)  return PXCImage::PixelFormat::PIXEL_FORMAT_Y8_IR_RELATIVE;
   if (format == gpu::FormatR_16_s) return PXCImage::PixelFormat::PIXEL_FORMAT_Y16;

   return PXCImage::PixelFormat::PIXEL_FORMAT_ANY;
   };

   bool CreativeDepthCamera::on(void)
   {
   pxcStatus status;

   for(sint32 i=0; i<PXCCapture::STREAM_LIMIT; ++i)
      {
      // Check if device supports this type of stream
      PXCCapture::StreamType type = PXCCapture::StreamTypeFromIndex(i);
      if (!(deviceInfo.streams & type)) 
         continue;

      std::string name = stringFromWchar(PXCCapture::StreamTypeToString(type), 1024);

      // Go through all available profiles for this stream type
      for(int p=0; p<device->QueryStreamProfileSetNum(type); ++p) 
          {
          PXCCapture::Device::StreamProfileSet profiles = {};
          status = device->QueryStreamProfileSet(type, p, &profiles);
          if (status < PXC_STATUS_NO_ERROR) 
             break;

          PXCCapture::Device::StreamProfile& profile = profiles[type];

          // Search for matching profile for color stream
          if (PXCCapture::StreamTypeFromIndex(i) == PXCCapture::STREAM_TYPE_COLOR)
             if ( !checkBit(active, Color))
                if ( stream[Color].width  == profile.imageInfo.width &&
                     stream[Color].height == profile.imageInfo.height &&
                     stream[Color].format == textureFormat(profile.imageInfo.format) &&
                     stream[Color].hz     <= static_cast<uint32>(profile.frameRate.min) )
                   {
                   setBit(active, Color);

                   #ifdef EN_DEBUG
                   // Debug log profile information
                   Log << "Stream[" << i << "-" << name << "] Profile[" << p << "]:\n";
                   Log << " - Format: " << stringFromWchar( reinterpret_cast<const wchar_t*>(PXCImage::PixelFormatToString(profile.imageInfo.format)), 256) << " " << 
                                           static_cast<float>(profile.frameRate.min) << "Hz - " << static_cast<float>(profile.frameRate.max) << "Hz\n";
                   Log << " - Width : " << (sint32)profile.imageInfo.width << std::endl;
                   Log << " - Height: " << (sint32)profile.imageInfo.height << std::endl;
                   #endif
                   }

          // Search for matching profile for depth stream
          if (PXCCapture::StreamTypeFromIndex(i) == PXCCapture::STREAM_TYPE_DEPTH)
             if ( !checkBit(active, Depth))
                if ( stream[Depth].width  == profile.imageInfo.width &&
                     stream[Depth].height == profile.imageInfo.height &&
                     stream[Depth].format == textureFormat(profile.imageInfo.format) &&
                     stream[Depth].hz     <= static_cast<uint32>(profile.frameRate.min) )
                   {
                   setBit(active, Depth);

                   #ifdef EN_DEBUG
                   // Debug log profile information
                   Log << "Stream[" << i << "-" << name << "] Profile[" << p << "]:\n";
                   Log << " - Format: " << stringFromWchar( reinterpret_cast<const wchar_t*>(PXCImage::PixelFormatToString(profile.imageInfo.format)), 256) << " " << 
                                           static_cast<float>(profile.frameRate.min) << "Hz - " << static_cast<float>(profile.frameRate.max) << "Hz\n";
                   Log << " - Width : " << (sint32)profile.imageInfo.width << std::endl;
                   Log << " - Height: " << (sint32)profile.imageInfo.height << std::endl;
                   #endif
                   }
          }
      }

   // Init streams after ensuring, that there are matching profiles for them
   memset(&set, 0, sizeof(PXCCapture::Device::StreamProfileSet));
   //device->QueryStreamProfileSet(&set);
   if (checkBit(active, Color))
      {
      set.color.imageInfo.width  = stream[Color].width;
      set.color.imageInfo.height = stream[Color].height;
      set.color.imageInfo.format = creativeFormat(stream[Color].format);
      set.color.frameRate.min    = static_cast<float>(stream[Color].hz);
      set.color.frameRate.max    = static_cast<float>(stream[Color].hz);
      }
   if (checkBit(active, Depth))
      {
      set.depth.imageInfo.width  = stream[Depth].width;
      set.depth.imageInfo.height = stream[Depth].height;
      set.depth.imageInfo.format = creativeFormat(stream[Depth].format);
      set.depth.frameRate.min    = static_cast<float>(stream[Depth].hz);
      set.depth.frameRate.max    = static_cast<float>(stream[Depth].hz);
      //set.depth.options          = PXCCapture::Device::STREAM_OPTION_DEPTH_PRECALCULATE_UVMAP;
      }
   if ( checkBit(active, Color) ||
        checkBit(active, Depth) )
      {
      status = device->SetStreamProfileSet(&set);
      }
   if (checkBit(active, Color))
      {
      status = device->ReadStreamsAsync(PXCCapture::STREAM_TYPE_COLOR, &sample[Color], &sync[Color]);
      }
   if (checkBit(active, Depth))
      {
      status = device->ReadStreamsAsync(PXCCapture::STREAM_TYPE_DEPTH, &sample[Depth], &sync[Depth]);
      }

   // If any data stream is turned on, we're on
   if (active)
      {
      currentState = Used;
      return true;
      }
   return false;
   }

   bool CreativeDepthCamera::off(void)
   {
   // TODO: Disable streams

   currentState = Ready;
   return true;
   }

   CameraState CreativeDepthCamera::state(void) const
   {
   return currentState;
   }

   CameraType CreativeDepthCamera::type(void) const
   {
   return settings.type;
   }

   bool CreativeDepthCamera::support(CameraStream type) const
   {
   return checkBit(settings.streams, type) > 0 ? true : false;
   }

   CameraInfo CreativeDepthCamera::info(void) const
   {
   return settings;
   }

   void CreativeDepthCamera::update(void)
   {
   if (checkBit(active, Color))
      {
      pxcStatus s = sync[Color]->Synchronize(0); 
      if (s >= PXC_STATUS_NO_ERROR)
         {
         PXCImage::ImageData data;
         sample[Color].color->AcquireAccess(PXCImage::ACCESS_READ, &data);

         // Call event handling function 
         CameraEvent event;
         event.type = CameraColorData;
         event.id   = id;
         event.ptr  = reinterpret_cast<void*>(data.planes[0]);
         event.size = stream[Color].width * stream[Color].height * (Gpu.texture.bitsPerTexel(stream[Color].format) / 8);
         InputContext.events.callback( reinterpret_cast<Event&>(event) ); 

         // Read next frame async
         sample[Color].color->ReleaseAccess(&data);
         sample[Color].ReleaseImages();
         sync[Color]->Release();

         /*status == */ device->ReadStreamsAsync(PXCCapture::STREAM_TYPE_COLOR, &sample[Color], &sync[Color]);
         }
      }

   if (checkBit(active, Depth))
      {
      pxcStatus s = sync[Depth]->Synchronize(0); 
      if (s >= PXC_STATUS_NO_ERROR)
         {
         PXCImage::ImageData data;
         sample[Depth].depth->AcquireAccess(PXCImage::ACCESS_READ, &data);

         // Call event handling function for Depth
         CameraEvent event;
         event.type = CameraDepthData;
         event.id   = id;
         event.ptr  = reinterpret_cast<uint8*>(data.planes[0]);
         event.size = stream[Depth].width * stream[Depth].height * (Gpu.texture.bitsPerTexel(stream[Depth].format) / 8);
         InputContext.events.callback( reinterpret_cast<Event&>(event) ); 

         // Calculate UV map for given depth map
         PXCProjection* projection = device->CreateProjection();
         PXCPointF32 *uvmap = new PXCPointF32[stream[Depth].width * stream[Depth].height];
         projection->QueryUVMap(sample[Depth].depth, uvmap);

         // Call event handling function for UV Map
         event.type = CameraUVMapData;
         event.id   = id;
         event.ptr  = reinterpret_cast<uint8*>(uvmap);
         event.size = stream[Depth].width * stream[Depth].height * 8;
         InputContext.events.callback( reinterpret_cast<Event&>(event) ); 

         // Clean up
         delete[] uvmap;
         projection->Release();

         // Read next frame async
         sample[Depth].depth->ReleaseAccess(&data);
         sample[Depth].ReleaseImages();
         sync[Depth]->Release();

         /*status == */ device->ReadStreamsAsync(PXCCapture::STREAM_TYPE_DEPTH, &sample[Depth], &sync[Depth]);
         }
      }

   if (checkBit(active, IR))
      {
      pxcStatus s = sync[IR]->Synchronize(0); 
      if (s >= PXC_STATUS_NO_ERROR)
         {
         PXCImage::ImageData data;
         sample[IR].ir->AcquireAccess(PXCImage::ACCESS_READ, &data);

         // Call event handling function for Infrared
         CameraEvent event;
         event.type = CameraInfraredData;
         event.id   = id;
         event.ptr  = static_cast<uint8*>(data.planes[0]);
         event.size = stream[IR].width * stream[IR].height * (Gpu.texture.bitsPerTexel(stream[IR].format) / 8);
         InputContext.events.callback( reinterpret_cast<Event&>(event) ); 

         // Read next frame async
         sample[IR].ir->ReleaseAccess(&data);
         sample[IR].ReleaseImages();
         sync[IR]->Release();

         /*status == */ device->ReadStreamsAsync(PXCCapture::STREAM_TYPE_IR, &sample[IR], &sync[IR]);
         }
      }
   }
#endif

#if MICROSOFT_KINECT
   class Kinect : public Camera
         {
         public:
         virtual bool        on(void);           // Turns camera on
         virtual bool        off(void);          // Turns camera off
         virtual CameraState state(void) const;  // Returns camera state (on/off)
         virtual CameraType  type(void) const;   // Returns camera type
         virtual bool        support(CameraStream type) const; // Returns true if camera supports given stream type
         virtual CameraInfo  info(void) const;   // Returns all camera properties
         virtual void        update(void);       // Update camera stream events

         Kinect(uint32 id,
                INuiSensor* sensor);
         virtual ~Kinect();
 
         private:
         uint32      id;             // Kinect id on the devices list
         CameraState currentState;   // Is it turned on
         std::string name;           // Device name
         CameraInfo  settings;       // Camera properties
         INuiSensor* sensor;         // Kiect sensor interface
         HANDLE      eventColor;     // Color, depth and skeleton events
         HANDLE      eventDepth;
         HANDLE      eventSkeleton;
         HANDLE      streamColor;    // Color, depth and skeleton data streams
         HANDLE      streamDepth;     
         };

   Kinect::Kinect(uint32 _id, INuiSensor* _sensor) :
      id(_id),
      currentState(Initializing),
      name("KinectForXbox360"),
      sensor(_sensor),
      eventColor(INVALID_HANDLE_VALUE),
      eventDepth(INVALID_HANDLE_VALUE),
      eventSkeleton(INVALID_HANDLE_VALUE),
      streamColor(INVALID_HANDLE_VALUE),
      streamDepth(INVALID_HANDLE_VALUE)
   {
   memset(&settings, 0, sizeof(settings));

   // Describe Kinect capabilities
   settings.type                = KinectForXbox360;
   setBit(settings.streams, Color);
   setBit(settings.streams, Depth);
   setBit(settings.streams, Skeletons);
   settings.color.width         = 640;
   settings.color.height        = 480;
   settings.color.fps           = 30;
   settings.color.hFOV          = 57.5;   
   settings.color.vFOV          = 43.5;   
   settings.depth.width         = 640;
   settings.depth.height        = 480;
   settings.depth.fps           = 30;
   settings.depth.hFOV          = 57.5;   
   settings.depth.vFOV          = 43.5; 
   settings.depth.minDistance   = 0.4f;
   settings.depth.maxDistance   = 4.0f;
   settings.skeleton.bodies     = 2;
   settings.skeleton.bones      = 20;

   currentState = Ready;
   }

   Kinect::~Kinect()
   {
   off();
   sensor->Release();
   }

   bool Kinect::on(void)
   {
   // Check if sensor is ready for connection
   HRESULT result = sensor->NuiStatus();
   if (result != S_OK) 
      {
      if (result == S_NUI_INITIALIZING) 
         currentState = Initializing;
      if (result == E_NUI_NOTCONNECTED)
         currentState = Disconnected;
      if (result == E_NUI_INSUFFICIENTBANDWIDTH)
         currentState = NoBandwith;
      if (result == E_NUI_NOTPOWERED)
         currentState = NoPower;
      return false;
      }

   // Try to initialize Kinect streams
   if (S_OK != sensor->NuiInitialize( NUI_INITIALIZE_FLAG_USES_COLOR | 
                                      NUI_INITIALIZE_FLAG_USES_DEPTH_AND_PLAYER_INDEX |
                                      NUI_INITIALIZE_FLAG_USES_SKELETON ))
      return false;
   
   // Create events that will be signaled when color, depth or skeleton data is available
   eventColor    = CreateEvent(NULL, TRUE, FALSE, NULL);
   eventDepth    = CreateEvent(NULL, TRUE, FALSE, NULL);
   eventSkeleton = CreateEvent(NULL, TRUE, FALSE, NULL);
   
   // Open color stream
   if (S_OK != sensor->NuiImageStreamOpen(NUI_IMAGE_TYPE_COLOR,
                                          NUI_IMAGE_RESOLUTION_640x480,
                                          0,
                                          NUI_IMAGE_STREAM_FRAME_LIMIT_MAXIMUM,
                                          eventColor,
                                          &streamColor))
      return false;
   
   // Open depth stream
   if (S_OK != sensor->NuiImageStreamOpen(NUI_IMAGE_TYPE_DEPTH_AND_PLAYER_INDEX,
                                          NUI_IMAGE_RESOLUTION_640x480,
                                          0,
                                          NUI_IMAGE_STREAM_FRAME_LIMIT_MAXIMUM,
                                          eventDepth,
                                          &streamDepth))
      return false;
   
   // Open skeleton stream
   if (S_OK != sensor->NuiSkeletonTrackingEnable(eventSkeleton, 0 /* NUI_SKELETON_TRACKING_FLAG_ENABLE_SEATED_SUPPORT */ ))
      return false;

   currentState = Used;
   return true;
   }

   bool Kinect::off(void)
   {
   // Check if sensor is ready 
   HRESULT result = sensor->NuiStatus();
   if (result != S_OK) 
      {
      if (result == S_NUI_INITIALIZING) 
         currentState = Initializing;
      if (result == E_NUI_NOTCONNECTED)
         currentState = Disconnected;
      if (result == E_NUI_INSUFFICIENTBANDWIDTH)
         currentState = NoBandwith;
      if (result == E_NUI_NOTPOWERED)
         currentState = NoPower;
      return false;
      }

   // Disable sensor
   sensor->NuiShutdown();
 
   // Disable events
   if (eventColor)
      if (eventColor != INVALID_HANDLE_VALUE)
         CloseHandle(eventColor);
   
   if (eventDepth)
      if (eventDepth != INVALID_HANDLE_VALUE)
         CloseHandle(eventDepth);
   
   if (eventSkeleton)
      if (eventSkeleton != INVALID_HANDLE_VALUE)
         CloseHandle(eventSkeleton);

   currentState = Ready;
   return true;
   }

   CameraState Kinect::state(void) const
   {
   return currentState;
   }

   CameraType Kinect::type(void) const
   {
   return settings.type;
   }

   bool Kinect::support(CameraStream type) const
   {
   return checkBit(settings.streams, type);
   }

   CameraInfo Kinect::info(void) const
   {
   return settings;
   }

   void Kinect::update(void)
   {
   //HRESULT NuiImageGetColorPixelCoordinateFrameFromDepthPixelFrameAtResolution(
   //         NUI_IMAGE_RESOLUTION_640x480,
   //         NUI_IMAGE_RESOLUTION_640x480,
   //         640x480,
   //         USHORT *pDepthValues,
   //         DWORD cColorCoordinates,
   //         LONG *pColorCoordinates

   // Color
   if (WaitForSingleObject(eventColor, 0) == WAIT_OBJECT_0)
      {
      NUI_IMAGE_FRAME image;
      if (sensor->NuiImageStreamGetNextFrame(streamColor, 0, &image) == S_OK)
         {
         // Locks image data to ensure it won't be modified by Kinect during read.
         NUI_LOCKED_RECT rect;
         INuiFrameTexture* texture = image.pFrameTexture;
         texture->LockRect(0, &rect, NULL, 0);
         if (rect.Pitch != 0)
            {
            // Call event handling function 
            CameraEvent event;
            event.type = CameraColorData;
            event.id   = id;
            event.ptr  = static_cast<void*>(rect.pBits);
            event.size = rect.size;
            InputContext.events.callback( reinterpret_cast<Event&>(event) ); 
            }
   
         // Release image from stream
         texture->UnlockRect(0);
         sensor->NuiImageStreamReleaseFrame(streamColor, &image);
         }
      }
   
   // Depth
   if (WaitForSingleObject(eventDepth, 0) == WAIT_OBJECT_0)
      {
      NUI_IMAGE_FRAME image;
      if (sensor->NuiImageStreamGetNextFrame(streamDepth, 0, &image) == S_OK)
         {
         // Locks image data to ensure it won't be modified by Kinect during read.
         NUI_LOCKED_RECT rect;
         INuiFrameTexture* texture = image.pFrameTexture;
         texture->LockRect(0, &rect, NULL, 0);
         if (rect.Pitch != 0)
            {
            // Call event handling function
            CameraEvent event;
            event.type = CameraDepthData;
            event.id   = id;
            event.ptr  = static_cast<void*>(rect.pBits);
            event.size = rect.size;
            InputContext.events.callback( reinterpret_cast<Event&>(event) ); 
            }
   
         // Release image from stream
         texture->UnlockRect(0);
         sensor->NuiImageStreamReleaseFrame(streamDepth, &image);
         }
      }
   
   // Skeleton
   if (WaitForSingleObject(eventSkeleton, 0) == WAIT_OBJECT_0)
      {
      // Create skeleton data event structure
      Skeleton* data = allocate<Skeleton>();
      memset(data, 0, sizeof(Skeleton));
   
      uint32 bodies = 0;
      NUI_SKELETON_FRAME skeleton = {0};
      if (sensor->NuiSkeletonGetNextFrame(0, &skeleton) == S_OK)
         {
         // Smooth out the skeleton data to prevent jitter between the frames
         sensor->NuiTransformSmooth(&skeleton, NULL);
   
         // Create and fill event structure
         SkeletonEvent event;
         event.type     = CameraSkeletonData;
         event.id       = id;
         event.bodies   = 6;
         event.skeleton = new Skeleton[6];

         for(uint8 j=0; j<6; ++j)
             {
             NUI_SKELETON_TRACKING_STATE tracking = skeleton.SkeletonData[j].eTrackingState;
   
             if (tracking == NUI_SKELETON_NOT_TRACKED)
                {
                event.skeleton[j].state    = Unknown;
                continue;
                }
   
             if (tracking == NUI_SKELETON_POSITION_ONLY)
                {
                event.skeleton[j].state    = Predicted;
                event.skeleton[j].position = float4(&skeleton.SkeletonData[j].Position.x);
                continue;
                }
   
             if (tracking == NUI_SKELETON_TRACKED)
                {
                event.skeleton[j].state     = Tracked;
                event.skeleton[j].position  = float4(&skeleton.SkeletonData[j].Position.x);
                event.skeleton[j].bone      = new float4[20];
                event.skeleton[j].boneState = new KinectTrackingState[20];
                for(uint8 k=0; k<20; ++k)
                   {
                   event.skeleton[j].bone[k] = float4(&skeleton.SkeletonData[j].SkeletonPositions[k].x);
   
                   NUI_SKELETON_POSITION_TRACKING_STATE jointState = skeleton.SkeletonData[j].eSkeletonPositionTrackingState[k];
                   if (jointState == NUI_SKELETON_POSITION_NOT_TRACKED)
                      event.skeleton[j].boneState[k] = Unknown;
                   if (jointState == NUI_SKELETON_POSITION_INFERRED)
                      event.skeleton[j].boneState[k] = Predicted;
                   if (jointState == NUI_SKELETON_POSITION_TRACKED)
                      event.skeleton[j].boneState[k] = Tracked;
                   }
                }
             }

         // Call event handling function    
         InputContext.events.callback( reinterpret_cast<Event&>(event) ); 
         for(uint8 j=0; j<6; ++j)
            if (event.skeleton[j].state == Tracked)
               {
               delete [] event.skeleton[j].bone;
               delete [] event.skeleton[j].boneState;
               }
         delete [] event.skeleton;
         }
      }
   }
#endif
#endif

   //# CONTEXT
   //##########################################################################

   void Context::Camera::init(void)
   {
#ifdef EN_PLATFORM_WINDOWS
#if INTEL_PERCEPTUAL_COMPUTING_2014
   // Intel Perceptual Computing Depth Cameras (Creative Senz3D, Creative RealSense)
   session = PXCSession::CreateInstance();
   if (session)
      {
      // SDK Version
      PXCSession::ImplVersion ver = session->QueryVersion();
      Log << "Perceptual Computing SDK Version " << (uint16)(ver.major) << "." << (uint16)(ver.minor) << std::endl;

      // Enumerate through available SDK modules
      PXCSession::ImplDesc desc;
      memset(&desc, 0, sizeof(desc));
      desc.group    = PXCSession::IMPL_GROUP_SENSOR;
      desc.subgroup = PXCSession::IMPL_SUBGROUP_VIDEO_CAPTURE;

      // Iterate over modules
      for(uint32 module=0; ; ++module) 
         {
         PXCSession::ImplDesc result;
         pxcStatus status = session->QueryImpl(&desc, module, &result);
         if (status < PXC_STATUS_NO_ERROR) 
            break;

         // Create capture for given module
         PXCCapture* capture = nullptr;
         status = session->CreateImpl<PXCCapture>(&result, &capture);
         if (status < PXC_STATUS_NO_ERROR)
            continue;

		 std::string nameModule = stringFromWchar((wchar_t*)&result.friendlyName, 256);
         Log << "Module[" << module << "]: " << nameModule << "\n";

         // Iterate over devices in module
         for(uint32 deviceId=0; ; ++deviceId) 
            {
            // Get device information
            PXCCapture::DeviceInfo info;
            status = capture->QueryDeviceInfo(deviceId, &info);
            if (status < PXC_STATUS_NO_ERROR) 
               break;

            // Create camera description
            PXCCapture::Device* ptr = capture->CreateDevice(deviceId);
            if (!ptr) 
               break;

            // Add Depth Camera to cameras list
            CreativeDepthCamera* dev = new CreativeDepthCamera(device.size(), ptr, info);
            device.push_back(std::shared_ptr<input::Camera>(dev));

            Log << "    Device[" << deviceId << "]: " << stringFromWchar((wchar_t*)&info.name, 1024) << "\n"; 
            }
         capture->Release();  
         }
      }
#endif

#if MICROSOFT_KINECT
   // Get count of available sensors
   sint32 kinects = 0;
   if (S_OK != NuiGetSensorCount(&kinects))
      kinects = 0;
         
   for(sint8 i=0; i<kinects; ++i)
      {
      INuiSensor* sensor;

      // Get device interface
      if (S_OK != NuiCreateSensorByIndex(i, &sensor))
         continue;

      // Check device status
      HRESULT result = sensor->NuiStatus();
      if (result == E_NUI_NOTGENUINE   ||
          result == E_NUI_NOTSUPPORTED ||
          result == E_NUI_NOTREADY)
         {
         sensor->Release();
         continue;
         }
   
      // Add Kinect to cameras list
      input::Kinect* dev = new input::Kinect(device.size(), sensor);
      device.push_back(std::shared_ptr<input::Camera>(dev));

      Log << "    Device[" << i << "]: KinectForXbox360\n"; 
      }
#endif
#endif
   }

   void Context::Camera::destroy(void)
   {
   // Cameras
   for(sint32 i=0; i<device.size(); ++i)
      device[i] = nullptr;

#ifdef EN_PLATFORM_WINDOWS
#if INTEL_PERCEPTUAL_COMPUTING_2014
   if (session)
      session->Release();

      //// OLD SDK 2013
      //camera.scheduler.ReleaseRef();
      //camera.session->Release();
      //camera.session.ReleaseRef();
#endif
#endif

   device.clear();
   }



#endif


//   bool Interface::Camera::on(uint8 index)
//   {
//   if (InputContext.camera.device.size() <= index)
//      return false;
//
//   Context::Camera::Device* dev = InputContext.camera.device[index];
//   if (!dev->on)
//      {
//      if (dev->info.type == CreativeSenz3D)
//         {
//#ifdef EN_PLATFORM_WINDOWS
//#ifndef PERCEPTUAL_COMPUTING_2014
//         Context::Camera::CreativeSenz3D* dev = reinterpret_cast<Context::Camera::CreativeSenz3D*>(InputContext.camera.device[index]);
//
//          Iterate over streams
//         bool color = false;
//         bool depth = false;
//         for(sint32 i=1; i>=0; --i)
//            {
//            PXCCapture::Device::StreamInfo info;
//            if (dev->device->QueryStream(i, &info) != PXC_STATUS_NO_ERROR) 
//               break;
// 
//            if (info.cuid != PXCCapture::VideoStream::CUID) 
//               continue;
//
//            PXCCapture::VideoStream* vstream = nullptr;
//            if (dev->device->CreateStream<PXCCapture::VideoStream>(i, &vstream) != PXC_STATUS_NO_ERROR) 
//               continue;
//
//            for(uint32 p=0; ; ++p) 
//               {
//               PXCCapture::VideoStream::ProfileInfo profile;
//               if (vstream->QueryValidate(p, &profile) != PXC_STATUS_NO_ERROR) 
//                  break;
//
//               Log << "Stream[" << i << "] Profile[" << p << "]:\n";
//               Log << " - Format: " << (uint32)(profile.imageInfo.format - 0x00010000) << std::endl;
//               Log << " - Width : " << (uint32)profile.imageInfo.width << std::endl;
//               Log << " - Height: " << (uint32)profile.imageInfo.height << std::endl;
//               Log << " - FPS min: " << (uint32)profile.frameRateMin.numerator << std::endl;
//               Log << " - FPS max: " << (uint32)profile.frameRateMax.numerator << std::endl;
//               if ( profile.imageOptions == 1) Log << " - No UV MAP!\n";
//               if ( profile.imageOptions == 2) Log << " - No IR MAP!\n";
//               }
//
//            for(uint32 p=0; p<256; ++p) 
//               {
//               PXCCapture::VideoStream::ProfileInfo profile;
//               if (vstream->QueryValidate(p, &profile) != PXC_STATUS_NO_ERROR) 
//                  break;
// 
//               float minFPS = 0.0f;
//               float maxFPS = 0.0f;
//               if (profile.frameRateMin.denominator)
//                  minFPS = static_cast<float>(profile.frameRateMin.numerator) / static_cast<float>(profile.frameRateMin.denominator);
//               if (profile.frameRateMax.denominator) 
//                  maxFPS = static_cast<float>(profile.frameRateMax.numerator) / static_cast<float>(profile.frameRateMax.denominator);
//
//                Color stream - 1280x720 30fps
//               if (!color && info.imageType == PXCImage::IMAGE_TYPE_COLOR)
//                  if (profile.imageInfo.format == PXCImage::COLOR_FORMAT_RGB24 &&
//                      profile.imageInfo.width  == 1280 &&
//                      profile.imageInfo.height == 720 &&
//                      minFPS == 30.0 &&
//                      maxFPS == 30.0 )
//                     if (vstream->SetValidate(&profile) == PXC_STATUS_NO_ERROR)
//                        if (vstream->ReadStreamAsync(&dev->colorImage, &dev->colorSync) == PXC_STATUS_NO_ERROR)
//                           {
//                            Wait 1 milisecond to give camera time to init the stream
//                           Timer timer;
//                           timer.start();
//                           for(;;)
//                              if (timer.elapsed().seconds() > 1.0)
//                                 break;
//
//                           Log << "COLOR: Stream[" << i << "] Profile[" << p << "]\n";
//
//                           dev->color = vstream;
//                           color = true;
//                           break; 
//                           }
//
//                Depth stream - 320x240 60fps
//               if (!depth && info.imageType == PXCImage::IMAGE_TYPE_DEPTH)
//                  if (profile.imageInfo.format == PXCImage::COLOR_FORMAT_DEPTH &&
//                      profile.imageInfo.width  == 320 &&
//                      profile.imageInfo.height == 240 &&
//                      minFPS == 60.0 &&
//                      maxFPS == 60.0 )
//                     if (vstream->SetValidate(&profile) == PXC_STATUS_NO_ERROR)
//                        if (vstream->ReadStreamAsync(&dev->depthImage, &dev->depthSync) == PXC_STATUS_NO_ERROR)
//                           {
//                            Wait 1 milisecond to give camera time to init the stream
//                           Timer timer;
//                           timer.start();
//                           for(;;)
//                              if (timer.elapsed().miliseconds() > 1.0)
//                                 break;
//
//                           Log << "DEPTH: Stream[" << i << "] Profile[" << p << "]\n";
//
//                           dev->device->SetProperty(PXCCapture::Device::PROPERTY_DEPTH_SMOOTHING, 1.0f);
//                           dev->depth = vstream;
//                           depth = true;
//                           break; 
//                           }
//               }
//
//             If both streams are set, finish
//            if (color && depth)
//               break;  
//            }
//
//          If couldn't init streams, correct device info
//         if (!color) dev->info.color.support = false;
//         if (!depth) dev->info.depth.support = false;
//
//          If any data stream is turned on, we're on
//         if (color || depth)
//            {
//            dev->on   = true;
//            dev->used = true;
//            }
//
//         if (color) dev->color->ReadStreamAsync(&dev->colorImage, &dev->colorSync);
//         if (depth) dev->depth->ReadStreamAsync(&dev->depthImage, &dev->depthSync);
//#endif
//#endif
//         }
//      }
//
//   return false;
//   }



   }
}
