#python

import testing

setup = testing.setup_mesh_source_test("Cylinder")

testing.mesh_reference_comparison(setup.document, setup.source.get_property("output_mesh"), "mesh.source.Cylinder", 1)

