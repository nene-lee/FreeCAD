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

#include <Base/Console.h>
#include <Gui/Application.h>

#include <Mod/Cfd/App/PostPipeline.h>
#include "ViewProviderPostPipeline.h"
#include "ViewProviderPostFunction.h"

using namespace CfdGui;


PROPERTY_SOURCE(CfdGui::ViewProviderPostPipeline, CfdGui::ViewProviderPostObject)

ViewProviderPostPipeline::ViewProviderPostPipeline()
{
    //sPixmap = "fem-post-data-pipline";
	sPixmap = "post_pipeline";
}

ViewProviderPostPipeline::~ViewProviderPostPipeline()
{
}

std::vector< App::DocumentObject* > ViewProviderPostPipeline::claimChildren(void) const {

    Cfd::PostPipeline* pipeline = static_cast<Cfd::PostPipeline*>(getObject());
    std::vector<App::DocumentObject*> children;

    if(pipeline->Functions.getValue())
        children.push_back(pipeline->Functions.getValue());

    children.insert(children.end(), pipeline->Filter.getValues().begin(), pipeline->Filter.getValues().end());
    return children;
}

std::vector< App::DocumentObject* > ViewProviderPostPipeline::claimChildren3D(void) const {

    return claimChildren();
}

void ViewProviderPostPipeline::updateData(const App::Property* prop) {
    CfdGui::ViewProviderPostObject::updateData(prop);

    if(strcmp(prop->getName(), "Function") == 0) {
        updateFunctionSize();
    }
}

void ViewProviderPostPipeline::updateFunctionSize() {

    //we need to get the bounding box and set the function provider size
    Cfd::PostPipeline* obj = static_cast<Cfd::PostPipeline*>(getObject());

    if(!obj->Functions.getValue() || !obj->Functions.getValue()->isDerivedFrom(Cfd::PostFunctionProvider::getClassTypeId()))
        return;

    //get the function provider
    CfdGui::ViewProviderPostFunctionProvider* vp = static_cast<CfdGui::ViewProviderPostFunctionProvider*>(
                                                    Gui::Application::Instance->getViewProvider(obj->Functions.getValue()));

    if(obj->Data.getValue() && obj->Data.getValue()->IsA("vtkDataSet")) {
        vtkBoundingBox box = obj->getBoundingBox();

        vp->SizeX.setValue(box.GetLength(0)*1.2);
        vp->SizeY.setValue(box.GetLength(1)*1.2);
        vp->SizeZ.setValue(box.GetLength(2)*1.2);
    }
}
