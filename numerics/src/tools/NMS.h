/* Siconos is a program dedicated to modeling, simulation and control
 * of non smooth dynamical systems.
 *
 * Copyright 2016 INRIA.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
*/

#ifndef NMS_H
#define NMS_H

/*!\file NMS.h
 * \brief Non-Monotone Stabilisation Scheme for path search
 *
 * Main reference for this implementation: Linear Algebra Enhancements to the
 * PATH Solver, by Li, Ferris and Munson
 *
 * \author Olivier Huber
 */

#include "SiconosConfig.h"

#include <float.h>

#include "ArmijoSearch.h"
#include "NumericsMatrix.h"
#include "SiconosSets.h"

//#define DEBUG_STDOUT
//#define DEBUG_MESSAGES
#include "debug.h"


/** \struct path_record NMS.h
 * Save the path generated by the algorithm
 */
typedef struct
{
  int* final_basis; /**< last basis before exit */
  int* basis_change; /**< record of the changes in the basis (leaving variable and index) use to
                       reconstruct the path. it is used as a circular buffer */
  int circular_indx; /**< index to mark the current position in the buffer */
} path_record;


/** \struct NMS_data NMS.h
 * Data for the Non-Monotone Stabilisation scheme
 */
typedef struct
{
  unsigned size; /**< dimension of the space */
  int watchdog_search_type; /**< type of search for the watchdog step: 0 -> line search, 1 -> arcsearch, 2 -> path search */
  int projected_gradient_search_type; /**< type of search for the projected gradient step: 0 -> line search, 1 -> arc search */
  double delta; /**< acceptable distance between 2 iterates to qualify as d-step. Value is updated at each iteration */
  double delta_var; /**< variation of delta */
  double sigma; /**< value for the line search decrease test*/
  double alpha_min_watchdog; /**< min value for alpha in line or arc search*/
  double alpha_min_pgrad; /**< min value for alpha in line or arc search*/
  int n; /**< current number of d_step taken */
  int n_max; /**< max number of d_step taken without checking the merit value (m-step) */
  double ref_merit; /**< reference value for the merit function */
  double merit_incr; /**< allowed increment in the merit function for d_step*/
  void* ref_merit_data; /**< data struct containing the data to update the reference value*/
  double* checkpoint; /**< checkpoint (z_c(0), z_c(T)), of size 2*n */
  double delta_checkpoint; /**< value of delta at checkpoint*/
  double* bestpoint; /**< best point, used for projected gradient step */
  double merit_bestpoint; /**< value of the merit function at the best point*/
  double delta_bestpoint; /**< value of delta at best point */
  double* workspace; /**< allocation memory for all the necessary computations */
  NumericsMatrix* H; /**< NumericsMatrix for the computation of the jacobian matrix */
  void* set; /**< set where z is constrained */
  path_record* path_data; /**< record of the path generated by the algorithm*/
  search_data* ls_data; /**< data for the line search */
} NMS_data;

#if defined(__cplusplus) && !defined(BUILD_AS_CPP)
extern "C"
{
#endif

  /** Non-Monotone Stabilisation scheme
   * \param data_NMS the NMS data
   * \param data opaque data for the computation of the problem
   * \param functions struct holding the functions for the computation of F, the merit function
   * \param[in,out] z on input, current iterate of the solution; on output, the new iterate
   * \param z_N proposed iterate for the solution
   * \param force_watchdog_step force a watchdog step if the path search failed!
   * \param force_d_step_merit_check accept the point at d_step even if the
   * merit function increases. Usually this means that the normal map norm
   * decreases a lot
   * \param check_ratio if the norm of the normal map decreases, allow the
   * merit to increase by that much
   * \return 0 if succeeded, 1 if failed
   */
  int NMS(NMS_data* data_NMS, void* data, functions_LSA* functions, double* z, double* z_N, int force_watchdog_step, int force_d_step_merit_check, double check_ratio);

  /** create (allocate) a NMS_data struct and do some allocation work
   * \param size size of the problem
   * \param matrix_type type of the NumericsMatrix used: dense, sparse, ...
   * \param iparam vector of integer parameters from a SolverOptions struct
   * \param dparam vector of double parameters from a SolverOptions struct
   * \return the allocated struct
   */
  NMS_data* create_NMS_data(unsigned size, int matrix_type, int* iparam, double* dparam);

  /** free a NMS_data struct
   * \param data the structure to free
   */
  void free_NMS_data(NMS_data* data);

  /** Get the F vector from the workspace data
   * \param workspace the workspace holding the allocated vectors
   * \param n size of vector
   * \return the F vector
   */
  static inline double* NMS_get_F(double* workspace, int n)
  {
    return &workspace[0];
  }

  /** Get the F_merit vector from the workspace data
   * \param workspace the workspace holding the allocated vectors
   * \param n size of vector
   * \return the F_merit vector
   */
  static inline double* NMS_get_F_merit(double* workspace, int n)
  {
    return &workspace[n];
  }

  /** Get the JacTheta_F_merit vector from the workspace data
   * \param workspace the workspace holding the allocated vectors
   * \param n size of vector
   * \return the JacTheta_F_merit vector
   */
  static inline double* NMS_get_JacTheta_F_merit(double* workspace, int n)
  {
    return &workspace[2*n];
  }

  /** Get the vector for the descent direction from the workspace data
   * \param workspace the workspace holding the allocated vectors
   * \param n size of vector
   * \return the vector for the descent direction
   */
  static inline double* NMS_get_dir(double* workspace, int n)
  {
    return &workspace[3*n];
  }

  /** Get the workV vector from the workspace data
   * \param workspace the workspace holding the allocated vectors
   * \param n size of vector
   * \return the workV vector
   */
  static inline double* NMS_get_generic_workV(double* workspace, int n)
  {
    return &workspace[4*n];
  }

  /** Get the z_c(0) from the workspace data
   * \param data_NMS the data for the NMS scheme
   * \param n size of vector
   * \return the checkpoint z_c(0)
   */
  static inline double* NMS_checkpoint_0(NMS_data* data_NMS, unsigned n)
  {
    return &data_NMS->checkpoint[n];
  }

  /** Get the z_c(T) from the workspace data
   * \param data_NMS the data for the NMS scheme
   * \param n size of vector
   * \return the checkpoint z_c(T)
   */
  static inline double* NMS_checkpoint_T(NMS_data* data_NMS, unsigned n)
  {
    return data_NMS->checkpoint;
  }

  /** Get the bestpoint z_b from the workspace data
   * \param data_NMS the data for the NMS scheme
   * \param n size of vector
   * \return the bestpoint z_b
   */
  static inline double* NMS_bestpoint(NMS_data* data_NMS, unsigned n)
  {
    return data_NMS->bestpoint;
  }

  /** check the non-monotone descent criterion
   * \param theta_iter current theta value
   * \param ref_merit the reference value for the merit function
   * \param sigma the \f$\sigma\f$ parameter
   * \param dotprod the value of the inner product between JacThetaF_merit and
   * the descent direction
   * \return 0 if the step is accepted, 1 otherwise
   */
  static inline int check_nmd_criterion(double theta_iter, double ref_merit, double sigma, double dotprod)
  {
    if (dotprod < 0.0)
    {
      /* check whether 0.5 \|F_merit(z_N)\|^2 <=  merit_R -
       * sigma*Jac_F_merit(z - z_N) */
      if (theta_iter <= ref_merit - sigma*dotprod)
      {
        DEBUG_PRINTF("NMS nmd accepted theta_iter = %2.2e < tol = %2.2e\n", theta_iter, ref_merit - sigma*dotprod);
        return 0;
      }
      /* else watchdog step */
        DEBUG_PRINTF("NMS nmd rejected theta_iter = %2.2e; tol = %2.2e; diff = %2.2e\n", theta_iter, ref_merit - sigma*dotprod, theta_iter - (ref_merit - sigma*dotprod));
    }
    else /* Just ensure a sufficient decrease of the reference value */
    {
      if (theta_iter <= (1.0 - sigma)*ref_merit)
      {
        DEBUG_PRINTF("NMS nmd accepted theta_iter = %2.2e < tol = %2.2e\n", theta_iter, (1.0 - sigma)*ref_merit);
        return 0;
      }
      /* else watchdog step */
        DEBUG_PRINTF("NMS nmd rejected theta_iter = %2.2e; tol = %2.2e; diff = %2.2e\n", theta_iter, (1.0 - sigma)*ref_merit, theta_iter - (1.0 - sigma)*ref_merit);
    }
    return 1;
  }

#if defined(__cplusplus) && !defined(BUILD_AS_CPP)
}
#endif

#endif
