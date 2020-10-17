
#include "PreCompiled.h"
#ifndef _PreComp_
# include <Standard_math.hxx>
# include <QApplication>
# include <QMessageBox>
# include <QAction>
#endif

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

//================================================================================================
 //================================================================================================
 // commands Mesh

 //================================================================================================
DEF_STD_CMD_A(CmdMeshImport);

CmdMeshImport::CmdMeshImport()
	:Command("Mesh_ImportMesh")
{
	sAppModule = "Cfd";
	sGroup = QT_TR_NOOP("Mesh");
	sMenuText = QT_TR_NOOP("Import mesh...");
	sToolTipText = QT_TR_NOOP("Imports a mesh from file");
	sWhatsThis = "Mesh_Import";
	sStatusTip = QT_TR_NOOP("Imports a mesh from file");
	sPixmap = "import_mesh";
}

void CmdMeshImport::activated(int)
{
	// use current path as default
	QStringList filter;
	filter << QString::fromLatin1("%1 (*.stl *.ast *.bms *.obj *.off *.ply)").arg(QObject::tr("All Mesh Files"));
	filter << QString::fromLatin1("%1 (*.stl)").arg(QObject::tr("Binary STL"));
	filter << QString::fromLatin1("%1 (*.ast)").arg(QObject::tr("ASCII STL"));
	filter << QString::fromLatin1("%1 (*.bms)").arg(QObject::tr("Binary Mesh"));
	filter << QString::fromLatin1("%1 (*.obj)").arg(QObject::tr("Alias Mesh"));
	filter << QString::fromLatin1("%1 (*.off)").arg(QObject::tr("Object File Format"));
	filter << QString::fromLatin1("%1 (*.iv)").arg(QObject::tr("Inventor V2.1 ascii"));
	filter << QString::fromLatin1("%1 (*.ply)").arg(QObject::tr("Stanford Polygon"));
	//filter << "Nastran (*.nas *.bdf)";
	filter << QString::fromLatin1("%1 (*.*)").arg(QObject::tr("All Files"));

	// Allow multi selection
	QStringList fn = Gui::FileDialog::getOpenFileNames(Gui::getMainWindow(),
		QObject::tr("Import mesh"), QString(), filter.join(QLatin1String(";;")));
	for (QStringList::Iterator it = fn.begin(); it != fn.end(); ++it) {
		QFileInfo fi;
		fi.setFile(*it);

		std::string unicodepath = Base::Tools::escapedUnicodeFromUtf8((*it).toUtf8().data());
		openCommand("Import Mesh");
		doCommand(Doc, "import Mesh");
		doCommand(Doc, "Mesh.insert(u\"%s\")", unicodepath.c_str());
		commitCommand();
		updateActive();
	}
}

bool CmdMeshImport::isActive(void)
{
	return (getActiveGuiDocument() ? true : false);
}


//================================================================================================

void CreateMeshCommands(void)
{
	Gui::CommandManager &rcCmdMgr = Gui::Application::Instance->commandManager();

	// mesh
	rcCmdMgr.addCommand(new CmdMeshImport());
}