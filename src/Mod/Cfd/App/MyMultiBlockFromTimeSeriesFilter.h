/*=========================================================================

  Program:   Visualization Toolkit
  Module:    MyMultiBlockFromTimeSeriesFilter.h

  Copyright (c) Ken Martin, Will Schroeder, Bill Lorensen
  All rights reserved.
  See Copyright.txt or http://www.kitware.com/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notice for more information.

=========================================================================*/
/**
 * @class   MyMultiBlockFromTimeSeriesFilter
 * @brief   collects multiple inputs into one multi-group dataset
 *
 * MyMultiBlockFromTimeSeriesFilter is a 1 to 1 filter that merges multiple
 * time steps from the input into one multiblock dataset.  It will assign each
 * time step from the input to one group of the multi-block dataset and will
 * assign each timestep's data as a block in the multi-block datset.
*/

#ifndef MyMultiBlockFromTimeSeriesFilter_h
#define MyMultiBlockFromTimeSeriesFilter_h

#include "vtkFiltersGeneralModule.h" // For export macro
#include "vtkMultiBlockDataSetAlgorithm.h"
#include "vtkSmartPointer.h" // Smart pointer

#include <vector> // Vector to hold timesteps

class vtkMultiBlockDataSet;

class MyMultiBlockFromTimeSeriesFilter : public vtkMultiBlockDataSetAlgorithm
{
public:
  vtkTypeMacro(MyMultiBlockFromTimeSeriesFilter,vtkMultiBlockDataSetAlgorithm);
  void PrintSelf(ostream& os, vtkIndent indent) override;

  static MyMultiBlockFromTimeSeriesFilter *New();

protected:
  MyMultiBlockFromTimeSeriesFilter();
  ~MyMultiBlockFromTimeSeriesFilter() override;

  int FillInputPortInformation(int, vtkInformation *) override;

  int RequestInformation(vtkInformation *,
                  vtkInformationVector **,
                  vtkInformationVector *) override;
  int RequestUpdateExtent(vtkInformation *,
                  vtkInformationVector **,
                  vtkInformationVector *) override;
  int RequestData(vtkInformation *,
                  vtkInformationVector **,
                  vtkInformationVector *) override;

private:
  MyMultiBlockFromTimeSeriesFilter(const MyMultiBlockFromTimeSeriesFilter&) = delete;
  void operator=(const MyMultiBlockFromTimeSeriesFilter&) = delete;

  int UpdateTimeIndex;
  std::vector<double> TimeSteps;
  vtkSmartPointer<vtkMultiBlockDataSet> TempDataset;
};

#endif
