#include "remesher3d.h"
#include "mesh.h"
#include "grid.h"
#include "element.h"
#include "webgl.h"
#include "sphere.h"
#include "./sizing-fields/edgelengthsizingfield.h"

#include "../marching-tets/marchingtet.h"
#include "../marching-tets/tet-functions.h"

using namespace flux;

int
main (int argc, char *argv[]) {
    /* Creating un-uniform sphere from marching-tet project */
    // Creating analytical sphere function
    Sphere<Triangle> sphere(3,3,.3);
    EdgelengthSizingField function(0.06);

    HalfEdgeMesh<Triangle> halfmesh(sphere);
    Remesher3d remesh(halfmesh, function);
    remesh.incremental_relaxation(10);

    remesh.run_viewer();
}
