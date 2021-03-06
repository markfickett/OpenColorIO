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
namespace OCIO = OCIO_NAMESPACE;

#include "PyColorSpace.h"
#include "PyConfig.h"
#include "PyFileTransform.h"
#include "PyGroupTransform.h"
#include "PyUtil.h"

namespace
{
    PyObject * PyOCIO_GetCurrentConfig(PyObject * /* self */)
    {
        try
        {
            return OCIO::BuildConstPyConfig( OCIO::GetCurrentConfig() );
        }
        catch(...)
        {
            OCIO::Python_Handle_Exception();
            return NULL;
        }
    }
    
    PyObject * PyOCIO_SetCurrentConfig(PyObject * /*self*/, PyObject * args)
    {
        try
        {
            PyObject * pyconfig;
            if (!PyArg_ParseTuple(args, "O!:SetCurrentConfig",
                &OCIO::PyOCIO_ConfigType, &pyconfig))
            {
                return NULL;
            }
            
            OCIO::ConstConfigRcPtr c = OCIO::GetConstConfig(pyconfig, true);
            OCIO::SetCurrentConfig(c);
            
            Py_RETURN_NONE;
        }
        catch(...)
        {
            OCIO::Python_Handle_Exception();
            return NULL;
        }
    }
    
    
    PyMethodDef PyOCIO_methods[] = {
        {"GetCurrentConfig",
            (PyCFunction) PyOCIO_GetCurrentConfig,
            METH_NOARGS,
            ""},
        
        {"SetCurrentConfig",
            (PyCFunction) PyOCIO_SetCurrentConfig,
            METH_VARARGS,
            ""},
        
        {NULL, NULL, 0, NULL} /* Sentinel */
    };
}

extern "C"
PyMODINIT_FUNC
initPyOpenColorIO(void)
{
    PyObject * m;
    
    m = Py_InitModule3("PyOpenColorIO", PyOCIO_methods, "OpenColorIO API");
    
    OCIO::AddColorSpaceObjectToModule( m );
    OCIO::AddConfigObjectToModule( m );
    OCIO::AddFileTransformObjectToModule( m );
    OCIO::AddGroupTransformObjectToModule( m );
    
    // Add Module Constants
    PyModule_AddStringConstant(m, "TRANSFORM_DIR_UNKNOWN",
        OCIO::TransformDirectionToString(OCIO::TRANSFORM_DIR_UNKNOWN));
    PyModule_AddStringConstant(m, "TRANSFORM_DIR_FORWARD",
        OCIO::TransformDirectionToString(OCIO::TRANSFORM_DIR_FORWARD));
    PyModule_AddStringConstant(m, "TRANSFORM_DIR_INVERSE",
        OCIO::TransformDirectionToString(OCIO::TRANSFORM_DIR_INVERSE));
    
    PyModule_AddStringConstant(m, "COLORSPACE_DIR_UNKNOWN",
        OCIO::ColorSpaceDirectionToString(OCIO::COLORSPACE_DIR_UNKNOWN));
    PyModule_AddStringConstant(m, "COLORSPACE_DIR_TO_REFERENCE",
        OCIO::ColorSpaceDirectionToString(OCIO::COLORSPACE_DIR_TO_REFERENCE));
    PyModule_AddStringConstant(m, "COLORSPACE_DIR_FROM_REFERENCE",
        OCIO::ColorSpaceDirectionToString(OCIO::COLORSPACE_DIR_FROM_REFERENCE));
    
    PyModule_AddStringConstant(m, "BIT_DEPTH_UNKNOWN",
        OCIO::BitDepthToString(OCIO::BIT_DEPTH_UNKNOWN));
    PyModule_AddStringConstant(m, "BIT_DEPTH_UINT8",
        OCIO::BitDepthToString(OCIO::BIT_DEPTH_UINT8));
    PyModule_AddStringConstant(m, "BIT_DEPTH_UINT10",
        OCIO::BitDepthToString(OCIO::BIT_DEPTH_UINT10));
    PyModule_AddStringConstant(m, "BIT_DEPTH_UINT12",
        OCIO::BitDepthToString(OCIO::BIT_DEPTH_UINT12));
    PyModule_AddStringConstant(m, "BIT_DEPTH_UINT14",
        OCIO::BitDepthToString(OCIO::BIT_DEPTH_UINT14));
    PyModule_AddStringConstant(m, "BIT_DEPTH_UINT16",
        OCIO::BitDepthToString(OCIO::BIT_DEPTH_UINT16));
    PyModule_AddStringConstant(m, "BIT_DEPTH_UINT32",
        OCIO::BitDepthToString(OCIO::BIT_DEPTH_UINT32));
    PyModule_AddStringConstant(m, "BIT_DEPTH_F16",
        OCIO::BitDepthToString(OCIO::BIT_DEPTH_F16));
    PyModule_AddStringConstant(m, "BIT_DEPTH_F32",
        OCIO::BitDepthToString(OCIO::BIT_DEPTH_F32));
    
    PyModule_AddStringConstant(m, "GPU_ALLOCATION_UNKNOWN",
        OCIO::GpuAllocationToString(OCIO::GPU_ALLOCATION_UNKNOWN));
    PyModule_AddStringConstant(m, "GPU_ALLOCATION_UNIFORM",
        OCIO::GpuAllocationToString(OCIO::GPU_ALLOCATION_UNIFORM));
    PyModule_AddStringConstant(m, "GPU_ALLOCATION_LG2",
        OCIO::GpuAllocationToString(OCIO::GPU_ALLOCATION_LG2));
    
    PyModule_AddStringConstant(m, "INTERP_UNKNOWN",
        OCIO::InterpolationToString(OCIO::INTERP_UNKNOWN));
    PyModule_AddStringConstant(m, "INTERP_NEAREST",
        OCIO::InterpolationToString(OCIO::INTERP_NEAREST));
    PyModule_AddStringConstant(m, "INTERP_LINEAR",
        OCIO::InterpolationToString(OCIO::INTERP_LINEAR));
    
    PyModule_AddStringConstant(m, "ROLE_REFERENCE", OCIO::ROLE_REFERENCE);
    PyModule_AddStringConstant(m, "ROLE_DATA", OCIO::ROLE_DATA);
    PyModule_AddStringConstant(m, "ROLE_COLOR_PICKING", OCIO::ROLE_COLOR_PICKING);
    PyModule_AddStringConstant(m, "ROLE_SCENE_LINEAR", OCIO::ROLE_SCENE_LINEAR);
    PyModule_AddStringConstant(m, "ROLE_COMPOSITING_LOG", OCIO::ROLE_COMPOSITING_LOG);
    PyModule_AddStringConstant(m, "ROLE_COLOR_TIMING", OCIO::ROLE_COLOR_TIMING);
}
