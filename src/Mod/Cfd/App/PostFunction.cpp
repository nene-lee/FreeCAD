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


#include "PreCompiled.h"

#ifndef _PreComp_
#endif

#include <Base/Console.h>

#include "PostFunction.h"

using namespace Cfd;
using namespace App;


PROPERTY_SOURCE(Cfd::PostFunction, App::DocumentObject)

PostFunction::PostFunction()
{
}

PostFunction::~PostFunction()
{
}

DocumentObjectExecReturn* PostFunction::execute(void) {

    return DocumentObject::StdReturn;
}

PROPERTY_SOURCE(Cfd::PostFunctionProvider, App::DocumentObject)

PostFunctionProvider::PostFunctionProvider(void) : DocumentObject() {

	ADD_PROPERTY(Functions, (0));
}

PostFunctionProvider::~PostFunctionProvider() {

}

void PostFunctionProvider::onChanged(const Property* prop) {
	App::DocumentObject::onChanged(prop);
}


////////////////////////////////////////////////////////////////////////////////////////////
PROPERTY_SOURCE(Cfd::PostPlaneFunction, Cfd::PostFunction)

PostPlaneFunction::PostPlaneFunction(void): PostFunction() {

    ADD_PROPERTY(Origin,(Base::Vector3d(0.0,0.0,0.0)));
    ADD_PROPERTY(Normal,(Base::Vector3d(0.0,0.0,1.0)));

    m_plane = vtkSmartPointer<vtkPlane>::New();
    m_implicit = m_plane;

    m_plane->SetOrigin(0., 0., 0.);
    m_plane->SetNormal(0., 0., 1.);
}

PostPlaneFunction::~PostPlaneFunction() {

}

void PostPlaneFunction::onChanged(const Property* prop) {

    if(prop == &Origin) {
        const Base::Vector3d& vec = Origin.getValue();
        m_plane->SetOrigin(vec[0], vec[1], vec[2]);
    }
    else if(prop == &Normal) {
        const Base::Vector3d& vec = Normal.getValue();
        m_plane->SetNormal(vec[0], vec[1], vec[2]);
    }

    Cfd::PostFunction::onChanged(prop);
}

////////////////////////////////////////////////////////////////////////////////////////////
PROPERTY_SOURCE(Cfd::PostSphereFunction, Cfd::PostFunction)

PostSphereFunction::PostSphereFunction(void): PostFunction() {

    ADD_PROPERTY(Radius,(5));
    ADD_PROPERTY(Center,(Base::Vector3d(1.0,0.0,0.0)));

    m_sphere = vtkSmartPointer<vtkSphere>::New();
    m_implicit = m_sphere;

    m_sphere->SetCenter(0., 0., 0.);
    m_sphere->SetRadius(5);
}

PostSphereFunction::~PostSphereFunction() {

}

void PostSphereFunction::onChanged(const Property* prop) {

    if(prop == &Center) {
        const Base::Vector3d& vec = Center.getValue();
        m_sphere->SetCenter(vec[0], vec[1], vec[2]);
    }
    else if(prop == &Radius) {
        m_sphere->SetRadius(Radius.getValue());
    }

    Cfd::PostFunction::onChanged(prop);
}
