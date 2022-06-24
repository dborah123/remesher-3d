#include "remesher3d.h"
#include "mesh.h"
#include "grid.h"
#include "element.h"
#include "webgl.h"

#include "../marching-tets/marchingtet.h"
#include "../marching-tets/tet-functions.h"

using namespace flux;

int
main (int argc, char *argv[]) {
    /* Creating un-uniform sphere from marching-tet project */
    // Creating analytical sphere function
    Grid<Tet> tet_grid({4,4,4});
    double center[3] = {0.5, 0.5, 0.5};
    double radius = 0.5;
    SphereTetFunction function(radius, center);

    // Peforming marching tet alg and getting sphere
    MarchingTet m_tet(tet_grid, function);
    m_tet.marching_tets();
    Mesh<Triangle>& sphere = m_tet.get_mesh();

    /* Remeshing Operations */
    HalfEdgeMesh<Triangle> halfmesh(sphere);
    Remesher3d remesh(halfmesh);
    remesh.tangential_relaxation(1);
    remesh.run_viewer();
}
