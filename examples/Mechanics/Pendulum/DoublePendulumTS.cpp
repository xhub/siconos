/* Siconos-sample , Copyright INRIA 2005-2011.
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
 * Contact: Vincent ACARY vincent.acary@inrialpes.fr
 */


// =============================== Double Pendulum Example ===============================
//
// Author: Vincent Acary
//
// Keywords: LagrangianDS, LagrangianLinear relation, MoreauJeanOSI TimeStepping, LCP.
//
// =============================================================================================

#include "SiconosKernel.hpp"
#include <stdlib.h>
using namespace std;

#include <boost/progress.hpp>

double gravity = 10.0;
double m1 = 1.0;
double m2 = 1.0 ;
double l1 = 1.0 ;
double l2 = 1.0 ;

int main(int argc, char* argv[])
{
  try
  {

    // ================= Creation of the model =======================

    // User-defined main parameters
    unsigned int nDof = 2;           // degrees of freedom for robot arm
    double t0 = 0;                   // initial computation time
    double T = 5.0;                   // final computation time
    double h = 0.0005;                // time step
    double criterion = 0.05;
    unsigned int maxIter = 20000;
    double e = 1.0;                  // nslaw
    double e1 = 0.0;

    // -> mind to set the initial conditions below.

    // -------------------------
    // --- Dynamical systems ---
    // -------------------------

    // --- DS: Double Pendulum ---

    // Initial position (angles in radian)
    SP::SiconosVector q0(new SiconosVector(nDof));
    SP::SiconosVector v0(new SiconosVector(nDof));

    q0->zero();
    v0->zero();

    // (*q0)(0) = 1.5;
    // (*q0)(1) = 1.5;

    // for sympy plugins uncomment below (relative parametrization)
    // Note, we have the relation :
    // absolute[(*q0)(0)] + relative[(*q0)(1)] = absolute[(*q0)(1)]
    // (*q0)(0) = 0.1;
    // (*q0)(1) = 0.1;

    (*q0)(0) = 0.1;
    (*q0)(1) = 0.2;

    /*REGULAR PLUGINS - uncomment to use*/
    SP::LagrangianDS doublependulum(new LagrangianDS(q0, v0, "plugins:mass"));
    doublependulum->setComputeFGyrFunction("plugins", "FGyr");
    doublependulum->setComputeJacobianFGyrqDotFunction("plugins", "jacobianVFGyr");
    doublependulum->setComputeJacobianFGyrqFunction("plugins", "jacobianFGyrq");
    doublependulum->setComputeFIntFunction("plugins", "FInt");
    doublependulum->setComputeJacobianFIntqDotFunction("plugins", "jacobianVFInt");
    doublependulum->setComputeJacobianFIntqFunction("plugins", "jacobianFIntq");

    /*SYMPY PLUGINS - uncomment to use*/
    // SP::LagrangianDS doublependulum(new LagrangianDS(q0, v0, "DoublePendulumSymPyPlugin:mass"));
    // doublependulum->setComputeFGyrFunction("DoublePendulumSymPyPlugin", "FGyr");
    // doublependulum->setComputeJacobianFGyrqDotFunction("DoublePendulumSymPyPlugin", "jacobianVFGyr");
    // doublependulum->setComputeJacobianFGyrqFunction("DoublePendulumSymPyPlugin", "jacobianFGyrq");

    // -------------------
    // --- Interactions---
    // -------------------

    // -- relations --

    string G = "plugins:G0";
    SP::NonSmoothLaw nslaw(new NewtonImpactNSL(e));
    SP::Relation relation(new LagrangianScleronomousR("plugins:h0", G));
    SP::Interaction inter(new Interaction(1, nslaw, relation));

    string G1 = "plugins:G1";
    SP::NonSmoothLaw nslaw1(new NewtonImpactNSL(e1));
    SP::Relation relation1(new LagrangianScleronomousR("plugins:h1", G1));
    SP::Interaction inter1(new Interaction(1, nslaw1, relation1));

    // -------------
    // --- Model ---
    // -------------

    SP::Model Pendulum(new Model(t0, T));
    Pendulum->nonSmoothDynamicalSystem()->insertDynamicalSystem(doublependulum);
    Pendulum->nonSmoothDynamicalSystem()->link(inter,doublependulum);
    Pendulum->nonSmoothDynamicalSystem()->link(inter1,doublependulum);

    // ----------------
    // --- Simulation ---
    // ----------------

    // -- Time discretisation --
    SP::TimeDiscretisation t(new TimeDiscretisation(t0, h));

    SP::TimeStepping s(new TimeStepping(t));
    //        s->setUseRelativeConvergenceCriteron(true);

    // -- OneStepIntegrators --

    //double theta=0.500001;
    double theta = 0.500001;

    SP::MoreauJeanOSI OSI(new MoreauJeanOSI(theta));
    OSI->insertDynamicalSystem(doublependulum);
    s->insertIntegrator(OSI);

    // -- OneStepNsProblem --
    SP::OneStepNSProblem osnspb(new LCP());

    s->insertNonSmoothProblem(osnspb);

    cout << "=== End of model loading === " << endl;

    // =========================== End of model definition ===========================  dataPlot(k,7) = (*inter->y(0))(0);


    // ================================= Computation =================================

    // --- Simulation initialization ---
    Pendulum->initialize(s);
    cout << "End of simulation initialisation" << endl;

    int k = 0;
    int N = ceil((T - t0) / h);
    cout << "Number of time step   " << N << endl;
    // --- Get the values to be plotted ---
    // -> saved in a matrix dataPlot
    unsigned int outputSize = 12;
    SimpleMatrix dataPlot(N + 1, outputSize);
    // For the initial time step:
    // time
    SP::SiconosVector q = doublependulum->q();
    SP::SiconosVector v = doublependulum->velocity();

    dataPlot(k, 0) =  t0;
    dataPlot(k, 1) = (*q)(0);
    dataPlot(k, 2) = (*v)(0);
    dataPlot(k, 3) = (*q)(1);
    dataPlot(k, 4) = (*v)(1);
    dataPlot(k, 5) =  l1 * sin((*q)(0));
    dataPlot(k, 6) = -l1 * cos((*q)(0));
    dataPlot(k, 7) =  l1 * sin((*q)(0)) + l2 * sin((*q)(1));
    dataPlot(k, 8) = -l1 * cos((*q)(0)) - l2 * cos((*q)(1));
    dataPlot(k, 9) =  l1 * cos((*q)(0)) * ((*v)(0));
    dataPlot(k, 10) = l1 * cos((*q)(0)) * ((*v)(0)) + l2 * cos((*q)(1)) * ((*v)(1));

    boost::timer time;
    time.restart();

    // --- Time loop ---
    cout << "Start computation ... " << endl;

    boost::progress_display show_progress(N);

    while (s->hasNextEvent())
    {
      k++;
      ++show_progress;
      //  if (!(div(k,1000).rem))  cout <<"Step number "<< k << "\n";

      // Solve problem
      s->newtonSolve(criterion, maxIter);
      // Data Output
      dataPlot(k, 0) =  s->nextTime();
      dataPlot(k, 1) = (*q)(0);
      dataPlot(k, 2) = (*v)(0);
      dataPlot(k, 3) = (*q)(1);
      // sympy plugin with relative parametrization:
      // dataPlot(k, 3) = (*q)(0) + (*q)(1);
      dataPlot(k, 4) = (*v)(1);
      // sympy plugin with relative parametrization:
      // dataPlot(k, 4) = (*v)(0) + (*v)(1);
      dataPlot(k, 5) =  l1 * sin((*q)(0));
      dataPlot(k, 6) = -l1 * cos((*q)(0));
      dataPlot(k, 7) =  l1 * sin((*q)(0)) + l2 * sin((*q)(1));
      dataPlot(k, 8) = -l1 * cos((*q)(0)) - l2 * cos((*q)(1));
      dataPlot(k, 9) =  l1 * cos((*q)(0)) * ((*v)(0));
      dataPlot(k, 10) = l1 * cos((*q)(0)) * ((*v)(0)) + l2 * cos((*q)(1)) * ((*v)(1));
      s->nextStep();
    }

    cout << "End of computation - Number of iterations done: " << k << endl;
    cout << "Computation Time " << time.elapsed()  << endl;

    // --- Output files ---
    ioMatrix::write("DoublePendulumResult.dat", "ascii", dataPlot, "noDim");
  }

  catch (SiconosException e)
  {
    cout << e.report() << endl;
  }
  catch (...)
  {
    cout << "Exception caught in \'sample/MultiBeadsColumn\'" << endl;
  }
}