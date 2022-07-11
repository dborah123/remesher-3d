#ifndef FLUX_REMESHER3D_H
#define FLUX_REMESHER3D_H

#include "halfedges.h"
#include "element.h"
#include "../marching-tets/tet-functions.h"
#include "kdtree.h"
#include "size.h"
#include <map>

namespace flux {

class Remesher3d {
public:

Remesher3d(HalfEdgeMesh<Triangle>& halfmesh, SizingField<3>& sizing_field);

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
const SizingField<3>& _sizing_field;
std::vector<HalfEdge*> _halfedge_vector;

/**
 * INCREMENTAL RELAXATION
 */
void incremental_relaxation(int num_iterations);
void build();

/**
 * SPLIT
 */
std::pair<int,int> split_edges();
void split(HalfEdge *halfedge);
void split_boundary(HalfEdge *halfedge);
void change_edge(
    HalfEdge *halfedge,
    HalfEdge *next,
    HalfEdge *twin,
    HalfVertex *vertex,
    HalfFace *face
);
void change_face(HalfFace *face, HalfEdge *halfedge);
void change_vertex(HalfVertex *vertex, HalfEdge *halfedge);
int check_split(HalfEdge *halfedge);

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


/**
 * HELPER FUNCTIONS
 */
void update_halfedge_vector();
int is_boundary_edge(HalfEdge* halfedge);

/**
 * COMPUTATION
 */
double get_length(HalfEdge *halfedge);
vec3d calculate_middle(HalfEdge *halfedge);
};

} // flux

#endif