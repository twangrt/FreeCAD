
# *   Copyright (c) 2019 Bernd Hahnebach <bernd@bimstatik.org>              *
# *   Copyright (c) 2020 Sudhanshu Dubey <sudhanshu.thethunder@gmail.com    *
# *                                                                         *
# *   This file is part of the FreeCAD CAx development system.              *
# *                                                                         *
# *   This program is free software; you can redistribute it and/or modify  *
# *   it under the terms of the GNU Lesser General Public License (LGPL)    *
# *   as published by the Free Software Foundation; either version 2 of     *
# *   the License, or (at your option) any later version.                   *
# *   for detail see the LICENCE text file.                                 *
# *                                                                         *
# *   This program is distributed in the hope that it will be useful,       *
# *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
# *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
# *   GNU Library General Public License for more details.                  *
# *                                                                         *
# *   You should have received a copy of the GNU Library General Public     *
# *   License along with this program; if not, write to the Free Software   *
# *   Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  *
# *   USA                                                                   *
# *                                                                         *
# ***************************************************************************

# to run the example use:
"""
from femexamples.boxanalysis_static import setup
setup()

"""

import FreeCAD
import Fem
import ObjectsFem

mesh_name = "Mesh"  # needs to be Mesh to work with unit tests

# Steel H-profile example HEA300
b = 290  # Width of flanges
t = 14  # Thickness of flanges
h = 300  # Total height of beam
d = 8  # Thickness of web
l = 4000  # Length of beam


def addbox(docxx, height, width, length, x, y, z):
    box_obj = docxx.addObject('Part::Box', 'Box')
    box_obj.Height = height
    box_obj.Width = width
    box_obj.Length = length
    box_obj.Placement = FreeCAD.Placement(FreeCAD.Vector(x, y, z), FreeCAD.Rotation(0, 0, 0))


def init_doc(doc=None):
    if doc is None:
        doc = FreeCAD.newDocument()
    return doc


def get_information():
    info = {"name": "Linear Buckling Analysis Static",
            "meshtype": "solid",
            "meshelement": "Tet10",
            "constraints": ["force", "displacement"],
            "solvers": ["calculix"],
            "material": "solid",
            "equation": "mechanical"
            }
    return info


def setup_base(doc=None, solvertype="ccxtools"):
    # setup box base model

    if doc is None:
        doc = init_doc()

    addbox(doc, t, b, l, 0, -b / 2, 0)
    addbox(doc, h - 2 * t, d, l, 0, -d / 2, t)
    addbox(doc, t, b, l, 0, -b / 2, h - t)

    shape = []
    for i in doc.Objects:

        if i.isDerivedFrom("Part::Feature"):
            shape.append(i)

    doc.addObject("Part::MultiFuse", "HEA300").Shapes = shape

    doc.recompute()

    geom_obj = doc.Box

    if FreeCAD.GuiUp:
        geom_obj.ViewObject.Document.activeView().viewAxonometric()
        geom_obj.ViewObject.Document.activeView().fitAll()

    # analysis
    analysis = ObjectsFem.makeAnalysis(doc, "Analysis")

    # material
    material_object = analysis.addObject(ObjectsFem.makeMaterialSolid(doc, "MechanicalMaterial"))[0]
    mat = material_object.Material
    mat["Name"] = "Steel-Generic"
    mat["YoungsModulus"] = "200000 MPa"
    mat["PoissonRatio"] = "0.30"
    mat["Density"] = "7900 kg/m^3"
    material_object.Material = mat

    # mesh
    from .meshes.mesh_linear_buckling import create_nodes, create_elements

    fem_mesh = Fem.FemMesh()
    control = create_nodes(fem_mesh)
    if not control:
        FreeCAD.Console.PrintError("Error on creating nodes.\n")
    control = create_elements(fem_mesh)
    if not control:
        FreeCAD.Console.PrintError("Error on creating elements.\n")
    femmesh_obj = analysis.addObject(ObjectsFem.makeMeshGmsh(doc, mesh_name))[0]
    femmesh_obj.FemMesh = fem_mesh
    femmesh_obj.Part = doc.HEA300
    #femmesh_obj.SecondOrderLinear = False
    #femmesh_obj.CharacteristicLengthMin = "8.0 mm"

    doc.recompute()
    return doc


def setup(doc=None, solvertype="ccxtools"):
    # setup box static, add a fixed, force and a pressure constraint

    doc = setup_base(doc, solvertype)
    geom_obj = doc.Box
    analysis = doc.Analysis

    # solver
    if solvertype == "calculix":
        solver_object = analysis.addObject(
            ObjectsFem.makeSolverCalculix(doc, "SolverCalculiX")
        )[0]
    elif solvertype == "ccxtools":
        solver_object = analysis.addObject(
            ObjectsFem.makeSolverCalculixCcxTools(doc, "CalculiXccxTools")
        )[0]
        solver_object.WorkingDir = u""

    else:
        FreeCAD.Console.PrintWarning(
            "Not known or not supported solver type: {}. "
            "No solver object was created.\n".format(solvertype)
        )
    if solvertype == "calculix" or solvertype == "ccxtools":
        solver_object.SplitInputWriter = False
        solver_object.AnalysisType = "buckling"  # <---
        #solver_object.BucklingFactor = "2"
        solver_object.GeometricalNonlinearity = "linear"
        solver_object.ThermoMechSteadyState = False
        solver_object.MatrixSolverType = "default"
        solver_object.IterationsControlParameterTimeUse = False

    ## displacement constraint
    displacement_constraint = ObjectsFem.makeConstraintDisplacement(doc, "FemConstraintDisplacement")
    displacement_constraint.References = [(doc.HEA300, ("Face12"))]
    displacement_constraint.zFix = True
    displacement_constraint.zFree = False
    displacement_constraint.yFix = True
    displacement_constraint.yFree = False
    analysis.addObject(displacement_constraint)

    displacement_constraint2 = ObjectsFem.makeConstraintDisplacement(doc, "FemConstraintDisplacement2")
    displacement_constraint2.References = [(doc.HEA300, ("Face4", "Face10", "Face1"))]

    displacement_constraint2.xFix = True
    displacement_constraint2.xFree = False
    analysis.addObject(displacement_constraint2)

    ## force_constraint
    force_constraint = ObjectsFem.makeConstraintForce(doc, "FemConstraintForce")
    force_constraint.References = [(doc.HEA300, "Face15")]
    force_constraint.Force = 100000.0
    force_constraint.Reversed = True
    analysis.addObject(force_constraint)

    doc.recompute()

    return doc