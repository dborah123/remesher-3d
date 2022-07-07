#include "remesher3d.h"
#include "vec.hpp"
#include "webgl.h"
#include "mesh.h"
#include "predicates.h"
#include <map>

namespace flux {
/**
 * CONSTRUCTOR AND INITIALIZERS
 */

Remesher3d::Remesher3d(
    HalfEdgeMesh<Triangle>& halfmesh
) :
_halfmesh(halfmesh)
{
    build();
}

void
Remesher3d::build() {
    Mesh<Triangle> mesh(3);
    _halfmesh.extract(mesh);
    _kd_tree = kdtree()
}

/**
 * UTILITY FUNCTIONS
 */
void
Remesher3d::print_stats() {
    /**
     * Prints statistics of the current halfmesh in Remesher3d objects
     */
}

int
Remesher3d::choose_algorithm() {
    /**
     * Prompts user to choose which remeshing algorithm to perform
     * 
     * RETURNS: -1 if invalid answer
     */
    std::cout << "Choose a remeshing algorithm to perform on sphere:" << '\n';
    std::cout << "0. Tangential Relaxation" << '\n';

    int user_response;

    std::cin >> user_response;

    if (user_response > 0 || user_response < 0) return -1;
    return user_response;
}

void
Remesher3d::run_viewer() {
    /**
     * Runs viewer in flux360
     */
    Mesh<Triangle> mesh(3);
    _halfmesh.extract(mesh);

    Viewer viewer;
    viewer.add(mesh);
    viewer.run();
}

HalfEdgeMesh<Triangle>&
Remesher3d::get_mesh() {
    return _halfmesh;
}

/**
 * TANGENTIAL RELAXATION
 */
void
Remesher3d::tangential_relaxation(int num_iterations) {
    /**
     * Performs tangential relaxation on the mesh
     */  
    for (int i = 0; i < num_iterations; ++i) {
        relax_vertices();
    }
}

void
Remesher3d::relax_vertices() {
    /**
     * Iterates through vertices in _halfmesh, relaxing them
     */
    // std::vector<vec3d> new_points;
    std::map<HalfVertex*, vec3d> new_points;
    vec3d new_coords;
    HalfVertex *halfvertex;

    for (auto& v : _halfmesh.vertices()) {
        halfvertex = v.get();
        new_coords = relax_vertex(halfvertex);
        new_points[halfvertex] = new_coords;
    }

    change_coordinates(new_points);
}

vec3d
Remesher3d::relax_vertex(HalfVertex *vertex) {
    /**
     * Relaxes halfvertex using tangential relaxation
     * Equation: p' = q + (n * (p - q)) * n
     * 
     * \return: new coordinates of point
     */
    vec3d p = vertex->point;
    vec3d n = calculate_n(vertex);
    vec3d q = calculate_q(vertex);

    vec3d p_prime = q + (dot(n, (p - q)) * n);
    return p_prime;
}

vec3d
Remesher3d::calculate_n(HalfVertex *p) {
    /**
     * Calculates n by getting onering of p and getting average of face normals
     */
    // Getting onering
    std::vector<HalfFace*> p_onering;
    _halfmesh.get_onering(p, p_onering);
    int num_faces = p_onering.size();

    // Getting face normals of the surrounding faces and averaging them
    vec3d avg_face_normals;
    avg_face_normals.zero();
    for (auto& face : p_onering) {
        avg_face_normals += calculate_face_normal(face);
    }

    for (int i = 0; i < 3; ++i) {
        avg_face_normals[i] = avg_face_normals[i] / num_faces;
    }
    return avg_face_normals;
}

vec3d
Remesher3d::calculate_face_normal(HalfFace *face) {
    /**
     * 
     */
    // Getting necessary connectivity elements
    HalfEdge *halfedge0 = face->edge;
    HalfEdge *halfedge1 = halfedge0->next;
    HalfEdge *halfedge2 = halfedge1->next;

    // Getting coords of vertices of triangle
    vec3d point0 = halfedge0->vertex->point;
    vec3d point1 = halfedge1->vertex->point;
    vec3d point2 = halfedge2->vertex->point;

    // Getting two edges to perform calculation on
    vec3d edge_a = point1 - point0;
    vec3d edge_b = point2 - point0;

    // Calculating face normal
    vec3d face_normal;
    int j, k;
    for (int i = 0; i < 3; ++i) {
        j = (i+1) % 3;
        k = (i+2) % 3;
        face_normal[i] = (edge_a[j] * edge_b[k]) - (edge_a[k] * edge_b[j]);
    }
    return face_normal;
}

vec3d
Remesher3d::calculate_q(HalfVertex *p) {
    /**
     * Calculate q using p. q is the average of the points of p's onering
     *
     * \return: vec3d of coordinates of q
     */
    std::vector<HalfVertex*> p_onering;
    _halfmesh.get_onering(p, p_onering);
    int onering_size = p_onering.size();

    // initializing vec3 to hold sum of vertex coordinates
    vec3d onering_sum;
    onering_sum.zero();

    for (HalfVertex *v : p_onering) {
        onering_sum += v->point;
    }

    vec3d result;
    result = onering_sum / (double) onering_size;

    return result;
}

void
Remesher3d::change_coordinates(std::map<HalfVertex*, vec3d>& new_points) {
    /**
     * Changes vertices in halfmesh to the new ones in new_points
     */
    HalfVertex *vertex;
    vec3d new_point;
    for (auto iter = new_points.begin(); iter != new_points.end(); iter++) {
        vertex = iter->first;
        new_point = iter->second;
        vertex->point = new_point;
    }
}

void
Remesher3d::correct_tangential_relaxation(SphereTetFunction& function) {
    /**
     * Realigns vertices to place on the sphere itself
     */
    HalfVertex *vertex;
    vec3d original_point;
    vec3d new_point;
    vec3d distance_to_new_point;
    for (auto& v : _halfmesh.vertices()) {
        vertex = v.get();
        original_point = vertex->point;
        distance_to_new_point = function(original_point);
        
        for (int i = 0; i < 3; ++i) {
            distance_to_new_point[i] = -1 * distance_to_new_point[i];
        }

        new_point = distance_to_new_point + original_point;

        vertex->point = new_point;
    }
}


void
Remesher3d::incremental_relaxation(int num_iterations) {
    /**
     * Implementation of the incremental relaxation remeshing algorithm
     */
    for (int i = 0; i < num_iterations; ++i) {
        // Split long edges

        // Collapse short edges

        // Equalize valences

        // Tangential relaxation

        // Project to surface
    }
}
} // flux