/* Siconos-Kernel, Copyright INRIA 2005-2012.
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


#include <boost/numeric/ublas/matrix_proxy.hpp>


#include "determinant.hpp"
#include "SiconosVector.hpp"
#include "SimpleMatrix.hpp"
#include "BlockMatrixIterators.hpp"
#include "BlockMatrix.hpp"

#include "SiconosAlgebra.hpp"

using namespace Siconos;

//=======================
//       get norm
//=======================

double SimpleMatrix::normInf() const
{
  if (num == 1)
    return norm_inf(*mat.Dense);
  else if (num == 2)
    return norm_inf(*mat.Triang);
  else if (num == 3)
    return norm_inf(*mat.Sym);
  else if (num == 4)
    return norm_inf(*mat.Sparse);
  else if (num == 5)
    return norm_inf(*mat.Banded);
  else if (num == 6)
    return 0;
  else // if(num==7)
    return 1;
}

void SimpleMatrix::normInfByColumn(SP::SiconosVector vIn) const
{
  if (num == 1)
  {
    if (vIn->size() != size(1))
      RuntimeException::selfThrow("SimpleMatrix::normInfByColumn: the given vector does not have the right length");
    DenseVect tmpV = DenseVect(size(0));
    for (unsigned int i = 0; i < size(1); i++)
    {
       ublas::noalias(tmpV) = ublas::column(*mat.Dense, i);
       (*vIn)(i) = norm_inf(tmpV);
    }
  }
  else
    RuntimeException::selfThrow("SimpleMatrix::normInfByColumn: not implemented for data other than DenseMat");
}
//=======================
//       determinant
//=======================

double SimpleMatrix::det() const
{
  if (num == 1)
    return determinant(*mat.Dense);
  else if (num == 2)
    return determinant(*mat.Triang);
  else if (num == 3)
    return determinant(*mat.Sym);
  else if (num == 4)
    return determinant(*mat.Sparse);
  else if (num == 5)
    return determinant(*mat.Banded);
  else if (num == 6)
    return 0;
  else // if(num==7)
    return 1;
}


void SimpleMatrix::trans()
{
  switch (num)
  {
  case 1:
    *mat.Dense = ublas::trans(*mat.Dense);
    break;
  case 2:
    SiconosMatrixException::selfThrow("SimpleMatrix::trans() failed, the matrix is triangular matrix and can not be transposed in place.");
    break;
  case 3:
    break;
  case 4:
    *mat.Sparse = ublas::trans(*mat.Sparse);
    break;
  case 5:
    *mat.Banded = ublas::trans(*mat.Banded);
    break;
  case 6:
    break;
  case 7:
    break;
  }
  resetLU();
}

void SimpleMatrix::trans(const SiconosMatrix &m)
{
  if (m.isBlock())
    SiconosMatrixException::selfThrow("SimpleMatrix::trans(m) failed, not yet implemented for m being a BlockMatrix.");


  if (&m == this)
    trans();//SiconosMatrixException::selfThrow("SimpleMatrix::trans(m) failed, m = this, use this->trans().");
  else
  {
    unsigned int numM = m.getNum();
    switch (numM)
    {
    case 1:
      if (num != 1)
        SiconosMatrixException::selfThrow("SimpleMatrix::trans(m) failed, try to transpose a dense matrix into another type.");
      noalias(*mat.Dense) = ublas::trans(*m.dense());
      break;
    case 2:
      if (num != 1)
        SiconosMatrixException::selfThrow("SimpleMatrix::trans(m) failed, try to transpose a triangular matrix into a non-dense one.");
      noalias(*mat.Dense) = ublas::trans(*m.triang());
      break;
    case 3:
      *this = m;
      break;
    case 4:
      if (num == 1)
        noalias(*mat.Dense) = ublas::trans(*m.sparse());
      else if (num == 4)
        noalias(*mat.Sparse) = ublas::trans(*m.sparse());
      else
        SiconosMatrixException::selfThrow("SimpleMatrix::trans(m) failed, try to transpose a sparse matrix into a forbidden type (not dense nor sparse).");
      break;
    case 5:
      if (num == 1)
        noalias(*mat.Dense) = ublas::trans(*m.banded());
      else if (num == 5)
        noalias(*mat.Banded) = ublas::trans(*m.banded());
      else
        SiconosMatrixException::selfThrow("SimpleMatrix::trans(m) failed, try to transpose a banded matrix into a forbidden type (not dense nor banded).");
      break;
    case 6:
      *this = m;
      break;
    case 7:
      *this = m;
    }
    // unsigned int tmp = dimRow;
    // dimRow = dimCol;
    // dimCol = tmp;
    resetLU();
  }
}







const SimpleMatrix pow(const SimpleMatrix& m, unsigned int power)
{
  if (m.isBlock())
    SiconosMatrixException::selfThrow("Matrix, pow function: not yet implemented for BlockMatrix.");
  if ( m.size(0) != m.size(1))
    SiconosMatrixException::selfThrow("pow(SimpleMatrix), matrix is not square.");

  if (power > 0)
  {
    unsigned int num = m.getNum();
    if (num == 1)
    {
      DenseMat p = *m.dense();
      for (unsigned int i = 1; i < power; i++)
        p = prod(p, *m.dense());
      return p;
    }
    else if (num == 2)
    {
      TriangMat t = *m.triang();
      for (unsigned int i = 1; i < power; i++)
        t = prod(t, *m.triang());
      return t;
    }
    else if (num == 3)
    {
      SymMat s = *m.sym();
      for (unsigned int i = 1; i < power; i++)
        s = prod(s, *m.sym());
      return s;
    }
    else if (num == 4)
    {
      SparseMat sp = *m.sparse();
      for (unsigned int i = 1; i < power; i++)
        sp = prod(sp, *m.sparse());
      return sp;
    }
    else if (num == 5)
    {
      DenseMat b = *m.banded();
      for (unsigned int i = 1; i < power; i++)
        b = prod(b, *m.banded());
      return b;
    }
    else if (num == 6)
    {
      ZeroMat z(m.size(0), m.size(1));
      return z;
    }
    else // if (num==7)
    {
      IdentityMat I(m.size(0), m.size(1));;
      return I;
    }
  }
  else// if(power == 0)
  {
    IdentityMat I = ublas::identity_matrix<double>(m.size(0), m.size(1));
    return I;
  }
}



/*
The following code inverts the matrix input using LU-decomposition with backsubstitution of unit vectors. Reference: Numerical Recipies in C, 2nd ed., by Press, Teukolsky, Vetterling & Flannery.

you can solve Ax=b using three lines of ublas code:

permutation_matrix<> piv;
lu_factorize(A, piv);
lu_substitute(A, piv, x);

*/
#ifndef INVERT_MATRIX_HPP
#define INVERT_MATRIX_HPP

// REMEMBER to update "lu.hpp" header includes from boost-CVS
#include <boost/numeric/ublas/vector.hpp>
#include <boost/numeric/ublas/vector_proxy.hpp>
#include <boost/numeric/ublas/matrix.hpp>
#include <boost/numeric/ublas/triangular.hpp>
#include <boost/numeric/ublas/lu.hpp>
#include <boost/numeric/ublas/io.hpp>

/* Matrix inversion routine.
Uses lu_factorize and lu_substitute in uBLAS to invert a matrix */
template<class T, class U, class V>
bool InvertMatrix(const ublas::matrix<T, U, V>& input, ublas::matrix<T, U, V>& inverse)
{
  using namespace boost::numeric::ublas;
  typedef permutation_matrix<std::size_t> pmatrix;
// create a working copy of the input
  matrix<T, U, V> A(input);
// create a permutation matrix for the LU-factorization
  pmatrix pm(A.size1());

// perform LU-factorization
  int res = lu_factorize(A,pm);
  if (res != 0) return false;

// create identity matrix of "inverse"
  inverse.assign(ublas::identity_matrix<T>(A.size1()));

// backsubstitute to get the inverse
  lu_substitute(A, pm, inverse);

  return true;
}

#endif //INVERT_MATRIX_HPP

void invertMatrix(const SimpleMatrix& input, SimpleMatrix& output)
{
  InvertMatrix(*input.dense(), *output.dense());
}


/* XXX Find out if we can use an elementwise ublas operation */
SP::SiconosVector compareMatrices(const SimpleMatrix& data, const SimpleMatrix& ref)
{
  SimpleMatrix diff(data.size(0), data.size(1));
  SP::SiconosVector res(new SiconosVector(data.size(1)));
  diff = data - ref;
  for (unsigned int i = 0; i < data.size(0); ++i)
  {
    for (unsigned int j = 0; j < data.size(1); ++j)
      diff(i, j) /= 1 + fabs(ref(i, j));
  }
  diff.normInfByColumn(res);
  return res;

}

