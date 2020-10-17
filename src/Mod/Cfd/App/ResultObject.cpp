/***************************************************************************
 *   Copyright (c) 2013 Jürgen Riegel (FreeCAD@juergen-riegel.net)         *
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
#endif

#include <App/FeaturePythonPyImp.h>
#include <App/DocumentObjectPy.h>

#include "ResultObject.h"
#include "ResultObjectPy.h"

using namespace Cfd;
using namespace App;

PROPERTY_SOURCE(Cfd::ResultObject, App::DocumentObject)


ResultObject::ResultObject() {

	ADD_PROPERTY_TYPE(Pipes, (0), "Data", App::Prop_None, "The pipelines used to analyze this result");


    //Time.setStatus(App::Property::ReadOnly, true);
	Pipes.setStatus(App::Property::ReadOnly, true);
}

ResultObject::~ResultObject() {

}

App::DocumentObjectExecReturn *ResultObject::execute(void) {

	return App::DocumentObject::StdReturn;
}

short ResultObject::mustExecute(void) const {

    return 0;
}

void ResultObject::onChanged(const Property* prop) {

	App::DocumentObject::onChanged(prop);
}

PyObject *ResultObject::getPyObject() {

    if (PythonObject.is(Py::_None())) {
        // ref counter is set to 1
        PythonObject = Py::Object(new ResultObjectPy(this),true);
    }
    return Py::new_reference_to(PythonObject);
}

// Python feature ---------------------------------------------------------

namespace App {
	/// @cond DOXERR
	PROPERTY_SOURCE_TEMPLATE(Cfd::ResultObjectPython, Cfd::ResultObject)
		template<> const char* Cfd::ResultObjectPython::getViewProviderName(void) const {
		return "CfdGui::ViewProviderResultPython";
	}
	/// @endcond

	template<> PyObject* Cfd::ResultObjectPython::getPyObject(void) {
		if (PythonObject.is(Py::_None())) {
			// ref counter is set to 1
			PythonObject = Py::Object(new App::FeaturePythonPyT<App::DocumentObjectPy>(this), true);
		}
		return Py::new_reference_to(PythonObject);
	}

	// explicit template instantiation
	template class CfdExport FeaturePythonT<Cfd::ResultObject>;

}