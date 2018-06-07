#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#define ERR_DIM "bad X or Y size input"
#define ERR_SIZE "invalid grid size"
#define ERR_GRID "could not read the whole grid"
#define ERR_VALUE "a grid value is out of range"
#define ERR_VECTOR "a vector was too short while calculating a normal"

// 10cm by 10cm max since the units are 1/10th of mm on the infile.
#define MAX_Y 1000
#define MAX_X 1000

double grid[MAX_Y][MAX_X];

int negX, negY;

void error(char *msg) {
  fprintf(stderr, "error: %s\n", msg);
  exit(-1);
}

void printFacetWithNormal(double x1, double y1, double z1,
			  double x2, double y2, double z2,
			  double x3, double y3, double z3,
			  double nx, double ny, double nz) {
  if (negX) {
    x1 = -x1;
    x2 = -x2;
    x3 = -x3;
    nx = -nx;
  }
  if (negY) {
    y1 = -y1;
    y2 = -y2;
    y3 = -y3;
    ny = -ny;
  }

  // All coordinates are assumed to come in 1/10th of a mm, and now have to
  // be converted to mm units so ReplicatorG can take them and render properly.
  printf("  facet normal %.6lf %.6lf %.6lf\n", nx, ny, nz);
  printf("    outer loop\n");
  printf("      vertex %.6lf %.6lf %.6lf\n", x1/10.0, y1/10.0, z1/10.0);
  printf("      vertex %.6lf %.6lf %.6lf\n", x2/10.0, y2/10.0, z2/10.0);
  printf("      vertex %.6lf %.6lf %.6lf\n", x3/10.0, y3/10.0, z3/10.0);
  printf("    endloop\n");
  printf("  endfacet\n");
}

void printFacet(double x1, double y1, double z1,
		double x2, double y2, double z2,
		double x3, double y3, double z3) {
  double dx21 = x2-x1, dy21 = y2-y1, dz21 = z2-z1;
  double dx31 = x3-x1, dy31 = y3-y1, dz31 = z3-z1;

  double s21 = sqrt(dx21*dx21+dy21*dy21+dz21*dz21);
  double s31 = sqrt(dx31*dx31+dy31*dy31+dz31*dz31);

  if (s21 < 0.1 || s31 < 0.1) error(ERR_VECTOR);

  dx21 /= s21; dy21 /= s21; dz21 /= s21;
  dx31 /= s31; dy31 /= s31; dz31 /= s31;

  // we need to calculate the normal, which should have a positive dZ,
  // since this is a facet from the top surface of the block
  double nx, ny, nz;
  nx = dz31*dy21-dz21*dy31;
  ny = dx31*dz21-dx21*dz31;
  nz = dy31*dx21-dy21*dx31;
  if (nz<0.0) {
    nx = -nx;
    ny = -ny;
    nz = -nz;
  }
  printFacetWithNormal(x1, y1, z1,
		       x2, y2, z2,
		       x3, y3, z3,
		       nx, ny, nz);
}

int main(int argc, char **argv) {
  int nX, nY;
  int iX, iY;

  //
  // input the grid spec, formatted as follows:
  //
  // X Y
  // g11 g12 g13 ... g1X
  // ... ... ... ... ...
  // gY1 gY2 gY3 ... gYX
  //

  // get the grid size in X/Y, and validate
  if (scanf("%d %d", &nY, &nX)!=2) error(ERR_DIM);

  if (nX < 0) { negX = 1; nX = -nX; }
  if (nY < 0) { negY = 1; nY = -nY; }

  if (nX<2 || nX>MAX_X || nY<2 || nY>MAX_Y) error(ERR_SIZE);
  double nXD = (nX-1), nYD = (nY-1);
  
  // read in the grid
  for (iY=0; iY<nY; iY++) {
    for (iX=0; iX<nX; iX++) {
      if (scanf("%lf", &(grid[iY][iX]))!=1) error(ERR_GRID);
      if (grid[iY][iX]<1.0 || grid[iY][iX]>10000.0) error(ERR_VALUE);
    }
  }

  //
  // output the STL formatted triangulation
  //

  printf("solid\n");
  
  // output the two (bottom) facets, normal has dZ == -1.0
  printFacetWithNormal(0.0, 0.0, 0.0,
		       nXD, 0.0, 0.0,
		       nXD, nYD, 0.0,
		       0.0, 0.0, -1.0);
  printFacetWithNormal(0.0, 0.0, 0.0,
		       0.0, nYD, 0.0,
		       nXD, nYD, 0.0,
		       0.0, 0.0, -1.0);

  // output the facets on the X == 0.0 and X == nXD sides
  // surface normal has dX == -1.0 and dX == 1.0 respectively
  for (iY=1; iY<nY; iY++) { 
    // X == 0.0 side
    printFacetWithNormal(0.0, (iY-1), 0.0,
			 0.0, (iY-1), grid[iY-1][0],
			 0.0, iY, 0.0,
			 -1.0, 0.0, 0.0);
    printFacetWithNormal(0.0, iY, 0.0,
			 0.0, (iY-1), grid[iY-1][0],
			 0.0, iY, grid[iY][0],
			 -1.0, 0.0, 0.0);
    // X == nX side
    printFacetWithNormal(nXD, (iY-1), 0.0,
			 nXD, (iY-1), grid[iY-1][nX-1],
			 nXD, iY, 0.0,
			 1.0, 0.0, 0.0);
    printFacetWithNormal(nXD, iY, 0.0,
			 nXD, (iY-1), grid[iY-1][nX-1],
			 nXD, iY, grid[iY][nX-1],
			 1.0, 0.0, 0.0);
  }

  // output the facets on the Y == 0.0 and Y == nYD sides
  // surface normal has dY == -1.0 and dY == 1.0 respectively
  for (iX=1; iX<nX; iX++) { 
    // Y == 0.0 side
    printFacetWithNormal((iX-1), 0.0, 0.0,
			 (iX-1), 0.0, grid[0][iX-1],
			 iX, 0.0, 0.0,
			 0.0, -1.0, 0.0);
    printFacetWithNormal(iX, 0.0, 0.0,
			 (iX-1), 0.0, grid[0][iX-1],
			 iX, 0.0, grid[0][iX],
			 0.0, -1.0, 0.0);
    // Y == nYD side
    printFacetWithNormal((iX-1), nYD, 0.0,
			 (iX-1), nYD, grid[nY-1][iX-1],
			 iX, nYD, 0.0,
			 0.0, 1.0, 0.0);
    printFacetWithNormal(iX, nYD, 0.0,
			 (iX-1), nYD, grid[nY-1][iX-1],
			 iX, nYD, grid[nY-1][iX],
			 0.0, 1.0, 0.0);
  }

  // now handle the top of the surface (the most interesting part)
  for (iY=1; iY<nY; iY++) {
    for (iX=1; iX<nX; iX++) {
      printFacet((iX-1), (iY-1), grid[iY-1][iX-1],
		 (iX-1), iY, grid[iY][iX-1],
		 iX, iY, grid[iY][iX]);
      printFacet((iX-1), (iY-1), grid[iY-1][iX-1],
		 iX, (iY-1), grid[iY-1][iX],
		 iX, iY, grid[iY][iX]);
    }
  }

  printf("endsolid\n");
}
