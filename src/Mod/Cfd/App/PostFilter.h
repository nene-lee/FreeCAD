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


#ifndef CFD_POST_FILTER_H
#define CFD_POST_FILTER_H

#include "PostObject.h"
#include <App/PropertyStandard.h>
#include <App/PropertyUnits.h>
#include <App/PropertyGeo.h>

#include <vtkSmartPointer.h>

class vtkAlgorithm;

class vtkPointSource;
class vtkLineSource;
class vtkArrowSource;

class vtkMaskPoints;
class vtkProbeFilter;
class vtkTableBasedClipDataSet;
class vtkExtractGeometry;
class vtkWarpVector;
class vtkCutter;
class vtkContourFilter;
class vtkGlyph3D;
class vtkStreamTracer;

namespace Cfd
{

class CfdExport PostFilter : public PostObject
{
    PROPERTY_HEADER(Cfd::PostFilter);

public:
    /// Constructor
    PostFilter(void);
    virtual ~PostFilter();

    App::PropertyLink Input;

    virtual App::DocumentObjectExecReturn* execute(void);

protected:
	virtual void onChanged(const App::Property* prop);

    vtkDataObject* getInputData();

    //pipeline handling for derived filter
    struct FilterPipeline {
       vtkSmartPointer<vtkAlgorithm>                    source, target;
       vtkSmartPointer<vtkProbeFilter>                  filterSource, filterTarget;
       std::vector<vtkSmartPointer<vtkAlgorithm> >      algorithmStorage;
    };

    void addFilterPipeline(const FilterPipeline& p, std::string name);
    void setActiveFilterPipeline(std::string name);
    FilterPipeline& getFilterPipeline(std::string name);
private:
    //handling of multiple pipelines which can be the filter
    std::map<std::string, FilterPipeline> m_pipelines;
    std::string m_activePipeline;
};

class CfdExport PostWarpVectorFilter : public PostFilter {

	PROPERTY_HEADER(Cfd::PostWarpVectorFilter);

public:
	PostWarpVectorFilter(void);
	virtual ~PostWarpVectorFilter();

	App::PropertyFloat        Factor;
	App::PropertyEnumeration  Vector;

	virtual const char* getViewProviderName(void) const {
		return "CfdGui::ViewProviderPostWarpVector";
	}
	virtual short int mustExecute(void) const;

protected:
	virtual App::DocumentObjectExecReturn* execute(void);
	virtual void onChanged(const App::Property* prop);

private:
	vtkSmartPointer<vtkWarpVector>   m_warp;
	App::Enumeration                 m_vectorFields;
};

class CfdExport PostClipFilter : public PostFilter {

	PROPERTY_HEADER(Cfd::PostClipFilter);

public:
	PostClipFilter(void);
	virtual ~PostClipFilter();

	App::PropertyLink           Function;
	App::PropertyBool           InsideOut;
	App::PropertyBool           CutCells;

	virtual const char* getViewProviderName(void) const {
		return "CfdGui::ViewProviderPostClip";
	}
	virtual short int mustExecute(void) const;
	virtual App::DocumentObjectExecReturn* execute(void);

protected:
	virtual void onChanged(const App::Property* prop);

private:
	vtkSmartPointer<vtkTableBasedClipDataSet>   m_clipper;
	vtkSmartPointer<vtkExtractGeometry>         m_extractor;
};



class CfdExport PostScalarClipFilter : public PostFilter {

	PROPERTY_HEADER(Cfd::PostScalarClipFilter);

public:
	PostScalarClipFilter(void);
	virtual ~PostScalarClipFilter();

	App::PropertyBool            InsideOut;
	App::PropertyFloatConstraint Value;
	App::PropertyEnumeration     Scalars;

	virtual const char* getViewProviderName(void) const {
		return "CfdGui::ViewProviderPostScalarClip";
	}
	virtual short int mustExecute(void) const;

protected:
	virtual App::DocumentObjectExecReturn* execute(void);
	virtual void onChanged(const App::Property* prop);
	void setConstraintForField();

private:
	vtkSmartPointer<vtkTableBasedClipDataSet>   m_clipper;
	App::Enumeration                            m_scalarFields;
	App::PropertyFloatConstraint::Constraints   m_constraints;
};

class CfdExport PostCutFilter : public PostFilter {

	PROPERTY_HEADER(Cfd::PostCutFilter);

public:
	PostCutFilter(void);
	virtual ~PostCutFilter();

	App::PropertyLink  Function;

	virtual const char* getViewProviderName(void) const {
		return "CfdGui::ViewProviderPostCut";
	}
	virtual short int mustExecute(void) const;
	virtual App::DocumentObjectExecReturn* execute(void);

protected:
	virtual void onChanged(const App::Property* prop);

private:
	vtkSmartPointer<vtkCutter>   m_cutter;
};

class CfdExport PostContourFilter : public PostFilter {

	PROPERTY_HEADER(Cfd::PostContourFilter);

public:
	PostContourFilter(void);
	virtual ~PostContourFilter();

	App::PropertyEnumeration  Field;

	App::PropertyFloat		RangeStart;
	App::PropertyFloat		RangeEnd;
	App::PropertyInteger	NumberOfContours;

	virtual const char* getViewProviderName(void) const {
		return "CfdGui::ViewProviderPostContour";
	}
	virtual short int mustExecute(void) const;
	virtual App::DocumentObjectExecReturn* execute(void);

protected:
	virtual void onChanged(const App::Property* prop);

private:
	App::Enumeration m_fieldEnum;
	vtkSmartPointer<vtkContourFilter> m_contour;
};

class CfdExport PostGlyph3DFilter : public PostFilter {

	PROPERTY_HEADER(Cfd::PostGlyph3DFilter);

public:
	PostGlyph3DFilter(void);
	virtual ~PostGlyph3DFilter();

	App::PropertyInteger		MaximumNumberOfPoints;

	App::PropertyEnumeration	Vector;
	App::PropertyFloat			ScaleFactor;

	virtual const char* getViewProviderName(void) const {
		return "CfdGui::ViewProviderPostGlyph3D";
	}
	virtual short int mustExecute(void) const;
	virtual App::DocumentObjectExecReturn* execute(void);

protected:
	virtual void onChanged(const App::Property* prop);

private:
	App::Enumeration					m_vectorFields;
	//vtkSmartPointer<vtkPlaneSource>         m_plane;
	//vtkSmartPointer<vtkGenericStreamTracer> m_tracer;
	vtkSmartPointer<vtkMaskPoints>		m_mask;
	vtkSmartPointer<vtkArrowSource>		m_arrow;
	vtkSmartPointer<vtkGlyph3D>			m_glyph;
};

class CfdExport PostStreamlineFilter : public PostFilter {

	PROPERTY_HEADER(Cfd::PostStreamlineFilter);

public:
	PostStreamlineFilter(void);
	virtual ~PostStreamlineFilter();

	App::PropertyEnumeration  Vector;

	App::PropertyVector	Point1;
	App::PropertyVector Point2;
	App::PropertyInteger  Resolution;

	virtual const char* getViewProviderName(void) const {
		return "CfdGui::ViewProviderPostStreamline";
	}
	virtual short int mustExecute(void) const;
	virtual App::DocumentObjectExecReturn* execute(void);

protected:
	virtual void onChanged(const App::Property* prop);

private:
	void updateProperties();

private:
	App::Enumeration                 m_vectorFields;
	//vtkSmartPointer<vtkPlaneSource>         m_plane;
	//vtkSmartPointer<vtkGenericStreamTracer> m_tracer;
	vtkSmartPointer<vtkLineSource>   m_line;
	vtkSmartPointer<vtkStreamTracer> m_tracer;
};

class CfdExport PostDataAlongLineFilter : public PostFilter {

	PROPERTY_HEADER(Cfd::PostDataAlongLineFilter);

public:
	PostDataAlongLineFilter(void);
	virtual ~PostDataAlongLineFilter();

	App::PropertyVector   Point2;
	App::PropertyVector   Point1;
	App::PropertyInteger  Resolution;
	App::PropertyFloatList XAxisData;
	App::PropertyFloatList YAxisData;
	App::PropertyString    PlotData;

	virtual const char* getViewProviderName(void) const {
		return "CfdGui::ViewProviderPostDataAlongLine";
	}
	virtual short int mustExecute(void) const;

protected:
	virtual App::DocumentObjectExecReturn* execute(void);
	virtual void onChanged(const App::Property* prop);
	void GetAxisData();

private:

	vtkSmartPointer<vtkLineSource>              m_line;
	vtkSmartPointer<vtkProbeFilter>             m_probe;

};

class CfdExport PostDataAtPointFilter : public PostFilter {

	PROPERTY_HEADER(Cfd::PostDataAtPointFilter);

public:
	PostDataAtPointFilter(void);
	virtual ~PostDataAtPointFilter();

	App::PropertyVectorDistance   Center;
	App::PropertyDistance         Radius;
	App::PropertyString           FieldName;
	App::PropertyFloatList        PointData;
	App::PropertyString           Unit;

	virtual const char* getViewProviderName(void) const {
		return "CfdGui::ViewProviderPostDataAtPoint";
	}
	virtual short int mustExecute(void) const;

protected:
	virtual App::DocumentObjectExecReturn* execute(void);
	virtual void onChanged(const App::Property* prop);
	void GetPointData();

private:

	vtkSmartPointer<vtkPointSource>             m_point;
	vtkSmartPointer<vtkProbeFilter>             m_probe;

};

} //namespace Cfd


#endif // CFD_POST_FILTER_H
