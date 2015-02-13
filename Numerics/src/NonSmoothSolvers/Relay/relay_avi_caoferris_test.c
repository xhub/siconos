/* Siconos-Numerics, Copyright INRIA 2005-2012.
 * Siconos is a program dedicated to modeling, simulation and control
 * of non smooth dynamical systems.
 * Siconos is a free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 * Siconos is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with Siconos; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
 *
 * Contact: Vincent ACARY, siconos-team@lists.gforge.inria.fr
 */

/*!\file lcp_avi_caoferris.c
 \brief Solve an LCP by reformulating it as an AVI and the solver by Cao and
Ferris solves the subsequent AVI.
 \author Olivier Huber
*/

#include "AVI_Solvers.h"
#include "Relay_Solvers.h"
#include "avi_caoferris.h"
#include "relay_cst.h"
#include "AffineVariationalInequalities.h"

//#define DEBUG_STDOUT
//#define DEBUG_MESSAGES
#include "debug.h"


void relay_avi_caoferris_test(RelayProblem* problem, double *z, double *w, int *info, SolverOptions* options)
{
  unsigned int n = problem->size;
  assert(n > 0);
  assert(problem->M);
  unsigned int s = 2*n;

  /* Copy the data from Relay problem to an AVI struct */
  AffineVariationalInequalities avi_pb;
  avi_pb.size = n;
  avi_pb.M = problem->M;
  avi_pb.q = problem->q;
  polyhedron poly;
  avi_pb.poly = &poly;

  poly.id = SICONOS_SET_POLYHEDRON;
  poly.size_ineq = s;
  poly.size_eq = 0;
  poly.H = (double *)calloc(s*n, sizeof(double));
  poly.K = (double *)malloc(s*sizeof(double));
  poly.Heq = NULL;
  poly.Keq = NULL;

  DEBUG_PRINT_VEC(problem->lb, n);
  DEBUG_PRINT_VEC(problem->ub, n);

  int starting_constraint = rand() % s;
  for (unsigned i = 0, j = starting_constraint; i < s; ++i, j = (j+1) % s)
  {
    if (j >= n)
    {
      poly.H[i + s*(j-n)] = 1.0;
      poly.K[i] = problem->lb[j-n];
    }
    else
    {
      poly.H[i + s*j] = -1.0;
      poly.K[i] = -problem->ub[j];
    }
  }
  DEBUG_PRINT("H matrix\n");
  DEBUG_EXPR_WE(for (unsigned i = 0; i < s; ++i)
      { for(unsigned j = 0 ; j < n; ++j)
      { DEBUG_PRINTF("% 2.2e ", poly.H[i + j*s]) }
      DEBUG_PRINT("\n")});

  DEBUG_PRINT("K vector\n");
  DEBUG_EXPR_WE(for (unsigned i = 0; i < s; ++i)
      { DEBUG_PRINTF("% 2.2e ", poly.K[i])
      DEBUG_PRINT("\n")});

 /* Call directly the 3rd stage 
   * Here w is used as u and z as s in the AVI */
  *info = avi_caoferris(&avi_pb, z, w, options);

  free_polyhedron(&poly);
}

int relay_avi_caoferris_test_setDefaultSolverOptions(SolverOptions* options)
{
  int ret = avi_caoferris_setDefaultSolverOptions(options);
  options->solverId = SICONOS_RELAY_AVI_CAOFERRIS_TEST;
  return ret;
}
