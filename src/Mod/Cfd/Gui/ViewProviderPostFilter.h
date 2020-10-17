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


#ifndef CFD_VIEW_PROVIDER_POST_FILTER_H
#define CFD_VIEW_PROVIDER_POST_FILTER_H

#include "ViewProviderPostObject.h"

namespace CfdGui
{
class CfdGuiExport ViewProviderPostClip : public ViewProviderPostObject {

	PROPERTY_HEADER(CfdGui::ViewProviderPostClip);

public:
	/// constructor.
	ViewProviderPostClip();
	~ViewProviderPostClip();

protected:
	virtual void setupTaskDialog(TaskDlgPost* dlg);
};


class CfdGuiExport ViewProviderPostDataAlongLine : public ViewProviderPostObject {

	PROPERTY_HEADER(CfdGui::ViewProviderPostDataAlongLine);

public:
	/// constructor.
	ViewProviderPostDataAlongLine();
	~ViewProviderPostDataAlongLine();

protected:
	virtual void setupTaskDialog(TaskDlgPost* dlg);
};

class CfdGuiExport ViewProviderPostDataAtPoint : public ViewProviderPostObject {

	PROPERTY_HEADER(CfdGui::ViewProviderPostDataAtPoint);

public:
	/// constructor.
	ViewProviderPostDataAtPoint();
	~ViewProviderPostDataAtPoint();

protected:
	virtual void setupTaskDialog(TaskDlgPost* dlg);
};

class CfdGuiExport ViewProviderPostScalarClip : public ViewProviderPostObject {

	PROPERTY_HEADER(CfdGui::ViewProviderPostScalarClip);

public:
	/// constructor.
	ViewProviderPostScalarClip();
	~ViewProviderPostScalarClip();

protected:
	virtual void setupTaskDialog(TaskDlgPost* dlg);
};

class CfdGuiExport ViewProviderPostWarpVector : public ViewProviderPostObject {

	PROPERTY_HEADER(CfdGui::ViewProviderPostWarpVector);

public:
	/// constructor.
	ViewProviderPostWarpVector();
	~ViewProviderPostWarpVector();

protected:
	virtual void setupTaskDialog(TaskDlgPost* dlg);
};

class CfdGuiExport ViewProviderPostCut : public ViewProviderPostObject {

	PROPERTY_HEADER(CfdGui::ViewProviderPostCut);

public:
	/// constructor.
	ViewProviderPostCut();
	~ViewProviderPostCut();

protected:
	virtual void setupTaskDialog(TaskDlgPost* dlg);
};

class CfdGuiExport ViewProviderPostContour : public ViewProviderPostObject {

	PROPERTY_HEADER(CfdGui::ViewProviderPostContour);

public:
	/// constructor.
	ViewProviderPostContour();
	~ViewProviderPostContour();

protected:
	virtual void setupTaskDialog(TaskDlgPost* dlg);
};

class CfdGuiExport ViewProviderPostGlyph3D : public ViewProviderPostObject {

	PROPERTY_HEADER(CfdGui::ViewProviderPostGlyph3D);

public:
	/// constructor.
	ViewProviderPostGlyph3D();
	~ViewProviderPostGlyph3D();

protected:
	virtual void setupTaskDialog(TaskDlgPost* dlg);
};

class CfdGuiExport ViewProviderPostStreamline : public ViewProviderPostObject {

	PROPERTY_HEADER(CfdGui::ViewProviderPostStreamline);

public:
	/// constructor.
	ViewProviderPostStreamline();
	~ViewProviderPostStreamline();

protected:
	virtual void setupTaskDialog(TaskDlgPost* dlg);
};


} //namespace CfdGui


#endif // CFD_VIEW_PROVIDER_POST_FILTER_H
