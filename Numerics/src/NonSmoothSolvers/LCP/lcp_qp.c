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

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include "LCP_Solvers.h"
#include "QP_Solvers.h"

void lcp_qp(LinearComplementarityProblem* problem, double *z, double *w, int *info , SolverOptions* options)
{
  /* size of the LCP */
  int n = problem->size;

  int i, j;

  int nmax;
  int m, me, mmax, mnn;


  double *Q, *A;
  double *p, *b, *xl, *xu;

  double *lambda;

  int lwar, liwar, iout, un;
  int *iwar;
  double *war;

  double tol = options->dparam[0];

  /*/ m :        total number of constraints.*/
  m = 0;
  /*/ me :       number of equality constraints.*/
  me = 0;
  /*/  mmax :     row dimension of a. mmax must be at least one and greater than m.*/
  mmax = m + 1;
  /*/n :        number of variables.
  //nmax :     row dimension of C. nmax must be greater or equal to n.*/
  nmax = n;
  /*/mnn :      must be equal to m + n + n. */
  mnn = m + n + n;

  for (i = 0; i < n; i++)
  {
    z[i] = 0.0;
    w[i] = 0.0;
  }

  /*/ Creation of objective function matrix Q and the the constant vector of the objective function p

  // Q= M;*/
  double * vec = problem->M->matrix0;
  Q = (double *)malloc(nmax * nmax * sizeof(double));
  for (j = 0; j < n; j++)
  {
    for (i = 0; i < n; i++) Q[j * nmax + i] = vec[j * n + i];
  }

  p = (double *)malloc(nmax * sizeof(double));
  for (i = 0; i < n; i++)
    p[i] = problem->q[i] ;

  /* / Creation of the data matrix of the linear constraints, A and  the constant data of the linear constraints b*/
  A = (double *)calloc(mmax * nmax, sizeof(double));

  b = (double *)calloc(mmax, sizeof(double));

  /* Creation of the the lower and upper bounds for the variables.*/
  xu = (double *)malloc(n * sizeof(double));
  for (i = 0; i < n; i++) xu[i] = 1e32 ;
  xl = (double *)calloc(n, sizeof(double));

  /*  on return, lambda contains the lagrange multipliers.*/
  lambda = (double *)malloc(mnn * sizeof(double));

  /* /   integer indicating the desired output unit number,*/
  iout = 6;

  /* /   output control.*/
  un = 1;

  /* / real working array. */
  lwar = 3 * nmax * nmax / 2 + 10 * nmax + 2 * mmax;
  war = (double *)malloc(lwar * sizeof(double));
  /* / integer working array. */
  liwar = n ;
  iwar = (int *)malloc(liwar * sizeof(int));
  iwar[0] = 1;


  /* / call ql0001_ */
  ql0001_(&m, &me, &mmax, &n, &nmax, &mnn, Q, p, A, b, xl, xu,
          z, lambda, &iout, info, &un, war, &lwar, iwar, &liwar, &tol);
  /* /    printf("tol = %10.4e\n",*tol);
  //for (i=0;i<mnn;i++) printf("lambda[%i] = %g\n",i,lambda[i]);

  // getting the multiplier due to the lower bounds*/
  for (i = 0; i < n; i++) w[i] = lambda[m + i] ;

  /*/ memory freeing*/
  free(A);
  free(p);
  free(b);
  free(xu);
  free(xl);
  free(lambda);
  free(war);
  free(iwar);
  free(Q);

}
int linearComplementarity_qp_setDefaultSolverOptions(SolverOptions* options)
{
  int i;
  if (verbose > 0)
  {
    printf("Set the Default SolverOptions for the QP Solver\n");
  }


  /*  strcpy(options->solverName,"QP");*/
  options->solverId = SICONOS_LCP_QP;
  options->numberOfInternalSolvers = 0;
  options->isSet = 1;
  options->filterOn = 1;
  options->iSize = 5;
  options->dSize = 5;
  options->iparam = (int *)malloc(options->iSize * sizeof(int));
  options->dparam = (double *)malloc(options->dSize * sizeof(double));
  options->dWork = NULL;
  options->iWork = NULL;   options->callback = NULL; options->numericsOptions = NULL;
  for (i = 0; i < 5; i++)
  {
    options->iparam[i] = 0;
    options->dparam[i] = 0.0;
  }
  options->dparam[0] = 1e-6;


  return 0;
}
