/* Siconos-Numerics, Copyright INRIA 2005-2015
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

/* Simple twisting example with the double integrator */

/* to have drand48 ...
 * hmhmhm ??? -- xhub */
#define _XOPEN_SOURCE

#include <stdio.h>
#include <stdlib.h>
#include <float.h>

#include "AVI_Solvers.h"
#include "SiconosSets.h"

#define TS 10e-3
#define NB_ITER 10000
#define TOL_NC 1e-12

int main(void)
{
  double x[2];
  unsigned short xsubi1[] = {0., 0., 0.};
  unsigned short xsubi2[] = {0., 0., 0.};
  x[0] = 50*erand48(xsubi1);
  x[1] = 50*erand48(xsubi2);
  /* column major */
  double H[8] = {1.0, -TS/2.0, -1.0, TS/2.0, 0.0, 1.0, 0.0, -1.0};
  double K[4] = {-1.0, -1.0, -1.0, -1.0};

  double v1[] = {-1.0, -1.0 -TS/2.0};
  double v2[] = {-1.0, 1.0 -TS/2.0};
  double v3[] = {1.0, 1.0 + TS/2.0};
  double v4[] = {1.0, -1.0 + TS/2.0};

  polyhedron poly = { SICONOS_SET_POLYHEDRON, 4, 0, H, K, NULL, NULL };

  /* twisting gain */
  double G = 10;
  double beta = .1;
  NumericsMatrix num_mat;
  double M[4] = { G*TS*TS/2.0, G*TS, beta*G*TS*TS/2.0, beta*G*TS };
  fillNumericsMatrix(&num_mat, NM_DENSE, 2, 2, M);

  double q[2] = { x[0] + TS*x[1], x[1] };

  AffineVariationalInequalities avi = {
    .size = 2,
    .M = &num_mat,
    .q = q,
    .d = NULL,
    .poly = &poly
  };

  SolverOptions options;
  avi_caoferris_setDefaultSolverOptions(&options);

  _Bool c1, c2, c3, c4;
  unsigned N = 0;
  int info = 0;
  double lambda[2] = {0.};
  double sigma[2] = {0.};
  do
  {
    N++;
    lambda[0] = 0.0;
    lambda[1] = 0.0;
    sigma[0] = 0.0;
    sigma[1] = 0.0;

    info = avi_caoferris(&avi, lambda, sigma, &options);
    if (info) fprintf(stderr, "SOLVER FAILED!\tinfo=%d\n", info);

/* /    printf("x_k: %2.6e %2.6e\n", x[0], x[1]);
    printf("lambda: %2.6e %2.6e\n", lambda[0], lambda[1]);
    printf("u = %2.6e\n", lambda[0] + beta*lambda[1]);*/
    sigma[0] = x[0] + TS*x[1] + M[0]*lambda[0] + M[2]*lambda[1];
    sigma[1] = x[1] + M[1]*lambda[0] + M[3]*lambda[1];
/*    printf("x_{k+1}: %2.6e %2.6e\n", sigma[0], sigma[1]);*/

    /* let's check is it is correct */
    c1 = (sigma[0]*(v1[0] + lambda[0]) + sigma[1]*(v1[1] + lambda[1])) <= TOL_NC;
    c2 = (sigma[0]*(v2[0] + lambda[0]) + sigma[1]*(v2[1] + lambda[1])) <= TOL_NC;
    c3 = (sigma[0]*(v3[0] + lambda[0]) + sigma[1]*(v3[1] + lambda[1])) <= TOL_NC;
    c4 = (sigma[0]*(v4[0] + lambda[0]) + sigma[1]*(v4[1] + lambda[1])) <= TOL_NC;

    if (!c1)
    {
      fprintf(stderr, "ERROR in implicit_twisting.c, bad value of lambda. test c1 failed\n");
      fprintf(stderr, "<v1-lambda, sigma> = %2.10e\n", (sigma[0]*(v1[0] + lambda[0]) + sigma[1]*(v1[1] + lambda[1])));
      goto expose_failure;
    }
    if (!c2)
    {
      fprintf(stderr, "ERROR in implicit_twisting.c, bad value of lambda. test c2 failed\n");
      fprintf(stderr, "<v2-lambda, sigma> = %2.10e\n", (sigma[0]*(v2[0] + lambda[0]) + sigma[1]*(v2[1] + lambda[1])));
      goto expose_failure;
    }

    if (!c3)
    {
      fprintf(stderr, "ERROR in implicit_twisting.c, bad value of lambda. test c3 failed\n");
      fprintf(stderr, "<v3-lambda, sigma> = %2.10e\n", (sigma[0]*(v3[0] + lambda[0]) + sigma[1]*(v3[1] + lambda[1])));
      goto expose_failure;
    }

    if (!c4)
    {
      fprintf(stderr, "ERROR in implicit_twisting.c, bad value of lambda. test c4 failed\n");
      fprintf(stderr, "<v4-lambda, sigma> = %2.10e\n", (sigma[0]*(v4[0] + lambda[0]) + sigma[1]*(v4[1] + lambda[1])));
      goto expose_failure;
    }
    x[0] = sigma[0];
    x[1] = sigma[1];
    q[0] = x[0] + TS*x[1];
    q[1] = x[1];
  }
  while (!info && c1 && c2 && c3 && c4 && (N <= NB_ITER));

  printf("final x: %2.6e %2.6e\n", sigma[0], sigma[1]);

  deleteSolverOptions(&options);
  return info;

expose_failure:
  fprintf(stderr, "x = %2.10e %2.10e\n", x[0], x[1]);
  fprintf(stderr, "q = %2.10e %2.10e\n", q[0], q[1]);
  fprintf(stderr, "lambda = %2.10e %2.10e\n", lambda[0], lambda[1]);

  return 1;
}
