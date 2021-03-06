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

#include "pystring/pystring.h"
#include "tinyxml/tinyxml.h"

#include <sstream>

OCIO_NAMESPACE_ENTER
{
    const char * BoolToString(bool val)
    {
        if(val) return "true";
        return "false";
    }
    
    bool BoolFromString(const char * s)
    {
        std::string str = pystring::lower(s);
        if((str == "true") || (str=="yes")) return true;
        return false;
    }
    
    const char * TransformDirectionToString(TransformDirection dir)
    {
        if(dir == TRANSFORM_DIR_FORWARD) return "forward";
        else if(dir == TRANSFORM_DIR_INVERSE) return "inverse";
        return "unknown";
    }
    
    TransformDirection TransformDirectionFromString(const char * s)
    {
        std::string str = pystring::lower(s);
        if(str == "forward") return TRANSFORM_DIR_FORWARD;
        else if(str == "inverse") return TRANSFORM_DIR_INVERSE;
        return TRANSFORM_DIR_UNKNOWN;
    }
    
    TransformDirection CombineTransformDirections(TransformDirection d1,
                                                  TransformDirection d2)
    {
        // Any unknowns always combine to be unknown.
        if(d1 == TRANSFORM_DIR_UNKNOWN || d2 == TRANSFORM_DIR_UNKNOWN)
            return TRANSFORM_DIR_UNKNOWN;
        
        if(d1 == TRANSFORM_DIR_FORWARD && d2 == TRANSFORM_DIR_FORWARD)
            return TRANSFORM_DIR_FORWARD;
        
        if(d1 == TRANSFORM_DIR_INVERSE && d2 == TRANSFORM_DIR_INVERSE)
            return TRANSFORM_DIR_FORWARD;
        
        return TRANSFORM_DIR_INVERSE;
    }
    
    TransformDirection GetInverseTransformDirection(TransformDirection dir)
    {
        if(dir == TRANSFORM_DIR_FORWARD) return TRANSFORM_DIR_INVERSE;
        else if(dir == TRANSFORM_DIR_INVERSE) return TRANSFORM_DIR_FORWARD;
        return TRANSFORM_DIR_UNKNOWN;
    }
    
    const char * ColorSpaceDirectionToString(ColorSpaceDirection dir)
    {
        if(dir == COLORSPACE_DIR_TO_REFERENCE) return "to_reference";
        else if(dir == COLORSPACE_DIR_FROM_REFERENCE) return "from_reference";
        return "unknown";
    }
    
    ColorSpaceDirection ColorSpaceDirectionFromString(const char * s)
    {
        std::string str = pystring::lower(s);
        if(str == "to_reference") return COLORSPACE_DIR_TO_REFERENCE;
        else if(str == "from_reference") return COLORSPACE_DIR_FROM_REFERENCE;
        return COLORSPACE_DIR_UNKNOWN;
    }
    
    const char * BitDepthToString(BitDepth bitDepth)
    {
        if(bitDepth == BIT_DEPTH_UINT8) return "8ui";
        else if(bitDepth == BIT_DEPTH_UINT10) return "10ui";
        else if(bitDepth == BIT_DEPTH_UINT12) return "12ui";
        else if(bitDepth == BIT_DEPTH_UINT14) return "14ui";
        else if(bitDepth == BIT_DEPTH_UINT16) return "16ui";
        else if(bitDepth == BIT_DEPTH_UINT32) return "32ui";
        else if(bitDepth == BIT_DEPTH_F16) return "16f";
        else if(bitDepth == BIT_DEPTH_F32) return "32f";
        return "unknown";
    }
    
    BitDepth BitDepthFromString(const char * s)
    {
        std::string str = pystring::lower(s);
        if(str == "8ui") return BIT_DEPTH_UINT8;
        else if(str == "10ui") return BIT_DEPTH_UINT10;
        else if(str == "12ui") return BIT_DEPTH_UINT12;
        else if(str == "14ui") return BIT_DEPTH_UINT14;
        else if(str == "16ui") return BIT_DEPTH_UINT16;
        else if(str == "32ui") return BIT_DEPTH_UINT32;
        else if(str == "16f") return BIT_DEPTH_F16;
        else if(str == "32f") return BIT_DEPTH_F32;
        return BIT_DEPTH_UNKNOWN;
    }
    
    bool BitDepthIsFloat(BitDepth bitDepth)
    {
        if(bitDepth == BIT_DEPTH_F16) return true;
        else if(bitDepth == BIT_DEPTH_F32) return true;
        return false;
    }
    
    int BitDepthToInt(BitDepth bitDepth)
    {
        if(bitDepth == BIT_DEPTH_UINT8) return 8;
        else if(bitDepth == BIT_DEPTH_UINT10) return 10;
        else if(bitDepth == BIT_DEPTH_UINT12) return 12;
        else if(bitDepth == BIT_DEPTH_UINT14) return 14;
        else if(bitDepth == BIT_DEPTH_UINT16) return 16;
        else if(bitDepth == BIT_DEPTH_UINT32) return 32;
        
        return 0;
    }
    
    const char * GpuAllocationToString(GpuAllocation alloc)
    {
        if(alloc == GPU_ALLOCATION_UNIFORM) return "uniform";
        else if(alloc == GPU_ALLOCATION_LG2) return "lg2";
        return "unknown";
    }
    
    GpuAllocation GpuAllocationFromString(const char * s)
    {
        std::string str = pystring::lower(s);
        if(str == "uniform") return GPU_ALLOCATION_UNIFORM;
        else if(str == "lg2") return GPU_ALLOCATION_LG2;
        return GPU_ALLOCATION_UNKNOWN;
    }
    
    const char * InterpolationToString(Interpolation interp)
    {
        if(interp == INTERP_NEAREST) return "nearest";
        else if(interp == INTERP_LINEAR) return "linear";
        return "unknown";
    }
    
    Interpolation InterpolationFromString(const char * s)
    {
        std::string str = pystring::lower(s);
        if(str == "nearest") return INTERP_NEAREST;
        else if(str == "linear") return INTERP_LINEAR;
        return INTERP_UNKNOWN;
    }
    
    
    
    const char * ROLE_REFERENCE = "reference";
    const char * ROLE_DATA = "data";
    const char * ROLE_COLOR_PICKING = "color_picking";
    const char * ROLE_SCENE_LINEAR = "scene_linear";
    const char * ROLE_COMPOSITING_LOG = "compositing_log";
    const char * ROLE_COLOR_TIMING = "color_timing";
    
    
    
    //////////////////////////////////////////////////////////////////////////
    
    
    // http://ticpp.googlecode.com/svn/docs/ticpp_8h-source.html#l01670

    void SetText( TiXmlElement* element, const char * value)
    {
        if ( element->NoChildren() )
        {
            element->LinkEndChild( new TiXmlText( value ) );
        }
        else
        {
            if ( 0 == element->GetText() )
            {
                element->InsertBeforeChild( element->FirstChild(), TiXmlText( value ) );
            }
            else
            {
                // There already is text, so change it
                element->FirstChild()->SetValue( value );
            }
        }
    }
    
    
    
    //////////////////////////////////////////////////////////////////////////
    
    namespace
    {
        const int FLOAT_DECIMALS = 7;
        const int DOUBLE_DECIMALS = 16;
    }
    
    std::string FloatToString(float value)
    {
        std::ostringstream pretty;
        pretty.precision(FLOAT_DECIMALS);
        pretty << value;
        return pretty.str();
    }
    
    std::string FloatVecToString(const float * fval, unsigned int size)
    {
        if(size<=0) return "";
        
        std::ostringstream pretty;
        pretty.precision(FLOAT_DECIMALS);
        for(unsigned int i=0; i<size; ++i)
        {
            if(i!=0) pretty << " ";
            pretty << fval[i];
        }
        
        return pretty.str();
    }
    
    bool StringToFloat(float * fval, const char * str)
    {
        if(!str) return false;
        
        std::istringstream inputStringstream(str);
        float x;
        if(!(inputStringstream >> x))
        {
            return false;
        }
        
        if(fval) *fval = x;
        return true;
    }
    
    std::string DoubleToString(double value)
    {
        std::ostringstream pretty;
        pretty.precision(DOUBLE_DECIMALS);
        pretty << value;
        return pretty.str();
    }
    
    
    bool StringVecToFloatVec(std::vector<float> &floatArray,
                             const std::vector<std::string> &lineParts)
    {
        floatArray.resize(lineParts.size());
        
        for(unsigned int i=0; i<lineParts.size(); i++)
        {
            std::istringstream inputStringstream(lineParts[i]);
            float x;
            if(!(inputStringstream >> x))
            {
                return false;
            }
            floatArray[i] = x;
        }
        
        return true;
    }
    
    
    bool StringVecToIntVec(std::vector<int> &intArray,
                           const std::vector<std::string> &lineParts)
    {
        intArray.resize(lineParts.size());
        
        for(unsigned int i=0; i<lineParts.size(); i++)
        {
            std::istringstream inputStringstream(lineParts[i]);
            int x;
            if(!(inputStringstream >> x))
            {
                return false;
            }
            intArray[i] = x;
        }
        
        return true;
    }

}
OCIO_NAMESPACE_EXIT
