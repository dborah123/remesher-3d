#include "remesher3d.h"
#include "vec.hpp"
#include "webgl.h"
#include "mesh.h"

namespace flux {
/**
 * CONSTRUCTOR AND INITIALIZERS
 */

Remesher3d::Remesher3d(
    HalfEdgeMesh<Triangle>& halfmesh
) :
halfmesh_(halfmesh)
{

}

/**
 * STATISTICAL AND VISUAL FUNCTIONS
 */
void
Remesher3d::print_stats() {
    /**
     * Prints statistics of the current halfmesh in Remesher3d objects
     */
}

void
Remesher3d::run_viewer() {
    /**
     * Runs viewer in flux360
     */
    Mesh<Triangle> mesh(3);
    halfmesh_.extract(mesh);

    Viewer viewer;
    viewer.add(mesh);
    viewer.run();
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
     * Iterates through vertices in halfmesh_, relaxing them
     */
    std::vector<vec3d> new_points;
    vec3d new_coords;
    HalfVertex *halfvertex;

    for (auto& v : halfmesh_.vertices()) {
        halfvertex = v.get();
        new_coords = relax_vertex(halfvertex);
        new_points.push_back(new_coords);
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
    double n = norm(p);
    vec3d q = calculate_q(vertex);
    
    vec3d pmq = n * (p-q);
    if (std::isinf(p-q)) {
        std::cout << pmq << std::endl;
    }

    

    vec3d p_prime = q + ((n * (p - q)) * n);

    return p_prime;
}

vec3d
Remesher3d::calculate_q(HalfVertex *p) {
    /**
     * Calculate q using p. q is the average of the points of p's onering
     *
     * \return: vec3d of coordinates of q
     */
    std::vector<HalfVertex*> p_onering;
    halfmesh_.get_onering(p, p_onering);
    int onering_size = p_onering.size();

    // initializing vec3 to hold sum of vertex coordinates
    vec3d onering_sum;
    onering_sum.zero();

    for (HalfVertex *v : p_onering) {
        onering_sum += v->point;
    }

    vec3d result = onering_sum / (double)onering_size;
    if (std::isinf(result[0])) {
        std::cout << "result is infinite" << std::endl;
    }
    return result;
}

void
Remesher3d::change_coordinates(std::vector<vec3d>& new_points) {
    /**
     * Changes vertices in halfmesh to the new ones in new_points
     */
    HalfVertex *vertex;
    int i = 0;
    for (auto& v : halfmesh_.vertices()) {
        vertex = v.get();
        vertex->point = new_points[i];
        i++;
    }
}

} // flux