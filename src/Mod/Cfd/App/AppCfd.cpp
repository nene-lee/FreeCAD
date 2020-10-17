/***************************************************************************
 *   Copyright (c) 2008 Jürgen Riegel (juergen.riegel@web.de)              *
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
#ifndef _PreComp_
# include <Python.h>
#endif


#include <Base/Console.h>
#include <Base/PyObjectBase.h>
#include <Base/Interpreter.h>
#include <CXX/Extensions.hxx>

#include "SolverObject.h"
#include "ResultObjectPy.h"
#include "PostPipelinePy.h"
#include "PostFilter.h"
#include "PostFunction.h"

#include "PropertyVtkDataObject.h"

namespace Cfd {

	extern PyObject* initModule();

}

/* Python entry */
PyMOD_INIT_FUNC(Cfd)
{
	// load dependent module
	try {
		//Base::Interpreter().loadModule("Part");
		//Base::Interpreter().loadModule("Mesh");
	}
	catch (const Base::Exception& e) {
		PyErr_SetString(PyExc_ImportError, e.what());
		PyMOD_Return(0);
	}
	PyObject* cfdModule = Cfd::initModule();
	Base::Console().Log("Loading CFD module... done\n");

	// Add Types to module
	Base::Interpreter().addType(&Cfd::ResultObjectPy::Type, cfdModule, "ResultObject");
	Base::Interpreter().addType(&Cfd::PostPipelinePy::Type, cfdModule, "PostPipeline");

	// init Type system
	Cfd::SolverObject::init();

	Cfd::ResultObject::init();
	Cfd::PostObject::init();
	Cfd::PostFilter::init();
	Cfd::PostPipeline::init();
	Cfd::PostFunction::init();
	Cfd::PostFunctionProvider::init();
	Cfd::PostPlaneFunction::init();
	Cfd::PostSphereFunction::init();
	Cfd::PostWarpVectorFilter::init();
	Cfd::PostCutFilter::init();
	Cfd::PostClipFilter::init();
	Cfd::PostScalarClipFilter::init();
	Cfd::PostContourFilter::init();
	Cfd::PostGlyph3DFilter::init();
	Cfd::PostStreamlineFilter::init();
	Cfd::PostDataAlongLineFilter::init();
	Cfd::PostDataAtPointFilter::init();
	
	Cfd::PropertyVtkDataObject::init();

    PyMOD_Return(cfdModule);
}
