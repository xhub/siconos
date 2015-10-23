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
#ifndef GLOBALFRICTIONCONTACT3DSOLVERS_H
#define GLOBALFRICTIONCONTACT3DSOLVERS_H

/*!\file gfc3d_Solvers.h
  Subroutines for the resolution of contact problems with friction (3-dimensional case).\n

  \author Vincent Acary

*/

/*! \page GlobalFC3DSolvers Global Friction-Contact 3D problems Solvers

This page gives an overview of the available solvers for friction-contact (3D) problems and their required parameters.

For each solver, the input argument are:
- a FrictionContactProblem
- the unknowns (reaction,velocity)
- info, the termination value (0: convergence, >0 problem which depends on the solver)
- a SolverOptions structure, which handles iparam and dparam

\section pfc3Dnsgs Non-Smooth Gauss Seidel Solver

 function: fc3d_nsgs()
 parameters:


*/
#include "GlobalFrictionContactProblem.h"
#include "NumericsOptions.h"
#include "SolverOptions.h"
#include "Friction_cst.h"
#include "gfc3d_nonsmooth_Newton_AlartCurnier.h"

typedef void (*SolverGlobalPtr)(int, int, double*, int*, double*);
typedef void (*PostSolverGlobalPtr)(int, double*);
typedef void (*ComputeErrorGlobalPtr)(GlobalFrictionContactProblem*, double*, double*, double *, double, double*);
typedef void (*FreeSolverGlobalPtr)(GlobalFrictionContactProblem*);




#if defined(__cplusplus) && !defined(BUILD_AS_CPP)
extern "C"
{
#endif
  /** General interface to solvers for global friction-contact 3D problem
    \param[in] problem the structure which handles the Friction-Contact problem
    \param[in,out] reaction global vector (n)
    \param[in,out] velocity global vector (n)
    \param[in,out] globalVelocity global vector
    \param[in,out] options structure used to define the solver(s) and their parameters
    \param[in] global_options general options for Numerics (verbose mode ...)
    \return result (0 if successful otherwise 1).
  */
  int gfc3d_driver(GlobalFrictionContactProblem* problem, double *reaction ,
                                     double *velocity, double* globalVelocity,
                                     SolverOptions* options, NumericsOptions* global_options);

  /** set the default solver parameters and perform memory allocation for gfc3d
      \param options the pointer to the array of options to set
      \param solverId int identifier of the solver
  */
  int gfc3d_setDefaultSolverOptions(SolverOptions* options, int solverId);

  /** Check for trivial solution in the friction-contact 3D problem
       \param dim of the problem
       \param q global vector (n)
       \param velocity global vector (n), in-out parameter
       \param reaction global vector (n), in-out parameters
       \param globalVelocity the velocity in global coordinates
       \param options the pointer to the array of options to set
       \return int =0 if a trivial solution has been found, else = -1
   */
  int checkTrivialCaseGlobal(int dim, double* q, double* velocity, double*reaction, double* globalVelocity, SolverOptions* options);

  /** Non-Smooth Gauss Seidel solver with reformulation for friction-contact 3D problem
      \param problem the friction-contact 3D problem to solve
      \param velocity global vector (n), in-out parameter
      \param reaction global vector (n), in-out parameters
      \param globalVelocity global vector (m), in-out parameters
      \param info return 0 if the solution is found
      \param options the solver options :
      iparam[0] : Maximum iteration number
      iparam[4] : localsolver choice 0: projection on Cone, 1: Newton/AlartCurnier,  2: projection on Cone with local iteration, 2: projection on Disk  with diagonalization,
      dparam[0] : tolerance
      dparam[2] : localtolerance
      dparam[1] : (out) error
  */
  void gfc3d_nsgs_wr(GlobalFrictionContactProblem* problem, double *reaction , double *velocity, double* globalVelocity, int* info,  SolverOptions* options);

  int gfc3d_nsgs_wr_setDefaultSolverOptions(SolverOptions* options);

  int gfc3d_globalAlartCurnier_wr_setDefaultSolverOptions(SolverOptions* options);

  void  gfc3d_globalAlartCurnier_wr(GlobalFrictionContactProblem* problem, double *reaction , double *velocity, double* globalVelocity, int *info, SolverOptions* options);

  /** Proximal point solver with reformulation for friction-contact 3D problem
    \param problem the friction-contact 3D problem to solve
    \param velocity global vector (n), in-out parameter
    \param reaction global vector (n), in-out parameters
    \param globalVelocity global vector (m), in-out parameters
    \param info return 0 if the solution is found
    \param options the solver options :
    iparam[0] : Maximum iteration number
    iparam[4] : localsolver choice 0: projection on Cone, 1: Newton/AlartCurnier,  2: projection on Cone with local iteration, 2: projection on Disk  with diagonalization,
    dparam[0] : tolerance
    dparam[2] : localtolerance
    dparam[1] : (out) error
  */
  void gfc3d_proximal_wr(GlobalFrictionContactProblem* problem, double *reaction , double *velocity, double* globalVelocity, int* info,  SolverOptions* options);

  int gfc3d_proximal_wr_setDefaultSolverOptions(SolverOptions* options);

  /** Fixed Point iteration on De Saxe formulation solver with reformulation for friction-contact 3D problem
     \param problem the friction-contact 3D problem to solve
     \param velocity global vector (n), in-out parameter
     \param reaction global vector (n), in-out parameters
     \param globalVelocity global vector (m), in-out parameters
     \param info return 0 if the solution is found
     \param options the solver options :
     iparam[0] : Maximum iteration number
     dparam[0] : tolerance
     dparam[2] : localtolerance
     dparam[1] : (out) error
  */
  void gfc3d_DeSaxceFixedPoint_wr(GlobalFrictionContactProblem* problem, double *reaction , double *velocity, double* globalVelocity, int* info,  SolverOptions* options);

  int gfc3d_DeSaxceFixedPoint_setDefaultSolverOptions(SolverOptions* options);

  /** Fied Point iteration on Tresca Friction Cylinder with reformulation for friction-contact 3D problem
     \param problem the friction-contact 3D problem to solve
     \param velocity global vector (n), in-out parameter
     \param reaction global vector (n), in-out parameters
     \param globalVelocity global vector (m), in-out parameters
     \param info return 0 if the solution is found
     \param options the solver options :
     iparam[0] : Maximum iteration number
     dparam[0] : tolerance
     dparam[2] : localtolerance
     dparam[1] : (out) error
  */
  void gfc3d_TrescaFixedPoint_wr(GlobalFrictionContactProblem* problem, double *reaction , double *velocity, double* globalVelocity, int* info,  SolverOptions* options);

  int gfc3d_TrescaFixedPoint_setDefaultSolverOptions(SolverOptions* options);

  /**  Non-Smooth Gauss Seidel solver  for friction-contact 3D problem with iteration on velocities
        \param problem the friction-contact 3D problem to solve
        \param velocity global vector (n), in-out parameter
        \param reaction global vector (n), in-out parameters
        \param globalVelocity global vector (m), in-out parameters
        \param info return 0 if the solution is found
        \param options the solver options :
        iparam[0] : Maximum iteration number
        dparam[0] : tolerance
        dparam[2] : localtolerance
        dparam[1] : (out) error
    */
  void  gfc3d_nsgs_velocity_wr(GlobalFrictionContactProblem* problem, double *reaction , double *velocity, double* globalVelocity, int *info, SolverOptions* options);

  int gfc3d_nsgs_velocity_wr_setDefaultSolverOptions(SolverOptions* options);

  /** Non-Smooth Gauss Seidel solver  for friction-contact 3D problem
        \param problem the friction-contact 3D problem to solve
        \param velocity global vector (n), in-out parameter
        \param reaction global vector (n), in-out parameters
        \param globalVelocity global vector (m), in-out parameters
        \param info return 0 if the solution is found
        \param options the solver options :
        iparam[0] : Maximum iteration number
        iparam[4] ; local strategy
        dparam[0] : tolerance
        dparam[2] : localtolerance
        dparam[1] : (out) error
        \todo Implement ProdTransSBM
        \todo Improve the splitting Algorithm with a smaller granularity
        \todo Use a global projection perhaps
    */
  void gfc3d_nsgs(GlobalFrictionContactProblem* problem, double *reaction , double *velocity, double* globalVelocity, int* info, SolverOptions* options);

   /** Solver based on the fixed-point iteration proposed by Cadoux for friction-contact 3D problem
        \param problem the friction-contact 3D problem to solve
        \param velocity global vector (n), in-out parameter
        \param reaction global vector (n), in-out parameters
        \param globalVelocity global vector (m), in-out parameters
        \param info return 0 if the solution is found
        \param options the solver options :
        iparam[0] : Maximum iteration number
        iparam[4] ; local strategy
        dparam[0] : tolerance
        dparam[2] : localtolerance
        dparam[1] : (out) error
    */
  void gfc3d_FixedPointCadoux(GlobalFrictionContactProblem* problem, double *reaction , double *velocity, double* globalVelocity, int* info, SolverOptions* options);

  /** solver using PATH (via GAMS) for friction-contact 3D problem based on an AVI reformulation
      \param problem the friction-contact 3D problem to solve
      \param velocity global vector (n), in-out parameter
      \param reaction global vector (n), in-out parameters
      \param info return 0 if the solution is found
      \param options the solver options
  */
  void gfc3d_AVI_gams_path(GlobalFrictionContactProblem* problem, double *reaction, double *velocity, int* info, SolverOptions* options);

  /** solver using PATHVI (via GAMS) for friction-contact 3D problem based on an AVI reformulation
      \param problem the friction-contact 3D problem to solve
      \param velocity global vector (n), in-out parameter
      \param reaction global vector (n), in-out parameters
      \param info return 0 if the solution is found
      \param options the solver options
  */
  void gfc3d_AVI_gams_pathvi(GlobalFrictionContactProblem* problem, double *reaction, double *velocity, int* info, SolverOptions* options);



#if defined(__cplusplus) && !defined(BUILD_AS_CPP)
}
#endif

#endif