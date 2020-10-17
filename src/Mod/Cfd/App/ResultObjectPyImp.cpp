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

#include <Mod/CFD/App/ResultObjectPy.h>
#include <Mod/CFD/App/ResultObjectPy.cpp>

using namespace Cfd;

// returns a string which represents the object e.g. when printed in python
std::string ResultObjectPy::representation(void) const
{
    return std::string("<ResultObject>");
}

//PyObject* ResultObjectPy::read(PyObject *args)
//{
//	char* Name;
//	if (PyArg_ParseTuple(args, "et", "utf-8", &Name)) {
//		getResultObjectPtr()->read(Base::FileInfo(Name));
//		PyMem_Free(Name);
//		Py_Return;
//	}
//	return 0;
//}
//
//PyObject* ResultObjectPy::advance(PyObject *args)
//{
//	long index = 0;
//    if (PyArg_ParseTuple(args, "l", &index)) {
//        getResultObjectPtr()->advance(index);
//        Py_Return;
//    }
//    return 0;
//}

PyObject *ResultObjectPy::getCustomAttributes(const char* /*attr*/) const
{
    return 0;
}

int ResultObjectPy::setCustomAttributes(const char* /*attr*/, PyObject* /*obj*/)
{
    return 0;
}
