#include "remesher3d.h"
#include "mesh.h"
#include "readwrite.h"
#include "halfedges.h"
#include "webgl.h"
#include "sphere.h"
#include "simplifier.h"

using namespace flux;

int
main (int argc, char *argv[]) {
    // std::unique_ptr<MeshBase> mesh_ptr = read_obj("../../../../data/fandisk.off" , false );
    // Mesh<Triangle>& mesh1 = *static_cast< Mesh<Triangle>* >(mesh_ptr.get());

    Sphere<Triangle> sphere(100, 100, 1);

    HalfEdgeMesh<Triangle> halfmesh = simplify_mesh(sphere, 1000);
    Mesh<Triangle> mesh(3);
    // halfmesh.extract(mesh);
    Remesher3d remesh(halfmesh);

    Viewer viewer;
    // viewer.add(sphere);
    viewer.add(mesh);
    remesh.run_viewer();


    // HalfEdgeMesh<Triangle> halfmesh(mesh1);
}
