#python

import testing

setup = testing.setup_mesh_source_test("LinearKnot")

testing.require_valid_mesh(setup.document, setup.source.get_property("output_mesh"))
testing.require_similar_mesh(setup.document, setup.source.get_property("output_mesh"), "mesh.source.LinearKnot", 4, testing.platform_specific)
