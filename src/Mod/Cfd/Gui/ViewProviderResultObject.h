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


#ifndef CFD_VIEW_PROVIDER_RESULT_H
#define CFD_VIEW_PROVIDER_RESULT_H

#include <Gui/ViewProviderDocumentObject.h>
#include <Gui/ViewProviderPythonFeature.h>

namespace CfdGui
{

	class CfdGuiExport ViewProviderResultObject : public Gui::ViewProviderDocumentObject
	{
		PROPERTY_HEADER(CfdGui::ViewProviderResultObject);

	public:
		/// constructor
		ViewProviderResultObject();

		/// destructor
		virtual ~ViewProviderResultObject();

		/** 
		 * deliver the children belonging to this object
		 * this method is used to deliver the objects to
		 * the 3DView which should be grouped under its
		 * scene graph. This affects the visibility and the 3D
		 * position of the object.
		 */
		virtual std::vector< App::DocumentObject* > claimChildren3D(void) const;

		/** 
		 * deliver the children belonging to this object
		 * this method is used to deliver the objects to
		 * the tree framework which should be grouped under its
		 * label. Obvious is the usage in the group but it can
		 * be used for any kind of grouping needed for a special
		 * purpose.
		 */
		virtual std::vector< App::DocumentObject* > claimChildren(void) const;

		/// indicates if the ViewProvider can be selected
		virtual bool isSelectable(void) const;

		/**
		 * @brief Asks the view provider if the given object that is part of its
         * outlist can be removed from there without breaking it.
         * @param obj is part of the outlist of the object associated to the view provider
         * @return true if the removal is approved by the view provider.
         */
		virtual bool canDelete(App::DocumentObject* obj) const;

		/** Check whether children can be removed from the view provider by drag and drop */
		virtual bool canDragObjects() const;

		/** Check whether objects can be added to the view provider by drag and drop */
		virtual bool canDropObjects() const;

		/** 
		 * Tell the tree view if this object should appear there 
		 */
		virtual bool showInTree() const;

		// shows solid in the tree
		virtual bool isShow(void) const;

		/// returns a list of all possible display modes
		virtual std::vector<std::string> getDisplayModes(void) const;
	};

	typedef Gui::ViewProviderPythonFeatureT<ViewProviderResultObject > ViewProviderResultObjectPython;

} //namespace CfdGui


#endif // CFD_VIEW_PROVIDER_RESULT_H
