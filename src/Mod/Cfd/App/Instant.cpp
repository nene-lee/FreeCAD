/***************************************************************************
 *   Copyright (c) Nene Li        <nene_lee@163.com>                       *
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

#include <Base/FileInfo.h>
#include <Base/TimeInfo.h>
#include <Base/Console.h>
#include <Base/Type.h>
#include <Base/Parameter.h>

#include <App/Application.h>
#include <App/Document.h>
#include <App/DocumentObject.h>


#include <vtkCollection.h>
#include <vtkPointData.h>
#include <vtkCellData.h>
#include <vtkCellArray.h>
#include <vtkDataArray.h>
#include <vtkDoubleArray.h>
#include <vtkIdList.h>
#include <vtkCellTypes.h>
#include <vtkInformation.h>
#include <vtkStructuredGrid.h>
#include <vtkImageData.h>
#include <vtkRectilinearGrid.h>
#include <vtkUnstructuredGrid.h>
#include <vtkMultiBlockDataSet.h>

#include <vtkTriangle.h>
#include <vtkQuad.h>
#include <vtkQuadraticTriangle.h>
#include <vtkQuadraticQuad.h>
#include <vtkTetra.h>
#include <vtkPyramid.h>
#include <vtkWedge.h>
#include <vtkHexahedron.h>
#include <vtkQuadraticTetra.h>
#include <vtkQuadraticPyramid.h>
#include <vtkQuadraticWedge.h>
#include <vtkQuadraticHexahedron.h>

#include <vtkExecutive.h>
#include <vtkAlgorithm.h>
#include <vtkExtractTimeSteps.h>
#include <vtkMultiTimeStepAlgorithm.h>
#include <vtkMultiBlockFromTimeSeriesFilter.h>
#include <vtkAppendFilter.h>
#include <vtkCellDataToPointData.h>
#include <vtkDataSetReader.h>
#include <vtkDataSetWriter.h>
#include <vtkUnstructuredGridReader.h>
#include <vtkUnstructuredGridWriter.h>
#include <vtkXMLUnstructuredGridReader.h>
#include <vtkXMLUnstructuredGridWriter.h>
#include <vtkOpenFOAMReader.h>
#include <vtkPOpenFOAMReader.h>
#include <vtkFLUENTReader.h>
#include <vtkCompositeDataWriter.h>
#include <vtkGenericDataObjectWriter.h>
#include <vtkNetCDFReader.h>
#include <vtkNetCDFCFReader.h>


//#include "MyMultiBlockFromTimeSeriesFilter.h"

#include "Instant.H"

using namespace Cfd;


//================================================================================================
vtkStandardNewMacro(Instant);

Instant::Instant()
{
	this->TimeValue = 0.0,
	this->Regions = nullptr;
}

Instant::~Instant()
{
	if (this->Regions)
	{
		this->Regions->UnRegister(this);
	}
}

void Instant::SetRegions(vtkMultiBlockDataSet *data)
{
	if (!data || this->Regions == data)
	{
		return;
	}

	if (this->Regions)
	{
		this->Regions->UnRegister(this);
		this->Regions = nullptr;
	}

	this->Regions = data;
	this->Regions->Register(this);

	this->Modified();
}

bool Instant::Equal(const scalar b) const
{
	return (this->TimeValue < b + SMALL && this->TimeValue > b - SMALL);
}

void Instant::PrintSelf(ostream& os, vtkIndent indent)
{
	this->Superclass::PrintSelf(os, indent);

	os << indent << "TimeValue: " << this->TimeValue << "\n";
	os << indent << "Regions: " << this->Regions << "\n";
}

vtkSmartPointer<vtkDataObject> Instant::GetAllRegions() const
{
	vtkSmartPointer<vtkAppendFilter> filter =
		vtkSmartPointer<vtkAppendFilter>::New();

	for (int i = 0; i < this->Regions->GetNumberOfBlocks(); ++i) {

		vtkSmartPointer<vtkDataObject> region =
			this->Regions->GetBlock(i);

		filter->AddInputData(region);
	}

	filter->Update();

	return filter->GetOutput();
}

std::vector<std::string> Instant::ListAllRegionNames()
{
	std::vector<std::string> regionNames;

	for (int i = 0; i < this->Regions->GetNumberOfBlocks(); ++i) {

		vtkSmartPointer<vtkDataObject> region =
			this->Regions->GetBlock(i);

		const char * regionName =
			this->Regions->GetMetaData(i)->Get(vtkCompositeDataSet::NAME());

		regionNames.push_back(regionName);
	}

	return regionNames;
}

bool Cfd::operator==(const Instant& a, const Instant& b)
{
	return a.Equal(b.TimeValue);
}


bool Cfd::operator!=(const Instant& a, const Instant& b)
{
	return !Cfd::operator==(a, b);
}


bool Cfd::operator<(const Instant& a, const Instant& b)
{
	return a.TimeValue < b.TimeValue;
}


bool Cfd::operator>(const Instant& a, const Instant& b)
{
	return a.TimeValue > b.TimeValue;
}

//================================================================================================
vtkStandardNewMacro(InstantCollection);

InstantCollection::InstantCollection()
{
}

InstantCollection::~InstantCollection()
{
}

bool InstantCollection::Read(const char *fileName)
{
	Base::TimeInfo Start;
	Base::Console().Log("Start: read result from file ======================\n");
	Base::FileInfo f(fileName);

	bool ret = false;

	if (f.fileName().compare("controlDict") == 0)
	{
		ret = ReadOpenFOAM(fileName);
	} 
	else if (f.hasExtension("vtk"))
	{
		ret = ReadDataSet(fileName);
	}
	else if (f.hasExtension("vtu"))
	{
		ret = ReadUnstructuredGrid(fileName);
	}
	else if (f.hasExtension("nc"))
	{
		ret = ReadNetCDF(fileName);
	}

	// some error occured!
	if (!ret)
	{
		Base::Console().Error("Error occured while reading \"%s\"\n", f.filePath());
		return false;
	}

	Base::Console().Log("    %f: Done \n", Base::TimeInfo::diffTimeF(Start, Base::TimeInfo()));
	Base::Console().Log("End: read result from file ======================\n");

	return true;
}

std::vector<scalar> InstantCollection::GetTimeValues() /*const*/
{
	std::vector<double> timeValues;

	for (int i = 0; i < this->GetNumberOfItems(); ++i) {

		Instant *inst = this->GetItem(i);
		timeValues.push_back(inst->GetTimeValue());
	}

	return timeValues;
}

Instant* InstantCollection::FindClosestInstant(const scalar) /*const*/
{
	return nullptr;
}

Instant* InstantCollection::FindInstant(const label timeIndex) /*const*/
{
	Instant *inst = this->GetItem(timeIndex);

	return inst;
}

bool InstantCollection::ReadDataSet(const char*fileName)
{
	vtkSmartPointer<vtkDataSetReader> reader =
		vtkSmartPointer<vtkDataSetReader>::New();
	reader->SetFileName(fileName);
	reader->Update();

	vtkSmartPointer<vtkDataSet> data =
		reader->GetOutput();

	if (!data)
	{
		return false;
	}

	vtkSmartPointer<vtkMultiBlockDataSet> regions =
		vtkSmartPointer<vtkMultiBlockDataSet>::New();
	regions->SetBlock(0, data);

	Instant *inst = Instant::New();
	inst->SetTimeValue(0.0);
	inst->SetRegions(regions);

	this->AddItem(inst);

	return true;
}

bool InstantCollection::ReadUnstructuredGrid(const char *fileName)
{
	vtkSmartPointer<vtkXMLUnstructuredGridReader> reader =
		vtkSmartPointer<vtkXMLUnstructuredGridReader>::New();
	reader->SetFileName(fileName);
	reader->Update();

	vtkSmartPointer<vtkUnstructuredGrid> data;

	if (reader->GetOutput())
	{
		data = reader->GetOutput();
	}
	else
	{
		vtkSmartPointer<vtkUnstructuredGridReader> reader2 =
			vtkSmartPointer<vtkUnstructuredGridReader>::New();
		reader2->SetFileName(fileName);
		reader2->Update();

		if (reader2->GetOutput())
		{
			data = reader2->GetOutput();
		}
	}

	if (!data)
	{
		return false;
	}

	vtkSmartPointer<vtkMultiBlockDataSet> regions =
		vtkSmartPointer<vtkMultiBlockDataSet>::New();
	regions->SetBlock(0, data);

	Instant *inst = Instant::New();
	inst->SetTimeValue(0.0);
	inst->SetRegions(regions);

	this->AddItem(inst);

	return true;
}

bool InstantCollection::ReadNetCDF(const char *fileName)
{
	//vtkSmartPointer<vtkNetCDFReader> reader =
	//	vtkSmartPointer<vtkNetCDFReader>::New();
	vtkSmartPointer<vtkNetCDFCFReader> reader =
		vtkSmartPointer<vtkNetCDFCFReader>::New();
	reader->SetFileName(fileName);
	reader->ReplaceFillValueWithNanOn();
	reader->SphericalCoordinatesOn();

	vtkSmartPointer<vtkMultiBlockFromTimeSeriesFilter> timeSeriesFilter
		= vtkSmartPointer<vtkMultiBlockFromTimeSeriesFilter>::New();
	timeSeriesFilter->SetInputConnection(reader->GetOutputPort());
	timeSeriesFilter->Update();

	vtkSmartPointer<vtkMultiBlockDataSet> blocks =
		timeSeriesFilter->GetOutput();

	for (int i = 0; i < blocks->GetNumberOfBlocks(); ++i) {

		vtkSmartPointer<vtkMultiBlockDataSet> regions =
			vtkMultiBlockDataSet::SafeDownCast(blocks->GetBlock(i));

		if (!regions) {

			continue;
		}

		double t = (double)i;

		Instant *inst = Instant::New();
		inst->SetTimeValue(t);
		inst->SetRegions(regions);

		this->AddItem(inst);
	}

	return true;
}

bool InstantCollection::ReadOpenFOAM(const char*fileName)
{
	vtkSmartPointer<vtkPOpenFOAMReader> reader =
		vtkSmartPointer<vtkPOpenFOAMReader>::New();
	reader->SetFileName(fileName);

	vtkSmartPointer<vtkMultiBlockFromTimeSeriesFilter> filter
		= vtkSmartPointer<vtkMultiBlockFromTimeSeriesFilter>::New();
	filter->SetInputConnection(reader->GetOutputPort());
	filter->Update();

	vtkSmartPointer<vtkMultiBlockDataSet> blocks =
		filter->GetOutput();

	vtkSmartPointer<vtkDoubleArray> timeValues =
		reader->GetTimeValues();

	for (int i = 0; i < timeValues->GetNumberOfTuples(); ++i) {

		vtkSmartPointer<vtkMultiBlockDataSet> regions =
			vtkMultiBlockDataSet::SafeDownCast(blocks->GetBlock(i));

		if (!regions) {

			continue;
		}

		double t = timeValues->GetTuple1(i);

		Instant *inst = Instant::New();
		inst->SetTimeValue(t);
		inst->SetRegions(regions);

		this->AddItem(inst);
	}

	return true;
}

