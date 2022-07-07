#ifndef FLUX_REMESHER3D_H
#define FLUX_REMESHER3D_H

#include "halfedges.h"
#include "element.h"
#include "../marching-tets/tet-functions.h"
#include "kdtree.h"
#include <map>

namespace flux {

class Remesher3d {
public:

Remesher3d(HalfEdgeMesh<Triangle>& halfmesh);

/* Remeshing Algorithms: */
void tangential_relaxation(int num_iterations);

/* Expeirmental Functions */
void correct_tangential_relaxation(SphereTetFunction& function);

/* Statistics and Visual Functions */
void print_stats();
int choose_algorithm();
void run_viewer();
HalfEdgeMesh<Triangle>& get_mesh();

private:
HalfEdgeMesh<Triangle>& _halfmesh;
kdtree<3> _kd_tree;


/**
 * INCREMENTAL RELAXATION
 */
void incremental_relaxation(int num_iterations);
void build();

/**
 * SPLIT
 */

/**
 * COllAPSE
 */

/**
 * EQUALIZE VALENCES
 */

/**
 * TANGENTIAL RELAXATION
 */
void relax_vertices();
vec3d relax_vertex(HalfVertex *vertex);
vec3d calculate_q(HalfVertex *p);
vec3d calculate_n(HalfVertex *p);
vec3d calculate_face_normal(HalfFace *face);
void change_coordinates(std::map<HalfVertex*, vec3d>& new_points);

/**
 * PROJECT TO SURFACE
 */
};

} // flux

#endif