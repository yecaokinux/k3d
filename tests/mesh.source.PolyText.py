#python

import testing

setup = testing.setup_mesh_source_test("PolyText")
testing.mesh_reference_comparison(setup.document, setup.source.get_property("output_mesh"), "mesh.source.PolyText", 1, testing.platform_specific)

