#!/usr/bin/env python
import numpy as np
import siconos.kernel as K

def test_autocast():
    dsA = K.LagrangianDS([0],[0],[[1]])
    dsB = K.FirstOrderLinearDS([0],[[1]])
    model = K.Model(0, 0)
    model.nonSmoothDynamicalSystem().insertDynamicalSystem(dsA)
    model.nonSmoothDynamicalSystem().insertDynamicalSystem(dsB)

    assert(type(model.nonSmoothDynamicalSystem().dynamicalSystem(dsA.number())) == K.LagrangianDS)
    assert(type(model.nonSmoothDynamicalSystem().dynamicalSystem(dsB.number())) == K.FirstOrderLinearDS)


def test_getVector():
    assert (K.getVector([1,2,3]) == np.array([1,2,3])).all()
    v = K.SiconosVector(3)
    v.setValue(0,1)
    v.setValue(1,2)
    v.setValue(2,4)

    assert (K.getVector(v) != np.array([1,2,3])).any()

    assert (K.getVector(v) == np.array([1,2,4])).all()


def test_getMatrix():
    assert (K.getMatrix([[1,2,3]]) == np.array([[1,2,3]])).all()

    m = K.SimpleMatrix(1,3)

    m.setValue(0,0,1)

    m.setValue(0,1,2)

    m.setValue(0,2,3)

    assert (K.getMatrix(m) == np.array([[1,2,3]])).all()

    assert (K.getMatrix(m) != np.array([[1,0,3]])).any()


def test_LagrangianDS_setMassPtr():
    class LDS(K.LagrangianDS):
        pass

    lds = LDS()

    lds.setMassPtr([[1,2,3],[4,5,6]])

    assert (lds.mass() == np.array([[1,2,3],[4,5,6]])).all()


def test_LagrangianScleronomousR_setJachqPtr():
    class Rel(K.LagrangianScleronomousR):
        pass

    r = Rel()
    j = np.array([[1,2,3],[4,5,6]])
    r.setJachqPtr(j)
    # C is transposed()
    r.C()

    assert np.max(r.C() - np.array([[1,2,3],[4,5,6]])) == 0.
    assert np.max(r.C() - np.array([[0,2,3],[4,5,6]])) == 1.

    r.setJachqPtr(r.C())

    r.C()

    assert np.max(r.C() - np.array([[1,2,3],[4,5,6]])) == 0.
    assert np.max(r.C() - np.array([[0,2,3],[4,5,6]])) == 1.


def test_SolverOption():
    lcp = K.LCP()

    i0 = lcp.numericsSolverOptions().iparam[0]

    lcp.numericsSolverOptions().iparam[0] = i0+1

    assert lcp.numericsSolverOptions().iparam[0] != i0

    assert lcp.numericsSolverOptions().iparam[0] == i0+1

    d0 = lcp.numericsSolverOptions().dparam[0]

    lcp.numericsSolverOptions().dparam[0] = 0.5 * d0


    assert lcp.numericsSolverOptions().dparam[0] !=  d0

    assert lcp.numericsSolverOptions().dparam[0] ==  0.5 * d0


def test_BoundaryCondition():
    B = K.BoundaryCondition([1,2,3])

    print(B)

    print(B.velocityIndices())

    B.velocityIndices()[2]=5

    assert (B.velocityIndices() == [1, 2, 5]).all()