

// Copyright (C) 2018-2022 - DevSH Graphics Programming Sp. z O.O.
// This file is part of the "Nabla Engine".
// For conditions of distribution and use, see copyright notice in nabla.h

#ifndef _NBL_BUILTIN_HLSL_COLOR_SPACE_ENCODE_CIE_XYZ_INCLUDED_
#define _NBL_BUILTIN_HLSL_COLOR_SPACE_ENCODE_CIE_XYZ_INCLUDED_

<<<<<<< HEAD
<<<<<<< HEAD
=======
#include <nbl/builtin/hlsl/cpp_compat.hlsl>

>>>>>>> 7156483209f3077dab03775532c0e9922176b765
=======
#include <nbl/builtin/hlsl/cpp_compat.hlsl>

>>>>>>> upstream/spirv_intrinsics
namespace nbl
{
namespace hlsl
{
namespace colorspace
{

<<<<<<< HEAD
<<<<<<< HEAD
static const float3x3 scRGBtoXYZ = float3x3(
    float3(0.4124564, 0.3575761, 0.1804375),
    float3(0.2126729, 0.7151522, 0.0721750),
    float3(0.0193339, 0.1191920, 0.9503041)
);

static const float3x3 sRGBtoXYZ = scRGBtoXYZ;

static const float3x3 BT709toXYZ = scRGBtoXYZ;


static const float3x3 Display_P3toXYZ = float3x3(
    float3(0.4865709, 0.2656677, 0.1982173),
    float3(0.2289746, 0.6917385, 0.0792869),
    float3(0.0000000, 0.0451134, 1.0439444)
);


static const float3x3 DCI_P3toXYZ = float3x3(
    float3(1.0, 0.0, 0.0),
    float3(0.0, 1.0, 0.0),
    float3(0.0, 0.0, 1.0)
);


static const float3x3 BT2020toXYZ = float3x3(
    float3(0.6369580, 0.1446169, 0.1688810),
    float3(0.2627002, 0.6779981, 0.0593017),
    float3(0.0000000, 0.0280727, 1.0609851)
);

static const float3x3 HDR10_ST2084toXYZ = BT2020toXYZ;

static const float3x3 DOLBYIVISIONtoXYZ = BT2020toXYZ;

static const float3x3 HDR10_HLGtoXYZ = BT2020toXYZ;


static const float3x3 AdobeRGBtoXYZ = float3x3(
    float3(0.57667, 0.18556, 0.18823),
    float3(0.29734, 0.62736, 0.07529),
    float3(0.02703, 0.07069, 0.99134)
);


static const float3x3 ACES2065_1toXYZ = float3x3(
    float3(0.9525523959, 0.0000000000,  0.0000936786),
    float3(0.3439664498, 0.7281660966, -0.0721325464),
    float3(0.0000000000, 0.0000000000,  1.0088251844)
);


static const float3x3 ACEScctoXYZ = float3x3(
    float3( 0.6624542, 0.1340042, 0.1561877),
    float3( 0.2722287, 0.6740818, 0.0536895),
    float3(-0.0055746, 0.6740818, 1.0103391)
);

static const float3x3 ACESccttoXYZ = ACEScctoXYZ;
=======
=======
>>>>>>> upstream/spirv_intrinsics
NBL_CONSTEXPR float32_t3x3 scRGBtoXYZ = float32_t3x3(
    float32_t3(0.412391f, 0.357584f, 0.180481f),
    float32_t3(0.212639f, 0.715169f, 0.072192f),
    float32_t3(0.019331f, 0.119195f, 0.950532f)
);

NBL_CONSTEXPR float32_t3x3 sRGBtoXYZ = scRGBtoXYZ;

NBL_CONSTEXPR float32_t3x3 BT709toXYZ = scRGBtoXYZ;


NBL_CONSTEXPR float32_t3x3 Display_P3toXYZ = float32_t3x3(
    float32_t3(0.4865709486f, 0.2656676932f, 0.1982172852f),
    float32_t3(0.2289745641f, 0.6917385218f, 0.0792869141f),
    float32_t3(0.0000000000f, 0.0451133819f, 1.0439443689f)
);


NBL_CONSTEXPR float32_t3x3 DCI_P3toXYZ = float32_t3x3(
    float32_t3(1.0f, 0.0f, 0.0f),
    float32_t3(0.0f, 1.0f, 0.0f),
    float32_t3(0.0f, 0.0f, 1.0f)
);


NBL_CONSTEXPR float32_t3x3 BT2020toXYZ = float32_t3x3(
    float32_t3(0.636958f, 0.144617f, 0.168881f),
    float32_t3(0.262700f, 0.677998f, 0.059302f),
    float32_t3(0.000000f, 0.028073f, 1.060985f)
);

NBL_CONSTEXPR float32_t3x3 HDR10_ST2084toXYZ = BT2020toXYZ;

NBL_CONSTEXPR float32_t3x3 DOLBYIVISIONtoXYZ = BT2020toXYZ;

NBL_CONSTEXPR float32_t3x3 HDR10_HLGtoXYZ = BT2020toXYZ;


NBL_CONSTEXPR float32_t3x3 AdobeRGBtoXYZ = float32_t3x3(
    float32_t3(0.5766690429f, 0.1855582379f, 0.1882286462f),
    float32_t3(0.2973449753f, 0.6273635663f, 0.0752914585f),
    float32_t3(0.0270313614f, 0.0706888525f, 0.9913375368f)
);


NBL_CONSTEXPR float32_t3x3 ACES2065_1toXYZ = float32_t3x3(
    float32_t3(0.9525523959f, 0.0000000000f,  0.0000936786f),
    float32_t3(0.3439664498f, 0.7281660966f, -0.0721325464f),
    float32_t3(0.0000000000f, 0.0000000000f,  1.0088251844f)
);


NBL_CONSTEXPR float32_t3x3 ACEScctoXYZ = float32_t3x3(
    float32_t3( 0.6624541811f, 0.1340042065f, 0.1561876870f),
    float32_t3( 0.2722287168f, 0.6740817658f, 0.0536895174f),
    float32_t3(-0.0055746495f, 0.0040607335f, 1.0103391003f)
);

NBL_CONSTEXPR float32_t3x3 ACESccttoXYZ = ACEScctoXYZ;
<<<<<<< HEAD
>>>>>>> 7156483209f3077dab03775532c0e9922176b765
=======
>>>>>>> upstream/spirv_intrinsics

}
}
}

#endif