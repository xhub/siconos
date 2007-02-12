/* Siconos-Kernel version 2.0.1, Copyright INRIA 2005-2006.
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
/*! \file
  Newton-Impact Non-Smooth Law
*/

#ifndef NEWTONIMPACTFRICTIONNSLAW_H
#define NEWTONIMPACTFRICTIONNSLAW_H

#include "NonSmoothLaw.h"
class NonSmoothLaw;

/** Newton Impact-Friction Non Smooth Law
 *
 *  \author SICONOS Development Team - copyright INRIA
 *  \version 2.0.1.
 *  \date (Creation) March 22, 2005
 *
 *
 */
class NewtonImpactFrictionNSL : public NonSmoothLaw
{

private:
  /*/** The Newton coefficient of restitution
   */
  double en;
  double et;
  /** friction coefficient */
  double mu;

public:

  /** default constructor
   *  \param unsigned int: size of the ns law, default = 2
   */
  NewtonImpactFrictionNSL(const unsigned int = 2);

  /** constructor with XML object of the NewtonImpactFrictionNSL
   *  \param NonSmoothLawXML* : the XML object corresponding
   */
  NewtonImpactFrictionNSL(NonSmoothLawXML*);

  /** constructor with the value of the NewtonImpactFrictionNSL attributes
   *  \param double : normal e coefficient
   *  \param double : tangent e coefficient
   *  \param double : friction coefficient
   *  \param unsigned int: size of the ns law, default = 2
   */
  NewtonImpactFrictionNSL(const double, const double, const double, const unsigned int);

  /** Destructor */
  ~NewtonImpactFrictionNSL();

  /** check the ns law to see if it is verified
   *  \return a boolean value whioch determines if the NS Law is verified
   */
  bool isVerified(void) const;

  // GETTERS/SETTERS

  /** getter of en
   *  \return the value of en
   */
  inline const double getEn() const
  {
    return en;
  };

  /** setter of en
   *  \param a double to set en
   */
  inline void setEn(const double newVal)
  {
    en = newVal;
  };

  /** getter of et
   *  \return the value of et
   */
  inline const double getEt() const
  {
    return et;
  };

  /** setter of et
   *  \param a double to set et
   */
  inline void setEt(const double newVal)
  {
    et = newVal;
  };

  /** getter of mu
   *  \return the value of mu
   */
  inline const double getMu() const
  {
    return mu;
  };

  /** setter of mu
   *  \param a double to set mu
   */
  inline void setMu(const double newVal)
  {
    mu = newVal;
  };

  // OTHER FUNCTIONS

  /** copy the data of the NonSmoothLaw in the XML tree
   */
  void saveNonSmoothLawToXML();

  /** print the data to the screen
   */
  void display() const;

  /** encapsulates an operation of dynamic casting. Needed by Python interface.
   *  \param NonSmoothLaw* : the law which must be converted
   * \return a pointer on the law if it is of the right type, NULL otherwise
   */
  static NewtonImpactFrictionNSL* convert(NonSmoothLaw* nsl);

};

#endif // NewtonImpactFrictionNSL_H
