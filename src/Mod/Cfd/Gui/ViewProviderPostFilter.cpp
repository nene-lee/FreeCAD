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

#include "ViewProviderPostFilter.h"
#include <Mod/Cfd/App/PostFilter.h>
#include "TaskPostBoxes.h"

using namespace CfdGui;

//================================================================================================
PROPERTY_SOURCE(CfdGui::ViewProviderPostClip, CfdGui::ViewProviderPostObject)

ViewProviderPostClip::ViewProviderPostClip() {

	sPixmap = "clip_region";
}

ViewProviderPostClip::~ViewProviderPostClip() {

}

void ViewProviderPostClip::setupTaskDialog(TaskDlgPost* dlg) {

	//add the function box
	dlg->appendBox(new TaskPostClip(dlg->getView(),
		&static_cast<Cfd::PostClipFilter*>(dlg->getView()->getObject())->Function));

	//add the display options
	CfdGui::ViewProviderPostObject::setupTaskDialog(dlg);
}

//================================================================================================
PROPERTY_SOURCE(CfdGui::ViewProviderPostDataAlongLine, CfdGui::ViewProviderPostObject)

ViewProviderPostDataAlongLine::ViewProviderPostDataAlongLine() {

	sPixmap = "data_along_line";
}

ViewProviderPostDataAlongLine::~ViewProviderPostDataAlongLine() {

}

void ViewProviderPostDataAlongLine::setupTaskDialog(TaskDlgPost* dlg) {

	//add the function box
	dlg->appendBox(new TaskPostDataAlongLine(dlg->getView()));

}

//================================================================================================
PROPERTY_SOURCE(CfdGui::ViewProviderPostDataAtPoint, CfdGui::ViewProviderPostObject)

ViewProviderPostDataAtPoint::ViewProviderPostDataAtPoint() {

	sPixmap = "data_at_point";
}

ViewProviderPostDataAtPoint::~ViewProviderPostDataAtPoint() {

}

void ViewProviderPostDataAtPoint::setupTaskDialog(TaskDlgPost* dlg) {

	//add the function box
	dlg->appendBox(new TaskPostDataAtPoint(dlg->getView()));

}

//================================================================================================
PROPERTY_SOURCE(CfdGui::ViewProviderPostScalarClip, CfdGui::ViewProviderPostObject)

ViewProviderPostScalarClip::ViewProviderPostScalarClip() {

	sPixmap = "clip_scalar";
}

ViewProviderPostScalarClip::~ViewProviderPostScalarClip() {

}

void ViewProviderPostScalarClip::setupTaskDialog(TaskDlgPost* dlg) {

	//add the function box
	dlg->appendBox(new TaskPostScalarClip(dlg->getView()));

	//add the display options
	CfdGui::ViewProviderPostObject::setupTaskDialog(dlg);
}

//================================================================================================
PROPERTY_SOURCE(CfdGui::ViewProviderPostWarpVector, CfdGui::ViewProviderPostObject)

ViewProviderPostWarpVector::ViewProviderPostWarpVector() {

	sPixmap = "warp_vector";
}

ViewProviderPostWarpVector::~ViewProviderPostWarpVector() {

}

void ViewProviderPostWarpVector::setupTaskDialog(TaskDlgPost* dlg) {

	//add the function box
	dlg->appendBox(new TaskPostWarpVector(dlg->getView()));

	//add the display options
	CfdGui::ViewProviderPostObject::setupTaskDialog(dlg);
}

//================================================================================================
PROPERTY_SOURCE(CfdGui::ViewProviderPostCut, CfdGui::ViewProviderPostObject)

ViewProviderPostCut::ViewProviderPostCut() {

	sPixmap = "cut_function";
}

ViewProviderPostCut::~ViewProviderPostCut() {

}

void ViewProviderPostCut::setupTaskDialog(TaskDlgPost* dlg) {

	//add the function box
	dlg->appendBox(new TaskPostCut(dlg->getView(),
		&static_cast<Cfd::PostCutFilter*>(dlg->getView()->getObject())->Function));

	//add the display options
	CfdGui::ViewProviderPostObject::setupTaskDialog(dlg);
}

//================================================================================================
PROPERTY_SOURCE(CfdGui::ViewProviderPostContour, CfdGui::ViewProviderPostObject)

ViewProviderPostContour::ViewProviderPostContour() {

	sPixmap = "contour";
}

ViewProviderPostContour::~ViewProviderPostContour() {

}

void ViewProviderPostContour::setupTaskDialog(TaskDlgPost* dlg) {

	//add the function box
	//dlg->appendBox(new TaskPostCut(dlg->getView(),
	//	&static_cast<Cfd::PostCutFilter*>(dlg->getView()->getObject())->Function));

	//add the display options
	CfdGui::ViewProviderPostObject::setupTaskDialog(dlg);
}

//================================================================================================
PROPERTY_SOURCE(CfdGui::ViewProviderPostGlyph3D, CfdGui::ViewProviderPostObject)

ViewProviderPostGlyph3D::ViewProviderPostGlyph3D() {

	sPixmap = "glyph24";
}

ViewProviderPostGlyph3D::~ViewProviderPostGlyph3D() {

}

void ViewProviderPostGlyph3D::setupTaskDialog(TaskDlgPost* dlg) {

	//add the function box
	//dlg->appendBox(new TaskPostCut(dlg->getView(),
	//	&static_cast<Cfd::PostCutFilter*>(dlg->getView()->getObject())->Function));

	//add the display options
	CfdGui::ViewProviderPostObject::setupTaskDialog(dlg);
}

//================================================================================================
PROPERTY_SOURCE(CfdGui::ViewProviderPostStreamline, CfdGui::ViewProviderPostObject)

ViewProviderPostStreamline::ViewProviderPostStreamline() {

	sPixmap = "stream_tracer24";
}

ViewProviderPostStreamline::~ViewProviderPostStreamline() {

}

void ViewProviderPostStreamline::setupTaskDialog(TaskDlgPost* dlg) {

	//add the function box
	//dlg->appendBox(new TaskPostCut(dlg->getView(),
	//	&static_cast<Cfd::PostCutFilter*>(dlg->getView()->getObject())->Function));

	//add the display options
	CfdGui::ViewProviderPostObject::setupTaskDialog(dlg);
}