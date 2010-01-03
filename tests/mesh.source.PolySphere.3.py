#python

import testing

setup = testing.setup_mesh_source_test("PolySphere")

setup.source.type = "sphereized_cylinder"

testing.mesh_reference_comparison(setup.document, setup.source.get_property("output_mesh"), "mesh.source.PolySphere.3", 1, testing.platform_specific)

