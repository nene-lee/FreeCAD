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
#include <App/Document.h>
#include <App/DocumentObjectPy.h>

#include <vtkFieldData.h>
#include <vtkPointData.h>
#include <vtkCellData.h>

#include <vtkPointSource.h>
#include <vtkLineSource.h>
#include <vtkPlane.h>
#include <vtkPlaneSource.h>
#include <vtkArrowSource.h>
#include <vtkMaskPoints.h>

#include <vtkTableBasedClipDataSet.h>
#include <vtkExtractGeometry.h>
#include <vtkGeometryFilter.h>
#include <vtkPassThrough.h>
#include <vtkWarpVector.h>
#include <vtkCutter.h>
#include <vtkProbeFilter.h>
#include <vtkThreshold.h>
#include <vtkContourFilter.h>
#include <vtkGlyph3D.h>
#include <vtkStreamTracer.h>
#include <vtkGenericStreamTracer.h>


#include "PostFilter.h"
#include "PostPipeline.h"
#include "PostFunction.h"

using namespace App;
using namespace Cfd;

//================================================================================================
PROPERTY_SOURCE(Cfd::PostFilter, Cfd::PostObject)

PostFilter::PostFilter()
{
    ADD_PROPERTY(Input,(0));
}

PostFilter::~PostFilter()
{

}

void PostFilter::addFilterPipeline(const PostFilter::FilterPipeline& p, std::string name) {
    m_pipelines[name] = p;
}

PostFilter::FilterPipeline& PostFilter::getFilterPipeline(std::string name) {
    return m_pipelines[name];
}

void PostFilter::setActiveFilterPipeline(std::string name) {

    if(m_activePipeline != name && isValid()) {
        m_activePipeline = name;
    }
}

DocumentObjectExecReturn* PostFilter::execute(void) {

    if(!m_pipelines.empty() && !m_activePipeline.empty()) {
        PostFilter::FilterPipeline& pipe = m_pipelines[m_activePipeline];
        if ((m_activePipeline.length() >= 13) || (m_activePipeline.length() >= 11)) {
            std::string LineClip = m_activePipeline.substr(0,13);
            std::string PointClip = m_activePipeline.substr(0,11);
            if ((LineClip == "DataAlongLine") || (PointClip == "DataAtPoint")) {
                pipe.filterSource->SetSourceData(getInputData());
                pipe.filterTarget->Update();

                Data.setValue(pipe.filterTarget->GetOutputDataObject(0));
            }
        } else {
            pipe.source->SetInputDataObject(getInputData());
            pipe.target->Update();
            Data.setValue(pipe.target->GetOutputDataObject(0));
        }

    }
    return StdReturn;
}

vtkDataObject* PostFilter::getInputData() {

    if(Input.getValue()) {
        return Input.getValue<PostObject*>()->Data.getValue();
    }
    else {
        //get the pipeline and use the pipelinedata
        std::vector<App::DocumentObject*> objs = getDocument()->getObjectsOfType(PostPipeline::getClassTypeId());
        for(std::vector<App::DocumentObject*>::iterator it = objs.begin(); it != objs.end(); ++it) {

            if(static_cast<PostPipeline*>(*it)->holdsPostObject(this)) {

                return static_cast<PostObject*>(*it)->Data.getValue();
            }
        }
    }

    return NULL;
}

void PostFilter::onChanged(const App::Property* prop)
{
	Cfd::PostObject::onChanged(prop);
}

//================================================================================================
PROPERTY_SOURCE(Cfd::PostWarpVectorFilter, Cfd::PostFilter)

PostWarpVectorFilter::PostWarpVectorFilter(void) : PostFilter() {

	ADD_PROPERTY_TYPE(Factor, (0), "Warp", App::Prop_None, "The factor by which the vector is added to the node positions");
	ADD_PROPERTY_TYPE(Vector, (long(0)), "Warp", App::Prop_None, "The field added to the node position");

	FilterPipeline warp;
	m_warp = vtkSmartPointer<vtkWarpVector>::New();
	warp.source = m_warp;
	warp.target = m_warp;
	addFilterPipeline(warp, "warp");
	setActiveFilterPipeline("warp");
}

PostWarpVectorFilter::~PostWarpVectorFilter() {

}


DocumentObjectExecReturn* PostWarpVectorFilter::execute(void) {

	std::string val;
	if (m_vectorFields.getEnums() && Vector.getValue() >= 0)
		val = Vector.getValueAsString();

	std::vector<std::string> array;

	vtkSmartPointer<vtkDataObject> data = getInputData();
	if (!data || !data->IsA("vtkDataSet"))
		return StdReturn;

	vtkDataSet* dset = vtkDataSet::SafeDownCast(data);
	vtkPointData* pd = dset->GetPointData();

	for (int i = 0; i < pd->GetNumberOfArrays(); ++i) {
		if (pd->GetArray(i)->GetNumberOfComponents() == 3)
			array.push_back(pd->GetArrayName(i));
	}

	App::Enumeration empty;
	Vector.setValue(empty);
	m_vectorFields.setEnums(array);
	Vector.setValue(m_vectorFields);

	std::vector<std::string>::iterator it = std::find(array.begin(), array.end(), val);
	if (!val.empty() && it != array.end())
		Vector.setValue(val.c_str());

	//recalculate the filter
	return Cfd::PostFilter::execute();
}

void PostWarpVectorFilter::onChanged(const Property* prop) {

	if (prop == &Factor) {
		m_warp->SetScaleFactor(Factor.getValue());
	}
	else if (prop == &Vector && (Vector.getValue() >= 0)) {
		m_warp->SetInputArrayToProcess(0, 0, 0,
			vtkDataObject::FIELD_ASSOCIATION_POINTS, Vector.getValueAsString());
	}

	Cfd::PostFilter::onChanged(prop);
}

short int PostWarpVectorFilter::mustExecute(void) const {

	if (Factor.isTouched() ||
		Vector.isTouched()) {

		return 1;
	}
	else return App::DocumentObject::mustExecute();
}


//================================================================================================
PROPERTY_SOURCE(Cfd::PostClipFilter, Cfd::PostFilter)

PostClipFilter::PostClipFilter(void) : PostFilter() {

	ADD_PROPERTY_TYPE(Function, (0), "Clip", App::Prop_None, "The function object which defines the clip regions");
	ADD_PROPERTY_TYPE(InsideOut, (false), "Clip", App::Prop_None, "Invert the clip direction");
	ADD_PROPERTY_TYPE(CutCells, (false), "Clip", App::Prop_None, "Decides if cells are cuttet and interpolated or if the cells are kept as a whole");

	FilterPipeline clip;
	m_clipper = vtkSmartPointer<vtkTableBasedClipDataSet>::New();
	clip.source = m_clipper;
	clip.target = m_clipper;
	addFilterPipeline(clip, "clip");

	FilterPipeline extr;
	m_extractor = vtkSmartPointer<vtkExtractGeometry>::New();
	extr.source = m_extractor;
	extr.target = m_extractor;
	addFilterPipeline(extr, "extract");

	m_extractor->SetExtractInside(0);
	setActiveFilterPipeline("extract");
}

PostClipFilter::~PostClipFilter() {

}

void PostClipFilter::onChanged(const Property* prop) {

	if (prop == &Function) {

		if (Function.getValue() && Function.getValue()->isDerivedFrom(PostFunction::getClassTypeId())) {
			m_clipper->SetClipFunction(static_cast<PostFunction*>(Function.getValue())->getImplicitFunction());
			m_extractor->SetImplicitFunction(static_cast<PostFunction*>(Function.getValue())->getImplicitFunction());
		}
	}
	else if (prop == &InsideOut) {

		m_clipper->SetInsideOut(InsideOut.getValue());
		m_extractor->SetExtractInside((InsideOut.getValue()) ? 1 : 0);
	}
	else if (prop == &CutCells) {

		if (!CutCells.getValue())
			setActiveFilterPipeline("extract");
		else
			setActiveFilterPipeline("clip");
	};

	Cfd::PostFilter::onChanged(prop);
}

short int PostClipFilter::mustExecute(void) const {

	if (Function.isTouched() ||
		InsideOut.isTouched() ||
		CutCells.isTouched()) {

		return 1;
	}
	else return App::DocumentObject::mustExecute();
}

DocumentObjectExecReturn* PostClipFilter::execute(void) {

	if (!m_extractor->GetImplicitFunction())
		return StdReturn;

	return Cfd::PostFilter::execute();
}


//================================================================================================
PROPERTY_SOURCE(Cfd::PostScalarClipFilter, Cfd::PostFilter)

PostScalarClipFilter::PostScalarClipFilter(void) : PostFilter() {

	ADD_PROPERTY_TYPE(Value, (0), "Clip", App::Prop_None, "The scalar value used to clip the selected field");
	ADD_PROPERTY_TYPE(Scalars, (long(0)), "Clip", App::Prop_None, "The field used to clip");
	ADD_PROPERTY_TYPE(InsideOut, (false), "Clip", App::Prop_None, "Invert the clip direction");

	Value.setConstraints(&m_constraints);

	FilterPipeline clip;
	m_clipper = vtkSmartPointer<vtkTableBasedClipDataSet>::New();
	clip.source = m_clipper;
	clip.target = m_clipper;
	addFilterPipeline(clip, "clip");
	setActiveFilterPipeline("clip");
}

PostScalarClipFilter::~PostScalarClipFilter() {

}

DocumentObjectExecReturn* PostScalarClipFilter::execute(void) {

	std::string val;
	if (m_scalarFields.getEnums() && Scalars.getValue() >= 0)
		val = Scalars.getValueAsString();

	std::vector<std::string> array;

	vtkSmartPointer<vtkDataObject> data = getInputData();
	if (!data || !data->IsA("vtkDataSet"))
		return StdReturn;

	vtkDataSet* dset = vtkDataSet::SafeDownCast(data);
	vtkPointData* pd = dset->GetPointData();

	for (int i = 0; i < pd->GetNumberOfArrays(); ++i) {
		if (pd->GetArray(i)->GetNumberOfComponents() == 1)
			array.push_back(pd->GetArrayName(i));
	}

	App::Enumeration empty;
	Scalars.setValue(empty);
	m_scalarFields.setEnums(array);
	Scalars.setValue(m_scalarFields);

	std::vector<std::string>::iterator it = std::find(array.begin(), array.end(), val);
	if (!val.empty() && it != array.end())
		Scalars.setValue(val.c_str());

	//recalculate the filter
	return Cfd::PostFilter::execute();
}


void PostScalarClipFilter::onChanged(const Property* prop) {

	if (prop == &Value) {
		m_clipper->SetValue(Value.getValue());
	}
	else if (prop == &InsideOut) {
		m_clipper->SetInsideOut(InsideOut.getValue());
	}
	else if (prop == &Scalars && (Scalars.getValue() >= 0)) {
		m_clipper->SetInputArrayToProcess(0, 0, 0,
			vtkDataObject::FIELD_ASSOCIATION_POINTS, Scalars.getValueAsString());
		setConstraintForField();
	}

	Cfd::PostFilter::onChanged(prop);
}

short int PostScalarClipFilter::mustExecute(void) const {

	if (Value.isTouched() ||
		InsideOut.isTouched() ||
		Scalars.isTouched()) {

		return 1;
	}
	else return App::DocumentObject::mustExecute();
}

void PostScalarClipFilter::setConstraintForField() {

	vtkSmartPointer<vtkDataObject> data = getInputData();
	if (!data || !data->IsA("vtkDataSet"))
		return;

	vtkDataSet* dset = vtkDataSet::SafeDownCast(data);

	vtkDataArray* pdata = dset->GetPointData()->GetArray(Scalars.getValueAsString());
	double p[2];
	pdata->GetRange(p);
	m_constraints.LowerBound = p[0];
	m_constraints.UpperBound = p[1];
	m_constraints.StepSize = (p[1] - p[0]) / 100.;
}

//================================================================================================
PROPERTY_SOURCE(Cfd::PostCutFilter, Cfd::PostFilter)

PostCutFilter::PostCutFilter(void) : PostFilter() {

	ADD_PROPERTY_TYPE(Function, (0), "Cut", App::Prop_None, "The function object which defines the clip cut function");

	FilterPipeline clip;
	m_cutter = vtkSmartPointer<vtkCutter>::New();
	m_cutter->GenerateCutScalarsOn();
	m_cutter->GenerateTrianglesOn();
	clip.source = m_cutter;
	clip.target = m_cutter;
	addFilterPipeline(clip, "cut");
	setActiveFilterPipeline("cut");
}

PostCutFilter::~PostCutFilter() {

}

void PostCutFilter::onChanged(const Property* prop) {

	if (prop == &Function) {

		if (Function.getValue() && Function.getValue()->isDerivedFrom(PostFunction::getClassTypeId())) {
			m_cutter->SetCutFunction(static_cast<PostFunction*>(Function.getValue())->getImplicitFunction());
		}
	}

	Cfd::PostFilter::onChanged(prop);
}

short int PostCutFilter::mustExecute(void) const {

	if (Function.isTouched()) {

		return 1;
	}
	else return App::DocumentObject::mustExecute();
}

DocumentObjectExecReturn* PostCutFilter::execute(void) {

	if (!m_cutter->GetCutFunction())
		return StdReturn;

	return Cfd::PostFilter::execute();
}

//================================================================================================

PROPERTY_SOURCE(Cfd::PostContourFilter, Cfd::PostFilter)

PostContourFilter::PostContourFilter(void) : PostFilter() {

	ADD_PROPERTY_TYPE(Field, ((long)0), "Contour Field", App::Prop_None, 
		"Select the field used for calculating the contours");

	ADD_PROPERTY_TYPE(RangeStart, (0.0), "Contour Values", App::Prop_None,
		"Specify lower value of range to generate a series of evenly spaced contours");
	ADD_PROPERTY_TYPE(RangeEnd, (1.0), "Contour Values", App::Prop_None,
		"Specify upper value of range to generate a series of evenly spaced contours");
	ADD_PROPERTY_TYPE(NumberOfContours, (10), "Contour Values", App::Prop_None,
		"Specify the number of evenly spaced contour values");

	m_contour = vtkSmartPointer<vtkContourFilter>::New();
	m_contour->ComputeScalarsOn();
	m_contour->GenerateValues
	(
		NumberOfContours.getValue(),
		RangeStart.getValue(),
		RangeEnd.getValue()
	);

	FilterPipeline filter;
	filter.source = m_contour;
	filter.target = m_contour;
	addFilterPipeline(filter, "Contour");
	setActiveFilterPipeline("Contour");
}

PostContourFilter::~PostContourFilter() {

}

void PostContourFilter::onChanged(const Property* prop) {

	if (prop == &RangeStart || prop == &RangeEnd || prop == &NumberOfContours) {
		m_contour->GenerateValues
		(
			NumberOfContours.getValue(),
			RangeStart.getValue(),
			RangeEnd.getValue()
		);
	} 
	else if (prop == &Field && (Field.getValue() >= 0)) {
		m_contour->SetInputArrayToProcess(0, 0, 0,
			vtkDataObject::FIELD_ASSOCIATION_POINTS, Field.getValueAsString());
	}

	Cfd::PostFilter::onChanged(prop);
}

short int PostContourFilter::mustExecute(void) const {

	if ( Field .isTouched() ||
		RangeStart.isTouched() || RangeEnd.isTouched() ||NumberOfContours.isTouched() ) {

		return 1;
	}

	return App::DocumentObject::mustExecute();
}

DocumentObjectExecReturn* PostContourFilter::execute(void) {

	vtkSmartPointer<vtkDataObject> data = getInputData();
	if (!data || !data->IsA("vtkDataSet"))
		return StdReturn;

	vtkDataSet *ds = vtkDataSet::SafeDownCast(data);

	std::string val;
	if (Field.getEnums() && Field.getValue() >= 0)
		val = Field.getValueAsString();

	std::vector<std::string> arrays;
	arrays.push_back("None");

	vtkPointData* pd = ds->GetPointData();
	for (int i = 0; i < pd->GetNumberOfArrays(); ++i) {
		
		if (pd->GetArray(i)->GetNumberOfComponents() == 1) {

			std::string fieldName = pd->GetArrayName(i);
			arrays.push_back(fieldName);
		}
	}

	App::Enumeration empty;
	Field.setValue(empty);
	m_fieldEnum.setEnums(arrays);
	Field.setValue(m_fieldEnum);

	std::vector<std::string>::iterator it = std::find(arrays.begin(), arrays.end(), val);
	if (!val.empty() && it != arrays.end())
		Field.setValue(val.c_str());

	Field.purgeTouched();

	return Cfd::PostFilter::execute();
}

//================================================================================================

PROPERTY_SOURCE(Cfd::PostGlyph3DFilter, Cfd::PostFilter)

PostGlyph3DFilter::PostGlyph3DFilter(void) : PostFilter() {

	ADD_PROPERTY_TYPE(MaximumNumberOfPoints, (100), "Mask Points", App::Prop_None,
		"Limit the number of points that can be passed through");

	ADD_PROPERTY_TYPE(Vector, ((long)0), "Glyph3D", App::Prop_None,
		"Specify whether to use vector or normal to perform vector operations");
	ADD_PROPERTY_TYPE(ScaleFactor, (0.01), "Glyph3D", App::Prop_None, 
		"Specify scale factor to scale object by");

	m_mask = vtkSmartPointer<vtkMaskPoints>::New();
	m_mask->SetRandomModeType(1);
	m_mask->SetMaximumNumberOfPoints(MaximumNumberOfPoints.getValue());
	m_mask->RandomModeOn();

	m_arrow = vtkSmartPointer<vtkArrowSource>::New();

	m_glyph = vtkSmartPointer<vtkGlyph3D>::New();
	m_glyph->SetInputConnection(m_mask->GetOutputPort());
	m_glyph->SetSourceConnection(m_arrow->GetOutputPort());
	//m_glyph->SetVectorModeToUseNormal();
	m_glyph->SetVectorModeToUseVector();
	m_glyph->SetScaleModeToScaleByScalar();
	//m_glyph->SetScaleModeToScaleByVector();
	//m_glyph->SetClamping(true);
	m_glyph->SetScaleFactor(ScaleFactor.getValue());

	FilterPipeline filter;
	//filter.algorithmStorage.push_back(seeds);
	filter.source = m_mask;
	filter.target = m_glyph;
	addFilterPipeline(filter, "Glyph3D");
	setActiveFilterPipeline("Glyph3D");
}

PostGlyph3DFilter::~PostGlyph3DFilter() {

}

void PostGlyph3DFilter::onChanged(const Property* prop) {

	if (prop == &MaximumNumberOfPoints) {
		m_mask->SetMaximumNumberOfPoints(MaximumNumberOfPoints.getValue());
	}
	else if (prop == &Vector) {
		if (Vector.getValue() >= 0) {
			m_glyph->SetVectorModeToUseVector();
			m_glyph->SetInputArrayToProcess(0, 0, 0,
				vtkDataObject::FIELD_ASSOCIATION_POINTS, Vector.getValueAsString());
		}
	}
	else if (prop == &ScaleFactor) {
		m_glyph->SetScaleFactor(ScaleFactor.getValue());
	}

	Cfd::PostFilter::onChanged(prop);
}

short int PostGlyph3DFilter::mustExecute(void) const {

	if (MaximumNumberOfPoints.isTouched() ||
		Vector.isTouched() ||
		ScaleFactor.isTouched()) {

		return 1;
	}

	return App::DocumentObject::mustExecute();
}

DocumentObjectExecReturn* PostGlyph3DFilter::execute(void) {

	vtkSmartPointer<vtkDataObject> data = getInputData();
	if (!data || !data->IsA("vtkDataSet"))
		return StdReturn;

	vtkDataSet *ds = vtkDataSet::SafeDownCast(data);
	vtkPointData *pd = ds->GetPointData();

	// Gather "vector" names
	std::string val;
	if (m_vectorFields.getEnums() && Vector.getValue() >= 0)
		val = Vector.getValueAsString();

	std::vector<std::string> array;
	for (int i = 0; i < pd->GetNumberOfArrays(); ++i) {
		if (pd->GetArray(i)->GetNumberOfComponents() == 3)
			array.push_back(pd->GetArrayName(i));
	}

	App::Enumeration empty;
	Vector.setValue(empty);
	m_vectorFields.setEnums(array);
	Vector.setValue(m_vectorFields);

	std::vector<std::string>::iterator it = std::find(array.begin(), array.end(), val);
	if (!val.empty() && it != array.end())
		Vector.setValue(val.c_str());

	return Cfd::PostFilter::execute();
}

//================================================================================================

PROPERTY_SOURCE(Cfd::PostStreamlineFilter, Cfd::PostFilter)

PostStreamlineFilter::PostStreamlineFilter(void) : PostFilter() {

	ADD_PROPERTY_TYPE(Vector, ((long)0), "Streamline Parameters", App::Prop_None, 
		"This property contains the vector arry from which to generate streamlines");

	ADD_PROPERTY_TYPE(Point1, (Base::Vector3d(0.0,0.0,0.0)), "Seeds", App::PropertyType(App::Prop_Output),
		"Set position of first end point");
	ADD_PROPERTY_TYPE(Point2, (Base::Vector3d(0.0,1.0,1.0)), "Seeds", App::PropertyType(App::Prop_Output),
		"Set position of other end point");
	ADD_PROPERTY_TYPE(Resolution, (100), "Seeds", App::PropertyType(App::Prop_Output),
		"Divide line into Resolution number of pieces");

	m_line = vtkSmartPointer<vtkLineSource>::New();
	const Base::Vector3d& vec1 = Point1.getValue();
	m_line->SetPoint1(vec1.x, vec1.y, vec1.z);
	const Base::Vector3d& vec2 = Point2.getValue();
	m_line->SetPoint2(vec2.x, vec2.y, vec2.z);
	m_line->SetResolution(Resolution.getValue());

	//m_tracer = vtkSmartPointer<vtkGenericStreamTracer>::New();
	m_tracer = vtkSmartPointer<vtkStreamTracer>::New();
	//streamline->SetInputData(Data.getValue());
	m_tracer->SetSourceConnection(m_line->GetOutputPort());

	FilterPipeline filter;
	//filter.algorithmStorage.push_back(seeds);
	filter.source = m_tracer;
	filter.target = m_tracer;
	addFilterPipeline(filter, "StreamLine");
	setActiveFilterPipeline("StreamLine");
}

PostStreamlineFilter::~PostStreamlineFilter() {

}

void PostStreamlineFilter::onChanged(const Property* prop) {

	if (prop == &Vector && (Vector.getValue() >= 0)) {
		m_tracer->SetInputArrayToProcess(0, 0, 0,
			vtkDataObject::FIELD_ASSOCIATION_POINTS, Vector.getValueAsString());
	}

	if (prop == &Point1) {
		const Base::Vector3d& vec1 = Point1.getValue();
		m_line->SetPoint1(vec1.x, vec1.y, vec1.z);
	}
	else if (prop == &Point2) {
		const Base::Vector3d& vec2 = Point2.getValue();
		m_line->SetPoint2(vec2.x, vec2.y, vec2.z);
	}
	else if (prop == &Resolution) {
		m_line->SetResolution(Resolution.getValue());
	}

	Cfd::PostFilter::onChanged(prop);
}

short int PostStreamlineFilter::mustExecute(void) const {

	if (Vector.isTouched() || 
		Point1.isTouched() ||
		Point2.isTouched() ||
		Resolution.isTouched()) {

		return 1;
	}

	return App::DocumentObject::mustExecute();
}

DocumentObjectExecReturn* PostStreamlineFilter::execute(void) {

	updateProperties();

	return Cfd::PostFilter::execute();
}

void PostStreamlineFilter::updateProperties() {

	vtkSmartPointer<vtkDataObject> data = getInputData();
	if (!data || !data->IsA("vtkDataSet"))
		return;

	vtkDataSet* ds = vtkDataSet::SafeDownCast(data);
	vtkPointData* pd = ds->GetPointData();

	std::string val;
	if (m_vectorFields.getEnums() && Vector.getValue() >= 0)
		val = Vector.getValueAsString();

	std::vector<std::string> array;

	for (int i = 0; i < pd->GetNumberOfArrays(); ++i) {
		if (pd->GetArray(i)->GetNumberOfComponents() == 3)
			array.push_back(pd->GetArrayName(i));
	}

	App::Enumeration empty;
	Vector.setValue(empty);
	m_vectorFields.setEnums(array);
	Vector.setValue(m_vectorFields);

	std::vector<std::string>::iterator it = std::find(array.begin(), array.end(), val);
	if (!val.empty() && it != array.end())
		Vector.setValue(val.c_str());

	// Point
	vtkBoundingBox bb;
	bb.AddBounds(ds->GetBounds());

	double minPnt[3], maxPnt[3];
	bb.GetMinPoint(minPnt);
	bb.GetMaxPoint(maxPnt);
	Base::Vector3d vec1(minPnt[0], minPnt[1], minPnt[2]);
	Base::Vector3d vec2(maxPnt[0], maxPnt[1], maxPnt[2]);
	Point1.setValue(vec1);
	Point2.setValue(vec2);
}

//================================================================================================
PROPERTY_SOURCE(Cfd::PostDataAlongLineFilter, Cfd::PostFilter)

PostDataAlongLineFilter::PostDataAlongLineFilter(void) : PostFilter() {

	ADD_PROPERTY_TYPE(Point1, (Base::Vector3d(0.0, 0.0, 0.0)), "DataAlongLine", App::Prop_None, "The point 1 used to define end point of line");
	ADD_PROPERTY_TYPE(Point2, (Base::Vector3d(0.0, 0.0, 1.0)), "DataAlongLine", App::Prop_None, "The point 2 used to define end point of line");
	ADD_PROPERTY_TYPE(Resolution, (100), "DataAlongLine", App::Prop_None, "The number of intervals between the 2 end points of line");
	ADD_PROPERTY_TYPE(XAxisData, (0), "DataAlongLine", App::Prop_None, "X axis data values used for plotting");
	ADD_PROPERTY_TYPE(YAxisData, (0), "DataAlongLine", App::Prop_None, "Y axis data values used for plotting");
	ADD_PROPERTY_TYPE(PlotData, (""), "DataAlongLine", App::Prop_None, "Field used for plotting");

	PlotData.setStatus(App::Property::ReadOnly, true);
	XAxisData.setStatus(App::Property::ReadOnly, true);
	YAxisData.setStatus(App::Property::ReadOnly, true);

	FilterPipeline clip;

	m_line = vtkSmartPointer<vtkLineSource>::New();
	const Base::Vector3d& vec1 = Point1.getValue();
	m_line->SetPoint1(vec1.x, vec1.y, vec1.z);
	const Base::Vector3d& vec2 = Point2.getValue();
	m_line->SetPoint2(vec2.x, vec2.y, vec2.z);
	m_line->SetResolution(Resolution.getValue());


	m_probe = vtkSmartPointer<vtkProbeFilter>::New();
	m_probe->SetInputConnection(m_line->GetOutputPort());
	m_probe->SetValidPointMaskArrayName("ValidPointArray");
	m_probe->SetPassPointArrays(1);
	m_probe->SetPassCellArrays(1);
	// needs vtk > 6.1
#if (VTK_MAJOR_VERSION > 6) || (VTK_MINOR_VERSION > 1)
	m_probe->ComputeToleranceOff();
	m_probe->SetTolerance(0.01);
#endif

	clip.filterSource = m_probe;
	clip.filterTarget = m_probe;

	addFilterPipeline(clip, "DataAlongLine");
	setActiveFilterPipeline("DataAlongLine");
}

PostDataAlongLineFilter::~PostDataAlongLineFilter() {

}

DocumentObjectExecReturn* PostDataAlongLineFilter::execute(void) {

	//recalculate the filter
	return Cfd::PostFilter::execute();
}


void PostDataAlongLineFilter::onChanged(const Property* prop) {
	if (prop == &Point1) {
		const Base::Vector3d& vec1 = Point1.getValue();
		m_line->SetPoint1(vec1.x, vec1.y, vec1.z);
	}
	else if (prop == &Point2) {
		const Base::Vector3d& vec2 = Point2.getValue();
		m_line->SetPoint2(vec2.x, vec2.y, vec2.z);
	}
	else if (prop == &Resolution) {
		m_line->SetResolution(Resolution.getValue());
	}
	else if (prop == &PlotData) {
		GetAxisData();
	}
	Cfd::PostFilter::onChanged(prop);
}

short int PostDataAlongLineFilter::mustExecute(void) const {

	if (Point1.isTouched() ||
		Point2.isTouched() ||
		Resolution.isTouched()) {

		return 1;
	}
	else return App::DocumentObject::mustExecute();
}

void PostDataAlongLineFilter::GetAxisData() {

	std::vector<double> coords;
	std::vector<double> values;

	vtkSmartPointer<vtkDataObject> data = m_probe->GetOutputDataObject(0);
	vtkDataSet* dset = vtkDataSet::SafeDownCast(data);
	vtkDataArray* pdata = dset->GetPointData()->GetArray(PlotData.getValue());
	vtkDataArray *tcoords = dset->GetPointData()->GetTCoords("Texture Coordinates");

	int component = 0;

	const Base::Vector3d& vec1 = Point1.getValue();
	const Base::Vector3d& vec2 = Point2.getValue();
	const Base::Vector3d diff = vec1 - vec2;
	double Len = diff.Length();

	for (int i = 0; i < dset->GetNumberOfPoints(); ++i) {

		double value = 0;
		if (pdata->GetNumberOfComponents() == 1)
			value = pdata->GetComponent(i, component);
		else {
			for (int j = 0; j < pdata->GetNumberOfComponents(); ++j)
				value += std::pow(pdata->GetComponent(i, j), 2);

			value = std::sqrt(value);
		}
		values.push_back(value);
		double tcoord = tcoords->GetComponent(i, component);
		coords.push_back(tcoord*Len);
	}
	YAxisData.setValues(values);
	XAxisData.setValues(coords);
}

//================================================================================================
PROPERTY_SOURCE(Cfd::PostDataAtPointFilter, Cfd::PostFilter)

PostDataAtPointFilter::PostDataAtPointFilter(void) : PostFilter() {

	ADD_PROPERTY_TYPE(Center, (Base::Vector3d(0.0, 0.0, 1.0)), "DataAtPoint", App::Prop_None, "The center used to define the center of the point");
	ADD_PROPERTY_TYPE(Radius, (0), "DataAtPoint", App::Prop_None, "The point 2 used to define end point of line");
	ADD_PROPERTY_TYPE(PointData, (0), "DataAtPoint", App::Prop_None, "Point data values used for plotting");
	ADD_PROPERTY_TYPE(FieldName, (""), "DataAtPoint", App::Prop_None, "Field used for plotting");
	ADD_PROPERTY_TYPE(Unit, (""), "DataAtPoint", App::Prop_None, "Unit used for Field");

	PointData.setStatus(App::Property::ReadOnly, true);
	FieldName.setStatus(App::Property::ReadOnly, true);
	Unit.setStatus(App::Property::ReadOnly, true);

	FilterPipeline clip;

	m_point = vtkSmartPointer<vtkPointSource>::New();
	const Base::Vector3d& vec = Center.getValue();
	m_point->SetCenter(vec.x, vec.y, vec.z);
	m_point->SetRadius(0);

	m_probe = vtkSmartPointer<vtkProbeFilter>::New();
	m_probe->SetInputConnection(m_point->GetOutputPort());
	m_probe->SetValidPointMaskArrayName("ValidPointArray");
	m_probe->SetPassPointArrays(1);
	m_probe->SetPassCellArrays(1);
	// needs vtk > 6.1
#if (VTK_MAJOR_VERSION > 6) || (VTK_MINOR_VERSION > 1)
	m_probe->ComputeToleranceOff();
	m_probe->SetTolerance(0.01);
#endif

	clip.filterSource = m_probe;
	clip.filterTarget = m_probe;

	addFilterPipeline(clip, "DataAtPoint");
	setActiveFilterPipeline("DataAtPoint");
}

PostDataAtPointFilter::~PostDataAtPointFilter() {

}

DocumentObjectExecReturn* PostDataAtPointFilter::execute(void) {

	//recalculate the filter
	return Cfd::PostFilter::execute();
}


void PostDataAtPointFilter::onChanged(const Property* prop) {
	if (prop == &Center) {
		const Base::Vector3d& vec = Center.getValue();
		m_point->SetCenter(vec.x, vec.y, vec.z);
	}
	else if (prop == &FieldName) {
		GetPointData();
	}
	Cfd::PostFilter::onChanged(prop);
}

short int PostDataAtPointFilter::mustExecute(void) const {

	if (Center.isTouched()) {

		return 1;
	}
	else return App::DocumentObject::mustExecute();
}

void PostDataAtPointFilter::GetPointData() {

	std::vector<double> values;

	vtkSmartPointer<vtkDataObject> data = m_probe->GetOutputDataObject(0);
	vtkDataSet* dset = vtkDataSet::SafeDownCast(data);
	vtkDataArray* pdata = dset->GetPointData()->GetArray(FieldName.getValue());

	int component = 0;

	for (int i = 0; i < dset->GetNumberOfPoints(); ++i) {

		double value = 0;
		if (pdata->GetNumberOfComponents() == 1)
			value = pdata->GetComponent(i, component);
		else {
			for (int j = 0; j < pdata->GetNumberOfComponents(); ++j)
				value += std::pow(pdata->GetComponent(i, j), 2);

			value = std::sqrt(value);
		}
		values.push_back(value);
	}
	PointData.setValues(values);
}
