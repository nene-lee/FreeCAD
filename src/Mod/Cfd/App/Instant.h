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

#ifndef CFD_INSTANT_H
#define CFD_INSTANT_H

#include <vtkCollection.h>

#include <cfloat>
#include <vector>

class vtkDataObject;
class vtkUnstructuredGrid;
class vtkMultiBlockDataSet;
class vtkCollection;


namespace Cfd
{
	#define SMALL DBL_MIN

	typedef double scalar;
	typedef unsigned int label;

	/**
     * @class   Instant
     * @brief   Contain a vtkMultiBlockDataSet object for a specified time.
     * @author  Nene Li
     *
     * InstantCollection is an object that represents the data for the given time value.
     * The vtkMultiBlockDataSet 'Data' corresponding to all the regions at that time.
     */
	class Instant : public vtkObject
	{
	public:
		//@{
		/**
		 * Standard methods for instantiation, type information, and printing.
		 */
		static Instant *New();
		vtkTypeMacro(Instant, vtkObject);
		void PrintSelf(ostream& os, vtkIndent indent) override;
		//@}

		//- Less function class used in sorting instants
		class less
		{
		public:

			bool operator()(const Instant& a, const Instant& b) const
			{
				return a.TimeValue < b.TimeValue;
			}
		};

		//@{
		/**
		 * Set / get the time value. The default is 0.0.
         */
		vtkSetMacro(TimeValue, double);
		vtkGetMacro(TimeValue, double);
		//@}

		/**
		 * Set/Get the underlying data. 
		 */
		virtual void SetRegions(vtkMultiBlockDataSet *);
		vtkMultiBlockDataSet *GetRegions() { return this->Regions; }


		//- Comparison used for instants to be equal
		bool Equal(const scalar) const;

		//- Value (const access)
		vtkSmartPointer<vtkDataObject> GetAllRegions() const;

		//vtkSmartPointer<vtkMultiBlockDataSet> getRegionsByIndex() const;

		//- List regions within this instant
		std::vector<std::string> ListAllRegionNames();

		// Friends
		friend bool operator==(const Instant&, const Instant&);
		friend bool operator!=(const Instant&, const Instant&);
		friend bool operator<(const Instant&, const Instant&);
		friend bool operator>(const Instant&, const Instant&);

	protected:
		Instant();
		~Instant() override;

	private:
		Instant(const Instant&) = delete;
		void operator=(const Instant&) = delete;

		scalar TimeValue;
		vtkMultiBlockDataSet *Regions;  // Array which represents data
	};

	bool operator==(const Instant&, const Instant&);
	bool operator!=(const Instant&, const Instant&);
	bool operator<(const Instant&, const Instant&);
	bool operator>(const Instant&, const Instant&);

	/**
     * @class   InstantCollection
     * @brief   maintain an unordered list of instant objects
	 * @author  Nene Li
     *
     * InstantCollection is an object that creates and manipulates ordered
     * lists of instants. See also vtkCollection and subclasses.
     */
	class InstantCollection : public vtkCollection
	{
	public:
		static InstantCollection *New();
		vtkTypeMacro(InstantCollection, vtkCollection);

		/**
		 * Add a dataset to the bottom of the list.
         */
		void AddItem(Instant *inst)
		{
			this->vtkCollection::AddItem(inst);
		}

		//@{
        /**
         * Get the next dataset in the list.
         */
		Instant *GetNextItem() {
			return static_cast<Instant *>(this->GetNextItemAsObject());
		};
		Instant *GetNextDataSet() {
			return static_cast<Instant *>(this->GetNextItemAsObject());
		};
		//@}

		//@{
		/**
		 * Get the ith dataset in the list.
		 */
		Instant *GetItem(int i) {
			return static_cast<Instant *>(this->GetItemAsObject(i));
		};
		Instant *GetDataSet(int i) {
			return static_cast<Instant *>(this->GetItemAsObject(i));
		};
		//@}

        /**
         * Reentrant safe way to get an object in a collection. Just pass the
         * same cookie back and forth.
        */
		Instant *GetNextDataSet(vtkCollectionSimpleIterator &cookie) {
			return static_cast<Instant *>(this->GetNextItemAsObject(cookie));
		};

		//- Read control dictionary, update controls and time
		virtual bool Read(const char *fileName);

		//- Search the case for valid time directories
		std::vector<scalar> GetTimeValues() /*const*/;

		//- Search the case for the time closest to the given time
		Instant *FindClosestInstant(const scalar timeValue) /*const*/;

		//- Find the instant at 'index'
		Instant *FindInstant(const label timeIndex) /*const*/;
		

	protected:
		InstantCollection();
		~InstantCollection() override;

		bool ReadDataSet(const char*fileName);
		bool ReadUnstructuredGrid(const char *fileName);
		bool ReadNetCDF(const char *fileName);
		bool ReadOpenFOAM(const char *fileName);

	private:
		// hide the standard AddItem from the user and the compiler.
		void AddItem(vtkObject *o) { this->vtkCollection::AddItem(o); };

	private:
		InstantCollection(const InstantCollection&) = delete;
		void operator=(const InstantCollection&) = delete;
	};
} //namespace CFD


#endif // CFD_INSTANT_H
