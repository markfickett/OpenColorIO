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

#include "PyFileTransform.h"
#include "PyUtil.h"

OCIO_NAMESPACE_ENTER
{
    ///////////////////////////////////////////////////////////////////////////
    ///
    
    bool AddFileTransformObjectToModule( PyObject* m )
    {
        PyOCIO_FileTransformType.tp_new = PyType_GenericNew;
        if ( PyType_Ready(&PyOCIO_FileTransformType) < 0 ) return false;
        
        Py_INCREF( &PyOCIO_FileTransformType );
        PyModule_AddObject(m, "FileTransform",
                (PyObject *)&PyOCIO_FileTransformType);
        
        return true;
    }
    
    PyObject * BuildConstPyFileTransform(ConstFileTransformRcPtr transform)
    {
        if (transform.get() == 0x0)
        {
            PyErr_SetString(PyExc_ValueError, "Cannot create PyFileTransform from null object.");
            return NULL;
        }
        
        PyOCIO_FileTransform * pytransform = PyObject_New(
                PyOCIO_FileTransform, (PyTypeObject * ) &PyOCIO_FileTransformType);
        
        pytransform->constcppobj = new OCIO::ConstFileTransformRcPtr();
        *pytransform->constcppobj = transform;
        
        pytransform->cppobj = new OCIO::FileTransformRcPtr();
        pytransform->isconst = true;
        
        return ( PyObject * ) pytransform;
    }
    
    PyObject * BuildEditablePyFileTransform(FileTransformRcPtr transform)
    {
        if (transform.get() == 0x0)
        {
            PyErr_SetString(PyExc_ValueError, "Cannot create PyFileTransform from null object.");
            return NULL;
        }
        
        PyOCIO_FileTransform * pytransform = PyObject_New(
                PyOCIO_FileTransform, (PyTypeObject * ) &PyOCIO_FileTransformType);
        
        pytransform->constcppobj = new OCIO::ConstFileTransformRcPtr();
        pytransform->cppobj = new OCIO::FileTransformRcPtr();
        *pytransform->cppobj = transform;
        
        pytransform->isconst = false;
        
        return ( PyObject * ) pytransform;
    }
    
    bool IsPyFileTransform(PyObject * pyobject)
    {
        if(!pyobject) return false;
        return (PyObject_Type(pyobject) == (PyObject *) (&PyOCIO_FileTransformType));
    }
    
    bool IsPyFileTransformEditable(PyObject * pyobject)
    {
        if(!IsPyFileTransform(pyobject))
        {
            throw Exception("PyObject must be an OCIO::FileTransform.");
        }
        
        PyOCIO_FileTransform * pytransform = reinterpret_cast<PyOCIO_FileTransform *> (pyobject);
        return (!pytransform->isconst);
    }
    
    ConstFileTransformRcPtr GetConstFileTransform(PyObject * pyobject, bool allowCast)
    {
        if(!IsPyFileTransform(pyobject))
        {
            throw Exception("PyObject must be an OCIO::FileTransform.");
        }
        
        PyOCIO_FileTransform * pytransform = reinterpret_cast<PyOCIO_FileTransform *> (pyobject);
        if(pytransform->isconst && pytransform->constcppobj)
        {
            return *pytransform->constcppobj;
        }
        
        if(allowCast && !pytransform->isconst && pytransform->cppobj)
        {
            return *pytransform->cppobj;
        }
        
        throw Exception("PyObject must be a valid OCIO::FileTransform.");
    }
    
    FileTransformRcPtr GetEditableFileTransform(PyObject * pyobject)
    {
        if(!IsPyFileTransform(pyobject))
        {
            throw Exception("PyObject must be an OCIO::FileTransform.");
        }
        
        PyOCIO_FileTransform * pytransform = reinterpret_cast<PyOCIO_FileTransform *> (pyobject);
        if(!pytransform->isconst && pytransform->cppobj)
        {
            return *pytransform->cppobj;
        }
        
        throw Exception("PyObject must be an editable OCIO::FileTransform.");
    }
    
    
    
    ///////////////////////////////////////////////////////////////////////////
    ///
    
    
    
    
    
    
    
    
    
    
    namespace
    {
        int PyOCIO_FileTransform_init( PyOCIO_FileTransform * self, PyObject * args, PyObject * kwds );
        void PyOCIO_FileTransform_delete( PyOCIO_FileTransform * self, PyObject * args );
        PyObject * PyOCIO_FileTransform_isEditable( PyObject * self );
        PyObject * PyOCIO_FileTransform_createEditableCopy( PyObject * self );
        
        PyObject * PyOCIO_FileTransform_getDirection( PyObject * self );
        PyObject * PyOCIO_FileTransform_setDirection( PyObject * self,  PyObject *args );
        PyObject * PyOCIO_FileTransform_getSrc( PyObject * self );
        PyObject * PyOCIO_FileTransform_setSrc( PyObject * self,  PyObject *args );
        PyObject * PyOCIO_FileTransform_getInterpolation( PyObject * self );
        PyObject * PyOCIO_FileTransform_setInterpolation( PyObject * self,  PyObject *args );
        
        ///////////////////////////////////////////////////////////////////////
        ///
        
        PyMethodDef PyOCIO_FileTransform_methods[] = {
            {"isEditable", (PyCFunction) PyOCIO_FileTransform_isEditable, METH_NOARGS, "" },
            {"createEditableCopy", (PyCFunction) PyOCIO_FileTransform_createEditableCopy, METH_NOARGS, "" },
            
            {"getDirection", (PyCFunction) PyOCIO_FileTransform_getDirection, METH_NOARGS, "" },
            {"setDirection", PyOCIO_FileTransform_setDirection, METH_VARARGS, "" },
            {"getSrc", (PyCFunction) PyOCIO_FileTransform_getSrc, METH_NOARGS, "" },
            {"setSrc", PyOCIO_FileTransform_setSrc, METH_VARARGS, "" },
            {"getInterpolation", (PyCFunction) PyOCIO_FileTransform_getInterpolation, METH_NOARGS, "" },
            {"setInterpolation", PyOCIO_FileTransform_setInterpolation, METH_VARARGS, "" },
            
            {NULL, NULL, 0, NULL}
        };
    }
    
    ///////////////////////////////////////////////////////////////////////////
    ///
    
    PyTypeObject PyOCIO_FileTransformType = {
        PyObject_HEAD_INIT(NULL)
        0,                                          //ob_size
        "OCIO.FileTransform",                        //tp_name
        sizeof(PyOCIO_FileTransform),                //tp_basicsize
        0,                                          //tp_itemsize
        (destructor)PyOCIO_FileTransform_delete,     //tp_dealloc
        0,                                          //tp_print
        0,                                          //tp_getattr
        0,                                          //tp_setattr
        0,                                          //tp_compare
        0,                                          //tp_repr
        0,                                          //tp_as_number
        0,                                          //tp_as_sequence
        0,                                          //tp_as_mapping
        0,                                          //tp_hash 
        0,                                          //tp_call
        0,                                          //tp_str
        0,                                          //tp_getattro
        0,                                          //tp_setattro
        0,                                          //tp_as_buffer
        Py_TPFLAGS_DEFAULT | Py_TPFLAGS_BASETYPE,   //tp_flags
        "FileTransform",                            //tp_doc 
        0,                                          //tp_traverse 
        0,                                          //tp_clear 
        0,                                          //tp_richcompare 
        0,                                          //tp_weaklistoffset 
        0,                                          //tp_iter 
        0,                                          //tp_iternext 
        PyOCIO_FileTransform_methods,                //tp_methods 
        0,                                          //tp_members 
        0,                                          //tp_getset 
        0,                                          //tp_base 
        0,                                          //tp_dict 
        0,                                          //tp_descr_get 
        0,                                          //tp_descr_set 
        0,                                          //tp_dictoffset 
        (initproc) PyOCIO_FileTransform_init,        //tp_init 
        0,                                          //tp_alloc 
        0,                                          //tp_new 
        0,                                          //tp_free
        0,                                          //tp_is_gc
        0,                                          //tp_bases
        0,                                          //tp_mro
        0,                                          //tp_cache
        0,                                          //tp_subclasses
        0,                                          //tp_weaklist
        0,                                          //tp_del
        #if PY_VERSION_HEX > 0x02060000
        0,                                          //tp_version_tag
        #endif
    };
    
    ///////////////////////////////////////////////////////////////////////////
    ///
    
    namespace
    {
        ///////////////////////////////////////////////////////////////////////
        ///
        int PyOCIO_FileTransform_init( PyOCIO_FileTransform *self, PyObject * /*args*/, PyObject * /*kwds*/ )
        {
            ///////////////////////////////////////////////////////////////////
            /// init pyobject fields
            
            self->constcppobj = new OCIO::ConstFileTransformRcPtr();
            self->cppobj = new OCIO::FileTransformRcPtr();
            self->isconst = true;
            
            try
            {
                *self->cppobj = OCIO::FileTransform::Create();
                self->isconst = false;
                return 0;
            }
            catch ( const std::exception & e )
            {
                std::string message = "Cannot create FileTransform: ";
                message += e.what();
                PyErr_SetString( PyExc_RuntimeError, message.c_str() );
                return -1;
            }
        }
        
        ////////////////////////////////////////////////////////////////////////
        
        void PyOCIO_FileTransform_delete( PyOCIO_FileTransform *self, PyObject * /*args*/ )
        {
            delete self->constcppobj;
            delete self->cppobj;
            
            self->ob_type->tp_free((PyObject*)self);
        }
        
        ////////////////////////////////////////////////////////////////////////
        
        PyObject * PyOCIO_FileTransform_isEditable( PyObject * self )
        {
            return PyBool_FromLong(IsPyFileTransformEditable(self));
        }
        
        PyObject * PyOCIO_FileTransform_createEditableCopy( PyObject * self )
        {
            try
            {
                ConstFileTransformRcPtr transform = GetConstFileTransform(self, true);
                FileTransformRcPtr copy = DynamicPtrCast<FileTransform>(transform->createEditableCopy());
                return BuildEditablePyFileTransform( copy );
            }
            catch(...)
            {
                Python_Handle_Exception();
                return NULL;
            }
        }
        
        ////////////////////////////////////////////////////////////////////////
        
        
        PyObject * PyOCIO_FileTransform_getDirection( PyObject * self )
        {
            try
            {
                ConstFileTransformRcPtr transform = GetConstFileTransform(self, true);
                TransformDirection dir = transform->getDirection();
                return PyString_FromString( TransformDirectionToString( dir ) );
            }
            catch(...)
            {
                Python_Handle_Exception();
                return NULL;
            }
        }
        
        PyObject * PyOCIO_FileTransform_setDirection( PyObject * self, PyObject * args )
        {
            try
            {
                TransformDirection dir;
                if (!PyArg_ParseTuple(args,"O&:setDirection",
                    ConvertPyObjectToTransformDirection, &dir)) return NULL;
                
                FileTransformRcPtr transform = GetEditableFileTransform(self);
                transform->setDirection( dir );
                
                Py_RETURN_NONE;
            }
            catch(...)
            {
                Python_Handle_Exception();
                return NULL;
            }
        }
        
        
        ////////////////////////////////////////////////////////////////////////
        
        
        
        PyObject * PyOCIO_FileTransform_getSrc( PyObject * self )
        {
            try
            {
                ConstFileTransformRcPtr transform = GetConstFileTransform(self, true);
                return PyString_FromString( transform->getSrc() );
            }
            catch(...)
            {
                Python_Handle_Exception();
                return NULL;
            }
        }
        
        PyObject * PyOCIO_FileTransform_setSrc( PyObject * self, PyObject * args )
        {
            try
            {
                char * src = 0;
                if (!PyArg_ParseTuple(args,"s:setSrc", &src)) return NULL;
                
                FileTransformRcPtr transform = GetEditableFileTransform(self);
                transform->setSrc( src );
                
                Py_RETURN_NONE;
            }
            catch(...)
            {
                Python_Handle_Exception();
                return NULL;
            }
        }
        
        ////////////////////////////////////////////////////////////////////////
        
        
        
        PyObject * PyOCIO_FileTransform_getInterpolation( PyObject * self )
        {
            try
            {
                ConstFileTransformRcPtr transform = GetConstFileTransform(self, true);
                Interpolation interp = transform->getInterpolation();
                return PyString_FromString( InterpolationToString( interp ) );
            }
            catch(...)
            {
                Python_Handle_Exception();
                return NULL;
            }
        }
        
        PyObject * PyOCIO_FileTransform_setInterpolation( PyObject * self, PyObject * args )
        {
            try
            {
                Interpolation interp;
                if (!PyArg_ParseTuple(args,"O&:setInterpolation",
                    ConvertPyObjectToInterpolation, &interp)) return NULL;
                
                FileTransformRcPtr transform = GetEditableFileTransform(self);
                transform->setInterpolation(interp);
                
                Py_RETURN_NONE;
            }
            catch(...)
            {
                Python_Handle_Exception();
                return NULL;
            }
        }
        
        
    }

}
OCIO_NAMESPACE_EXIT
