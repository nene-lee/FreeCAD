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


#ifndef CFD_POST_PIPELINE_H
#define CFD_POST_PIPELINE_H

#include <vtkSmartPointer.h>

#include <App/PropertyStandard.h>

#include "PostFilter.h"

namespace Cfd
{
class ResultObject;
class InstantCollection;

class CfdExport PostPipeline : public Cfd::PostFilter
{
    PROPERTY_HEADER(Cfd::PostPipeline);

public:
    /// Constructor
    PostPipeline(void);
    virtual ~PostPipeline();

	/// Stats of analysis
	App::PropertyIntegerConstraint		TimeIndex;
    App::PropertyLinkList				Filter;
    App::PropertyLink					Functions;
    App::PropertyEnumeration			Mode;

    short mustExecute(void) const;
    virtual App::DocumentObjectExecReturn* execute(void);
    PyObject* getPyObject();

    virtual const char* getViewProviderName(void) const {
        return "CfdGui::ViewProviderPostPipeline";
    }

    //Pipeline handling
    PostObject* getLastPostObject();
    bool           holdsPostObject(PostObject* obj);

	//load data from files
	static bool canRead(Base::FileInfo file);
	void read(Base::FileInfo file);

	//load from results
	void load(ResultObject* res);

	///  Retrieve the time values
	std::vector<double> listTimeValues() const;

	///  Retrieve the names of the regions
	std::vector<std::string> listRegions() const;

	/// Fetch vtkDataSet at 'timeIndex' 
	vtkSmartPointer<vtkDataObject> fetch() const;

	/// Advance in time index
	int advance(int index);

protected:
    virtual void onChanged(const App::Property* prop);

private:
    static const char* ModeEnums[];

	App::PropertyIntegerConstraint::Constraints  m_timeIndexRange;

	InstantCollection *m_instants;
};

} //namespace Fem


#endif // CFD_POST_PIPELINE_H
