
// Copyright (C) 2018-2022 - DevSH Graphics Programming Sp. z O.O.
// This file is part of the "Nabla Engine".
// For conditions of distribution and use, see copyright notice in nabla.h

#ifndef _NBL_BUILTIN_HLSL_SCENE_KEYFRAME_INCLUDED_
#define _NBL_BUILTIN_HLSL_SCENE_KEYFRAME_INCLUDED_



#include <nbl/builtin/hlsl/scene/keyframe.hlsl>
#include <nbl/builtin/hlsl/math/quaternions.hlsl>



namespace nbl
{
namespace hlsl
{
namespace scene
{


using namespace math;

float3 Keyframe_t::getScale()
{
	return decodeRGB18E7S3(this->data[2]);
}

quaternion_t Keyframe_t::getRotation()
{
	return { decode8888Quaternion(this->data[1][1]) };
}

float3 Keyframe_t::getTranslation()
{
	return uintBitsToFloat(uint3(this->data[0].xy, keyframe.data[1][0]));
}



FatKeyframe_t FatKeyframe_t::FatKeyframe_t(in Keyframe_t keyframe)
{
	FatKeyframe_t result;
	result.scale       = Keyframe_t::getScale(keyframe);
	result.rotation    = Keyframe_t::getRotation(keyframe);
	result.translation = Keyframe_t::getTranslation(keyframe);

	return result;
}

FatKeyframe_t FatKeyframe_t::interpolate(in FatKeyframe_t start, in FatKeyframe_t end, in float fraction)
{
	FatKeyframe_t result;
	result.scale = mix(start.scale, end.scale, fraction);
	result.rotation = flerp(start.rotation, end.rotation, fraction);
	result.translation = mix(start.translation, end.translation, fraction);

	return result;
}

float4x3 FatKeyframe_t::constructMatrix()
{
	float3x3 rotation = constructMatrix(this->rotation);
	float4x3 tform = float4x3(rotation[0], rotation[1], rotation[2], keyframe.translation);

	for (int i=0; i<3; i++)
		tform[i] *= this->scale[i];


	return tform;
}


} 
}
}





#endif