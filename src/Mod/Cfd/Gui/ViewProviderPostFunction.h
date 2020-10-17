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


#ifndef CFD_VIEW_PROVIDER_POST_FUNCTION_H
#define CFD_VIEW_PROVIDER_POST_FUNCTION_H

#include <Gui/ViewProviderDocumentObject.h>
#include <Inventor/SbMatrix.h>
#include <QWidget>
#include <boost/signals2.hpp>

#include <Mod/Cfd/App/PostFunction.h>

class SoScale;
class SoSurroundScale;
class SoTransformManip;
class SoComposeMatrix;
class SoMatrixTransform;
class SoDragger;
class SoSphere;
class Ui_PlaneWidget;
class Ui_SphereWidget;

namespace CfdGui
{

class ViewProviderPostFunction;

class CfdGuiExport FunctionWidget : public QWidget {

    Q_OBJECT
public:
    FunctionWidget() {}
    virtual ~FunctionWidget() {}

    virtual void applyPythonCode() = 0;
    virtual void setViewProvider(ViewProviderPostFunction* view);
    void onObjectsChanged(const App::DocumentObject& obj, const App::Property&);

protected:
    ViewProviderPostFunction* getView()  {return m_view;}
    Cfd::PostFunction*        getObject(){return m_object;}

    bool blockObjectUpdates() {return m_block;}
    void setBlockObjectUpdates(bool val) {m_block = val;}

    virtual void onChange(const App::Property& p) = 0;

private:
    bool                                        m_block;
    ViewProviderPostFunction*                   m_view;
    Cfd::PostFunction*							m_object;
    boost::signals2::scoped_connection          m_connection;
};

class CfdGuiExport ViewProviderPostFunctionProvider : public Gui::ViewProviderDocumentObject
{
    PROPERTY_HEADER(CfdGui::ViewProviderPostFunction);

public:
    ViewProviderPostFunctionProvider();
    virtual ~ViewProviderPostFunctionProvider();

    App::PropertyFloat SizeX;
    App::PropertyFloat SizeY;
    App::PropertyFloat SizeZ;

protected:
    virtual std::vector< App::DocumentObject* > claimChildren(void) const;
    virtual std::vector< App::DocumentObject* > claimChildren3D(void) const;
    virtual void onChanged(const App::Property* prop);
    virtual void updateData(const App::Property*);

    void updateSize();
};

class CfdGuiExport ViewProviderPostFunction : public Gui::ViewProviderDocumentObject
{
    PROPERTY_HEADER(CfdGui::ViewProviderPostFunction);

public:
    /// constructor.
    ViewProviderPostFunction();
    ~ViewProviderPostFunction();

    App::PropertyFloat AutoScaleFactorX;
    App::PropertyFloat AutoScaleFactorY;
    App::PropertyFloat AutoScaleFactorZ;

    void attach(App::DocumentObject *pcObject);
    bool doubleClicked(void);
    std::vector<std::string> getDisplayModes() const;

    //creates the widget used in the task dalogs, either for the function itself or for
    //the fiter using it
    virtual FunctionWidget* createControlWidget() {return NULL;}

protected:
    virtual bool setEdit(int ModNum);
    virtual void unsetEdit(int ModNum);
    virtual void onChanged(const App::Property* prop);

    void setAutoScale(bool value) {m_autoscale = value;}
    bool autoScale()              {return m_autoscale;}

    bool isDragging() {return m_isDragging;}

    virtual SoTransformManip*   setupManipulator();
    virtual void                draggerUpdate(SoDragger*) {}
    SoTransformManip*           getManipulator() {return m_manip;}
    SoSeparator*                getGeometryNode() {return m_geometrySeperator;}
    SoScale*                    getScaleNode() {return m_scale;}
    SoTransform*                getTransformNode() {return m_transform;}

private:
    static void dragStartCallback(void * data, SoDragger * d);
    static void dragFinishCallback(void * data, SoDragger * d);
    static void dragMotionCallback(void * data, SoDragger * d);

    SoSeparator*        m_geometrySeperator;
    SoTransformManip*   m_manip;
    SoScale*            m_scale;
    SoTransform*        m_transform;
    bool                m_autoscale, m_isDragging, m_autoRecompute;
};

//###############################################################################################

class CfdGuiExport PlaneWidget : public FunctionWidget {

    Q_OBJECT
public:
    PlaneWidget();
    virtual ~PlaneWidget();

    virtual void applyPythonCode();
    virtual void onChange(const App::Property& p);
    virtual void setViewProvider(ViewProviderPostFunction* view);

private Q_SLOTS:
    void originChanged(double);
    void normalChanged(double);

private:
    Ui_PlaneWidget* ui;
};

class CfdGuiExport ViewProviderPostPlaneFunction : public ViewProviderPostFunction {

    PROPERTY_HEADER(FemGui::ViewProviderFemPostPlaneFunction);

public:
    ViewProviderPostPlaneFunction();
    virtual ~ViewProviderPostPlaneFunction();

    virtual FunctionWidget* createControlWidget();

protected:
    virtual void draggerUpdate(SoDragger* mat);
    virtual void updateData(const App::Property*);
};

//###############################################################################################

class CfdGuiExport SphereWidget : public FunctionWidget {

    Q_OBJECT
public:
    SphereWidget();
    virtual ~SphereWidget();

    virtual void applyPythonCode();
    virtual void onChange(const App::Property& p);
    virtual void setViewProvider(ViewProviderPostFunction* view);

private Q_SLOTS:
    void centerChanged(double);
    void radiusChanged(double);

private:
    Ui_SphereWidget* ui;
};

class CfdGuiExport ViewProviderPostSphereFunction : public ViewProviderPostFunction {

    PROPERTY_HEADER(CfdGui::ViewProviderPostSphereFunction);

public:
    ViewProviderPostSphereFunction();
    virtual ~ViewProviderPostSphereFunction();

    virtual SoTransformManip* setupManipulator();
    virtual FunctionWidget* createControlWidget();

protected:
    virtual void draggerUpdate(SoDragger* mat);
    virtual void updateData(const App::Property*);
};

} //namespace CfdGui


#endif // CFD_VIEW_PROVIDER_POST_FUNCTION_H
