/***************************************************************************
 *   Copyright (c) 2004 Werner Mayer <wmayer[at]users.sourceforge.net>     *
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
# include <QDockWidget>
# include <QStatusBar>
#endif

#include <QApplication>
#include <QString>
#include <QLabel>
#include <QLineEdit>
#include <QSpinBox>
#include <QToolbar>

#include <Gui/Application.h>
#include <Gui/Command.h>
#include <Gui/ToolBarManager.h>
#include <Gui/MenuManager.h>
#include <Gui/DockWindowManager.h>
#include <Gui/MainWindow.h>

#include "Workbench.h"



using namespace CfdGui;

// --------------------------------------------------------------------

TYPESYSTEM_SOURCE(CfdGui::Workbench, Gui::StdWorkbench)

Workbench::Workbench()
	: StdWorkbench()
{
}

Workbench::~Workbench()
{
}

void Workbench::setupContextMenu(const char* recipient,Gui::MenuItem* item) const
{
	StdWorkbench::setupContextMenu(recipient, item);
}

void Workbench::createMainWindowPopupMenu(Gui::MenuItem*) const
{
	
}

void Workbench::activated()
{
	// Let us be notified when a document is activated, so that we can update the ActivePartObject
	Gui::Application::Instance->signalActiveDocument.connect(boost::bind(&Workbench::slotActiveDocument, this, _1));
	App::GetApplication().signalNewDocument.connect(boost::bind(&Workbench::slotNewDocument, this, _1));
	App::GetApplication().signalFinishRestoreDocument.connect(boost::bind(&Workbench::slotFinishRestoreDocument, this, _1));
	App::GetApplication().signalDeleteDocument.connect(boost::bind(&Workbench::slotDeleteDocument, this, _1));
	// Watch out for objects being added to the active document, so that we can add them to the body
	//App::GetApplication().signalNewObject.connect(boost::bind(&Workbench::slotNewObject, this, _1));	
}

void Workbench::deactivated()
{
	// Let us be notified when a document is activated, so that we can update the ActivePartObject
	Gui::Application::Instance->signalActiveDocument.disconnect(boost::bind(&Workbench::slotActiveDocument, this, _1));
	App::GetApplication().signalNewDocument.disconnect(boost::bind(&Workbench::slotNewDocument, this, _1));
	App::GetApplication().signalFinishRestoreDocument.disconnect(boost::bind(&Workbench::slotFinishRestoreDocument, this, _1));
	App::GetApplication().signalDeleteDocument.disconnect(boost::bind(&Workbench::slotDeleteDocument, this, _1));
	//App::GetApplication().signalNewObject.disconnect(boost::bind(&Workbench::slotNewObject, this, _1));	
}

Gui::MenuItem* Workbench::setupMenuBar() const
{
	Gui::MenuItem* root = StdWorkbench::setupMenuBar();
	Gui::MenuItem* item = root->findItem("&Windows");

	// Model
	{
		Gui::MenuItem* model = new Gui::MenuItem;
		root->insertItem(item, model);
		model->setCommand("Model");
	}

	// Constraints
	{
		Gui::MenuItem* constraints = new Gui::MenuItem;
		root->insertItem(item, constraints);
		constraints->setCommand("Constraints");
	}

	// Mesh
	{
		Gui::MenuItem* mesh = new Gui::MenuItem;
		root->insertItem(item, mesh);
		mesh->setCommand("Mesh");
		*mesh << "Mesh_ImportMesh";
	}

	// Solve
	{
		Gui::MenuItem* solve = new Gui::MenuItem;
		root->insertItem(item, solve);
		solve->setCommand("Solve");
	}

	// Results
	{
		Gui::MenuItem* results = new Gui::MenuItem;
		root->insertItem(item, results);
		results->setCommand("&Results");
		*results << "Post_ApplyChanges"
			<< "Post_PipelineFromResult"
			<< "Post_CreateFunction"
			<< "Separator"
			<< "Post_CreateWarpVectorFilter"
			<< "Post_CreateClipFilter"
			<< "Post_CreateScalarClipFilter"
			<< "Post_CreateCutFilter"
			<< "Post_Contour"
			<< "Post_Glyph3D"
			<< "Post_Streamline"
			<< "Post_CreateDataAlongLineFilter"
			<< "Post_CreateDataAtPointFilter";
	}


	return root;
}

Gui::ToolBarItem* Workbench::setupToolBars() const
{
	//// Add the time slider
	//QToolBar *toolbar = Gui::getMainWindow()->addToolBar(
	//	QApplication::translate("Workbench", "Time")); // i18n
	//toolbar->addWidget(new QLabel(QApplication::translate("Workbench", "Time")));
	//toolbar->addWidget(new QLineEdit());
	//toolbar->addWidget(new QSpinBox());

	// Add the commands (QActions)
	Gui::ToolBarItem* root = StdWorkbench::setupToolBars();

	Gui::ToolBarItem* timeline = new Gui::ToolBarItem(root);
	timeline->setCommand("VCR");
	*timeline << "VCR_First" << "VCR_Back"
		<< "VCR_Play"
		<< "VCR_Forward" << "VCR_Last";


	Gui::ToolBarItem* results = new Gui::ToolBarItem(root);
	results->setCommand("Results");
	*results << "Post_ApplyChanges"
		<< "Post_PipelineFromResult"
		<< "Post_CreateFunction"
		<< "Separator"
		<< "Post_CreateWarpVectorFilter"
		<< "Post_CreateClipFilter"
		<< "Post_CreateScalarClipFilter"
		<< "Post_CreateCutFilter"
		<< "Post_Contour"
		<< "Post_Glyph3D"
		<< "Post_Streamline"
		<< "Post_CreateDataAlongLineFilter"
		<< "Post_CreateDataAtPointFilter";

	return root;
}

Gui::ToolBarItem* Workbench::setupCommandBars() const
{
	Gui::ToolBarItem* root = Gui::StdWorkbench::setupCommandBars();
	// your changes
	return root;

}

Gui::DockWindowItems* Workbench::setupDockWindows() const
{
    Gui::DockWindowItems* root = Gui::StdWorkbench::setupDockWindows();
    root->setVisibility(false); // hide all dock windows by default
    root->setVisibility("Std_TreeView",true); // except of the tree view
    return root;	
}

void Workbench::slotActiveDocument(const Gui::Document& /*Doc*/)
{
	//     _switchToDocument(Doc.getDocument());
}

void Workbench::slotNewDocument(const App::Document& Doc)
{
	//     _switchToDocument(&Doc);
	App::Document &doc= const_cast<App::Document &>(Doc);

	std::string solverName = Doc.getUniqueObjectName("Solver");
	doc.addObject("Cfd::SolverObject", solverName.c_str(), true);

	std::string resultName = Doc.getUniqueObjectName("Result");
	doc.addObject("Cfd::ResultObject", resultName.c_str(), true);
}

void Workbench::slotFinishRestoreDocument(const App::Document& /*Doc*/)
{
	//     _switchToDocument(&Doc);
}

void Workbench::slotDeleteDocument(const App::Document&)
{
	//ActivePartObject = 0;
	//ActiveGuiDoc = 0;
	//ActiveAppDoc = 0;
	//ActiveVp = 0;
}

