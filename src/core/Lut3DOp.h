/*
Copyright (c) 2003-2010 Sony Pictures Imageworks Inc., et al.
All Rights Reserved.

Redistribution and use in source and binary forms, with or without
modification, are permitted provided that the following conditions are
met:
* Redistributions of source code must retain the above copyright
  notice, this list of conditions and the following disclaimer.
* Redistributions in binary form must reproduce the above copyright
  notice, this list of conditions and the following disclaimer in the
  documentation and/or other materials provided with the distribution.
* Neither the name of Sony Pictures Imageworks nor the names of its
  contributors may be used to endorse or promote products derived from
  this software without specific prior written permission.
THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
"AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
(INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/


#ifndef INCLUDED_OCIO_LUT3DOP_H
#define INCLUDED_OCIO_LUT3DOP_H

#include <OpenColorIO/OpenColorIO.h>

#include "Op.h"
#include "Processor.h"

#include <vector>

OCIO_NAMESPACE_ENTER
{
    // TODO: Do not allow for a naked Lut3D object, always have it be an rc ptr to a lut3d.
    // Expose static factory fcn, make constructor private?
    // TODO: turn into a class instead of a struct?
    
    struct Lut3D
    {
        float from_min[3];
        float from_max[3];
        int size[3];
        
        typedef std::vector<float> fv_t;
        fv_t lut;
        
        std::string cacheID;
        
        Lut3D()
        {
            for(int i=0; i<3; ++i)
            {
                from_min[i] = 0.0f;
                from_max[i] = 1.0f;
                size[i] = 0;
            }
        };
        
        // TODO: This must be explicitly called now. Make it deferred / lazy?
        void generateCacheID();
    };
    
    typedef SharedPtr<Lut3D> Lut3DRcPtr;
    
    // This memory arrangement order corresponds to the standard
    // OpenGL memory layout, as well as the SPI Lut
    
    inline int GetGLLut3DArrayOffset(int indexR, int indexG, int indexB,
                                     int sizeR,  int sizeG,  int /*sizeB*/)
    {
        return 3 * (indexR + sizeR * (indexG + sizeG * indexB));
    }
    
    inline int GetAutodeskLut3DArrayOffset(int indexR, int indexG, int indexB,
                                           int /*sizeR*/,  int sizeG,  int sizeB)
    {
        return 3 * (indexB + sizeB * (indexG + sizeG * indexR));
    }
    
    // This generates an identity 3d lut in GL layout
    void GenerateIdentityLut3D(float* img, int edgeLen, int numChannels);
    
    void CreateLut3DOp(LocalProcessor & processor,
                       Lut3DRcPtr lut,
                       Interpolation interpolation,
                       TransformDirection direction);
}
OCIO_NAMESPACE_EXIT

#endif
