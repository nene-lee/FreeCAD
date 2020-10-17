/***************************************************************************
 *   Copyright (c) 2009 Jürgen Riegel <juergen.riegel@web.de>              *
 *   Copyright (c) 2017 Qingfeng Xia  <qingfeng.xia at oxford uni>         *
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
# include <cstdlib>
# include <memory>
# include <cmath>
# include <map>
#endif

#include <Base/FileInfo.h>
#include <Base/TimeInfo.h>
#include <Base/Console.h>
#include <Base/Type.h>
#include <Base/Parameter.h>

#include <App/Application.h>
#include <App/Document.h>
#include <App/DocumentObject.h>

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
#include <vtkDataSetReader.h>
#include <vtkDataSetWriter.h>
#include <vtkUnstructuredGridReader.h>
#include <vtkXMLUnstructuredGridReader.h>
#include <vtkXMLUnstructuredGridWriter.h>
#include <vtkOpenFOAMReader.h>
#include <vtkCompositeDataWriter.h>
#include <vtkGenericDataObjectWriter.h>
#include <vtkExtractTimeSteps.h>
#include <vtkMultiTimeStepAlgorithm.h>
#include <vtkMultiBlockFromTimeSeriesFilter.h>

#include "VTKTools.h"
#include "PropertyVtkDataObject.h"

using namespace Cfd;

template<class TReader> 
vtkDataObject* readVTKFile(const char*fileName)
{
  vtkSmartPointer<TReader> reader =
    vtkSmartPointer<TReader>::New();
  reader->SetFileName(fileName);
  reader->Update();
  reader->GetOutput()->Register(reader);
  return vtkDataObject::SafeDownCast(reader->GetOutput());
}

template<class TWriter> 
void writeVTKFile(const char* filename, vtkSmartPointer<vtkDataObject> data)
{
  vtkSmartPointer<TWriter> writer =
    vtkSmartPointer<TWriter>::New();
  writer->SetFileName(filename);
  writer->SetInputData(data);
  writer->Write();
}

vtkSmartPointer<vtkMultiBlockDataSet> readDataSet(const char*fileName)
{
	vtkSmartPointer<vtkDataSetReader> reader =
		vtkSmartPointer<vtkDataSetReader>::New();
	reader->SetFileName(fileName);
	reader->Update();

	if (reader->GetOutput())
	{
		vtkSmartPointer<vtkMultiBlockDataSet> region =
			vtkSmartPointer<vtkMultiBlockDataSet>::New();
		region->SetBlock(0, reader->GetOutput());

		vtkSmartPointer<vtkMultiBlockDataSet> instant =
			vtkSmartPointer<vtkMultiBlockDataSet>::New();
		instant->SetBlock(0, region);

		return instant;
	}

	return nullptr;
}

vtkSmartPointer<vtkMultiBlockDataSet> readUnstructuredGrid(const char*fileName)
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

	if (data)
	{
		vtkSmartPointer<vtkMultiBlockDataSet> region =
			vtkSmartPointer<vtkMultiBlockDataSet>::New();
		region->SetBlock(0, reader->GetOutput());

		vtkSmartPointer<vtkMultiBlockDataSet> instant =
			vtkSmartPointer<vtkMultiBlockDataSet>::New();
		instant->SetBlock(0, region);

		return instant;
	}

	return nullptr;
}

vtkSmartPointer<vtkMultiBlockDataSet> readOpenFOAM(const char*fileName)
{
	vtkSmartPointer<vtkOpenFOAMReader> reader =
		vtkSmartPointer<vtkOpenFOAMReader>::New();
	reader->SetFileName(fileName);

	vtkSmartPointer<vtkMultiBlockFromTimeSeriesFilter> filter
		= vtkSmartPointer<vtkMultiBlockFromTimeSeriesFilter>::New();
	filter->SetInputConnection(reader->GetOutputPort());
	filter->Update();

	return filter->GetOutput();
}

vtkSmartPointer<vtkMultiBlockDataSet> VTKTools::readResult(const std::string &fileName)
{
    Base::TimeInfo Start;
    Base::Console().Log("Start: read result from file ======================\n");
    Base::FileInfo f(fileName);

	vtkSmartPointer<vtkMultiBlockDataSet> blocks;
	
	if (f.hasExtension("vtk"))
	{
		blocks = readDataSet(fileName.c_str());
	}
    else if (f.hasExtension("vtu"))
    {
		blocks = readUnstructuredGrid(fileName.c_str());
    }
	else if ( f.fileName().compare("controlDict") == 0 )
	{
		blocks = readOpenFOAM(fileName.c_str());
	}

	// some error occured!
    if (!blocks)
    {
        Base::Console().Error("Error occured while reading \"%s\"\n", f.filePath());
		return blocks;
    }

    Base::Console().Log("    %f: Done \n", Base::TimeInfo::diffTimeF(Start, Base::TimeInfo()));
    Base::Console().Log("End: read result from file ======================\n");

    return blocks;
}