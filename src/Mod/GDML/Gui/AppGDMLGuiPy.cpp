/***************************************************************************
 *   Copyright (c) YEAR YOUR NAME         <Your e-mail address>            *
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
# include <QFileInfo>
#endif

#include <Base/Console.h>
#include <Base/Exception.h>
#include <Base/PyObjectBase.h>

#include <CXX/Extensions.hxx>
#include <CXX/Objects.hxx>


 ///BEGIN CAD-GDML
#include <xercesc/parsers/XercesDOMParser.hpp>
#include <xercesc/util/PlatformUtils.hpp>
#include <xercesc/sax/HandlerBase.hpp>
#include <xercesc/util/XMLUni.hpp>
#include <xercesc/dom/DOM.hpp>

#include "./GDML/persistency/G4GDMLParser.hh"
#include "./GDML/G4FreeCAD.h"

namespace GDMLGui {
	class Module : public Py::ExtensionModule<Module>
	{
	public:
		Module() : Py::ExtensionModule<Module>("GDMLGui")
		{
			add_varargs_method("open", &Module::open,
				"open(string) -- Create a new document and load the GDML file into the document."
			);
			add_varargs_method("export", &Module::exporter,
				"export(list,string) -- Export a list of objects into a single file."
			);

			initialize("This module is the CfdGui module."); // register with Python
		}

		virtual ~Module() {}

	private:
		virtual Py::Object invoke_method_varargs(void *method_def, const Py::Tuple &args)
		{
			try {
				return Py::ExtensionModule<Module>::invoke_method_varargs(method_def, args);
			}
			catch (const Base::Exception &e) {
				throw Py::RuntimeError(e.what());
			}
			catch (const std::exception &e) {
				throw Py::RuntimeError(e.what());
			}
		}

		Py::Object open(const Py::Tuple& args)
		{
			const char* Name;
			if (!PyArg_ParseTuple(args.ptr(), "s", &Name))
				throw Py::Exception();

			try {
				//	Base::Console().Message("parser.Read %s \n",Name);
				Base::FileInfo file(Name);
				G4GDMLParser parser;
				parser.Read(Name,true);
				Base::Console().Message("GDML2FreeCAD\n");
				G4FreeCAD Conversion;
				//Begin Get filename without extension
				/**/	
					char *filename = (char *)Name;
					char *moduleId = std::max(strrchr(filename, '/'), strrchr(filename,'\\'));
					moduleId = moduleId ? moduleId : filename;
					std::string resultString = moduleId;
					size_t pos = resultString.rfind('.');
					resultString = resultString.substr(0, pos);
				/**/
				//End Get filename without extension
				//	Conversion.GDML2FreeCAD(&parser,moduleId);
				Conversion.GDML2FreeCAD(&parser,resultString);
			} 
			catch (const Base::Exception& e) {
				throw Py::RuntimeError(e.what());
			}

			return Py::None();
		}

		Py::Object exporter(const Py::Tuple& args)
		{
			PyObject* object;
			const char* filename;
			if (!PyArg_ParseTuple(args.ptr(), "Os", &object, &filename))
				throw Py::Exception();

			try {
				Base::Console().Message("FreeCAD2GDML");
				Base::FileInfo file(filename);
				G4FreeCAD Conversion;
				G4GDMLParser parser;
				//Base::Console().Message("FileName %s \n",resultString.c_str());
				//Conversion.FreeCAD2GDML(&parser,filename,resultString);
				Conversion.FreeCAD2GDML(&parser,filename);
			}
			catch (const Base::Exception& e) {
				throw Py::RuntimeError(e.what());
			}

			return Py::None();
		}
	};

	PyObject* initModule()
	{
		return (new Module)->module().ptr();
	}

} // namespace GDMLGui
