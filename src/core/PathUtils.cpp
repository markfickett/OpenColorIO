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

#include "PathUtils.h"
#include "pystring/pystring.h"

#include <sstream>
#include <iostream>

OCIO_NAMESPACE_ENTER
{
    namespace path
    {
        // TODO: make these also work on windows
        // This attempts to match python's path.join, including
        // the relative absolute handling
        
        std::string join(const std::string & path1, const std::string & path2)
        {
            std::string pathtoken = "/";
            
            // Absolute paths should be treated as absolute
            if(pystring::startswith(path2, pathtoken))
                return path2;
            
            // Relative paths will be appended.
            if (pystring::endswith(path1, pathtoken))
                return path1 + path2;
            
            return path1 + pathtoken + path2;
        }
        
        // TODO: This doesnt return the same result for python in '/foo' case
        std::string dirname(const std::string & path)
        {
            std::vector<std::string> result;
            pystring::rsplit(path, result, "/", 1);
            if(result.size() == 2)
            {
                return result[0];
            }
            return "";
        }
    }
}
OCIO_NAMESPACE_EXIT
