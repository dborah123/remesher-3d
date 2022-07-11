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
    HalfEdgeMesh<Triangle>& halfmesh,
    SizingField<3>& sizing_field
) :
_halfmesh(halfmesh),
_sizing_field(sizing_field)
{  }

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
        std::cout << "split" << std::endl;
        // Split long edges
        split_edges();
        // Collapse short edges

        // Equalize valences

        // Tangential relaxation

        // Project to surface
    }
}

/**
 * SPLIT
 */
std::pair<int,int>
Remesher3d::split_edges() {
    /**
     * Performs splits on appropriate edges
     */
    int num_splits = 0, num_boundary_splits = 0;
    update_halfedge_vector();

    for (auto& halfedge : _halfedge_vector) {
        switch (check_split(halfedge))
        {
        case 1:
            split(halfedge);
            num_splits++;
            break;
        case 2:
            split_boundary(halfedge);
            num_boundary_splits++;
            break;
        default:
            break;
        }
    }

    return std::make_pair(num_splits, num_boundary_splits);
}

int
Remesher3d::check_split(HalfEdge* halfedge) {
    double length = get_length(halfedge);

    // Get midpoint of halfedge
    double midpoint[3];
    vec3d midpoint_vec = calculate_middle(halfedge);
    midpoint[0] = midpoint_vec[0];
    midpoint[1] = midpoint_vec[1];
    midpoint[2] = midpoint_vec[2];

    // Check if creates short edges
    vec3d r_point = halfedge->next->next->vertex->point;
    vec3d s_point = halfedge->twin->next->next->vertex->point;

    double new_edge1 = norm(r_point - midpoint_vec);
    double new_edge2 = norm(midpoint_vec - s_point); 

    double analytical_length = _sizing_field(midpoint);

    // Edge check
    double ratio = length / analytical_length;

    if (ratio < sqrt(2)) return 0;     // no split

    // Short edge creation check
    double p_ratio = new_edge1 / analytical_length;
    double s_ratio = new_edge2 / analytical_length;

    if (p_ratio < 0.6 || s_ratio < 0.6) return 0; // no split >>> creates short edges
    if (is_boundary_edge(halfedge)) return 2;     // boundary split
    return 1;    
}

void
Remesher3d::split(HalfEdge* halfedge) {
    /**
     * 
     */
        /**
     * Splits (long) halfedge into 4 edges
     */
    // Calculating coordinates for new point
    vec3d new_point_coords = calculate_middle(halfedge);
    double new_points_arr[3] = {
        new_point_coords[0],
        new_point_coords[1],
        new_point_coords[2]
    };

    // Defining current setup
    HalfEdge *twin = halfedge->twin;
    HalfEdge *tr = halfedge->next;
    HalfEdge *tl = tr->next;
    HalfEdge *bl = twin->next;
    HalfEdge *br = bl->next;

    HalfVertex *p = halfedge->twin->vertex;
    HalfVertex *r = halfedge->next->next->vertex;
    HalfVertex *s = twin->next->next->vertex;

    HalfFace *f1 = halfedge->face;
    HalfFace *f4 = twin->face;

    // Initialize new vertex
    HalfVertex *new_vertex = _halfmesh.create_vertex(3, new_points_arr);

    // Setting up triangles
    HalfEdge *a = _halfmesh.create_edge();
    HalfEdge *b = _halfmesh.create_edge();
    HalfEdge *c = _halfmesh.create_edge();
    HalfEdge *d = _halfmesh.create_edge();
    HalfEdge *e = _halfmesh.create_edge();
    HalfEdge *f = _halfmesh.create_edge();

    HalfFace *f2 = _halfmesh.create_face();
    HalfFace *f3 = _halfmesh.create_face();

    // Setting edges a,b,c,d,e,f
    change_edge(a, tl, b, new_vertex, f1);
    change_edge(b, c, a, r, f2);
    change_edge(c, tr, d, new_vertex, f2);
    change_edge(d, e, c, p, f3);
    change_edge(e, br, f, new_vertex, f3);
    change_edge(f, twin, e, s, f4);
    
    // Connecting previous mesh componenets to new ones (and the new vertex)
    change_edge(halfedge, a, nullptr, nullptr, nullptr);
    change_edge(twin, nullptr, nullptr, new_vertex, nullptr);

    change_edge(tr, b, nullptr, nullptr, f2);
    change_edge(br, d, nullptr, nullptr, f3);
    change_edge(bl, f, nullptr, nullptr, f4);

    change_vertex(new_vertex, a);
    change_vertex(p, d);
    
    change_face(f1, halfedge);
    change_face(f2, b);
    change_face(f3, d);
    change_face(f4, twin);

}

void
Remesher3d::split_boundary(HalfEdge *halfedge) {
    /**
     * Peforms split operation on boundary edge
     */
    HalfEdge *inner, *twin;

    flux_assert((halfedge->face == nullptr )!= (halfedge->twin->face == nullptr));

    if (halfedge->face != nullptr) {
        inner = halfedge;
    } else {
        inner = halfedge->twin;
    }
    // Defining current setup
    twin = inner->twin;
    HalfEdge *tr = inner->next;
    HalfEdge *tl = tr->next;

    HalfVertex *p = twin->vertex;
    HalfVertex *r = tl->vertex;

    vec3d new_point_coords = calculate_middle(halfedge);
    double new_points_arr[3] = {
        new_point_coords[0],
        new_point_coords[1],
        new_point_coords[2]
    };

    HalfFace *f1 = inner->face;

    // Initialize new vertex
    HalfVertex *new_vertex = _halfmesh.create_vertex(3, new_points_arr);
    new_vertex->index = -1;

    // Setting up triangles
    HalfEdge *a = _halfmesh.create_edge();
    HalfEdge *b = _halfmesh.create_edge();
    HalfEdge *c = _halfmesh.create_edge();
    HalfEdge *d = _halfmesh.create_edge();

    HalfFace *f2 = _halfmesh.create_face();

    change_edge(a, tl, b, new_vertex, f1);
    change_edge(b, c, a, r, f2);
    change_edge(c, tr, d, new_vertex, f2);
    change_edge(d, twin, c, p, nullptr);

    change_edge(inner, a, nullptr, nullptr, nullptr);
    change_edge(twin, nullptr, nullptr, new_vertex, nullptr);
    change_edge(tr, b, nullptr, nullptr, f2);

    change_vertex(new_vertex, a);
    change_vertex(p, tr);

    change_face(f1, inner);
    change_face(f2, b);

    // Connecting prev edge to d
    twin->prev->next = d;
    d->prev = twin->prev;
    twin->prev = d;
}

void
Remesher3d::change_edge(
    HalfEdge* halfedge,
    HalfEdge *next,
    HalfEdge *twin,
    HalfVertex *vertex, 
    HalfFace *face
) {
    /**
     * Changes halfedge's fields according to inputs
     * If a param is nullptr, it will be ignored
     */
    if (next) halfedge->next = next;
    if (twin) halfedge->twin = twin;
    if (vertex) halfedge->vertex = vertex;
    if (face) halfedge->face = face;
}

void
Remesher3d::change_face(HalfFace *face, HalfEdge *halfedge) {
    /**
     * Changes halfedge's field according to input
     */
    if (halfedge) face->edge = halfedge;
}

void
Remesher3d::change_vertex(HalfVertex *vertex, HalfEdge *halfedge) {
    /**
     * Changes vertex's fields according to inputs
     */
    if (halfedge) vertex->edge = halfedge;
}


/**
 * HELPER METHODS
 */
void
Remesher3d::update_halfedge_vector() {
    /**
     * Clears halfedge_vector and then adds current halfedges from halfmesh_ into
     * halfedge_vector
     */
    _halfedge_vector.clear();
    for (auto& e : _halfmesh.edges()) {
        _halfedge_vector.push_back(e.get());
    }
}

int
Remesher3d::is_boundary_edge(HalfEdge* halfedge) {
    /**
     * Determines if edge is boundary (1) or not (0)
     */
    if (!halfedge->face || !halfedge->twin->face) return 1;
    return 0;
}

/**
 * COMPUTATION
 */
double
Remesher3d::get_length(HalfEdge* halfedge) {
    /**
     * Calculates length of a halfedge
     */
    HalfVertex *v1 = halfedge->vertex;
    HalfVertex *v2 = halfedge->twin->vertex;

    double x1 = v1->point[0];
    double y1 = v1->point[1];
    double x2 = v2->point[0];
    double y2 = v2->point[1];

    return sqrt(pow((x1-x2), 2) + pow((y1-y2), 2));
}

vec3d
Remesher3d::calculate_middle(HalfEdge *halfedge) {
    /**
     * Calculates the center coordinates of the halfedge
     *
     * Returns a array of 2 doubles
     */
    vec3d result_coords;

    HalfVertex *v1 = halfedge->vertex;
    HalfVertex *v2 = halfedge->twin->vertex;

    vec3d v1_point = v1->point;
    vec3d v2_point = v2->point;

    result_coords[0] = (v1_point[0] + v2_point[0]) / 2.0;
    result_coords[1] = (v1_point[1] + v2_point[1]) / 2.0;
    result_coords[2] = (v1_point[2] + v2_point[2]) / 2.0;;

    return result_coords;
}
} // flux