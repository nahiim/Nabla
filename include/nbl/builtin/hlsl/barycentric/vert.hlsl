// Copyright (C) 2018-2020 - DevSH Graphics Programming Sp. z O.O.
// This file is part of the "Nabla Engine".
// For conditions of distribution and use, see copyright notice in nabla.h

#ifndef _NBL_BUILTIN_HLSL_BARYCENTRIC_VERT_INCLUDED_
#define _NBL_BUILTIN_HLSL_BARYCENTRIC_VERT_INCLUDED_

namespace nbl
{
namespace hlsl
{
namespace barycentric
{

    Struct Vert
    {
        #ifndef NBL_BUILTIN_HLSL_BARYCENTRIC_VERT_POS_OUTPUT
        #define NBL_BUILTIN_HLSL_BARYCENTRIC_VERT_POS_OUTPUT
            #ifndef NBL_BUILTIN_HLSL_BARYCENTRIC_VERT_POS_OUTPUT_LOC
            #define NBL_BUILTIN_HLSL_BARYCENTRIC_VERT_POS_OUTPUT_LOC 0
            #endif
            
        layout(location = NBL_BUILTIN_HLSL_BARYCENTRIC_VERT_POS_OUTPUT_LOC) out float3 pos;
        #endif

        #ifndef NBL_BUILTIN_HLSL_BARYCENTRIC_VERT_PROVOKINGPOS_OUTPUT
        #define NBL_BUILTIN_HLSL_BARYCENTRIC_VERT_PROVOKINGPOS_OUTPUT
            #ifndef NBL_BUILTIN_HLSL_BARYCENTRIC_VERT_PROVOKINGPOS_OUTPUT_LOC
            #define NBL_BUILTIN_HLSL_BARYCENTRIC_VERT_PROVOKINGPOS_OUTPUT_LOC 1
            #endif

            layout(location = NBL_BUILTIN_HLSL_BARYCENTRIC_VERT_PROVOKINGPOS_OUTPUT_LOC) flat out float3 provokingPos;
        #endif


        void set(float3 pos);
    }




    void set(in float3 pos)
    {
        this->pos = pos;
        this->provokingPos = pos;
    }


    #endif    
}
}
}




#endif