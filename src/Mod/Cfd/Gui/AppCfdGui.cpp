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
# include <Standard_math.hxx>
#endif

#include <Base/Console.h>
#include <Base/PyObjectBase.h>
#include <Base/Interpreter.h>
#include <Gui/Application.h>
#include <Gui/WidgetFactory.h>
#include <Gui/Language/Translator.h>

#include "Workbench.h"
#include "ViewProviderSolverObject.h"
#include "ViewProviderResultObject.h"
#include "ViewProviderPostObject.h"
#include "ViewProviderPostPipeline.h"
#include "ViewProviderPostFunction.h"
#include "ViewProviderPostFilter.h"


// use a different name to CreateCommand()
void CreateMeshCommands(void);
void CreatePostCommands(void);

void loadCfdResource()
{
    // add resources and reloads the translators
    Q_INIT_RESOURCE(Cfd);
    Gui::Translator::instance()->refresh();
}

namespace CfdGui {

	extern PyObject* initModule();

}


/* Python entry */
PyMOD_INIT_FUNC(CfdGui)
{
    if (!Gui::Application::Instance) {
        PyErr_SetString(PyExc_ImportError, "Cannot load Gui module in console application.");
        PyMOD_Return(0);
    }
	// load dependent module
	try {
		Base::Interpreter().loadModule("Cfd");
	}
	catch (const Base::Exception& e) {
		PyErr_SetString(PyExc_ImportError, e.what());
		PyMOD_Return(0);
	}

    PyObject* mod = CfdGui::initModule();
    Base::Console().Log("Loading GUI of Cfd module... done\n");

    // instantiating the commands
    CreateMeshCommands();
	CreatePostCommands();

    // addition objects
    CfdGui::Workbench                                           ::init();

	CfdGui::ViewProviderSolverObject::init();

	CfdGui::ViewProviderResultObject::init();
	CfdGui::ViewProviderPostObject::init();
	CfdGui::ViewProviderPostPipeline::init();
	CfdGui::ViewProviderPostClip::init();
	CfdGui::ViewProviderPostCut::init();
	CfdGui::ViewProviderPostDataAlongLine::init();
	CfdGui::ViewProviderPostDataAtPoint::init();
	CfdGui::ViewProviderPostScalarClip::init();
	CfdGui::ViewProviderPostWarpVector::init();
	CfdGui::ViewProviderPostContour::init();
	CfdGui::ViewProviderPostGlyph3D::init();
	CfdGui::ViewProviderPostStreamline::init();
	
	CfdGui::ViewProviderPostFunction::init();
	CfdGui::ViewProviderPostFunctionProvider::init();
	CfdGui::ViewProviderPostPlaneFunction::init();
	CfdGui::ViewProviderPostSphereFunction::init();

     // add resources and reloads the translators
    loadCfdResource();

    PyMOD_Return(mod);
}
