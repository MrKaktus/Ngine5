

//   https://msdn.microsoft.com/en-us/library/windows/desktop/dn770441(v=vs.85).aspx#D3D12_TEXTURE_ADDRESS_MODE_BORDER
//   
//   typedef enum D3D12_TEXTURE_ADDRESS_MODE { 
//     D3D12_TEXTURE_ADDRESS_MODE_WRAP         = 1,
//     D3D12_TEXTURE_ADDRESS_MODE_MIRROR       = 2,
//     D3D12_TEXTURE_ADDRESS_MODE_CLAMP        = 3,
//     D3D12_TEXTURE_ADDRESS_MODE_BORDER       = 4,
//     D3D12_TEXTURE_ADDRESS_MODE_MIRROR_ONCE  = 5
//   } D3D12_TEXTURE_ADDRESS_MODE;

// https://msdn.microsoft.com/en-us/library/windows/desktop/dn770403(v=vs.85).aspx
//
// typedef struct D3D12_SAMPLER_DESC {
//   D3D12_FILTER               Filter;
//   D3D12_TEXTURE_ADDRESS_MODE AddressU;
//   D3D12_TEXTURE_ADDRESS_MODE AddressV;
//   D3D12_TEXTURE_ADDRESS_MODE AddressW;
//   FLOAT                      MipLODBias;
//   UINT                       MaxAnisotropy;     // 1 .. 16
//   D3D12_COMPARISON_FUNC      ComparisonFunc;
//   FLOAT                      BorderColor[4];
//   FLOAT                      MinLOD;
//   FLOAT                      MaxLOD;
// } D3D12_SAMPLER_DESC;

  // Sampler Filtering modes:

  //  // bits 1-0 - MipMap          0-Neares 1-Linear
  //  // bits 3-2 - Magnification
  //  // bits 5-4 - Minification
  //  // bit    6 - Anisotropic
  //  // bit    7 - Comparison
  //  // bit    8 - Minimum Value
  //  //
  //  // 7 & 8 set - Maximum Value
  //  
  //  D3D12_FILTER_MIN_MAG_MIP_POINT                           = 0,
  //  D3D12_FILTER_MIN_MAG_POINT_MIP_LINEAR                    = 0x1,
  //  D3D12_FILTER_MIN_POINT_MAG_LINEAR_MIP_POINT              = 0x4,
  //  D3D12_FILTER_MIN_POINT_MAG_MIP_LINEAR                    = 0x5,
  //  D3D12_FILTER_MIN_LINEAR_MAG_MIP_POINT                    = 0x10,
  //  D3D12_FILTER_MIN_LINEAR_MAG_POINT_MIP_LINEAR             = 0x11,
  //  D3D12_FILTER_MIN_MAG_LINEAR_MIP_POINT                    = 0x14,
  //  D3D12_FILTER_MIN_MAG_MIP_LINEAR                          = 0x15,
  //  
  //  D3D12_FILTER_ANISOTROPIC                                 = 0x55,
  //  
  //  D3D12_FILTER_COMPARISON_MIN_MAG_MIP_POINT                = 0x80,
  //  D3D12_FILTER_COMPARISON_MIN_MAG_POINT_MIP_LINEAR         = 0x81,
  //  D3D12_FILTER_COMPARISON_MIN_POINT_MAG_LINEAR_MIP_POINT   = 0x84,
  //  D3D12_FILTER_COMPARISON_MIN_POINT_MAG_MIP_LINEAR         = 0x85,
  //  D3D12_FILTER_COMPARISON_MIN_LINEAR_MAG_MIP_POINT         = 0x90,
  //  D3D12_FILTER_COMPARISON_MIN_LINEAR_MAG_POINT_MIP_LINEAR  = 0x91,
  //  D3D12_FILTER_COMPARISON_MIN_MAG_LINEAR_MIP_POINT         = 0x94,
  //  D3D12_FILTER_COMPARISON_MIN_MAG_MIP_LINEAR               = 0x95,
  //  D3D12_FILTER_COMPARISON_ANISOTROPIC                      = 0xd5,
  //  
  //  // Features that need to be queried :
  //  
  //  // Returns minimum from the fetched texels:
  //  //
  //  //  D3D12_FILTER_MINIMUM_MIN_MAG_MIP_POINT                   = 0x100,
  //  //  D3D12_FILTER_MINIMUM_MIN_MAG_POINT_MIP_LINEAR            = 0x101,
  //  //  D3D12_FILTER_MINIMUM_MIN_POINT_MAG_LINEAR_MIP_POINT      = 0x104,
  //  //  D3D12_FILTER_MINIMUM_MIN_POINT_MAG_MIP_LINEAR            = 0x105,
  //  //  D3D12_FILTER_MINIMUM_MIN_LINEAR_MAG_MIP_POINT            = 0x110,
  //  //  D3D12_FILTER_MINIMUM_MIN_LINEAR_MAG_POINT_MIP_LINEAR     = 0x111,
  //  //  D3D12_FILTER_MINIMUM_MIN_MAG_LINEAR_MIP_POINT            = 0x114,
  //  //  D3D12_FILTER_MINIMUM_MIN_MAG_MIP_LINEAR                  = 0x115,
  //  //  D3D12_FILTER_MINIMUM_ANISOTROPIC                         = 0x155,
  //  //
  //  // Returns maximum from the fetched texels:
  //  //
  //  //  D3D12_FILTER_MAXIMUM_MIN_MAG_MIP_POINT                   = 0x180,
  //  //  D3D12_FILTER_MAXIMUM_MIN_MAG_POINT_MIP_LINEAR            = 0x181,
  //  //  D3D12_FILTER_MAXIMUM_MIN_POINT_MAG_LINEAR_MIP_POINT      = 0x184,
  //  //  D3D12_FILTER_MAXIMUM_MIN_POINT_MAG_MIP_LINEAR            = 0x185,
  //  //  D3D12_FILTER_MAXIMUM_MIN_LINEAR_MAG_MIP_POINT            = 0x190,
  //  //  D3D12_FILTER_MAXIMUM_MIN_LINEAR_MAG_POINT_MIP_LINEAR     = 0x191,
  //  //  D3D12_FILTER_MAXIMUM_MIN_MAG_LINEAR_MIP_POINT            = 0x194,
  //  //  D3D12_FILTER_MAXIMUM_MIN_MAG_MIP_LINEAR                  = 0x195,
  //  //  D3D12_FILTER_MAXIMUM_ANISOTROPIC                         = 0x1d5