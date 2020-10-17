/***************************************************************************
 *   Copyright (c) 2015 Stefan Tröger <stefantroeger@gmx.net>              *
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


#ifndef CFD_POST_FUNCTION_H
#define CFD_POST_FUNCTION_H

#include <App/Property.h>
#include <App/PropertyUnits.h>

#include <vtkSmartPointer.h>
#include <vtkImplicitFunction.h>
#include <vtkPlane.h>
#include <vtkSphere.h>
#include <vtkBoundingBox.h>

#include "PostObject.h"

namespace Cfd
{

class CfdExport PostFunction : public App::DocumentObject
{
    PROPERTY_HEADER(Cfd::PostFunction);

public:
    /// Constructor
    PostFunction(void);
    virtual ~PostFunction();

    virtual const char* getViewProviderName(void) const {
        return "CfdGui::ViewProviderPostFunction";
    }

    virtual App::DocumentObjectExecReturn* execute(void);

    //bound box handling
    void setBoundingBox(vtkBoundingBox b) {m_boundingBox = b;};

    //get the algorithm or the data
    vtkSmartPointer<vtkImplicitFunction> getImplicitFunction() {return m_implicit;};

protected:
    vtkSmartPointer<vtkImplicitFunction>  m_implicit;
    vtkBoundingBox                        m_boundingBox;
};

class CfdExport PostFunctionProvider : public App::DocumentObject {

	PROPERTY_HEADER(Cfd::PostFunctionProvider);

public:
	PostFunctionProvider(void);
	virtual ~PostFunctionProvider();

	virtual const char* getViewProviderName(void) const {
		return "CfdGui::ViewProviderPostFunctionProvider";
	}

	App::PropertyLinkList Functions;

protected:
	virtual void onChanged(const App::Property* prop);
};

////////////////////////////////////////////////////////////////////////////////////////////

class CfdExport PostPlaneFunction : public PostFunction
{
    PROPERTY_HEADER(Cfd::PostPlaneFunction);

public:

    PostPlaneFunction(void);
    virtual ~PostPlaneFunction();

    App::PropertyVector           Normal;
    App::PropertyVectorDistance   Origin;

    virtual const char* getViewProviderName(void) const {
        return "CfdGui::ViewProviderPostPlaneFunction";
    }

protected:
    virtual void onChanged(const App::Property* prop);

    vtkSmartPointer<vtkPlane> m_plane;
};

////////////////////////////////////////////////////////////////////////////////////////////

class CfdExport PostSphereFunction : public PostFunction
{
    PROPERTY_HEADER(Cfd::PostSphereFunction);

public:

    PostSphereFunction(void);
    virtual ~PostSphereFunction();

    App::PropertyDistance         Radius;
    App::PropertyVectorDistance   Center;

    virtual const char* getViewProviderName(void) const {
        return "FemGui::ViewProviderPostSphereFunction";
    }

protected:
    virtual void onChanged(const App::Property* prop);

    vtkSmartPointer<vtkSphere> m_sphere;
};

} //namespace Cfd


#endif // CFD_POST_FUNCTION_H
