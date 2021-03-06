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

#include <OpenColorIO/OpenColorIO.h>

#include "GpuShaderUtils.h"
#include "HashUtils.h"
#include "Lut3DOp.h"
#include "Processor.h"
#include "ScanlineHelper.h"

#include <cstring>
#include <sstream>

#include <iostream>

OCIO_NAMESPACE_ENTER
{
    Processor::~Processor()
    { }
    
    
    //////////////////////////////////////////////////////////////////////////
    
    LocalProcessorRcPtr LocalProcessor::Create()
    {
        return LocalProcessorRcPtr(new LocalProcessor(), &deleter);
    }
    
    void LocalProcessor::deleter(LocalProcessor* p)
    {
        delete p;
    }
    
    LocalProcessor::LocalProcessor()
    { }
    
    LocalProcessor::~LocalProcessor()
    { }
    
    void LocalProcessor::registerOp(OpRcPtr op)
    {
        m_opVec.push_back(op);
    }
    
    void LocalProcessor::finalizeOps()
    {
        // TODO: Optimize (collapse?) chunks of the OpVec
        
        // After construction, finalize the setup
        for(unsigned int i=0; i<m_opVec.size(); ++i)
        {
            m_opVec[i]->setup();
        }
        
        // std::cerr << getInfo() << std::endl;
    }
    
    
    bool LocalProcessor::isNoOp() const
    {
        return m_opVec.empty();
    }
    
    void LocalProcessor::apply(ImageDesc& img) const
    {
        if(m_opVec.empty()) return;
        
        ScanlineHelper scanlineHelper(img);
        float * rgbaBuffer = 0;
        long numPixels = 0;
        
        while(true)
        {
            scanlineHelper.prepRGBAScanline(&rgbaBuffer, &numPixels);
            if(numPixels == 0) break;
            if(!rgbaBuffer)
                throw Exception("Cannot apply transform; null image.");
            
            for(OpRcPtrVec::size_type i=0, size = m_opVec.size(); i<size; ++i)
            {
                m_opVec[i]->apply(rgbaBuffer, numPixels);
            }
            
            scanlineHelper.finishRGBAScanline();
        }
    }
    
    void LocalProcessor::applyRGB(float * pixel) const
    {
        if(m_opVec.empty()) return;
        
        // We need to allocate a temp array as the pixel must be 4 floats in size
        // (otherwise, sse loads will potentially fail)
        
        float rgbaBuffer[4] = { pixel[0], pixel[1], pixel[2], 0.0f };
        
        for(OpRcPtrVec::size_type i=0, size = m_opVec.size(); i<size; ++i)
        {
            m_opVec[i]->apply(rgbaBuffer, 1);
        }
        
        pixel[0] = rgbaBuffer[0];
        pixel[1] = rgbaBuffer[1];
        pixel[2] = rgbaBuffer[2];
    }
    
    void LocalProcessor::applyRGBA(float * pixel) const
    {
        for(OpRcPtrVec::size_type i=0, size = m_opVec.size(); i<size; ++i)
        {
            m_opVec[i]->apply(pixel, 1);
        }
    }
    
    
    
    
    
    
    
    
    ///////////////////////////////////////////////////////////////////////////
    
    namespace
    {
        void WriteShaderHeader(std::ostringstream* shader, const std::string & pixelName,
                               const GpuShaderDesc & shaderDesc)
        {
            if(!shader) return;
            
            std::string lut3dName = "lut3d";
            
            *shader << "\n// Generated by OpenColorIO" << "\n";
            *shader << "\n";
            
            GpuLanguage lang = shaderDesc.getLanguage();
            
            std::string fcnName = shaderDesc.getFunctionName();
            
            if(lang == GPU_LANGUAGE_CG)
            {
                *shader << "half4 " << fcnName << "(in half4 inPixel," << "\n";
            }
            else if(lang == GPU_LANGUAGE_GLSL_1_0 || lang == GPU_LANGUAGE_GLSL_1_3)
            {
                *shader << "vec4 " << fcnName << "(in vec4 inPixel, \n";
            }
            else throw Exception("Unsupported shader language.");
            
            *shader << "    const uniform sampler3D " << lut3dName << ") \n";
            *shader << "{" << "\n";
            
            if(lang == GPU_LANGUAGE_CG)
            {
                *shader << "    half4 " << pixelName << " = inPixel; \n";
            }
            else if(lang == GPU_LANGUAGE_GLSL_1_0 || lang == GPU_LANGUAGE_GLSL_1_3)
            {
                *shader << "    vec4 " << pixelName << " = inPixel; \n";
            }
            else throw Exception("Unsupported shader language.");
        }
        
        
        void WriteShaderFooter(std::ostringstream* shader,
                               const std::string & pixelName,
                               const GpuShaderDesc & /*shaderDesc*/)
        {
            *shader << "    return " << pixelName << ";\n";
            *shader << "}" << "\n";
            *shader << "\n";
        }
        
        // Find the minimal index range in the opVec that does not support
        // shader text generation.  The endIndex *is* inclusive.
        // 
        // I.e., if the entire opVec does not support GPUShaders, the
        // result will be startIndex = 0, endIndex = opVec.size() - 1
        // 
        // If the entire opVec supports GPU generation, both the
        // startIndex and endIndex will equal -1
        
        void GetGpuUnsupportedIndexRange(int * startIndex, int * endIndex,
                                         const OpRcPtrVec & opVec)
        {
            int start = -1;
            int end = -1;
            
            for(unsigned int i=0; i<opVec.size(); ++i)
            {
                // We've found a gpu unsupported op.
                // If it's the first, save it as our start.
                // Otherwise, update the end.
                
                if(!opVec[i]->supportsGpuShader())
                {
                    if(start<0)
                    {
                        start = i;
                        end = i;
                    }
                    else end = i;
                }
            }
            
            if(startIndex) *startIndex = start;
            if(endIndex) *endIndex = end;
        }
    }
    
    std::string LocalProcessor::getInfo() const
    {
        std::ostringstream os;
        os << "Local Processor" << std::endl;
        os << "Size " << m_opVec.size() << std::endl;
        
        // Partition the op vector into the 
        // interior index range does not support the gpu shader.
        // This is used to bound our analytical shader text generation
        // start index and end index are inclusive.
        
        int lut3DOpStartIndex = 0;
        int lut3DOpEndIndex = 0;
        
        GetGpuUnsupportedIndexRange(&lut3DOpStartIndex,
                                    &lut3DOpEndIndex,
                                    m_opVec);
        
        os << "Lut3DOpStartIndex: " << lut3DOpStartIndex << std::endl;
        os << "Lut3DOpEndIndex: " << lut3DOpEndIndex << std::endl;
        os << std::endl;
        
        for(int i=0; i<(int)m_opVec.size(); ++i)
        {
            os << "Index " << i << " -- " << *m_opVec[i] << std::endl;
            os << "      supportsGPUShader: " << m_opVec[i]->supportsGpuShader() << std::endl;
            if(i>=lut3DOpStartIndex && i<=lut3DOpEndIndex)
            {
                os << "      Will be processed on Lut3D lattice" << std::endl;
            }
            else
            {
                os << "      Will be processed as shader text" << std::endl;
            }
            os << "      cacheID " << m_opVec[i]->getCacheID() << std::endl;
        }
        
        return os.str();
    }
    
    const char * LocalProcessor::getGpuShaderText(const GpuShaderDesc & shaderDesc) const
    {
        // Partition the op vector into the 
        // interior index range does not support the gpu shader.
        // This is used to bound our analytical shader text generation
        // start index and end index are inclusive.
        
        int lut3DOpStartIndex = 0;
        int lut3DOpEndIndex = 0;
        
        GetGpuUnsupportedIndexRange(&lut3DOpStartIndex,
                                    &lut3DOpEndIndex,
                                    m_opVec);
        
        
        ///////////////////////////////
        
        
        std::ostringstream shader;
        
        std::string pixelName = "out_pixel";
        std::string lut3dName = "lut3d";
        
        WriteShaderHeader(&shader, pixelName, shaderDesc);
        
        // Write the entire shader using only shader text.
        // (3d lut is unused)
        if(lut3DOpStartIndex == -1 && lut3DOpEndIndex == -1)
        {
            for(unsigned int i=0; i<m_opVec.size(); ++i)
            {
                m_opVec[i]->writeGpuShader(shader, pixelName, shaderDesc);
            }
        }
        // Analytical -> 3dlut -> analytical
        else
        {
            // Handle analytical shader block before start index.
            for(int i=0; i<lut3DOpStartIndex; ++i)
            {
                m_opVec[i]->writeGpuShader(shader, pixelName, shaderDesc);
            }
            
            // We're at the end of the 3D block,
            // sample the lut3D, with the proper allocation,
            // and return to shader text generation
            // TODO: Sample lut with proper allocation
            
            int lut3DEdgeLen = shaderDesc.getLut3DEdgeLen();
            
            shader << "    " << pixelName << ".rgb = ";
            Write_sampleLut3D_rgb(&shader, pixelName,
                                  lut3dName, lut3DEdgeLen,
                                  shaderDesc.getLanguage());
            
            // Handle analytical shader block after end index.
            
            for(int i=lut3DOpEndIndex+1; i<(int)m_opVec.size(); ++i)
            {
                m_opVec[i]->writeGpuShader(shader, pixelName, shaderDesc);
            }
        }
        
        WriteShaderFooter(&shader, pixelName, shaderDesc);
        
        // TODO: This is not multi-thread safe. Cache result or mutex
        m_shaderText = shader.str();
        
        return m_shaderText.c_str();
    }
    
    const char * LocalProcessor::getGpuLut3DCacheID(const GpuShaderDesc & shaderDesc) const
    {
        int lut3DOpStartIndex = 0;
        int lut3DOpEndIndex = 0;
        
        GetGpuUnsupportedIndexRange(&lut3DOpStartIndex,
                                    &lut3DOpEndIndex,
                                    m_opVec);
        
        // Can we write the entire shader using only shader text?
        // Lut3D is not needed. Blank it.
        
        if(lut3DOpStartIndex == -1 && lut3DOpEndIndex == -1)
        {
            // TODO: This is not multi-thread safe. Cache result or mutex
            m_lut3DHash = "<NULL>";
            return m_lut3DHash.c_str();
        }
        
        // For all ops that will contribute to the 3D lut,
        // add it to the hash
        
        std::ostringstream idhash;
        
        for(int i=lut3DOpStartIndex; i<=lut3DOpEndIndex; ++i)
        {
            idhash << m_opVec[i]->getCacheID() << " ";
        }
        
        // Also, add a hash of the shader description
        idhash << shaderDesc.getLanguage() << " ";
        idhash << shaderDesc.getFunctionName() << " ";
        idhash << shaderDesc.getLut3DEdgeLen() << " ";
        std::string fullstr = idhash.str();
        
        // TODO: This is not multi-thread safe. Cache result or mutex
        m_lut3DHash = CacheIDHash(fullstr.c_str(), (int)fullstr.size());
        return m_lut3DHash.c_str();
    }
    
    void LocalProcessor::getGpuLut3D(float* lut3d, const GpuShaderDesc & shaderDesc) const
    {
        if(!lut3d) return;
        
        // Partition the op vector into the 
        // interior index range does not support the Gpu shader.
        // This is used to bound our analytical shader text generation
        // start index and end index are inclusive.
        
        int lut3DOpStartIndex = 0;
        int lut3DOpEndIndex = 0;
        
        GetGpuUnsupportedIndexRange(&lut3DOpStartIndex,
                                    &lut3DOpEndIndex,
                                    m_opVec);
        
        ///////////////////////////////
        
        // Can we write the entire shader using only shader text
        // Lut3D is not needed. Blank it.
        
        int lut3DEdgeLen = shaderDesc.getLut3DEdgeLen();
        int lut3DNumPixels = lut3DEdgeLen*lut3DEdgeLen*lut3DEdgeLen;
        
        if(lut3DOpStartIndex == -1 && lut3DOpEndIndex == -1)
        {
            memset(lut3d, 0, sizeof(float) * 3 * lut3DNumPixels);
            return;
        }
        
        
        // Allocate rgba 3dlut image
        float lut3DRGBABuffer[lut3DNumPixels*4];
        GenerateIdentityLut3D(lut3DRGBABuffer, lut3DEdgeLen, 4);
        
        // TODO: Sample lut with proper allocation
        // For now, assume a range of [0,1] has been handled
        
        for(int i=lut3DOpStartIndex; i<=lut3DOpEndIndex; ++i)
        {
            m_opVec[i]->apply(lut3DRGBABuffer, lut3DNumPixels);
        }
        
        // Copy the lut3d rgba image to the lut3d
        for(int i=0; i<lut3DNumPixels; ++i)
        {
            lut3d[3*i+0] = lut3DRGBABuffer[4*i+0];
            lut3d[3*i+1] = lut3DRGBABuffer[4*i+1];
            lut3d[3*i+2] = lut3DRGBABuffer[4*i+2];
        }
    }
}
OCIO_NAMESPACE_EXIT
