#ifndef FLUX_REMESHER3D_H
#define FLUX_REMESHER3D_H

#include "halfedges.h"
#include "element.h"
#include <map>

namespace flux {

class Remesher3d {
public:

Remesher3d(HalfEdgeMesh<Triangle>& halfmesh);

/* Remeshing Algorithms: */
void tangential_relaxation(int num_iterations);

/* Statistics and Visual Functions */
void print_stats();
void run_viewer();
HalfEdgeMesh<Triangle>& get_mesh();

private:
HalfEdgeMesh<Triangle>& halfmesh_;

/**
 * TANGENTIAL RELAXATION
 */
void relax_vertices();
vec3d relax_vertex(HalfVertex *vertex);
vec3d calculate_q(HalfVertex *p);
vec3d calculate_n(HalfVertex *p);
vec3d calculate_face_normal(HalfFace *face);
void change_coordinates(std::map<HalfVertex*, vec3d>& new_points);

};

} // flux

#endif