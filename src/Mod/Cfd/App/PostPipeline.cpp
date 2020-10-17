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

#include <vtkInformation.h>
#include <vtkMultiBlockDataSet.h>
#include <vtkMultiBlockFromTimeSeriesFilter.h>
#include <vtkExtractTimeSteps.h>
#include <vtkMultiTimeStepAlgorithm.h>
#include <vtkAppendFilter.h>
#include "VTKTools.h"
#include "Instant.h"

#include <Base/Console.h>

#include "PostPipeline.h"
#include "PostPipelinePy.h"
#include "ResultObject.h"
#include "PostFunction.h"

using namespace Cfd;
using namespace App;

PROPERTY_SOURCE(Cfd::PostPipeline, Cfd::PostFilter)
const char* PostPipeline::ModeEnums[]= {"Serial","Parallel",NULL};

PostPipeline::PostPipeline() 
{
	ADD_PROPERTY_TYPE(TimeIndex, (0), "Pipeline", Prop_None, "Time index of analysis incement");
    ADD_PROPERTY_TYPE(Filter, (0), "Pipeline", App::Prop_None, "The filter used in in this pipeline");
    ADD_PROPERTY_TYPE(Functions, (0), "Pipeline", App::Prop_Hidden, "The function provider which groups all pipeline functions");
    ADD_PROPERTY_TYPE(Mode,(long(0)), "Pipeline", App::Prop_None, "Selects the pipeline data transition mode. In serial every filter"
                                                              "gets the output of the previous one as input, in parallel every"
                                                              "filter gets the pipelien source as input.");
    Mode.setEnums(ModeEnums);

	// Contain result data at different times.
	m_instants = InstantCollection::New();
}

PostPipeline::~PostPipeline() 
{
	m_instants->Delete();
}

short PostPipeline::mustExecute(void) const 
{
    if(Mode.isTouched())
        return 1;

    return PostFilter::mustExecute();
}

DocumentObjectExecReturn* PostPipeline::execute(void) 
{
    //if we are the toplevel pipeline our data object is not created by filters, we are the main source!
    if(!Input.getValue())
        return StdReturn;

    //now if we are a filter than our data object is created by the filter we hold

    //if we are in serial mode we just copy over the data of the last filter,
    //but if we are in parallel we need to combine all filter results
    if(Mode.getValue() == 0) {

        //serial
        Data.setValue(getLastPostObject()->Data.getValue());
    }
    else {

        //parallel. go through all filters and append the result
        const std::vector<App::DocumentObject*>& filters = Filter.getValues();
        std::vector<App::DocumentObject*>::const_iterator it = filters.begin();

        vtkSmartPointer<vtkAppendFilter> append = vtkSmartPointer<vtkAppendFilter>::New();
        for(;it != filters.end(); ++it) {

            append->AddInputDataObject(static_cast<PostObject*>(*it)->Data.getValue());
        }

        append->Update();
        Data.setValue(append->GetOutputDataObject(0));
    }


    return Cfd::PostFilter::execute();
}

// PyObject *FemPostPipeline::getPyObject()
// {
//     if (PythonObject.is(Py::_None())){
//         // ref counter is set to 1
//         PythonObject = Py::Object(new DocumentObjectPy(this),true);
//     }
//     return Py::new_reference_to(PythonObject);
// }

void PostPipeline::onChanged(const Property* prop) 
{
    if(prop == &Filter || prop == &Mode) {

        //we check if all connections are right and add new ones if needed
        std::vector<App::DocumentObject*> objs = Filter.getValues();

        if(objs.empty())
            return;

        std::vector<App::DocumentObject*>::iterator it = objs.begin();
        PostFilter* filter = static_cast<PostFilter*>(*it);

        //If we have a Input we need to ensure our filters are connected correctly
        if(Input.getValue()) {

            //the first filter is always connected to the input
            if(filter->Input.getValue() != Input.getValue())
                filter->Input.setValue(Input.getValue());

            //all the others need to be connected to the previous filter or the source, dependent on the mode
            ++it;
            for(; it != objs.end(); ++it) {
                PostFilter* nextFilter = static_cast<PostFilter*>(*it);

                if(Mode.getValue() == 0) { //serial mode
                    if( nextFilter->Input.getValue() != filter)
                        nextFilter->Input.setValue(filter);
                }
                else { //Parallel mode
                    if( nextFilter->Input.getValue() != Input.getValue())
                        nextFilter->Input.setValue(Input.getValue());
                }

                filter = nextFilter;
            };
        }
        //if we have no input the filters are responsible of grabbing the pipeline data themself
        else {
            //the first filter must always grab the data
            if(filter->Input.getValue() != NULL)
                filter->Input.setValue(NULL);

            //all the others need to be connected to the previous filter or grab the data, dependent on mode
            ++it;
            for(; it != objs.end(); ++it) {
                PostFilter* nextFilter = static_cast<PostFilter*>(*it);

                if(Mode.getValue() == 0) { //serial mode
                    if( nextFilter->Input.getValue() != filter)
                        nextFilter->Input.setValue(filter);
                }
                else { //Parallel mode
                    if( nextFilter->Input.getValue() != NULL)
                        nextFilter->Input.setValue(NULL);
                }

                filter = nextFilter;
            };
        }
    }
	else if (prop == &TimeIndex) {

		Data.setValue(fetch());
	}

    Cfd::PostFilter::onChanged(prop);
}

PostObject* PostPipeline::getLastPostObject() 
{
    if(Filter.getValues().empty())
        return this;

    return static_cast<PostObject*>(Filter.getValues().back());
}

PyObject *PostPipeline::getPyObject() 
{
	if (PythonObject.is(Py::_None())) {
		// ref counter is set to 1
		PythonObject = Py::Object(new PostPipelinePy(this), true);
	}

	return Py::new_reference_to(PythonObject);
}

bool PostPipeline::holdsPostObject(PostObject* obj) 
{
    std::vector<App::DocumentObject*>::const_iterator it = Filter.getValues().begin();
    for(; it != Filter.getValues().end(); ++it) {

        if(*it == obj)
            return true;
    }

    return false;
}

bool PostPipeline::canRead(Base::FileInfo File) 
{
	if (File.hasExtension("vtk") ||
		// from FemResult only unstructural mesh is supported in femvtktoools.cpp
		File.hasExtension("vtp") ||
		File.hasExtension("vts") ||
		File.hasExtension("vtr") ||
		File.hasExtension("vti") ||
		File.hasExtension("vtu"))
		return true;

	return false;
}

void PostPipeline::read(Base::FileInfo File) 
{
	// checking on the file
	if (!File.isReadable())
		throw Base::FileException("File to load not existing or not readable", File);

	m_instants->Read(File.filePath().c_str());

	int upperIndex = m_instants->GetNumberOfItems();

	if (upperIndex > 0) {
		// show the at the last time index
		upperIndex = upperIndex -1;
	}
	else {
		// just set to 0
		upperIndex = 0;
	}

	m_timeIndexRange.LowerBound = 0;
	m_timeIndexRange.UpperBound = upperIndex;
	m_timeIndexRange.StepSize = 1;

	this->TimeIndex.setConstraints(&m_timeIndexRange);

	// show the last time index
	TimeIndex.setValue(upperIndex);
}

void PostPipeline::load(ResultObject* res) 
{
	if(!res) {
		Base::Console().Warning("Data of result object is empty\n");
		return;
	}

	std::vector<App::DocumentObject*> objs 
		= res->Pipes.getValues();
	objs.push_back(this);
	res->Pipes.setValues(objs);
}

std::vector<double> PostPipeline::listTimeValues() const 
{
	return std::vector<double>();
}

std::vector<std::string> PostPipeline::listRegions() const 
{
	return std::vector<std::string>();
}

vtkSmartPointer<vtkDataObject> PostPipeline::fetch() const 
{
	// Check the data
	long timeIndex = TimeIndex.getValue();

	// Try to get the result at 'timeIndex'
	vtkSmartPointer<Instant> inst =
		m_instants->FindInstant((unsigned int)timeIndex);
	if (!inst) {

		return nullptr;
	}

	vtkSmartPointer<vtkDataObject> regions =
		inst->GetAllRegions();

	return regions;
}

int PostPipeline::advance(int index) 
{
	long newIndex = TimeIndex.getValue() + index;

	TimeIndex.setValue(newIndex);

	return 0;
}