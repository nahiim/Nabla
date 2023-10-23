
// Copyright (C) 2018-2022 - DevSH Graphics Programming Sp. z O.O.
// This file is part of the "Nabla Engine".
// For conditions of distribution and use, see copyright notice in nabla.h

#ifndef _NBL_BUILTIN_HLSL_COLOR_SPACE_OETF_INCLUDED_
#define _NBL_BUILTIN_HLSL_COLOR_SPACE_OETF_INCLUDED_

<<<<<<< HEAD
#include <nbl/builtin/hlsl/common.hlsl>
=======
//#include <nbl/builtin/hlsl/common.hlsl>
#include <nbl/builtin/hlsl/cpp_compat.hlsl>
#include <nbl/builtin/hlsl/cpp_compat/promote.hlsl>
#include <nbl/builtin/hlsl/type_traits.hlsl>
>>>>>>> 7156483209f3077dab03775532c0e9922176b765

namespace nbl
{
namespace hlsl
{
namespace colorspace
{
namespace oetf
{

<<<<<<< HEAD
float3 identity(in float3 _linear)
=======
template<typename T>
T identity(NBL_CONST_REF_ARG(T) _linear)
>>>>>>> 7156483209f3077dab03775532c0e9922176b765
{
    return _linear;
}

<<<<<<< HEAD
float3 impl_shared_2_4(in float3 _linear, in float vertex)
{
    bool3 right = (_linear > vertex.xxx);
    return lerp(_linear * 12.92, pow(_linear, (1.0 / 2.4).xxx) * 1.055 - (0.055).xxx, right);
}

// compatible with scRGB as well
float3 sRGB(in float3 _linear)
{
    bool3 negatif = (_linear < (0.0).xxx);
    float3 absVal = impl_shared_2_4(abs(_linear), 0.0031308);
=======
template<typename T>
T impl_shared_2_4(NBL_CONST_REF_ARG(T) _linear, typename scalar_type<T>::type vertex)
{
    typedef typename scalar_type<T>::type Val_t;
    bool3 right = (_linear > promote<T, Val_t>(vertex));
    return lerp(_linear * Val_t(12.92), pow(_linear, promote<T, Val_t>(1.0 / 2.4)) * Val_t(1.055) - (Val_t(0.055)), right);
}

// compatible with scRGB as well
template<typename T>
T sRGB(NBL_CONST_REF_ARG(T) _linear)
{
    typedef typename scalar_type<T>::type Val_t;
    bool3 negatif = (_linear < promote<T, Val_t>(0.0));
    T absVal = impl_shared_2_4<T>(abs(_linear), 0.0031308);
>>>>>>> 7156483209f3077dab03775532c0e9922176b765
    return lerp(absVal, -absVal, negatif);
}

// also known as P3-D65
<<<<<<< HEAD
float3 Display_P3(in float3 _linear)
{
    return impl_shared_2_4(_linear, 0.0030186);
}

float3 DCI_P3_XYZ(in float3 _linear)
{
    return pow(_linear / 52.37, (1.0 / 2.6).xxx);
}

float3 SMPTE_170M(in float3 _linear)
{
    // ITU specs (and the outlier BT.2020) give different constants for these, but they introduce discontinuities in the mapping
    // because HDR swapchains often employ the RGBA16_SFLOAT format, this would become apparent because its higher precision than 8,10,12 bits
    const float alpha = 1.099296826809443; // 1.099 for all ITU but the BT.2020 12 bit encoding, 1.0993 otherwise
    const float3 beta = (0.018053968510808).xxx; // 0.0181 for all ITU but the BT.2020 12 bit encoding, 0.18 otherwise
    return lerp(_linear * 4.5, pow(_linear, (0.45).xxx) * alpha - (alpha - 1.0).xxx, (_linear >= beta));
}

float3 SMPTE_ST2084(in float3 _linear)
{
    const float3 m1 = (0.1593017578125).xxx;
    const float3 m2 = (78.84375).xxx;
    const float c2 = 18.8515625;
    const float c3 = 18.68875;
    const float3 c1 = (c3 - c2 + 1.0).xxx;

    float3 L_m1 = pow(_linear, m1);
    return pow((c1 + L_m1 * c2) / ((1.0).xxx + L_m1 * c3), m2);
}

// did I do this right by applying the function for every color?
float3 HDR10_HLG(in float3 _linear)
{
    // done with log2 so constants are different
    const float a = 0.1239574303172;
    const float3 b = (0.02372241).xxx;
    const float3 c = (1.0042934693729).xxx;
    bool3 right = (_linear > (1.0 / 12.0).xxx);
    return lerp(sqrt(_linear * 3.0), log2(_linear - b) * a + c, right);
}

float3 AdobeRGB(in float3 _linear)
{
    return pow(_linear, (1.0 / 2.19921875).xxx);
}

float3 Gamma_2_2(in float3 _linear)
{
    return pow(_linear, (1.0 / 2.2).xxx);
}

float3 ACEScc(in float3 _linear)
{
    bool3 mid = (_linear >= (0.0).xxx);
    bool3 right = (_linear >= (0.000030517578125).xxx);
    return (log2(lerp((0.0000152587890625).xxx, (0.0).xxx, right) + _linear * lerp((0.0).xxx, lerp((0.5).xxx, (1.0).xxx, right), mid)) + (9.72).xxx) / 17.52;
}

float3 ACEScct(in float3 _linear)
{
    bool3 right = (_linear > (0.0078125).xxx);
    return lerp(10.5402377416545 * _linear + 0.0729055341958355, (log2(_linear) + (9.72).xxx) / 17.52, right);
=======
template<typename T>
T Display_P3(NBL_CONST_REF_ARG(T) _linear)
{
    return impl_shared_2_4<T>(_linear, 0.0030186);
}

template<typename T>
T DCI_P3_XYZ(NBL_CONST_REF_ARG(T) _linear)
{
    typedef typename scalar_type<T>::type Val_t;
    return pow(_linear / Val_t(52.37), promote<T, Val_t>(1.0 / 2.6));
}

template<typename T>
T SMPTE_170M(NBL_CONST_REF_ARG(T) _linear)
{
    typedef typename scalar_type<T>::type Val_t;
    // ITU specs (and the outlier BT.2020) give different constants for these, but they introduce discontinuities in the mapping
    // because HDR swapchains often employ the RGBA16_SFLOAT format, this would become apparent because its higher precision than 8,10,12 bits
    const Val_t alpha = 1.099296826809443; // 1.099 for all ITU but the BT.2020 12 bit encoding, 1.0993 otherwise
    const T beta = promote<T, Val_t>(0.018053968510808); // 0.0181 for all ITU but the BT.2020 12 bit encoding, 0.18 otherwise
    return lerp(_linear * Val_t(4.5), pow(_linear, promote<T, Val_t>(0.45)) * alpha - promote<T, Val_t>(alpha - 1.0), (_linear >= beta));
}

template<typename T>
T SMPTE_ST2084(NBL_CONST_REF_ARG(T) _linear)
{
    typedef typename scalar_type<T>::type Val_t;
    const T m1 = promote<T, Val_t>(0.1593017578125);
    const T m2 = promote<T, Val_t>(78.84375);
    const Val_t c2 = 18.8515625;
    const Val_t c3 = 18.68875;
    const T c1 = promote<T, Val_t>(c3 - c2 + 1.0);

    T L_m1 = pow(_linear, m1);
    return pow((c1 + L_m1 * c2) / (promote<T, Val_t>(1.0) + L_m1 * c3), m2);
}

// did I do this right by applying the function for every color?
template<typename T>
T HDR10_HLG(NBL_CONST_REF_ARG(T) _linear)
{
    typedef typename scalar_type<T>::type Val_t;
    
    // done with log2 so constants are different
    const Val_t a = 0.1239574303172;
    const T b = promote<T, Val_t>(0.02372241);
    const T c = promote<T, Val_t>(1.0042934693729);
    bool3 right = (_linear > promote<T, Val_t>(1.0 / 12.0));
    return lerp(sqrt(_linear * Val_t(3.0)), log2(_linear - b) * a + c, right);
}

template<typename T>
T AdobeRGB(NBL_CONST_REF_ARG(T) _linear)
{
    typedef typename scalar_type<T>::type Val_t;
    return pow(_linear, promote<T, Val_t>(1.0 / 2.19921875));
}

template<typename T>
T Gamma_2_2(NBL_CONST_REF_ARG(T) _linear)
{
    typedef typename scalar_type<T>::type Val_t;
    return pow(_linear, promote<T, Val_t>(1.0 / 2.2));
}

template<typename T>
T ACEScc(NBL_CONST_REF_ARG(T) _linear)
{
    typedef typename scalar_type<T>::type Val_t;
    bool3 mid = (_linear >= promote<T, Val_t>(0.0));
    bool3 right = (_linear >= promote<T, Val_t>(0.000030517578125));
    return (log2(lerp(promote<T, Val_t>(0.0000152587890625), promote<T, Val_t>(0.0), right) + _linear * lerp(promote<T, Val_t>(0.0), lerp(promote<T, Val_t>(0.5), promote<T, Val_t>(1.0), right), mid)) + promote<T, Val_t>(9.72)) / Val_t(17.52);
}

template<typename T>
T ACEScct(NBL_CONST_REF_ARG(T) _linear)
{
    typedef typename scalar_type<T>::type Val_t;
    bool3 right = (_linear > promote<T, Val_t>(0.0078125));
    return lerp(Val_t(10.5402377416545) * _linear + Val_t(0.0729055341958355), (log2(_linear) + promote<T, Val_t>(9.72)) / Val_t(17.52), right);
>>>>>>> 7156483209f3077dab03775532c0e9922176b765
}

}
}
}
}

#endif