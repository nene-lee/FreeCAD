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

#include "Gui/Command.h"

#include <Mod/Cfd/App/ResultObject.h>
#include "ViewProviderResultObject.h"

using namespace CfdGui;

PROPERTY_SOURCE(CfdGui::ViewProviderResultObject, Gui::ViewProviderDocumentObject)

ViewProviderResultObject::ViewProviderResultObject()
{
    sPixmap = "post_result_show";
}

ViewProviderResultObject::~ViewProviderResultObject()
{

}

std::vector< App::DocumentObject* > ViewProviderResultObject::claimChildren(void) const {

	Cfd::ResultObject* res = static_cast<Cfd::ResultObject*>(getObject());
	std::vector<App::DocumentObject*> children;

	children.insert(children.end(), res->Pipes.getValues().begin(), res->Pipes.getValues().end());
	return children;
}

std::vector< App::DocumentObject* > ViewProviderResultObject::claimChildren3D(void) const {

	return claimChildren();
}

bool ViewProviderResultObject::isSelectable(void) const
{
	return false;
}

bool ViewProviderResultObject::canDragObjects() const
{
	return false;
}

bool ViewProviderResultObject::canDropObjects() const
{
	return false;
}

bool ViewProviderResultObject::showInTree() const
{
	return true;
}

bool ViewProviderResultObject::isShow(void) const
{
	return Visibility.getValue();
}

bool ViewProviderResultObject::canDelete(App::DocumentObject* obj) const
{
	return false;
}

std::vector<std::string> ViewProviderResultObject::getDisplayModes(void) const
{
	return { "Result" };
}

/* not needed since _ViewProviderFemResult.py is made
bool ViewProviderResultObject::doubleClicked(void)
{
    Gui::Command::runCommand(Gui::Command::Gui, "Gui.runCommand('Fem_ResultShow')");
    return true;
}
*/

// Python feature -----------------------------------------------------------------------

namespace Gui {
/// @cond DOXERR
PROPERTY_SOURCE_TEMPLATE(CfdGui::ViewProviderResultObjectPython, CfdGui::ViewProviderResultObject)
/// @endcond

// explicit template instantiation
template class CfdGuiExport ViewProviderPythonFeatureT<ViewProviderResultObject>;
}
