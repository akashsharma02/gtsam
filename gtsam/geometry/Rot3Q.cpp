/* ----------------------------------------------------------------------------

 * GTSAM Copyright 2010, Georgia Tech Research Corporation, 
 * Atlanta, Georgia 30332-0415
 * All Rights Reserved
 * Authors: Frank Dellaert, et al. (see THANKS for the full author list)

 * See LICENSE for the license information

 * -------------------------------------------------------------------------- */

/**
 * @file    Rot3Q.cpp
 * @brief   Rotation (internal: quaternion representation*)
 * @author  Richard Roberts
 */

#include <gtsam/config.h> // Get GTSAM_USE_QUATERNIONS macro

#ifdef GTSAM_USE_QUATERNIONS

#include <boost/math/constants/constants.hpp>
#include <gtsam/geometry/Rot3.h>
#include <cmath>

using namespace std;

namespace gtsam {

  static const Matrix I3 = eye(3);

  /* ************************************************************************* */
  Rot3::Rot3() : quaternion_(Quaternion::Identity()) {}

  /* ************************************************************************* */
  Rot3::Rot3(const Point3& col1, const Point3& col2, const Point3& col3) :
      quaternion_((Eigen::Matrix3d() <<
          col1.x(), col2.x(), col3.x(),
          col1.y(), col2.y(), col3.y(),
          col1.z(), col2.z(), col3.z()).finished()) {}

  /* ************************************************************************* */
  Rot3::Rot3(double R11, double R12, double R13,
      double R21, double R22, double R23,
      double R31, double R32, double R33) :
        quaternion_((Eigen::Matrix3d() <<
            R11, R12, R13,
            R21, R22, R23,
            R31, R32, R33).finished()) {}

  /* ************************************************************************* */
  Rot3::Rot3(const Matrix3& R) :
      quaternion_(R) {}

  /* ************************************************************************* */
  Rot3::Rot3(const Quaternion& q) :
      quaternion_(q) {
  }

  /* ************************************************************************* */
  Rot3 Rot3::Rx(double t) {
    return Quaternion(Eigen::AngleAxisd(t, Eigen::Vector3d::UnitX()));
  }

  /* ************************************************************************* */
  Rot3 Rot3::Ry(double t) {
    return Quaternion(Eigen::AngleAxisd(t, Eigen::Vector3d::UnitY()));
  }

  /* ************************************************************************* */
  Rot3 Rot3::Rz(double t) {
    return Quaternion(Eigen::AngleAxisd(t, Eigen::Vector3d::UnitZ()));
  }

  /* ************************************************************************* */
  Rot3 Rot3::RzRyRx(double x, double y, double z) { return Rot3(
      Quaternion(Eigen::AngleAxisd(z, Eigen::Vector3d::UnitZ())) *
      Quaternion(Eigen::AngleAxisd(y, Eigen::Vector3d::UnitY())) *
      Quaternion(Eigen::AngleAxisd(x, Eigen::Vector3d::UnitX())));
  }

  /* ************************************************************************* */
  Rot3 Rot3::rodriguez(const Vector3& w, double theta) {
    return Quaternion(Eigen::AngleAxis<double>(theta, w));
  }
  /* ************************************************************************* */
  Rot3 Rot3::operator*(const Rot3& R2) const {
    return Rot3(quaternion_ * R2.quaternion_);
  }

  /* ************************************************************************* */
  // TODO: Could we do this? It works in Rot3M but not here, probably because
  // here we create an intermediate value by calling matrix()
  // const Eigen::Transpose<const Matrix3> Rot3::transpose() const {
  Matrix3 Rot3::transpose() const {
    return matrix().transpose();
  }

  /* ************************************************************************* */
  Point3 Rot3::rotate(const Point3& p,
        OptionalJacobian<3,3> H1,  OptionalJacobian<3,3> H2) const {
    Matrix R = matrix();
    if (H1) *H1 = R * skewSymmetric(-p.x(), -p.y(), -p.z());
    if (H2) *H2 = R;
    Eigen::Vector3d r = R * p.vector();
    return Point3(r.x(), r.y(), r.z());
  }

  /* ************************************************************************* */
  Vector3 Rot3::Logmap(const Rot3& R, OptionalJacobian<3, 3> H) {
    return traits<Quaternion>::Logmap(R.quaternion_, H);
  }

  /* ************************************************************************* */
  Rot3 Rot3::ChartAtOrigin::Retract(const Vector3& omega, ChartJacobian H) {
    static const CoordinatesMode mode = ROT3_DEFAULT_COORDINATES_MODE;
    if (mode == Rot3::EXPMAP) return Expmap(omega, H);
    else throw std::runtime_error("Rot3::Retract: unknown mode");
  }

  /* ************************************************************************* */
  Vector3 Rot3::ChartAtOrigin::Local(const Rot3& R, ChartJacobian H) {
    static const CoordinatesMode mode = ROT3_DEFAULT_COORDINATES_MODE;
    if (mode == Rot3::EXPMAP) return Logmap(R, H);
    else throw std::runtime_error("Rot3::Local: unknown mode");
  }

  /* ************************************************************************* */
  Matrix3 Rot3::matrix() const {return quaternion_.toRotationMatrix();}

  /* ************************************************************************* */
  Point3 Rot3::r1() const { return Point3(quaternion_.toRotationMatrix().col(0)); }

  /* ************************************************************************* */
  Point3 Rot3::r2() const { return Point3(quaternion_.toRotationMatrix().col(1)); }

  /* ************************************************************************* */
  Point3 Rot3::r3() const { return Point3(quaternion_.toRotationMatrix().col(2)); }

  /* ************************************************************************* */
  Quaternion Rot3::toQuaternion() const { return quaternion_; }

 /* ************************************************************************* */

} // namespace gtsam

#endif
