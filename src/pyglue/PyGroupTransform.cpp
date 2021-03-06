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

#include "PyGroupTransform.h"
#include "PyUtil.h"

OCIO_NAMESPACE_ENTER
{
    ///////////////////////////////////////////////////////////////////////////
    ///
    
    bool AddGroupTransformObjectToModule( PyObject* m )
    {
        PyOCIO_GroupTransformType.tp_new = PyType_GenericNew;
        if ( PyType_Ready(&PyOCIO_GroupTransformType) < 0 ) return false;
        
        Py_INCREF( &PyOCIO_GroupTransformType );
        PyModule_AddObject(m, "GroupTransform",
                (PyObject *)&PyOCIO_GroupTransformType);
        
        return true;
    }
    
    PyObject * BuildConstPyGroupTransform(ConstGroupTransformRcPtr transform)
    {
        if (transform.get() == 0x0)
        {
            PyErr_SetString(PyExc_ValueError, "Cannot create PyGroupTransform from null object.");
            return NULL;
        }
        
        PyOCIO_GroupTransform * pytransform = PyObject_New(
                PyOCIO_GroupTransform, (PyTypeObject * ) &PyOCIO_GroupTransformType);
        
        pytransform->constcppobj = new OCIO::ConstGroupTransformRcPtr();
        *pytransform->constcppobj = transform;
        
        pytransform->cppobj = new OCIO::GroupTransformRcPtr();
        pytransform->isconst = true;
        
        return ( PyObject * ) pytransform;
    }
    
    PyObject * BuildEditablePyGroupTransform(GroupTransformRcPtr transform)
    {
        if (transform.get() == 0x0)
        {
            PyErr_SetString(PyExc_ValueError, "Cannot create PyGroupTransform from null object.");
            return NULL;
        }
        
        PyOCIO_GroupTransform * pytransform = PyObject_New(
                PyOCIO_GroupTransform, (PyTypeObject * ) &PyOCIO_GroupTransformType);
        
        pytransform->constcppobj = new OCIO::ConstGroupTransformRcPtr();
        pytransform->cppobj = new OCIO::GroupTransformRcPtr();
        *pytransform->cppobj = transform;
        
        pytransform->isconst = false;
        
        return ( PyObject * ) pytransform;
    }
    
    bool IsPyGroupTransform(PyObject * pyobject)
    {
        if(!pyobject) return false;
        return (PyObject_Type(pyobject) == (PyObject *) (&PyOCIO_GroupTransformType));
    }
    
    bool IsPyGroupTransformEditable(PyObject * pyobject)
    {
        if(!IsPyGroupTransform(pyobject))
        {
            throw Exception("PyObject must be an OCIO::GroupTransform.");
        }
        
        PyOCIO_GroupTransform * pytransform = reinterpret_cast<PyOCIO_GroupTransform *> (pyobject);
        return (!pytransform->isconst);
    }
    
    ConstGroupTransformRcPtr GetConstGroupTransform(PyObject * pyobject, bool allowCast)
    {
        if(!IsPyGroupTransform(pyobject))
        {
            throw Exception("PyObject must be an OCIO::GroupTransform.");
        }
        
        PyOCIO_GroupTransform * pytransform = reinterpret_cast<PyOCIO_GroupTransform *> (pyobject);
        if(pytransform->isconst && pytransform->constcppobj)
        {
            return *pytransform->constcppobj;
        }
        
        if(allowCast && !pytransform->isconst && pytransform->cppobj)
        {
            return *pytransform->cppobj;
        }
        
        throw Exception("PyObject must be a valid OCIO::GroupTransform.");
    }
    
    GroupTransformRcPtr GetEditableGroupTransform(PyObject * pyobject)
    {
        if(!IsPyGroupTransform(pyobject))
        {
            throw Exception("PyObject must be an OCIO::GroupTransform.");
        }
        
        PyOCIO_GroupTransform * pytransform = reinterpret_cast<PyOCIO_GroupTransform *> (pyobject);
        if(!pytransform->isconst && pytransform->cppobj)
        {
            return *pytransform->cppobj;
        }
        
        throw Exception("PyObject must be an editable OCIO::GroupTransform.");
    }
    
    
    
    ///////////////////////////////////////////////////////////////////////////
    ///
    
    
    
    
    
    
    
    
    
    
    namespace
    {
        int PyOCIO_GroupTransform_init( PyOCIO_GroupTransform * self, PyObject * args, PyObject * kwds );
        void PyOCIO_GroupTransform_delete( PyOCIO_GroupTransform * self, PyObject * args );
        PyObject * PyOCIO_GroupTransform_isEditable( PyObject * self );
        PyObject * PyOCIO_GroupTransform_createEditableCopy( PyObject * self );
        
        PyObject * PyOCIO_GroupTransform_getDirection( PyObject * self );
        PyObject * PyOCIO_GroupTransform_setDirection( PyObject * self,  PyObject *args );
        PyObject * PyOCIO_GroupTransform_getTransform( PyObject * self,  PyObject *args );
        PyObject * PyOCIO_GroupTransform_getEditableTransform( PyObject * self,  PyObject *args );
        
        PyObject * PyOCIO_GroupTransform_size( PyObject * self );
        PyObject * PyOCIO_GroupTransform_push_back( PyObject * self,  PyObject *args );
        PyObject * PyOCIO_GroupTransform_clear( PyObject * self );
        PyObject * PyOCIO_GroupTransform_empty( PyObject * self );
        
        ///////////////////////////////////////////////////////////////////////
        ///
        
        PyMethodDef PyOCIO_GroupTransform_methods[] = {
            {"isEditable", (PyCFunction) PyOCIO_GroupTransform_isEditable, METH_NOARGS, "" },
            {"createEditableCopy", (PyCFunction) PyOCIO_GroupTransform_createEditableCopy, METH_NOARGS, "" },
            
            {"getDirection", (PyCFunction) PyOCIO_GroupTransform_getDirection, METH_NOARGS, "" },
            {"setDirection", PyOCIO_GroupTransform_setDirection, METH_VARARGS, "" },
            {"getTransform", PyOCIO_GroupTransform_getTransform, METH_VARARGS, "" },
            {"getEditableTransform", PyOCIO_GroupTransform_getEditableTransform, METH_VARARGS, "" },
            
            {"size", (PyCFunction) PyOCIO_GroupTransform_size, METH_NOARGS, "" },
            {"push_back", PyOCIO_GroupTransform_push_back, METH_VARARGS, "" },
            {"clear", (PyCFunction) PyOCIO_GroupTransform_clear, METH_NOARGS, "" },
            {"empty", (PyCFunction) PyOCIO_GroupTransform_empty, METH_NOARGS, "" },
            
            {NULL, NULL, 0, NULL}
        };
    }
    
    ///////////////////////////////////////////////////////////////////////////
    ///
    
    PyTypeObject PyOCIO_GroupTransformType = {
        PyObject_HEAD_INIT(NULL)
        0,                                          //ob_size
        "OCIO.GroupTransform",                       //tp_name
        sizeof(PyOCIO_GroupTransform),               //tp_basicsize
        0,                                          //tp_itemsize
        (destructor)PyOCIO_GroupTransform_delete,    //tp_dealloc
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
        "GroupTransform",                           //tp_doc 
        0,                                          //tp_traverse 
        0,                                          //tp_clear 
        0,                                          //tp_richcompare 
        0,                                          //tp_weaklistoffset 
        0,                                          //tp_iter 
        0,                                          //tp_iternext 
        PyOCIO_GroupTransform_methods,               //tp_methods 
        0,                                          //tp_members 
        0,                                          //tp_getset 
        0,                                          //tp_base 
        0,                                          //tp_dict 
        0,                                          //tp_descr_get 
        0,                                          //tp_descr_set 
        0,                                          //tp_dictoffset 
        (initproc) PyOCIO_GroupTransform_init,       //tp_init 
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
        int PyOCIO_GroupTransform_init( PyOCIO_GroupTransform *self, PyObject * /*args*/, PyObject * /*kwds*/ )
        {
            ///////////////////////////////////////////////////////////////////
            /// init pyobject fields
            
            self->constcppobj = new OCIO::ConstGroupTransformRcPtr();
            self->cppobj = new OCIO::GroupTransformRcPtr();
            self->isconst = true;
            
            try
            {
                *self->cppobj = OCIO::GroupTransform::Create();
                self->isconst = false;
                return 0;
            }
            catch ( const std::exception & e )
            {
                std::string message = "Cannot create GroupTransform: ";
                message += e.what();
                PyErr_SetString( PyExc_RuntimeError, message.c_str() );
                return -1;
            }
        }
        
        ////////////////////////////////////////////////////////////////////////
        
        void PyOCIO_GroupTransform_delete( PyOCIO_GroupTransform *self, PyObject * /*args*/ )
        {
            delete self->constcppobj;
            delete self->cppobj;
            
            self->ob_type->tp_free((PyObject*)self);
        }
        
        ////////////////////////////////////////////////////////////////////////
        
        PyObject * PyOCIO_GroupTransform_isEditable( PyObject * self )
        {
            return PyBool_FromLong(IsPyGroupTransformEditable(self));
        }
        
        PyObject * PyOCIO_GroupTransform_createEditableCopy( PyObject * self )
        {
            try
            {
                ConstGroupTransformRcPtr transform = GetConstGroupTransform(self, true);
                GroupTransformRcPtr copy = DynamicPtrCast<GroupTransform>(transform->createEditableCopy());
                return BuildEditablePyGroupTransform( copy );
            }
            catch(...)
            {
                Python_Handle_Exception();
                return NULL;
            }
        }
        
        ////////////////////////////////////////////////////////////////////////
        
        
        PyObject * PyOCIO_GroupTransform_getDirection( PyObject * self )
        {
            try
            {
                ConstGroupTransformRcPtr transform = GetConstGroupTransform(self, true);
                TransformDirection dir = transform->getDirection();
                return PyString_FromString( TransformDirectionToString( dir ) );
            }
            catch(...)
            {
                Python_Handle_Exception();
                return NULL;
            }
        }
        
        PyObject * PyOCIO_GroupTransform_setDirection( PyObject * self, PyObject * args )
        {
            try
            {
                TransformDirection dir;
                if (!PyArg_ParseTuple(args,"O&:setDirection",
                    ConvertPyObjectToTransformDirection, &dir)) return NULL;
                
                GroupTransformRcPtr transform = GetEditableGroupTransform(self);
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
        
        
        PyObject * PyOCIO_GroupTransform_getTransform( PyObject * self, PyObject * args )
        {
            try
            {
                int index = 0;
                
                if (!PyArg_ParseTuple(args,"i:getTransform", &index)) return NULL;
                
                ConstGroupTransformRcPtr transform = GetConstGroupTransform(self, true);
                ConstTransformRcPtr childTransform = transform->getTransform(index);
                
                return BuildConstPyTransform(childTransform);
            }
            catch(...)
            {
                Python_Handle_Exception();
                return NULL;
            }
        }
        
        PyObject * PyOCIO_GroupTransform_getEditableTransform( PyObject * self, PyObject * args )
        {
            try
            {
                int index = 0;
                
                if (!PyArg_ParseTuple(args,"i:getEditableTransform", &index)) return NULL;
                
                GroupTransformRcPtr transform = GetEditableGroupTransform(self);
                TransformRcPtr childTransform = transform->getEditableTransform(index);
                
                return BuildEditablePyTransform(childTransform);
            }
            catch(...)
            {
                Python_Handle_Exception();
                return NULL;
            }
        }
        
        
        
        ////////////////////////////////////////////////////////////////////////
        
        
        PyObject * PyOCIO_GroupTransform_size( PyObject * self )
        {
            try
            {
                ConstGroupTransformRcPtr transform = GetConstGroupTransform(self, true);
                return PyInt_FromLong( transform->size() );
            }
            catch(...)
            {
                Python_Handle_Exception();
                return NULL;
            }
        }
        
        
        PyObject * PyOCIO_GroupTransform_push_back( PyObject * self,  PyObject *args )
        {
            try
            {
                PyObject * pytransform = 0;
                
                if (!PyArg_ParseTuple(args,"O:push_back", &pytransform)) return NULL;
                
                GroupTransformRcPtr transform = GetEditableGroupTransform(self);
                
                if(!IsPyTransform(pytransform))
                {
                    throw Exception("GroupTransform.push_back requires a transform as the first arg.");
                }
                
                transform->push_back( GetConstTransform(pytransform, true) );
                
                Py_RETURN_NONE;
            }
            catch(...)
            {
                Python_Handle_Exception();
                return NULL;
            }
        }
        
        
        PyObject * PyOCIO_GroupTransform_clear( PyObject * self )
        {
            try
            {
                GroupTransformRcPtr transform = GetEditableGroupTransform(self);
                transform->clear();
                
                Py_RETURN_NONE;
            }
            catch(...)
            {
                Python_Handle_Exception();
                return NULL;
            }
        }
        
        
        PyObject * PyOCIO_GroupTransform_empty( PyObject * self )
        {
            try
            {
                ConstGroupTransformRcPtr transform = GetConstGroupTransform(self, true);
                return PyBool_FromLong( transform->empty() );
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
