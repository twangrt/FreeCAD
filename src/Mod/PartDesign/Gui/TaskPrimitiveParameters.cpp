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
# include <sstream>
# include <QRegExp>
# include <QTextStream>
# include <QMessageBox>
# include <Precision.hxx>
#endif

#include "TaskPrimitiveParameters.h"
#include "ViewProviderDatumCS.h"
#include <Mod/PartDesign/App/FeaturePrimitive.h>
#include <Mod/PartDesign/App/DatumCS.h>
#include <Mod/Part/App/DatumFeature.h>
#include <Gui/Application.h>
#include <Gui/Document.h>
#include <Gui/Command.h>
#include <Gui/BitmapFactory.h>

using namespace PartDesignGui;

TaskBoxPrimitives::TaskBoxPrimitives(ViewProviderPrimitive* vp, QWidget* parent)
  : vp(vp), TaskBox(QPixmap(),tr("Primitive parameters"), true, parent)
{
    proxy = new QWidget(this);
    ui.setupUi(proxy);
    
    // box
    ui.boxLength->setMaximum(INT_MAX);    
    ui.boxWidth->setMaximum(INT_MAX);
    ui.boxHeight->setMaximum(INT_MAX);
    connect(ui.boxLength, SIGNAL(valueChanged(double)), this, SLOT(onBoxLengthChanged(double)));
    connect(ui.boxWidth, SIGNAL(valueChanged(double)), this, SLOT(onBoxWidthChanged(double)));
    connect(ui.boxHeight, SIGNAL(valueChanged(double)), this, SLOT(onBoxHeightChanged(double)));

    // cylinder
    ui.cylinderRadius->setMaximum(INT_MAX);
    ui.cylinderHeight->setMaximum(INT_MAX);
    connect(ui.cylinderRadius, SIGNAL(valueChanged(double)), this, SLOT(onCylinderRadiusChanged(double)));
    connect(ui.cylinderHeight, SIGNAL(valueChanged(double)), this, SLOT(onCylinderHeightChanged(double)));
    connect(ui.cylinderAngle, SIGNAL(valueChanged(double)), this, SLOT(onCylinderAngleChanged(double)));

    // cone
    ui.coneRadius1->setMaximum(INT_MAX);
    ui.coneRadius2->setMaximum(INT_MAX);
    ui.coneHeight->setMaximum(INT_MAX);
    connect(ui.coneRadius1, SIGNAL(valueChanged(double)), this, SLOT(onConeRadius1Changed(double)));
    connect(ui.coneRadius2, SIGNAL(valueChanged(double)), this, SLOT(onConeRadius2Changed(double)));
    connect(ui.coneAngle, SIGNAL(valueChanged(double)), this, SLOT(onConeAngleChanged(double)));
    connect(ui.coneHeight, SIGNAL(valueChanged(double)), this, SLOT(onConeHeightChanged(double)));
    
    // sphere
    ui.sphereRadius->setMaximum(INT_MAX);
    connect(ui.sphereRadius, SIGNAL(valueChanged(double)), this, SLOT(onSphereRadiusChanged(double)));
    connect(ui.sphereAngle1, SIGNAL(valueChanged(double)), this, SLOT(onSphereAngle1Changed(double)));
    connect(ui.sphereAngle2, SIGNAL(valueChanged(double)), this, SLOT(onSphereAngle2Changed(double)));
    connect(ui.sphereAngle3, SIGNAL(valueChanged(double)), this, SLOT(onSphereAngle3Changed(double)));
    
    // ellipsoid
    ui.ellipsoidRadius1->setMaximum(INT_MAX);
    ui.ellipsoidRadius2->setMaximum(INT_MAX);
    ui.ellipsoidRadius3->setMaximum(INT_MAX);
    connect(ui.ellipsoidRadius1, SIGNAL(valueChanged(double)), this, SLOT(onEllipsoidRadius1Changed(double)));
    connect(ui.ellipsoidRadius2, SIGNAL(valueChanged(double)), this, SLOT(onEllipsoidRadius2Changed(double)));
    connect(ui.ellipsoidRadius3, SIGNAL(valueChanged(double)), this, SLOT(onEllipsoidRadius3Changed(double)));
    connect(ui.ellipsoidAngle1, SIGNAL(valueChanged(double)), this, SLOT(onEllipsoidAngle1Changed(double)));
    connect(ui.ellipsoidAngle2, SIGNAL(valueChanged(double)), this, SLOT(onEllipsoidAngle2Changed(double)));
    connect(ui.ellipsoidAngle3, SIGNAL(valueChanged(double)), this, SLOT(onEllipsoidAngle3Changed(double)));
    
    // torus
    ui.torusRadius1->setMaximum(INT_MAX);
    ui.torusRadius2->setMaximum(INT_MAX);
    connect(ui.torusRadius1, SIGNAL(valueChanged(double)), this, SLOT(onTorusRadius1Changed(double)));
    connect(ui.torusRadius2, SIGNAL(valueChanged(double)), this, SLOT(onTorusRadius2Changed(double)));
    connect(ui.torusAngle1, SIGNAL(valueChanged(double)), this, SLOT(onTorusAngle1Changed(double)));
    connect(ui.torusAngle2, SIGNAL(valueChanged(double)), this, SLOT(onTorusAngle2Changed(double)));
    connect(ui.torusAngle3, SIGNAL(valueChanged(double)), this, SLOT(onTorusAngle3Changed(double)));
    
    //prism
    connect(ui.prismCircumradius, SIGNAL(valueChanged(double)), this, SLOT(onPrismCircumradiusChanged(double)));
    connect(ui.prismHeight, SIGNAL(valueChanged(double)), this, SLOT(onPrismHeightChanged(double)));
    connect(ui.prismPolygon, SIGNAL(valueChanged(int)), this, SLOT(onPrismPolygonChanged(int)));
  
    // wedge
    ui.wedgeXmin->setMinimum(INT_MIN);
    ui.wedgeXmin->setMaximum(INT_MAX);
    ui.wedgeYmin->setMinimum(INT_MIN);
    ui.wedgeYmin->setMaximum(INT_MAX);
    ui.wedgeZmin->setMinimum(INT_MIN);
    ui.wedgeZmin->setMaximum(INT_MAX);
    ui.wedgeX2min->setMinimum(INT_MIN);
    ui.wedgeX2min->setMaximum(INT_MAX);
    ui.wedgeZ2min->setMinimum(INT_MIN);
    ui.wedgeZ2min->setMaximum(INT_MAX);
    ui.wedgeXmax->setMinimum(INT_MIN);
    ui.wedgeXmax->setMaximum(INT_MAX);
    ui.wedgeYmax->setMinimum(INT_MIN);
    ui.wedgeYmax->setMaximum(INT_MAX);
    ui.wedgeZmax->setMinimum(INT_MIN);
    ui.wedgeZmax->setMaximum(INT_MAX);
    ui.wedgeX2max->setMinimum(INT_MIN);
    ui.wedgeX2max->setMaximum(INT_MAX);
    ui.wedgeZ2max->setMinimum(INT_MIN);
    ui.wedgeZ2max->setMaximum(INT_MAX);
    connect(ui.wedgeXmax, SIGNAL(valueChanged(double)), this, SLOT(onWedgeXmaxChanged(double)));
    connect(ui.wedgeXmin, SIGNAL(valueChanged(double)), this, SLOT(onWedgeXinChanged(double)));
    connect(ui.wedgeYmax, SIGNAL(valueChanged(double)), this, SLOT(onWedgeYmaxChanged(double)));
    connect(ui.wedgeYmin, SIGNAL(valueChanged(double)), this, SLOT(onWedgeYinChanged(double)));
    connect(ui.wedgeZmax, SIGNAL(valueChanged(double)), this, SLOT(onWedgeZmaxChanged(double)));
    connect(ui.wedgeZmin, SIGNAL(valueChanged(double)), this, SLOT(onWedgeZinChanged(double)));
    connect(ui.wedgeX2max, SIGNAL(valueChanged(double)), this, SLOT(onWedgeX2maxChanged(double)));
    connect(ui.wedgeX2min, SIGNAL(valueChanged(double)), this, SLOT(onWedgeX2inChanged(double)));
    connect(ui.wedgeZ2max, SIGNAL(valueChanged(double)), this, SLOT(onWedgeZ2maxChanged(double)));
    connect(ui.wedgeZ2min, SIGNAL(valueChanged(double)), this, SLOT(onWedgeZ2inChanged(double)));
    
    this->groupLayout()->addWidget(proxy);
    
    int index = 0;
    switch(static_cast<PartDesign::FeaturePrimitive*>(vp->getObject())->getPrimitiveType()) {
        
        case PartDesign::FeaturePrimitive::Box:
            index = 1;
            ui.boxLength->setValue(static_cast<PartDesign::Box*>(vp->getObject())->Length.getValue());
            ui.boxHeight->setValue(static_cast<PartDesign::Box*>(vp->getObject())->Height.getValue());
            ui.boxWidth->setValue(static_cast<PartDesign::Box*>(vp->getObject())->Width.getValue());
            break;
        case PartDesign::FeaturePrimitive::Cylinder:
            index = 2;
            ui.cylinderAngle->setValue(static_cast<PartDesign::Cylinder*>(vp->getObject())->Angle.getValue());
            ui.cylinderHeight->setValue(static_cast<PartDesign::Cylinder*>(vp->getObject())->Height.getValue());
            ui.cylinderRadius->setValue(static_cast<PartDesign::Cylinder*>(vp->getObject())->Radius.getValue());
            break;
        case PartDesign::FeaturePrimitive::Sphere:
            index = 4;
            ui.sphereAngle1->setValue(static_cast<PartDesign::Sphere*>(vp->getObject())->Angle1.getValue());
            ui.sphereAngle2->setValue(static_cast<PartDesign::Sphere*>(vp->getObject())->Angle2.getValue());
            ui.sphereAngle3->setValue(static_cast<PartDesign::Sphere*>(vp->getObject())->Angle3.getValue());
            ui.sphereRadius->setValue(static_cast<PartDesign::Sphere*>(vp->getObject())->Radius.getValue());
            break;
        case PartDesign::FeaturePrimitive::Cone:
            index = 3;
            ui.coneAngle->setValue(static_cast<PartDesign::Cone*>(vp->getObject())->Angle.getValue());
            ui.coneHeight->setValue(static_cast<PartDesign::Cone*>(vp->getObject())->Height.getValue());
            ui.coneRadius1->setValue(static_cast<PartDesign::Cone*>(vp->getObject())->Radius1.getValue());
            ui.coneRadius2->setValue(static_cast<PartDesign::Cone*>(vp->getObject())->Radius2.getValue());
            break;
        case PartDesign::FeaturePrimitive::Ellipsoid:
            index = 5;
            ui.ellipsoidAngle1->setValue(static_cast<PartDesign::Ellipsoid*>(vp->getObject())->Angle1.getValue());
            ui.ellipsoidAngle2->setValue(static_cast<PartDesign::Ellipsoid*>(vp->getObject())->Angle2.getValue());
            ui.ellipsoidAngle3->setValue(static_cast<PartDesign::Ellipsoid*>(vp->getObject())->Angle3.getValue());
            ui.ellipsoidRadius1->setValue(static_cast<PartDesign::Ellipsoid*>(vp->getObject())->Radius1.getValue());
            ui.ellipsoidRadius2->setValue(static_cast<PartDesign::Ellipsoid*>(vp->getObject())->Radius2.getValue());
            ui.ellipsoidRadius3->setValue(static_cast<PartDesign::Ellipsoid*>(vp->getObject())->Radius3.getValue());
            break;
        case PartDesign::FeaturePrimitive::Torus:
            index = 6;
            ui.torusAngle1->setValue(static_cast<PartDesign::Torus*>(vp->getObject())->Angle1.getValue());
            ui.torusAngle2->setValue(static_cast<PartDesign::Torus*>(vp->getObject())->Angle2.getValue());
            ui.torusAngle3->setValue(static_cast<PartDesign::Torus*>(vp->getObject())->Angle3.getValue());
            ui.torusRadius1->setValue(static_cast<PartDesign::Torus*>(vp->getObject())->Radius1.getValue());
            ui.torusRadius2->setValue(static_cast<PartDesign::Torus*>(vp->getObject())->Radius2.getValue());
            break;
        case PartDesign::FeaturePrimitive::Prism:
            index = 7;
            ui.prismPolygon->setValue(static_cast<PartDesign::Prism*>(vp->getObject())->Polygon.getValue());
            ui.prismCircumradius->setValue(static_cast<PartDesign::Prism*>(vp->getObject())->Circumradius.getValue());
            ui.prismHeight->setValue(static_cast<PartDesign::Prism*>(vp->getObject())->Height.getValue());
            break;
        case PartDesign::FeaturePrimitive::Wedge:
            index = 8;
            ui.wedgeXmax->setValue(static_cast<PartDesign::Wedge*>(vp->getObject())->Xmax.getValue());
            ui.wedgeXmin->setValue(static_cast<PartDesign::Wedge*>(vp->getObject())->Xmin.getValue());
            ui.wedgeX2max->setValue(static_cast<PartDesign::Wedge*>(vp->getObject())->X2max.getValue());
            ui.wedgeX2min->setValue(static_cast<PartDesign::Wedge*>(vp->getObject())->X2min.getValue());
            ui.wedgeYmax->setValue(static_cast<PartDesign::Wedge*>(vp->getObject())->Ymax.getValue());
            ui.wedgeYmin->setValue(static_cast<PartDesign::Wedge*>(vp->getObject())->Ymin.getValue());
            ui.wedgeZmax->setValue(static_cast<PartDesign::Wedge*>(vp->getObject())->Zmax.getValue());
            ui.wedgeZmin->setValue(static_cast<PartDesign::Wedge*>(vp->getObject())->Zmin.getValue());
            ui.wedgeZ2max->setValue(static_cast<PartDesign::Wedge*>(vp->getObject())->Z2max.getValue());
            ui.wedgeZ2min->setValue(static_cast<PartDesign::Wedge*>(vp->getObject())->Z2min.getValue());
            break;       
    }
    
    ui.widgetStack->setCurrentIndex(index);
    ui.widgetStack->setMinimumSize(ui.widgetStack->widget(index)->minimumSize());
    for(int i=0; i<ui.widgetStack->count(); ++i) {
    
        if(i != index)
            ui.widgetStack->widget(i)->setSizePolicy(QSizePolicy(QSizePolicy::Ignored, QSizePolicy::Ignored));
    }
}

/*  
 *  Destroys the object and frees any allocated resources
 */
TaskBoxPrimitives::~TaskBoxPrimitives()
{
}

void TaskBoxPrimitives::onBoxHeightChanged(double v) {
    PartDesign::Box* box = static_cast<PartDesign::Box*>(vp->getObject());
    box->Height.setValue(v);
    vp->getObject()->getDocument()->recomputeFeature(vp->getObject());
}

void TaskBoxPrimitives::onBoxWidthChanged(double v) {
    PartDesign::Box* box = static_cast<PartDesign::Box*>(vp->getObject());
    box->Width.setValue(v);
    vp->getObject()->getDocument()->recomputeFeature(vp->getObject());
}

void TaskBoxPrimitives::onBoxLengthChanged(double v) {
    PartDesign::Box* box = static_cast<PartDesign::Box*>(vp->getObject());
    box->Length.setValue(v);
    vp->getObject()->getDocument()->recomputeFeature(vp->getObject());
}

void TaskBoxPrimitives::onCylinderAngleChanged(double v) {
    PartDesign::Cylinder* cyl = static_cast<PartDesign::Cylinder*>(vp->getObject());
    cyl->Angle.setValue(v);
    vp->getObject()->getDocument()->recomputeFeature(vp->getObject());
}

void TaskBoxPrimitives::onCylinderHeightChanged(double v) {
    PartDesign::Cylinder* cyl = static_cast<PartDesign::Cylinder*>(vp->getObject());
    cyl->Height.setValue(v);
    vp->getObject()->getDocument()->recomputeFeature(vp->getObject());
}

void TaskBoxPrimitives::onCylinderRadiusChanged(double v) {
    PartDesign::Cylinder* cyl = static_cast<PartDesign::Cylinder*>(vp->getObject());
    cyl->Radius.setValue(v);
    vp->getObject()->getDocument()->recomputeFeature(vp->getObject());
}

void TaskBoxPrimitives::onSphereAngle1Changed(double v) {
    PartDesign::Sphere* sph = static_cast<PartDesign::Sphere*>(vp->getObject());
    sph->Angle1.setValue(v);
    vp->getObject()->getDocument()->recomputeFeature(vp->getObject());
}

void TaskBoxPrimitives::onSphereAngle2Changed(double v) {
    PartDesign::Sphere* sph = static_cast<PartDesign::Sphere*>(vp->getObject());
    sph->Angle2.setValue(v);
    vp->getObject()->getDocument()->recomputeFeature(vp->getObject());
}

void TaskBoxPrimitives::onSphereAngle3Changed(double v) {
    PartDesign::Sphere* sph = static_cast<PartDesign::Sphere*>(vp->getObject());
    sph->Angle3.setValue(v);
    vp->getObject()->getDocument()->recomputeFeature(vp->getObject());
}

void TaskBoxPrimitives::onSphereRadiusChanged(double  v) {
    PartDesign::Sphere* sph = static_cast<PartDesign::Sphere*>(vp->getObject());
    sph->Radius.setValue(v);
    vp->getObject()->getDocument()->recomputeFeature(vp->getObject());
}

void TaskBoxPrimitives::onConeAngleChanged(double v) {

    PartDesign::Cone* sph = static_cast<PartDesign::Cone*>(vp->getObject());
    sph->Angle.setValue(v);
    vp->getObject()->getDocument()->recomputeFeature(vp->getObject());
}

void TaskBoxPrimitives::onConeHeightChanged(double v) {
    PartDesign::Cone* sph = static_cast<PartDesign::Cone*>(vp->getObject());
    sph->Height.setValue(v);
    vp->getObject()->getDocument()->recomputeFeature(vp->getObject());
}

void TaskBoxPrimitives::onConeRadius1Changed(double v) {
    PartDesign::Cone* sph = static_cast<PartDesign::Cone*>(vp->getObject());
    sph->Radius1.setValue(v);
    vp->getObject()->getDocument()->recomputeFeature(vp->getObject());
}

void TaskBoxPrimitives::onConeRadius2Changed(double v) {
    PartDesign::Cone* sph = static_cast<PartDesign::Cone*>(vp->getObject());
    sph->Radius2.setValue(v);
    vp->getObject()->getDocument()->recomputeFeature(vp->getObject());
}

void TaskBoxPrimitives::onEllipsoidAngle1Changed(double v) {
    PartDesign::Ellipsoid* sph = static_cast<PartDesign::Ellipsoid*>(vp->getObject());
    sph->Angle1.setValue(v);
    vp->getObject()->getDocument()->recomputeFeature(vp->getObject());
}

void TaskBoxPrimitives::onEllipsoidAngle2Changed(double v) {
    PartDesign::Ellipsoid* sph = static_cast<PartDesign::Ellipsoid*>(vp->getObject());
    sph->Angle2.setValue(v);
    vp->getObject()->getDocument()->recomputeFeature(vp->getObject());
}

void TaskBoxPrimitives::onEllipsoidAngle3Changed(double v) {
    PartDesign::Ellipsoid* sph = static_cast<PartDesign::Ellipsoid*>(vp->getObject());
    sph->Angle3.setValue(v);
    vp->getObject()->getDocument()->recomputeFeature(vp->getObject());
}

void TaskBoxPrimitives::onEllipsoidRadius1Changed(double v) {
    PartDesign::Ellipsoid* sph = static_cast<PartDesign::Ellipsoid*>(vp->getObject());
    sph->Radius1.setValue(v);
    vp->getObject()->getDocument()->recomputeFeature(vp->getObject());
}

void TaskBoxPrimitives::onEllipsoidRadius2Changed(double v) {
    PartDesign::Ellipsoid* sph = static_cast<PartDesign::Ellipsoid*>(vp->getObject());
    sph->Radius2.setValue(v);
    vp->getObject()->getDocument()->recomputeFeature(vp->getObject());
}

void TaskBoxPrimitives::onEllipsoidRadius3Changed(double v) {
    PartDesign::Ellipsoid* sph = static_cast<PartDesign::Ellipsoid*>(vp->getObject());
    sph->Radius3.setValue(v);
    vp->getObject()->getDocument()->recomputeFeature(vp->getObject());
}

void TaskBoxPrimitives::onTorusAngle1Changed(double v) {
    PartDesign::Torus* sph = static_cast<PartDesign::Torus*>(vp->getObject());
    sph->Angle1.setValue(v);
    vp->getObject()->getDocument()->recomputeFeature(vp->getObject());
}

void TaskBoxPrimitives::onTorusAngle2Changed(double v) {
    PartDesign::Torus* sph = static_cast<PartDesign::Torus*>(vp->getObject());
    sph->Angle2.setValue(v);
    vp->getObject()->getDocument()->recomputeFeature(vp->getObject());
}

void TaskBoxPrimitives::onTorusAngle3Changed(double v) {
    PartDesign::Torus* sph = static_cast<PartDesign::Torus*>(vp->getObject());
    sph->Angle3.setValue(v);
    vp->getObject()->getDocument()->recomputeFeature(vp->getObject());
}

void TaskBoxPrimitives::onTorusRadius1Changed(double v) {
    PartDesign::Torus* sph = static_cast<PartDesign::Torus*>(vp->getObject());
    sph->Radius1.setValue(v);
    vp->getObject()->getDocument()->recomputeFeature(vp->getObject());
}

void TaskBoxPrimitives::onTorusRadius2Changed(double v) {
    PartDesign::Torus* sph = static_cast<PartDesign::Torus*>(vp->getObject());
    sph->Radius2.setValue(v);
    vp->getObject()->getDocument()->recomputeFeature(vp->getObject());
}

void TaskBoxPrimitives::onPrismCircumradiusChanged(double v) {
    PartDesign::Prism* sph = static_cast<PartDesign::Prism*>(vp->getObject());
    sph->Circumradius.setValue(v);
    vp->getObject()->getDocument()->recomputeFeature(vp->getObject());
}

void TaskBoxPrimitives::onPrismHeightChanged(double v) {
    PartDesign::Prism* sph = static_cast<PartDesign::Prism*>(vp->getObject());
    sph->Height.setValue(v);
    vp->getObject()->getDocument()->recomputeFeature(vp->getObject());
}

void TaskBoxPrimitives::onPrismPolygonChanged(int v) {
    PartDesign::Prism* sph = static_cast<PartDesign::Prism*>(vp->getObject());
    sph->Polygon.setValue(v);
    vp->getObject()->getDocument()->recomputeFeature(vp->getObject());
}


void TaskBoxPrimitives::onWedgeX2inChanged(double v) {
    PartDesign::Wedge* sph = static_cast<PartDesign::Wedge*>(vp->getObject());
    sph->X2min.setValue(v);
    vp->getObject()->getDocument()->recomputeFeature(vp->getObject());
}

void TaskBoxPrimitives::onWedgeX2maxChanged(double v) {
    PartDesign::Wedge* sph = static_cast<PartDesign::Wedge*>(vp->getObject());
    sph->X2max.setValue(v);
    vp->getObject()->getDocument()->recomputeFeature(vp->getObject());
}

void TaskBoxPrimitives::onWedgeXinChanged(double v) {
    PartDesign::Wedge* sph = static_cast<PartDesign::Wedge*>(vp->getObject());
    sph->Xmin.setValue(v);
    vp->getObject()->getDocument()->recomputeFeature(vp->getObject());
}

void TaskBoxPrimitives::onWedgeXmaxChanged(double v) {
    PartDesign::Wedge* sph = static_cast<PartDesign::Wedge*>(vp->getObject());
    sph->Xmax.setValue(v);
    vp->getObject()->getDocument()->recomputeFeature(vp->getObject());
}

void TaskBoxPrimitives::onWedgeYinChanged(double v) {
    PartDesign::Wedge* sph = static_cast<PartDesign::Wedge*>(vp->getObject());
    sph->Ymin.setValue(v);
    vp->getObject()->getDocument()->recomputeFeature(vp->getObject());
}

void TaskBoxPrimitives::onWedgeYmaxChanged(double v) {
    PartDesign::Wedge* sph = static_cast<PartDesign::Wedge*>(vp->getObject());
    sph->Ymax.setValue(v);
    vp->getObject()->getDocument()->recomputeFeature(vp->getObject());
}

void TaskBoxPrimitives::onWedgeZ2inChanged(double v) {
    PartDesign::Wedge* sph = static_cast<PartDesign::Wedge*>(vp->getObject());
    sph->Z2min.setValue(v);
    vp->getObject()->getDocument()->recomputeFeature(vp->getObject());
}

void TaskBoxPrimitives::onWedgeZ2maxChanged(double v) {
    PartDesign::Wedge* sph = static_cast<PartDesign::Wedge*>(vp->getObject());
    sph->Z2max.setValue(v);
    vp->getObject()->getDocument()->recomputeFeature(vp->getObject());
}

void TaskBoxPrimitives::onWedgeZinChanged(double v) {
    PartDesign::Wedge* sph = static_cast<PartDesign::Wedge*>(vp->getObject());
    sph->Zmin.setValue(v);
    vp->getObject()->getDocument()->recomputeFeature(vp->getObject());
}

void TaskBoxPrimitives::onWedgeZmaxChanged(double v) {
    PartDesign::Wedge* sph = static_cast<PartDesign::Wedge*>(vp->getObject());
    sph->Zmax.setValue(v);
    vp->getObject()->getDocument()->recomputeFeature(vp->getObject());
}



/*
void  TaskBoxPrimitives::createPrimitive(const QString& placement)
{
    try {
        QString cmd; QString name;
        App::Document* doc = App::GetApplication().getActiveDocument();
        if (!doc) {
            QMessageBox::warning(this, tr("Create %1")
                .arg(ui.comboBox1->currentText()), tr("No active document"));
            return;
        }
        if (ui.comboBox1->currentIndex() == 0) {         // plane
            name = QString::fromAscii(doc->getUniqueObjectName("Plane").c_str());
            cmd = QString::fromAscii(
                "App.ActiveDocument.addObject(\"Part::Plane\",\"%1\")\n"
                "App.ActiveDocument.%1.Length=%2\n"
                "App.ActiveDocument.%1.Width=%3\n"
                "App.ActiveDocument.%1.Placement=%4\n"
                "App.ActiveDocument.%1.Label='%5'\n")
                .arg(name)
                .arg(ui.planeLength->value().getValue(),0,'f',2)
                .arg(ui.planeWidth->value().getValue(),0,'f',2)
                .arg(placement)
                .arg(tr("Plane"));
        }
        else if (ui.comboBox1->currentIndex() == 1) {         // box
            name = QString::fromAscii(doc->getUniqueObjectName("Box").c_str());
            cmd = QString::fromAscii(
                "App.ActiveDocument.addObject(\"Part::Box\",\"%1\")\n"
                "App.ActiveDocument.%1.Length=%2\n"
                "App.ActiveDocument.%1.Width=%3\n"
                "App.ActiveDocument.%1.Height=%4\n"
                "App.ActiveDocument.%1.Placement=%5\n"
                "App.ActiveDocument.%1.Label='%6'\n")
                .arg(name)
                .arg(ui.boxLength->value().getValue(),0,'f',2)
                .arg(ui.boxWidth->value().getValue(),0,'f',2)
                .arg(ui.boxHeight->value().getValue(),0,'f',2)
                .arg(placement)
                .arg(tr("Box"));
        }
        else if (ui.comboBox1->currentIndex() == 2) {  // cylinder
            name = QString::fromAscii(doc->getUniqueObjectName("Cylinder").c_str());
            cmd = QString::fromAscii(
                "App.ActiveDocument.addObject(\"Part::Cylinder\",\"%1\")\n"
                "App.ActiveDocument.%1.Radius=%2\n"
                "App.ActiveDocument.%1.Height=%3\n"
                "App.ActiveDocument.%1.Angle=%4\n"
                "App.ActiveDocument.%1.Placement=%5\n"
                "App.ActiveDocument.%1.Label='%6'\n")
                .arg(name)
                .arg(ui.cylinderRadius->value().getValue(),0,'f',2)
                .arg(ui.cylinderHeight->value().getValue(),0,'f',2)
                .arg(ui.cylinderAngle->value().getValue(),0,'f',2)
                .arg(placement)
                .arg(tr("Cylinder"));
        }
        else if (ui.comboBox1->currentIndex() == 3) {  // cone
            name = QString::fromAscii(doc->getUniqueObjectName("Cone").c_str());
            cmd = QString::fromAscii(
                "App.ActiveDocument.addObject(\"Part::Cone\",\"%1\")\n"
                "App.ActiveDocument.%1.Radius1=%2\n"
                "App.ActiveDocument.%1.Radius2=%3\n"
                "App.ActiveDocument.%1.Height=%4\n"
                "App.ActiveDocument.%1.Angle=%5\n"
                "App.ActiveDocument.%1.Placement=%6\n"
                "App.ActiveDocument.%1.Label='%7'\n")
                .arg(name)
                .arg(ui.coneRadius1->value().getValue(),0,'f',2)
                .arg(ui.coneRadius2->value().getValue(),0,'f',2)
                .arg(ui.coneHeight->value().getValue(),0,'f',2)
                .arg(ui.coneAngle->value().getValue(),0,'f',2)
                .arg(placement)
                .arg(tr("Cone"));
        }
        else if (ui.comboBox1->currentIndex() == 4) {  // sphere
            name = QString::fromAscii(doc->getUniqueObjectName("Sphere").c_str());
            cmd = QString::fromAscii(
                "App.ActiveDocument.addObject(\"Part::Sphere\",\"%1\")\n"
                "App.ActiveDocument.%1.Radius=%2\n"
                "App.ActiveDocument.%1.Angle1=%3\n"
                "App.ActiveDocument.%1.Angle2=%4\n"
                "App.ActiveDocument.%1.Angle3=%5\n"
                "App.ActiveDocument.%1.Placement=%6\n"
                "App.ActiveDocument.%1.Label='%7'\n")
                .arg(name)
                .arg(ui.sphereRadius->value().getValue(),0,'f',2)
                .arg(ui.sphereAngle1->value().getValue(),0,'f',2)
                .arg(ui.sphereAngle2->value().getValue(),0,'f',2)
                .arg(ui.sphereAngle3->value().getValue(),0,'f',2)
                .arg(placement)
                .arg(tr("Sphere"));
        }
        else if (ui.comboBox1->currentIndex() == 5) {  // ellipsoid
            name = QString::fromAscii(doc->getUniqueObjectName("Ellipsoid").c_str());
            cmd = QString::fromAscii(
                "App.ActiveDocument.addObject(\"Part::Ellipsoid\",\"%1\")\n"
                "App.ActiveDocument.%1.Radius1=%2\n"
                "App.ActiveDocument.%1.Radius2=%3\n"
                "App.ActiveDocument.%1.Radius3=%4\n"
                "App.ActiveDocument.%1.Angle1=%5\n"
                "App.ActiveDocument.%1.Angle2=%6\n"
                "App.ActiveDocument.%1.Angle3=%7\n"
                "App.ActiveDocument.%1.Placement=%8\n"
                "App.ActiveDocument.%1.Label='%9'\n")
                .arg(name)
                .arg(ui.ellipsoidRadius1->value().getValue(),0,'f',2)
                .arg(ui.ellipsoidRadius2->value().getValue(),0,'f',2)
                .arg(ui.ellipsoidRadius3->value().getValue(),0,'f',2)
                .arg(ui.ellipsoidAngle1->value().getValue(),0,'f',2)
                .arg(ui.ellipsoidAngle2->value().getValue(),0,'f',2)
                .arg(ui.ellipsoidAngle3->value().getValue(),0,'f',2)
                .arg(placement)
                .arg(tr("Ellipsoid"));
        }
        else if (ui.comboBox1->currentIndex() == 6) {  // torus
            name = QString::fromAscii(doc->getUniqueObjectName("Torus").c_str());
            cmd = QString::fromAscii(
                "App.ActiveDocument.addObject(\"Part::Torus\",\"%1\")\n"
                "App.ActiveDocument.%1.Radius1=%2\n"
                "App.ActiveDocument.%1.Radius2=%3\n"
                "App.ActiveDocument.%1.Angle1=%4\n"
                "App.ActiveDocument.%1.Angle2=%5\n"
                "App.ActiveDocument.%1.Angle3=%6\n"
                "App.ActiveDocument.%1.Placement=%7\n"
                "App.ActiveDocument.%1.Label='%8'\n")
                .arg(name)
                .arg(ui.torusRadius1->value().getValue(),0,'f',2)
                .arg(ui.torusRadius2->value().getValue(),0,'f',2)
                .arg(ui.torusAngle1->value().getValue(),0,'f',2)
                .arg(ui.torusAngle2->value().getValue(),0,'f',2)
                .arg(ui.torusAngle3->value().getValue(),0,'f',2)
                .arg(placement)
                .arg(tr("Torus"));
        }
        else if (ui.comboBox1->currentIndex() == 7) {  // prism
            name = QString::fromAscii(doc->getUniqueObjectName("Prism").c_str());
            cmd = QString::fromAscii(
                "App.ActiveDocument.addObject(\"Part::Prism\",\"%1\")\n"
                "App.ActiveDocument.%1.Polygon=%2\n"
                "App.ActiveDocument.%1.Circumradius=%3\n"
                "App.ActiveDocument.%1.Height=%4\n"
                "App.ActiveDocument.%1.Placement=%5\n"
                "App.ActiveDocument.%1.Label='%6'\n")
                .arg(name)
                .arg(ui.prismPolygon->value())
                .arg(ui.prismCircumradius->value().getValue(),0,'f',2)
                .arg(ui.prismHeight->value().getValue(),0,'f',2)
                .arg(placement)
                .arg(tr("Prism"));
        }
        else if (ui.comboBox1->currentIndex() == 8) {  // wedge
            name = QString::fromAscii(doc->getUniqueObjectName("Wedge").c_str());
            cmd = QString::fromAscii(
                "App.ActiveDocument.addObject(\"Part::Wedge\",\"%1\")\n"
                "App.ActiveDocument.%1.Xmin=%2\n"
                "App.ActiveDocument.%1.Ymin=%3\n"
                "App.ActiveDocument.%1.Zmin=%4\n"
                "App.ActiveDocument.%1.X2min=%5\n"
                "App.ActiveDocument.%1.Z2min=%6\n"
                "App.ActiveDocument.%1.Xmax=%7\n"
                "App.ActiveDocument.%1.Ymax=%8\n"
                "App.ActiveDocument.%1.Zmax=%9\n"
                "App.ActiveDocument.%1.X2max=%10\n"
                "App.ActiveDocument.%1.Z2max=%11\n"
                "App.ActiveDocument.%1.Placement=%12\n"
                "App.ActiveDocument.%1.Label='%13'\n")
                .arg(name)
                .arg(ui.wedgeXmin->value().getValue(),0,'f',2)
                .arg(ui.wedgeYmin->value().getValue(),0,'f',2)
                .arg(ui.wedgeZmin->value().getValue(),0,'f',2)
                .arg(ui.wedgeX2min->value().getValue(),0,'f',2)
                .arg(ui.wedgeZ2min->value().getValue(),0,'f',2)
                .arg(ui.wedgeXmax->value().getValue(),0,'f',2)
                .arg(ui.wedgeYmax->value().getValue(),0,'f',2)
                .arg(ui.wedgeZmax->value().getValue(),0,'f',2)
                .arg(ui.wedgeX2max->value().getValue(),0,'f',2)
                .arg(ui.wedgeZ2max->value().getValue(),0,'f',2)
                .arg(placement)
                .arg(tr("Wedge"));
        }
        else if (ui.comboBox1->currentIndex() == 9) {  // helix
            name = QString::fromAscii(doc->getUniqueObjectName("Helix").c_str());
            cmd = QString::fromAscii(
                "App.ActiveDocument.addObject(\"Part::Helix\",\"%1\")\n"
                "App.ActiveDocument.%1.Pitch=%2\n"
                "App.ActiveDocument.%1.Height=%3\n"
                "App.ActiveDocument.%1.Radius=%4\n"
                "App.ActiveDocument.%1.Angle=%5\n"
                "App.ActiveDocument.%1.LocalCoord=%6\n"
                "App.ActiveDocument.%1.Style=1\n"
                "App.ActiveDocument.%1.Placement=%7\n"
                "App.ActiveDocument.%1.Label='%8'\n")
                .arg(name)
                .arg(ui.helixPitch->value().getValue(),0,'f',2)
                .arg(ui.helixHeight->value().getValue(),0,'f',2)
                .arg(ui.helixRadius->value().getValue(),0,'f',2)
                .arg(ui.helixAngle->value().getValue(),0,'f',2)
                .arg(ui.helixLocalCS->currentIndex())
                .arg(placement)
                .arg(tr("Helix"));
        }
        else if (ui.comboBox1->currentIndex() == 10) {  // spiral
            name = QString::fromAscii(doc->getUniqueObjectName("Spiral").c_str());
            cmd = QString::fromAscii(
                "App.ActiveDocument.addObject(\"Part::Spiral\",\"%1\")\n"
                "App.ActiveDocument.%1.Growth=%2\n"
                "App.ActiveDocument.%1.Rotations=%3\n"
                "App.ActiveDocument.%1.Radius=%4\n"
                "App.ActiveDocument.%1.Placement=%5\n"
                "App.ActiveDocument.%1.Label='%6'\n")
                .arg(name)
                .arg(ui.spiralGrowth->value().getValue(),0,'f',2)
                .arg(ui.spiralRotation->value(),0,'f',2)
                .arg(ui.spiralRadius->value().getValue(),0,'f',2)
                .arg(placement)
                .arg(tr("Spiral"));
        }
        else if (ui.comboBox1->currentIndex() == 11) {  // circle
            name = QString::fromAscii(doc->getUniqueObjectName("Circle").c_str());
            cmd = QString::fromAscii(
                "App.ActiveDocument.addObject(\"Part::Circle\",\"%1\")\n"
                "App.ActiveDocument.%1.Radius=%2\n"
                "App.ActiveDocument.%1.Angle0=%3\n"
                "App.ActiveDocument.%1.Angle1=%4\n"
                "App.ActiveDocument.%1.Placement=%5\n"
                "App.ActiveDocument.%1.Label='%6'\n")
                .arg(name)
                .arg(ui.circleRadius->value().getValue(),0,'f',2)
                .arg(ui.circleAngle0->value().getValue(),0,'f',2)
                .arg(ui.circleAngle1->value().getValue(),0,'f',2)
                .arg(placement)
                .arg(tr("Circle"));
        }
        else if (ui.comboBox1->currentIndex() == 12) {  // ellipse
            name = QString::fromAscii(doc->getUniqueObjectName("Ellipse").c_str());
            cmd = QString::fromAscii(
                "App.ActiveDocument.addObject(\"Part::Ellipse\",\"%1\")\n"
                "App.ActiveDocument.%1.MajorRadius=%2\n"
                "App.ActiveDocument.%1.MinorRadius=%3\n"
                "App.ActiveDocument.%1.Angle0=%4\n"
                "App.ActiveDocument.%1.Angle1=%5\n"
                "App.ActiveDocument.%1.Placement=%6\n"
                "App.ActiveDocument.%1.Label='%7'\n")
                .arg(name)
                .arg(ui.ellipseMajorRadius->value().getValue(),0,'f',2)
                .arg(ui.ellipseMinorRadius->value().getValue(),0,'f',2)
                .arg(ui.ellipseAngle0->value().getValue(),0,'f',2)
                .arg(ui.ellipseAngle1->value().getValue(),0,'f',2)
                .arg(placement)
                .arg(tr("Ellipse"));
        }
        else if (ui.comboBox1->currentIndex() == 13) {  // vertex
            name = QString::fromAscii(doc->getUniqueObjectName("Vertex").c_str());
            cmd = QString::fromAscii(
                "App.ActiveDocument.addObject(\"Part::Vertex\",\"%1\")\n"
                "App.ActiveDocument.%1.X=%2\n"
                "App.ActiveDocument.%1.Y=%3\n"
                "App.ActiveDocument.%1.Z=%4\n"
                "App.ActiveDocument.%1.Placement=%5\n"
                "App.ActiveDocument.%1.Label='%6'\n")
                .arg(name)
                .arg(ui.vertexX->value().getValue(),0,'f',2)
                .arg(ui.vertexY->value().getValue(),0,'f',2)
                .arg(ui.vertexZ->value().getValue(),0,'f',2)
                .arg(placement)
                .arg(tr("Vertex"));
        }
        else if (ui.comboBox1->currentIndex() == 14) {  // line
            name = QString::fromAscii(doc->getUniqueObjectName("Line").c_str());
            cmd = QString::fromAscii(
                "App.ActiveDocument.addObject(\"Part::Line\",\"%1\")\n"
                "App.ActiveDocument.%1.X1=%2\n"
                "App.ActiveDocument.%1.Y1=%3\n"
                "App.ActiveDocument.%1.Z1=%4\n"
                "App.ActiveDocument.%1.X2=%5\n"
                "App.ActiveDocument.%1.Y2=%6\n"
                "App.ActiveDocument.%1.Z2=%7\n"
                "App.ActiveDocument.%1.Placement=%8\n"
                "App.ActiveDocument.%1.Label='%9'\n")
                .arg(name)
                .arg(ui.edgeX1->value().getValue(),0,'f',2)
                .arg(ui.edgeY1->value().getValue(),0,'f',2)
                .arg(ui.edgeZ1->value().getValue(),0,'f',2)
                .arg(ui.edgeX2->value().getValue(),0,'f',2)
                .arg(ui.edgeY2->value().getValue(),0,'f',2)
                .arg(ui.edgeZ2->value().getValue(),0,'f',2)
                .arg(placement)
                .arg(tr("Line"));
        }
        else if (ui.comboBox1->currentIndex() == 15) {  // RegularPolygon
            name = QString::fromAscii(doc->getUniqueObjectName("RegularPolygon").c_str());
            cmd = QString::fromAscii(
                "App.ActiveDocument.addObject(\"Part::RegularPolygon\",\"%1\")\n"
                "App.ActiveDocument.%1.Polygon=%2\n"
                "App.ActiveDocument.%1.Circumradius=%3\n"
                "App.ActiveDocument.%1.Placement=%4\n"
                "App.ActiveDocument.%1.Label='%5'\n")
                .arg(name)
                .arg(ui.regularPolygonPolygon->value())
                .arg(ui.regularPolygonCircumradius->value().getValue(),0,'f',2)
                .arg(placement)
                .arg(tr("Regular polygon"));
        }

        // Execute the Python block
        QString prim = tr("Create %1").arg(ui.comboBox1->currentText());
        Gui::Application::Instance->activeDocument()->openCommand(prim.toUtf8());
        Gui::Command::doCommand(Gui::Command::Doc, (const char*)cmd.toUtf8());
        Gui::Application::Instance->activeDocument()->commitCommand();
        Gui::Command::doCommand(Gui::Command::Doc, "App.ActiveDocument.recompute()");
        Gui::Command::doCommand(Gui::Command::Gui, "Gui.SendMsgToActiveView(\"ViewFit\")");
    }
    catch (const Base::PyException& e) {
        QMessageBox::warning(this, tr("Create %1")
            .arg(ui.comboBox1->currentText()), QString::fromLatin1(e.what()));
    }
}*/

TaskPrimitiveParameters::TaskPrimitiveParameters(ViewProviderPrimitive* PrimitiveView) : vp_prm(PrimitiveView)
{
    
    assert(PrimitiveView);
    
    PartDesign::FeaturePrimitive* prm = static_cast<PartDesign::FeaturePrimitive*>(PrimitiveView->getObject());
    cs  = static_cast<PartDesign::CoordinateSystem*>(prm->CoordinateSystem.getValue());
    
    //if no coordinate system exist we need to add one, it is highly important that it exists!
    if(!cs) {
        std::string CSName = App::GetApplication().getActiveDocument()->getUniqueObjectName("CoordinateSystem");
        cs = static_cast<PartDesign::CoordinateSystem*>(
                App::GetApplication().getActiveDocument()->addObject("PartDesign::CoordinateSystem", CSName.c_str()));
        prm->CoordinateSystem.setValue(cs);
    }
            
    ViewProviderDatumCoordinateSystem* vp = static_cast<ViewProviderDatumCoordinateSystem*>(
            Gui::Application::Instance->activeDocument()->getViewProvider(cs)); 
    
    assert(vp);    
    
    //make sure the relevant things are visible
    cs_visibility = vp->isVisible();
    vp->Visibility.setValue(true);
    if(prm->BaseFeature.getValue()) {
        Gui::Application::Instance->activeDocument()->getViewProvider(prm->BaseFeature.getValue())->setVisible(true); 
        vp_prm->setVisible(false); 
    }        
    
    parameter  = new TaskDatumParameters(vp);
    Content.push_back(parameter);
    
    primitive = new TaskBoxPrimitives(PrimitiveView);
    Content.push_back(primitive);
}

TaskPrimitiveParameters::~TaskPrimitiveParameters()
{
    ViewProviderDatumCoordinateSystem* vp = static_cast<ViewProviderDatumCoordinateSystem*>(
            Gui::Application::Instance->activeDocument()->getViewProvider(cs)); 
    vp->setVisible(cs_visibility);
    
    auto* prm = static_cast<PartDesign::FeaturePrimitive*>(vp_prm->getObject());
    if(prm->BaseFeature.getValue()) {
        Gui::Application::Instance->activeDocument()->getViewProvider(prm->BaseFeature.getValue())->setVisible(false); 
    }
    vp_prm->setVisible(true); 
}

bool TaskPrimitiveParameters::accept()
{
    Gui::Command::doCommand(Gui::Command::Doc,"App.ActiveDocument.recompute()");
    Gui::Command::doCommand(Gui::Command::Gui,"Gui.activeDocument().resetEdit()");
        
    return true;
}

bool TaskPrimitiveParameters::reject() {
    
    Gui::Command::doCommand(Gui::Command::Doc,"App.ActiveDocument.recompute()");
    Gui::Command::doCommand(Gui::Command::Gui,"Gui.activeDocument().resetEdit()");
    return true;
}

QDialogButtonBox::StandardButtons TaskPrimitiveParameters::getStandardButtons(void) const {
    return Gui::TaskView::TaskDialog::getStandardButtons();
}


#include "moc_TaskPrimitiveParameters.cpp"