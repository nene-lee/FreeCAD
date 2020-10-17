/***************************************************************************
 *   Copyright (c) 2017 Werner Mayer <wmayer[at]users.sourceforge.net>     *
 *                                                                         *
 *   This file is part of the FreeCAD CAx development system.              *
 *                                                                         *
 *   This library is free software; you can redistribute it and/or         *
 *   modify it under the terms of the GNU Library General Public           *
 *   License as published by the Free Software Foundation; either          *
 *   version 2 of the License, or (at your option) any later version.      *
 *                                                                         *
 *   This library  is distributed in the hope that it will be useful,      *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU Library General Public License for more details.                  *
 *                                                                         *
 *   You should have received a copy of the GNU Library General Public     *
 *   License along with this library; see the file COPYING.LIB. If not,    *
 *   write to the Free Software Foundation, Inc., 59 Temple Place,         *
 *   Suite 330, Boston, MA  02111-1307, USA                                *
 *                                                                         *
 ***************************************************************************/


#include "PreCompiled.h"

#include "PostPipeline.h"
#include <Base/FileInfo.h>

#include <Mod/CFD/App/PostObjectPy.h>
#include <Mod/CFD/App/PostObjectPy.cpp>

using namespace Cfd;

// returns a string which represents the object e.g. when printed in python
std::string PostObjectPy::representation(void) const
{
    return std::string("<PostObject>");
}

PyObject* PostObjectPy::load(PyObject *args)
{
    PyObject* py;
    if (!PyArg_ParseTuple(args, "O!", &(App::DocumentObjectPy::Type), &py))
        return 0;

    App::DocumentObject* obj = static_cast<App::DocumentObjectPy*>(py)->getDocumentObjectPtr();
    if (!obj->getTypeId().isDerivedFrom(PostObject::getClassTypeId())) {
        PyErr_SetString(PyExc_TypeError, "object is not a post object");
        return 0;
    }

    getPostObjectPtr()->load(static_cast<PostObject*>(obj));
    Py_Return;
}

PyObject *PostObjectPy::getCustomAttributes(const char* /*attr*/) const
{
    return 0;
}

int PostObjectPy::setCustomAttributes(const char* /*attr*/, PyObject* /*obj*/)
{
    return 0;
}
