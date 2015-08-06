/* Siconos-Numerics, Copyright INRIA 2005-2014
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

#include "LCP_Solvers.h"
#include "lcp_pivot.h"

#include <assert.h>

#include "SolverOptions.h"
#include "SiconosBlas.h"
#include "NSSTools.h"

//#define DEBUG_STDOUT
//#define DEBUG_MESSAGES
#include "debug.h"

#include "NCP_PathSearch.h"

/* This function is here to test the pivotal code of the path search */
void lcp_pathsearch(LinearComplementarityProblem* problem, double *z, double *w, int *info , SolverOptions* options)
{

  assert(problem);
  assert(problem->M);

  unsigned n = problem->size;

  /* we allocate each time here since this function is onyl for testing */
  double* x = (double*)malloc(n*sizeof(double));
  double* x_plus = (double*)malloc(n*sizeof(double));
  double* r = (double*)malloc(n*sizeof(double));

  /* w = F(z) = M z + q */
  cblas_dcopy(n, problem->q, 1, w, 1);
  prodNumericsMatrix(n, n, 1.0, problem->M, z, 1.0, w);

  /* find x from z and w */
  ncp_pathsearch_compute_x_from_z(n, z, w, x);
  pos_part(n, x, x_plus); /* update x_plus */

  /* compute F(x_plus) = q + M x_plus*/
  cblas_dcopy(n, problem->q, 1, w, 1);
  prodNumericsMatrix(n, n, 1.0, problem->M, x_plus, 1.0, w);

  /* r = F_+(x) = F(x_+) + x - x_+ */
  /* the real q = q - r = x_+ - x - M x_plus */

  /* first compute r = x - x_+ */
  cblas_dcopy(n, x, 1, r, 1); /* r = x */
  cblas_daxpy(n, -1.0, x_plus, 1, r, 1); /* r -= x_plus */

  /* save original q */
  cblas_dcopy(n, problem->q, 1, x, 1);
  /* start with q = -M x_plus */
  prodNumericsMatrix(n, n, -1.0, problem->M, x_plus, 0.0, problem->q);
  /* we factorized computations and compute the final value of q */
  cblas_daxpy(n, -1.0, r, 1, problem->q, 1); /* q -= x - x_plus */

  /* Finish r */
  cblas_daxpy(n, 1.0, w, 1, r, 1); /* r += F(x_plus) */

  /* end update M, q and r */

  options->iparam[SICONOS_IPARAM_PATHSEARCH_STACKSIZE] = 1;
  options->iparam[SICONOS_IPARAM_PIVOT_RULE] = SICONOS_LCP_PIVOT_PATHSEARCH;

  DEBUG_PRINT("x_plus r q\n");
  DEBUG_EXPR_WE(for (unsigned i = 0; i < n; ++i)
      { DEBUG_PRINTF("%e %e %e\n", x_plus[i], r[i], problem->q[i]) });

  lcp_pivot_covering_vector(problem, x_plus, w, info, options, r);

  switch (*info)
  {
    case LCP_PIVOT_SUCCESS:
      DEBUG_PRINT("lcp_pathsearch :: path search procedure was successful!\n");
      break;
    case LCP_PIVOT_RAY_TERMINATION:
      DEBUG_PRINT("lcp_pathsearch :: ray termination, let's fastened your seat belt!\n");
      break;
    case LCP_PATHSEARCH_NON_ENTERING_T:
      DEBUG_PRINT("lcp_pathsearch :: the variable t could not enter !\n");
      break;
    case LCP_PIVOT_NUL:
    case LCP_PATHSEARCH_LEAVING_T:
      printf("lcp_pathsearch :: kaboom, kaboom still more work needs to be done\n");
      break;
    default:
      printf("lcp_pathsearch :: unknown code returned by the path search\n");
  }

  /* recover solution */
  pos_part(n, x_plus, z);
  /* restore original q */
  cblas_dcopy(n, x, 1, problem->q, 1);
  cblas_dcopy(n, problem->q, 1, w, 1);
  prodNumericsMatrix(n, n, 1.0, problem->M, z, 1.0, w);

  free(x);
  free(x_plus);
  free(r);
}
