/***************************************************************************
 *   Copyright (c) 2008 J¨¹rgen Riegel (juergen.riegel@web.de)              *
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
# include <Standard_math.hxx>
# include <QApplication>
# include <QMessageBox>
# include <QAction>
#endif

#include <Inventor/nodes/SoEventCallback.h>
#include <Inventor/nodes/SoCamera.h>
#include <Inventor/events/SoMouseButtonEvent.h>

#include <Base/Console.h>
#include <Base/Exception.h>
#include <Base/Interpreter.h>
#include <Base/Tools.h>

#include <App/Application.h>
#include <App/Document.h>
#include <App/DocumentObject.h>

#include <Gui/Application.h>
#include <Gui/Control.h>
#include <Gui/Command.h>
#include <Gui/MainWindow.h>
#include <Gui/FileDialog.h>
#include <Gui/Selection.h>
#include <Gui/SelectionFilter.h>
#include <Gui/Document.h>
#include <Gui/WaitCursor.h>
#include <Gui/View3DInventor.h>
#include <Gui/View3DInventorViewer.h>
#include <Gui/Utilities.h>
#include <Gui/Action.h>
#include <Gui/BitmapFactory.h>


#include <Mod/Cfd/App/ResultObject.h>
#include <Mod/Cfd/App/PostObject.h>
#include <Mod/Cfd/App/PostPipeline.h>
#include <Mod/Cfd/App/PostFunction.h>

 //================================================================================================
 //================================================================================================
 // commands VCR

//================================================================================================
DEF_STD_CMD_A(CmdVCRFirst);

CmdVCRFirst::CmdVCRFirst()
	:Command("VCR_First")
{
	sAppModule = "Cfd";
	sGroup = QT_TR_NOOP("VCR");
	sMenuText = QT_TR_NOOP("First step");
	sToolTipText = QT_TR_NOOP("First step");
	sWhatsThis = "VCR_First";
	sStatusTip = QT_TR_NOOP("First step");
	sPixmap = "vcr_first32";
}

void CmdVCRFirst::activated(int)
{
}

bool CmdVCRFirst::isActive(void)
{
	return (getActiveGuiDocument() ? true : false);
}

 //================================================================================================
DEF_STD_CMD_A(CmdVCRBack);

CmdVCRBack::CmdVCRBack()
	:Command("VCR_Back")
{
	sAppModule = "Cfd";
	sGroup = QT_TR_NOOP("VCR");
	sMenuText = QT_TR_NOOP("Back step");
	sToolTipText = QT_TR_NOOP("Back step");
	sWhatsThis = "VCR_Back";
	sStatusTip = QT_TR_NOOP("Back step");
	sPixmap = "vcr_back32";
}

void CmdVCRBack::activated(int)
{
	// go through active document change some Visibility
	Gui::Document* doc = Gui::Application::Instance->activeDocument();
	App::Document* app = doc->getDocument();
	const std::vector<App::DocumentObject*> obj = app->getObjectsOfType
	(App::DocumentObject::getClassTypeId());

	std::vector<Cfd::PostPipeline*> pipelines = getSelection().getObjectsOfType<Cfd::PostPipeline>();
	if (pipelines.size() == 1) {
		openCommand("Next frame");
		doCommand(Doc, "App.activeDocument().%s.advance(-1)", pipelines[0]->getNameInDocument());
		commitCommand();

		this->updateActive();

	}
	else {
		QMessageBox::warning(Gui::getMainWindow(),
			qApp->translate("CmdVCRBack", "Wrong selection type"),
			qApp->translate("CmdVCRBack", "Select a PostPipeline object, please."));
	}
}

bool CmdVCRBack::isActive(void)
{
	return (getActiveGuiDocument() ? true : false);
}

//================================================================================================
DEF_STD_CMD_A(CmdVCRPlay);

CmdVCRPlay::CmdVCRPlay()
	:Command("VCR_Play")
{
	sAppModule = "Cfd";
	sGroup = QT_TR_NOOP("VCR");
	sMenuText = QT_TR_NOOP("Play steps");
	sToolTipText = QT_TR_NOOP("Play steps");
	sWhatsThis = "VCR_Play";
	sStatusTip = QT_TR_NOOP("Play steps");
	sPixmap = "vcr_play32";
}

void CmdVCRPlay::activated(int)
{
}

bool CmdVCRPlay::isActive(void)
{
	return (getActiveGuiDocument() ? true : false);
}

//================================================================================================
DEF_STD_CMD_A(CmdVCRForward);

CmdVCRForward::CmdVCRForward()
	:Command("VCR_Forward")
{
	sAppModule = "Cfd";
	sGroup = QT_TR_NOOP("VCR");
	sMenuText = QT_TR_NOOP("Next step");
	sToolTipText = QT_TR_NOOP("Next step");
	sWhatsThis = "VCR_Forward";
	sStatusTip = QT_TR_NOOP("Next step");
	sPixmap = "vcr_forward32";
}

void CmdVCRForward::activated(int)
{
	// go through active document change some Visibility
	Gui::Document* doc = Gui::Application::Instance->activeDocument();
	App::Document* app = doc->getDocument();
	const std::vector<App::DocumentObject*> obj = app->getObjectsOfType
	(App::DocumentObject::getClassTypeId());

	std::vector<Cfd::PostPipeline*> pipelines = getSelection().getObjectsOfType<Cfd::PostPipeline>();
	if (pipelines.size() == 1) {
		openCommand("Next frame");
		doCommand(Doc, "App.activeDocument().%s.advance(1)", pipelines[0]->getNameInDocument());
		commitCommand();

		this->updateActive();

	}
	else {
		QMessageBox::warning(Gui::getMainWindow(),
			qApp->translate("CmdVCRForward", "Wrong selection type"),
			qApp->translate("CmdVCRForward", "Select a PostPipeline object, please."));
	}
}

bool CmdVCRForward::isActive(void)
{
	return (getActiveGuiDocument() ? true : false);
}

//================================================================================================
DEF_STD_CMD_A(CmdVCRLast);

CmdVCRLast::CmdVCRLast()
	:Command("VCR_Last")
{
	sAppModule = "Cfd";
	sGroup = QT_TR_NOOP("VCR");
	sMenuText = QT_TR_NOOP("Last step");
	sToolTipText = QT_TR_NOOP("Last step");
	sWhatsThis = "VCR_Last";
	sStatusTip = QT_TR_NOOP("Last step");
	sPixmap = "vcr_last32";
}

void CmdVCRLast::activated(int)
{
}

bool CmdVCRLast::isActive(void)
{
	return (getActiveGuiDocument() ? true : false);
}

//================================================================================================
//================================================================================================
// commands vtk post processing

//================================================================================================
// helper vtk post processing

void setupFilter(Gui::Command* cmd, std::string Name) {
	// get the pipeline object the filter should be added too
	// if nothing is selected and there is exact one FemPostPipeline --> use this
	// if the user selects a FemPostPipeline --> use this
	// else --> error message

	Cfd::PostPipeline* pipeline = nullptr;
	Gui::SelectionFilter pipelinesFilter("SELECT Cfd::PostPipeline COUNT 1");
	if (pipelinesFilter.match()) {
		std::vector<Gui::SelectionObject> result = pipelinesFilter.Result[0];
		pipeline = static_cast<Cfd::PostPipeline*>(result.front().getObject());
	}
	else {
		std::vector<Cfd::PostPipeline*> pipelines = 
			App::GetApplication().getActiveDocument()->getObjectsOfType<Cfd::PostPipeline>();
		if (pipelines.size() == 1) {
			pipeline = pipelines.front();
		}
	}

	if (pipeline == nullptr) {
		QMessageBox::warning(Gui::getMainWindow(),
			qApp->translate("setupFilter", "Error: Wrong or no or to many vtk post processing objects."),
			qApp->translate("setupFilter", "The filter could not set up. Select one vtk post processing pipeline object, or select nothing and make sure there is exact one vtk post processing pipline object in the document."));
		return;
	}
	else {
		std::string FeatName = cmd->getUniqueObjectName(Name.c_str());

		cmd->openCommand("Create filter");
		cmd->doCommand(Gui::Command::Doc, "App.activeDocument().addObject('Cfd::Post%sFilter','%s')", Name.c_str(), FeatName.c_str());
		cmd->doCommand(Gui::Command::Doc, "__list__ = App.ActiveDocument.%s.Filter", pipeline->getNameInDocument());
		cmd->doCommand(Gui::Command::Doc, "__list__.append(App.ActiveDocument.%s)", FeatName.c_str());
		cmd->doCommand(Gui::Command::Doc, "App.ActiveDocument.%s.Filter = __list__", pipeline->getNameInDocument());
		cmd->doCommand(Gui::Command::Doc, "del __list__");

		cmd->updateActive();
		cmd->doCommand(Gui::Command::Gui, "Gui.activeDocument().setEdit('%s')", FeatName.c_str());
	}
};

//================================================================================================
DEF_STD_CMD_AC(CmdPostApplyChanges);

CmdPostApplyChanges::CmdPostApplyChanges()
	: Command("Post_ApplyChanges")
{
	sAppModule = "Cfd";
	sGroup = QT_TR_NOOP("Post");
	sMenuText = QT_TR_NOOP("Apply changes to pipeline");
	sToolTipText = QT_TR_NOOP("Apply changes to parameters directly and not on recompute only...");
	sWhatsThis = "Post_ApplyChanges";
	sStatusTip = sToolTipText;
	sPixmap = "view-refresh";
	eType = eType | ForEdit;
}

void CmdPostApplyChanges::activated(int iMsg)
{
	ParameterGrp::handle hGrp = App::GetApplication().GetParameterGroupByPath("User parameter:BaseApp/Preferences/Mod/Cfd");

	if (iMsg == 1)
		hGrp->SetBool("PostAutoRecompute", true);
	else
		hGrp->SetBool("PostAutoRecompute", false);
}

bool CmdPostApplyChanges::isActive(void)
{
	if (getActiveGuiDocument())
		return true;
	else
		return false;
}

Gui::Action * CmdPostApplyChanges::createAction(void)
{
	Gui::Action *pcAction = Command::createAction();
	pcAction->setCheckable(true);
	ParameterGrp::handle hGrp = App::GetApplication().GetParameterGroupByPath("User parameter:BaseApp/Preferences/Mod/Cfd");
	pcAction->setChecked(hGrp->GetBool("PostAutoRecompute", false));

	return pcAction;
}

//================================================================================================
DEF_STD_CMD_A(CmdPostReadResult);

CmdPostReadResult::CmdPostReadResult()
	: Command("Post_ReadResult")
{
	sAppModule = "Cfd";
	sGroup = QT_TR_NOOP("Post");
	sMenuText = QT_TR_NOOP("Read result");
	sToolTipText = QT_TR_NOOP("Read result from files");
	sWhatsThis = "Post_ReadResult";
	sStatusTip = sToolTipText;
	sPixmap = "post_pipeline";
}

void CmdPostReadResult::activated(int)
{
	// use current path as default
	QStringList filter;
	filter << QString::fromLatin1("%1 (*.vtk  *.vtu)").arg(QObject::tr("VTK Files"))
		<< QString::fromLatin1("%1 (controlDict)").arg(QObject::tr("OpenFOAM Files"));

	// Allow multi selection
	QStringList fn = Gui::FileDialog::getOpenFileNames(Gui::getMainWindow(),
		QObject::tr("Read result"), QString(), filter.join(QLatin1String(";;")));

	//std::vector<Cfd::ResultObject*> results = getSelection().getObjectsOfType<Cfd::ResultObject>();

	for (QStringList::Iterator it = fn.begin(); it != fn.end(); ++it) {
		QFileInfo fi;
		fi.setFile(*it);

		QString baseName = fi.baseName();
		std::string fileName = Base::Tools::escapedUnicodeFromUtf8((*it).toUtf8().data());
		//std::string resName = getUniqueObjectName("Result");
		std::string resName = getUniqueObjectName(baseName.toStdString().c_str());
		std::string featName = getUniqueObjectName("PostPipeline");
		openCommand("Read result");
		doCommand(Doc, "import Cfd");
		//doCommand(Doc, "res = App.activeDocument().getObject(\"%s\")", results[0]->getNameInDocument());
		doCommand(Doc, "res = App.ActiveDocument.addObject('Cfd::ResultObject','%s')", resName.c_str());
		doCommand(Doc, "res.read(u\"%s\")", fileName.c_str());
		doCommand(Doc, "feat = App.ActiveDocument.addObject('Cfd::PostPipeline','%s')", featName.c_str());
		doCommand(Doc, "feat.load(res)");
		commitCommand();
		updateActive();
	}
}

bool CmdPostReadResult::isActive(void)
{
	return hasActiveDocument();
}

//================================================================================================
DEF_STD_CMD_A(CmdPostPipelineFromResult);

CmdPostPipelineFromResult::CmdPostPipelineFromResult()
	: Command("Post_PipelineFromResult")
{
	sAppModule = "Cfd";
	sGroup = QT_TR_NOOP("Post");
	sMenuText = QT_TR_NOOP("Post pipeline from result");
	sToolTipText = QT_TR_NOOP("Creates a post processing pipeline from a result object");
	sWhatsThis = "Post_PipelineFromResult";
	sStatusTip = sToolTipText;
	sPixmap = "post_pipeline";
}

void CmdPostPipelineFromResult::activated(int)
{
	// use current path as default
	QStringList filter;
	filter << QString::fromLatin1("%1 (*.vtk  *.vtu)").arg(QObject::tr("VTK Files"))
		<< QString::fromLatin1("%1 (*.nc)").arg(QObject::tr("NetCDF Files"))
		<< QString::fromLatin1("%1 (controlDict)").arg(QObject::tr("OpenFOAM Files"));

	// Allow multi selection
	QStringList fn = Gui::FileDialog::getOpenFileNames(Gui::getMainWindow(),
		QObject::tr("Read result"), QString(), filter.join(QLatin1String(";;")));

	//// go through active document change some Visibility
	Gui::Document* doc = Gui::Application::Instance->activeDocument();
	App::Document* app = doc->getDocument();
	//const std::vector<App::DocumentObject*> obj = app->getObjectsOfType
	//(App::DocumentObject::getClassTypeId());
	//for (std::vector<App::DocumentObject*>::const_iterator it = obj.begin(); it != obj.end(); ++it) {
	//	doCommand(Gui, "Gui.getDocument(\"%s\").getObject(\"%s\").Visibility=False"
	//		, app->getName(), (*it)->getNameInDocument());
	//}

	std::vector<Cfd::ResultObject*> results = app->getObjectsOfType<Cfd::ResultObject>();
	if (results.size() == 1) {

		for (QStringList::Iterator it = fn.begin(); it != fn.end(); ++it) {
			QFileInfo fi;
			fi.setFile(*it);

			//doc->createView(Gui::View3DInventor::getClassTypeId());

			QString baseName = fi.baseName();
			std::string fileName = Base::Tools::escapedUnicodeFromUtf8((*it).toUtf8().data());
			std::string featName = Base::Tools::escapedUnicodeFromUtf8(baseName.toUtf8().data());
			openCommand("Create pipeline from result");
			doCommand(Doc, "import Cfd");
			doCommand(Doc, "feat = App.ActiveDocument.addObject('Cfd::PostPipeline','%s')", featName.c_str());
			doCommand(Doc, "feat.read(u\"%s\")", fileName.c_str());
			doCommand(Doc, "__list__ = App.ActiveDocument.%s.Pipes", results[0]->getNameInDocument());
			doCommand(Doc, "__list__.append(App.ActiveDocument.%s)", featName.c_str());
			doCommand(Doc, "App.ActiveDocument.%s.Pipes = __list__", results[0]->getNameInDocument());
			doCommand(Doc, "del __list__");
			commitCommand();
			updateActive();
		}
	}
	else {

		QMessageBox::warning(Gui::getMainWindow(),
			qApp->translate("CmdPostPipelineFromResult", "Wrong selection type"),
			qApp->translate("CmdPostPipelineFromResult", "Select a result object, please."));
	}
}

bool CmdPostPipelineFromResult::isActive(void)
{
	return hasActiveDocument();
}

//================================================================================================
DEF_STD_CMD_ACL(CmdPostFunction);

CmdPostFunction::CmdPostFunction()
	: Command("Post_CreateFunction")
{
	sAppModule = "Cfd";
	sGroup = QT_TR_NOOP("Post");
	sMenuText = QT_TR_NOOP("Function");
	sToolTipText = QT_TR_NOOP("Functions for use in postprocessing filter...");
	sWhatsThis = "Post_Function";
	sStatusTip = sToolTipText;
	eType = eType | ForEdit;
}

void CmdPostFunction::activated(int iMsg)
{
	std::string name;
	if (iMsg == 0)
		name = "Plane";
	else if (iMsg == 1)
		name = "Sphere";
	else
		return;

	//create the object
	std::vector<Cfd::PostPipeline*> pipelines = App::GetApplication().getActiveDocument()->getObjectsOfType<Cfd::PostPipeline>();
	if (!pipelines.empty()) {
		Cfd::PostPipeline *pipeline = pipelines.front();

		openCommand("Create function");

		//check if the pipeline has a filter provider and add one if needed
		Cfd::PostFunctionProvider* provider;
		if (!pipeline->Functions.getValue() || pipeline->Functions.getValue()->getTypeId() != Cfd::PostFunctionProvider::getClassTypeId()) {
			std::string funcName = getUniqueObjectName("Functions");
			doCommand(Doc, "App.ActiveDocument.addObject('Cfd::PostFunctionProvider','%s')", funcName.c_str());
			doCommand(Doc, "App.ActiveDocument.%s.Functions = App.ActiveDocument.%s", pipeline->getNameInDocument(), funcName.c_str());
			provider = static_cast<Cfd::PostFunctionProvider*>(getDocument()->getObject(funcName.c_str()));
		}
		else
			provider = static_cast<Cfd::PostFunctionProvider*>(pipeline->Functions.getValue());

		//build the object
		std::string featName = getUniqueObjectName(name.c_str());
		doCommand(Doc, "App.activeDocument().addObject('Cfd::Post%sFunction','%s')", name.c_str(), featName.c_str());
		doCommand(Doc, "__list__ = App.ActiveDocument.%s.Functions", provider->getNameInDocument());
		doCommand(Doc, "__list__.append(App.ActiveDocument.%s)", featName.c_str());
		doCommand(Doc, "App.ActiveDocument.%s.Functions = __list__", provider->getNameInDocument());
		doCommand(Doc, "del __list__");

		//set the default values, for this get the bounding box
		vtkBoundingBox box = pipeline->getBoundingBox();

		double center[3];
		box.GetCenter(center);

		if (iMsg == 0)
			doCommand(Doc, "App.ActiveDocument.%s.Origin = App.Vector(%f, %f, %f)", featName.c_str(), center[0],
				center[1], center[2]);
		else if (iMsg == 1) {
			doCommand(Doc, "App.ActiveDocument.%s.Center = App.Vector(%f, %f, %f)", featName.c_str(), center[0],
				center[1] + box.GetLength(1) / 2, center[2] + box.GetLength(2) / 2);
			doCommand(Doc, "App.ActiveDocument.%s.Radius = %f", featName.c_str(), box.GetDiagonalLength() / 2);
		}


		this->updateActive();
		//most of the times functions are added inside of a filter, make sure this still works
		if (Gui::Application::Instance->activeDocument()->getInEdit() == NULL)
			doCommand(Gui, "Gui.activeDocument().setEdit('%s')", featName.c_str());
	}
	else {
		QMessageBox::warning(Gui::getMainWindow(),
			qApp->translate("CmdPostFunction", "Wrong selection"),
			qApp->translate("CmdPostFunction", "Select a pipeline, please."));
	}

	// Since the default icon is reset when enabing/disabling the command we have
	// to explicitly set the icon of the used command.
	Gui::ActionGroup* pcAction = qobject_cast<Gui::ActionGroup*>(_pcAction);
	QList<QAction*> a = pcAction->actions();

	assert(iMsg < a.size());
	pcAction->setIcon(a[iMsg]->icon());
}

Gui::Action * CmdPostFunction::createAction(void)
{
	Gui::ActionGroup* pcAction = new Gui::ActionGroup(this, Gui::getMainWindow());
	pcAction->setDropDownMenu(true);
	applyCommandData(this->className(), pcAction);

	QAction* cmd0 = pcAction->addAction(QString());
	cmd0->setIcon(Gui::BitmapFactory().iconFromTheme("geo_plane"));

	QAction* cmd1 = pcAction->addAction(QString());
	cmd1->setIcon(Gui::BitmapFactory().iconFromTheme("geo_sphere"));

	_pcAction = pcAction;
	languageChange();

	pcAction->setIcon(cmd1->icon());
	int defaultId = 0;
	pcAction->setProperty("defaultAction", QVariant(defaultId));

	return pcAction;
}

void CmdPostFunction::languageChange()
{
	Command::languageChange();

	if (!_pcAction)
		return;
	Gui::ActionGroup* pcAction = qobject_cast<Gui::ActionGroup*>(_pcAction);
	QList<QAction*> a = pcAction->actions();

	QAction* cmd = a[0];
	cmd->setText(QApplication::translate("CmdPostFunction", "Plane"));
	cmd->setToolTip(QApplication::translate("Post_Function", "Create a plane function, defined by its origin and normal"));
	cmd->setStatusTip(cmd->toolTip());

	cmd = a[1];
	cmd->setText(QApplication::translate("CmdPostFunction", "Sphere"));
	cmd->setToolTip(QApplication::translate("Post_Function", "Create a phere function, defined by its center and radius"));
	cmd->setStatusTip(cmd->toolTip());

}

bool CmdPostFunction::isActive(void)
{
	if (getActiveGuiDocument())
		return true;
	else
		return false;
}

//================================================================================================
DEF_STD_CMD_A(CmdPostWarpVectorFilter);

CmdPostWarpVectorFilter::CmdPostWarpVectorFilter()
	: Command("Post_CreateWarpVectorFilter")
{
	sAppModule = "Cfd";
	sGroup = QT_TR_NOOP("Post");
	sMenuText = QT_TR_NOOP("Warp");
	sToolTipText = QT_TR_NOOP("Warp the geometry along a vector field by a certain factor");
	sWhatsThis = "Post_CreateWarpVectorFilter";
	sStatusTip = sToolTipText;
	sPixmap = "warp_vector";
}

void CmdPostWarpVectorFilter::activated(int)
{
	setupFilter(this, "WarpVector");
}

bool CmdPostWarpVectorFilter::isActive(void)
{
	return hasActiveDocument();
}

//================================================================================================
DEF_STD_CMD_A(CmdPostClipFilter);

CmdPostClipFilter::CmdPostClipFilter()
	: Command("Post_CreateClipFilter")
{
	sAppModule = "Cfd";
	sGroup = QT_TR_NOOP("Post");
	sMenuText = QT_TR_NOOP("Region clip");
	sToolTipText = QT_TR_NOOP("Define/create a clip filter which uses functions to define the cliped region");
	sWhatsThis = "Post_CreateClipFilter";
	sStatusTip = sToolTipText;
	sPixmap = "clip_region";
}

void CmdPostClipFilter::activated(int)
{
	setupFilter(this, "Clip");
}

bool CmdPostClipFilter::isActive(void)
{
	return hasActiveDocument();
}

//================================================================================================
DEF_STD_CMD_A(CmdPostScalarClipFilter);

CmdPostScalarClipFilter::CmdPostScalarClipFilter()
	: Command("Post_CreateScalarClipFilter")
{
	sAppModule = "Cfd";
	sGroup = QT_TR_NOOP("Post");
	sMenuText = QT_TR_NOOP("Scalar clip");
	sToolTipText = QT_TR_NOOP("Define/create a clip filter which clips a field with a scalar value");
	sWhatsThis = "Post_CreateScalarClipFilter";
	sStatusTip = sToolTipText;
	sPixmap = "clip_scalar";
}

void CmdPostScalarClipFilter::activated(int)
{
	setupFilter(this, "ScalarClip");
}

bool CmdPostScalarClipFilter::isActive(void)
{
	return hasActiveDocument();
}

//================================================================================================
DEF_STD_CMD_A(CmdPostCutFilter);

CmdPostCutFilter::CmdPostCutFilter()
	: Command("Post_CreateCutFilter")
{
	sAppModule = "Cfd";
	sGroup = QT_TR_NOOP("Post");
	sMenuText = QT_TR_NOOP("Function cut");
	sToolTipText = QT_TR_NOOP("Cut the data along an implicit function");
	sWhatsThis = "FEM_PostCreateCutFilter";
	sStatusTip = sToolTipText;
	sPixmap = "cut_function";
}

void CmdPostCutFilter::activated(int)
{
	setupFilter(this, "Cut");
}

bool CmdPostCutFilter::isActive(void)
{
	return hasActiveDocument();
}

//================================================================================================
DEF_STD_CMD_A(CmdPostContour);

CmdPostContour::CmdPostContour()
	: Command("Post_Contour")
{
	sAppModule = "Cfd";
	sGroup = QT_TR_NOOP("Post");
	sMenuText = QT_TR_NOOP("Contour");
	sToolTipText = QT_TR_NOOP("Generates isosurfaces and/or isolines");
	sWhatsThis = "Post_Contour";
	sStatusTip = sToolTipText;
	sPixmap = "contour";
}

void CmdPostContour::activated(int)
{
	setupFilter(this, "Contour");
}

bool CmdPostContour::isActive(void)
{
	return hasActiveDocument();
}

//================================================================================================
DEF_STD_CMD_A(CmdPostGlyph3D);

CmdPostGlyph3D::CmdPostGlyph3D()
	: Command("Post_Glyph3D")
{
	sAppModule = "Cfd";
	sGroup = QT_TR_NOOP("Post");
	sMenuText = QT_TR_NOOP("Glyph");
	sToolTipText = QT_TR_NOOP("Orient and scale glyph geometry to every input point");
	sWhatsThis = "Post_Glyph3D";
	sStatusTip = sToolTipText;
	sPixmap = "glyph24";
}

void CmdPostGlyph3D::activated(int)
{
	setupFilter(this, "Glyph3D");
}

bool CmdPostGlyph3D::isActive(void)
{
	return hasActiveDocument();
}

//================================================================================================
DEF_STD_CMD_A(CmdPostStreamline);

CmdPostStreamline::CmdPostStreamline()
	: Command("Post_Streamline")
{
	sAppModule = "Cfd";
	sGroup = QT_TR_NOOP("Cfd");
	sMenuText = QT_TR_NOOP("Streamline");
	sToolTipText = QT_TR_NOOP("Creates streamline(s) within a active pipeline");
	sWhatsThis = "Post_Streamline";
	sStatusTip = sToolTipText;
	sPixmap = "stream_tracer24";
}

void CmdPostStreamline::activated(int)
{
	setupFilter(this, "Streamline");
}

bool CmdPostStreamline::isActive(void)
{
	return hasActiveDocument();
}


//================================================================================================
DEF_STD_CMD_A(CmdPostDataAlongLineFilter);

CmdPostDataAlongLineFilter::CmdPostDataAlongLineFilter()
	: Command("Post_CreateDataAlongLineFilter")
{
	sAppModule = "Cfd";
	sGroup = QT_TR_NOOP("Post");
	sMenuText = QT_TR_NOOP("Data along line");
	sToolTipText = QT_TR_NOOP("Define/create a clip filter which clips a field along a line");
	sWhatsThis = "FEM_PostCreateDataAlongLineFilter";
	sStatusTip = sToolTipText;
	sPixmap = "data_along_line";
}

void CmdPostDataAlongLineFilter::activated(int)
{
	setupFilter(this, "DataAlongLine");
}

bool CmdPostDataAlongLineFilter::isActive(void)
{
	return hasActiveDocument();
}

//================================================================================================
DEF_STD_CMD_A(CmdPostDataAtPointFilter);

CmdPostDataAtPointFilter::CmdPostDataAtPointFilter()
	: Command("Post_CreateDataAtPointFilter")
{
	sAppModule = "Fem";
	sGroup = QT_TR_NOOP("Fem");
	sMenuText = QT_TR_NOOP("Data at point");
	sToolTipText = QT_TR_NOOP("Define/create a clip filter which clips a field data at point");
	sWhatsThis = "FEM_PostCreateDataAtPointFilter";
	sStatusTip = sToolTipText;
	sPixmap = "data_at_point";
}

void CmdPostDataAtPointFilter::activated(int)
{

	setupFilter(this, "DataAtPoint");

}

bool CmdPostDataAtPointFilter::isActive(void)
{
	return hasActiveDocument();
}

//================================================================================================

void CreatePostCommands(void)
{
	Gui::CommandManager &rcCmdMgr = Gui::Application::Instance->commandManager();

	// VCR
	rcCmdMgr.addCommand(new CmdVCRFirst());
	rcCmdMgr.addCommand(new CmdVCRBack());
	rcCmdMgr.addCommand(new CmdVCRPlay());
	rcCmdMgr.addCommand(new CmdVCRForward());
	rcCmdMgr.addCommand(new CmdVCRLast());

	// vtk post processing
	rcCmdMgr.addCommand(new CmdPostApplyChanges);
	rcCmdMgr.addCommand(new CmdPostPipelineFromResult());
	rcCmdMgr.addCommand(new CmdPostFunction());
	rcCmdMgr.addCommand(new CmdPostWarpVectorFilter());
	rcCmdMgr.addCommand(new CmdPostCutFilter());
	rcCmdMgr.addCommand(new CmdPostClipFilter());
	rcCmdMgr.addCommand(new CmdPostScalarClipFilter());;
	rcCmdMgr.addCommand(new CmdPostContour());
	rcCmdMgr.addCommand(new CmdPostGlyph3D());
	rcCmdMgr.addCommand(new CmdPostStreamline());
	rcCmdMgr.addCommand(new CmdPostDataAtPointFilter());
	rcCmdMgr.addCommand(new CmdPostDataAlongLineFilter());
}
