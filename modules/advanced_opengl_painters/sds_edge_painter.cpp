// K-3D
// Copyright (c) 1995-2005, Timothy M. Shead
//
// Contact: tshead@k-3d.com
//
// This program is free software; you can redistribute it and/or
// modify it under the terms of the GNU General Public
// License as published by the Free Software Foundation; either
// version 2 of the License, or (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
// General Public License for more details.
//
// You should have received a copy of the GNU General Public
// License along with this program; if not, write to the Free Software
// Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA

/** \file Paint points using a VBO
 */

#include "texture_colored_selection_painter_gl.h"
#include "painter_cache.h"
#include "sds_cache.h"

#include <k3d-i18n-config.h>
#include <k3dsdk/document_plugin_factory.h>
#include <k3dsdk/gl/extension.h>
#include <k3dsdk/gl.h>
#include <k3dsdk/hints.h>
#include <k3dsdk/imesh_painter_gl.h>
#include <k3dsdk/node.h>
#include <k3dsdk/painter_render_state_gl.h>
#include <k3dsdk/painter_selection_state_gl.h>
#include <k3dsdk/polyhedron.h>
#include <k3dsdk/selection.h>
#include <k3dsdk/utility_gl.h>

#include <boost/scoped_ptr.hpp>

namespace module
{

namespace opengl
{

namespace painters
{

	/////////////////////////////////////////////////////////////////////////////
// sds_edge_painter

class sds_edge_painter :
	public texture_colored_selection_painter
{
	typedef texture_colored_selection_painter base;

public:
	sds_edge_painter(k3d::iplugin_factory& Factory, k3d::idocument& Document) :
		base(Factory, Document),
		m_levels(init_owner(*this) + init_name("levels") + init_label(_("Levels")) + init_description(_("Number of SDS levels")) + init_value(2) + init_constraint(constraint::minimum<k3d::int32_t>(2)) + init_step_increment(1) + init_units(typeid(k3d::measurement::scalar)))
	{
		m_levels.changed_signal().connect(make_async_redraw_slot());
	}

	class painting_visitor : public k3d::sds::ipatch_boundary_visitor
	{
	public:
		painting_visitor(const k3d::mesh::selection_t& PointSelections) : m_edge_selections(PointSelections) {}

		void start_edge(const k3d::uint_t Edge)
		{
			glTexCoord1d(m_edge_selections[Edge]);
		}

		void finish_edge(const k3d::uint_t Edge)
		{
		}
		
		void add_vertex(const k3d::point3& Point)
		{
			k3d::gl::vertex3d(Point);
		}
	private:
		const k3d::mesh::selection_t& m_edge_selections;
	};

	void on_paint_mesh(const k3d::mesh& Mesh, const k3d::gl::painter_render_state& RenderState, k3d::iproperty::changed_signal_t& ChangedSignal)
	{
		const k3d::uint_t levels = m_levels.pipeline_value();

		for(k3d::mesh::primitives_t::const_iterator primitive = Mesh.primitives.begin(); primitive != Mesh.primitives.end(); ++primitive)
		{
			boost::scoped_ptr<k3d::polyhedron::const_primitive> polyhedron(k3d::polyhedron::validate(Mesh, **primitive));
			if(!polyhedron || !k3d::polyhedron::is_sds(*polyhedron))
				continue;

			const k3d::sds::catmull_clark_subdivider& subdivider = get_cached_data<sds_cache>(std::make_pair(levels, primitive->get()), ChangedSignal).value(Mesh);
			painting_visitor visitor(polyhedron->edge_selections);

			glBegin(GL_LINES);
			subdivider.visit_boundary(*polyhedron, levels, visitor);
			glEnd();
		}
	}
	
	class selecting_visitor : public k3d::sds::ipatch_boundary_visitor
	{
	public:
		void start_edge(const k3d::uint_t Edge)
		{
			k3d::gl::push_selection_token(k3d::selection::EDGE, Edge);
		}

		void finish_edge(const k3d::uint_t Edge)
		{
			k3d::gl::pop_selection_token(); // EDGE
		}

		void add_vertex(const k3d::point3& Point)
		{
			k3d::gl::vertex3d(Point);
		}
	};

	void on_select_mesh(const k3d::mesh& Mesh, const k3d::gl::painter_render_state& RenderState, const k3d::gl::painter_selection_state& SelectionState, k3d::iproperty::changed_signal_t& ChangedSignal)
	{
		glDisable(GL_LIGHTING);
		
		if(!SelectionState.select_component.count(k3d::selection::EDGE))
			return;

		const k3d::uint_t levels = m_levels.pipeline_value();

		k3d::uint_t primitive_index = 0;
		for(k3d::mesh::primitives_t::const_iterator primitive = Mesh.primitives.begin(); primitive != Mesh.primitives.end(); ++primitive, ++primitive_index)
		{
			boost::scoped_ptr<k3d::polyhedron::const_primitive> polyhedron(k3d::polyhedron::validate(Mesh, **primitive));
			if(!polyhedron || !k3d::polyhedron::is_sds(*polyhedron))
				continue;

			const k3d::sds::catmull_clark_subdivider& subdivider = get_cached_data<sds_cache>(std::make_pair(levels, primitive->get()), ChangedSignal).value(Mesh);
			selecting_visitor visitor;

			k3d::gl::push_selection_token(k3d::selection::PRIMITIVE, primitive_index);
			subdivider.visit_boundary(*polyhedron, levels, visitor);
			k3d::gl::pop_selection_token(); // PRIMITIVE
		}
	}

	static k3d::iplugin_factory& get_factory()
	{
		static k3d::document_plugin_factory<sds_edge_painter, k3d::interface_list<k3d::gl::imesh_painter > > factory(
						k3d::uuid(0x55238c22, 0xed466597, 0xb86c86a3, 0x0de46700),
				"OpenGLSDSEdgePainter",
				_("Renders mesh as SDS patch borders using OpenGL 1.1"),
				"Development",
				k3d::iplugin_factory::EXPERIMENTAL);

		return factory;
	}
	k3d_data(k3d::int32_t, immutable_name, change_signal, with_undo, local_storage, with_constraint, measurement_property, with_serialization) m_levels;
};

	/////////////////////////////////////////////////////////////////////////////
// sds_edge_painter_factory

	k3d::iplugin_factory& sds_edge_painter_factory()
	{
		return sds_edge_painter::get_factory();
	}

} // namespace painters

} // namespace opengl

} // namespace module


