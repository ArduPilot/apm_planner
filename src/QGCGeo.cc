/**
 * @file
 *   @brief Implementation of class Vector3d and friends
 *
 *   @author Roland Bosa <roland.bosa@gmail.com>
 *   @author Bill Bonney <billbonney@communstech.com>
 *
 */

#include "QGCGeo.h"
#include <stdexcept>

// Using alglib for least squares calc
#include "libs/alglib/src/ap.h"
#include "libs/alglib/src/optimization.h"
#include "libs/alglib/src/interpolation.h"


const Vector3d &Vector3d::operator=(const Vector3d &rhs)
{
    m_x = rhs.m_x;
    m_y = rhs.m_y;
    m_z = rhs.m_z;
    return *this;
}

Vector3d &Vector3d::operator+=(const Vector3d &rhs)
{
    m_x += rhs.m_x;
    m_y += rhs.m_y;
    m_z += rhs.m_z;
    return *this;
}

Vector3d &Vector3d::operator-=(const Vector3d &rhs)
{
    m_x -= rhs.m_x;
    m_y -= rhs.m_y;
    m_z -= rhs.m_z;
    return *this;
}

double Vector3d::operator[](int i) const
{
    if (0 == i)
    {
        return m_x;
    }
    else if (1 == i)
    {
        return m_y;
    }
    else if (2 == i)
    {
        return m_z;
    }
    throw new std::out_of_range("subscript index to Vector3d is out of range");
}

double &Vector3d::operator[](int i)
{
    if (0 == i)
    {
        return m_x;
    }
    else if (1 == i)
    {
        return m_y;
    }
    else if (2 == i)
    {
        return m_z;
    }
    throw new std::out_of_range("subscript index to Vector3d is out of range!");
}

bool Vector3d::isNull(void) const
{
    return ((0.0 == m_x) && (0.0 == m_y) && (0.0 == m_z));
}

bool Vector3d::isEqualTo(const Vector3d &other) const
{
    return ((m_x == other.m_x) && (m_y == other.m_y) && (m_z == other.m_z));
}

double Vector3d::length(void) const
{
    return sqrt(lengthSquared());
}

double Vector3d::lengthSquared(void) const
{
    return (m_x * m_x) + (m_y * m_y) + (m_z * m_z);
}

static void sphereError(const alglib::real_1d_array &xi, alglib::real_1d_array &fi, void *obj)
{
    Vector3d offset(xi[0], xi[1], xi[2]);
    double xofs = xi[0];
    double yofs = xi[1];
    double zofs = xi[2];
    double r = xi[3];

    const QVector<Vector3d> &rawImuVector = *reinterpret_cast<QVector<Vector3d> *>(obj);
    for (int count = 0; count < rawImuVector.count(); ++count)
    {
        Vector3d d = rawImuVector[count];
        double x = d.x();
        double y = d.y();
        double z = d.z();
        double err = r - sqrt(pow((x + xofs), 2) + pow((y + yofs), 2) + pow((z + zofs), 2));
        fi[count] = err;
    }
}

bool Vector3d::setToLeastSquaresSphericalCenter(const QVector<Vector3d> &pointsOnSphere)
{
    if (pointsOnSphere.count() < 10)
    {
        return false;
    }

    // initialize the set
    alglib::real_1d_array x("[0.0 , 0.0 , 0.0 , 0.0]");
    alglib::minlmstate state;
    alglib::minlmcreatev(pointsOnSphere.count(), x, 100.0f,  state);

    // termination conditions
    double epsg = 0.0000000001;
    double epsf = 0;
    double epsx = 0;
    int maxits = 0;
    alglib::minlmsetcond(state, epsg, epsf, epsx, maxits);

    // optimize it!
    alglib::minlmoptimize(state, &sphereError, NULL, (void *)&pointsOnSphere);

    // retrieve output report
    alglib::minlmreport rep;
    alglib::minlmresults(state, x, rep);

    // TODO: decide to run more optimization, if the termination type is 7 (see definition)
    //QLOG_INFO() << "rep.terminationType" << rep.terminationtype;
    //        QLOG_DEBUG() << "alglib" << alglib::ap::format(x, 2));

    set(x[0], x[1], x[2]);
    return true;
}

QVector3D Vector3d::toQVector3D(void) const
{
    return QVector3D(m_x, m_y, m_z);
}

double Vector3d::dot(const Vector3d &v) const
{
    return m_x * v.m_x + m_y * v.m_y + m_z * v.m_z;
}

double Vector3d::dotProduct(const Vector3d &v1, const Vector3d &v2)
{
    return v1.m_x * v2.m_x + v1.m_y * v2.m_y + v1.m_z * v2.m_z;
}

Vector3d &Vector3d::cross(const Vector3d &v)
{
    *this = crossProduct(*this, v);
    return *this;
}

Vector3d Vector3d::crossProduct(const Vector3d &lhs, const Vector3d &rhs)
{
    Vector3d result(lhs.y() * rhs.z() - rhs.y() * lhs.z(),
                    lhs.z() * rhs.x() - rhs.z() * lhs.x(),
                    lhs.x() * rhs.y() - rhs.x() * lhs.y());
    return result;
}

Vector3d &Vector3d::rotateWithQuaternion(const QQuaternion &q)
{
    /** @link http://gamedev.stackexchange.com/questions/28395/rotating-vector3-by-a-quaternion */
    // Extract the vector part of the quaternion
    Vector3d u(q.vector());

    // Extract the scalar part of the quaternion
    float s = q.scalar();

    // Do the math
    Vector3d v(*this);
    *this = 2.0 * dotProduct(u, v) * u
            + (s*s - dotProduct(u, u)) * v
            + 2.0 * s * crossProduct(u, v);
    return *this;
}

Vector3d Vector3d::rotateVectorWithQuaternion(const Vector3d &v, const QQuaternion &q)
{
    Vector3d result(v);
    result.rotateWithQuaternion(q);
    return result;
}

bool operator==(const Vector3d &lhs, const Vector3d &rhs)
{
    return lhs.isEqualTo(rhs);
}

bool operator!=(const Vector3d &lhs, const Vector3d &rhs)
{
    return !lhs.isEqualTo(rhs);
}

const Vector3d operator+(const Vector3d &v1, const Vector3d &v2)
{
    return Vector3d(v1.x() + v2.x(), v1.y() + v2.y(), v1.z() + v2.z());
}

const Vector3d operator-(const Vector3d &v1, const Vector3d &v2)
{
    return Vector3d(v1.x() - v2.x(), v1.y() - v2.y(), v1.z() - v2.z());
}

const Vector3d operator*(double scalar, const Vector3d &v)
{
    return Vector3d(v.x() * scalar, v.y() * scalar, v.z() * scalar);
}

const Vector3d operator*(const Vector3d &v, double scalar)
{
    return Vector3d(v.x() * scalar, v.y() * scalar, v.z() * scalar);
}

/** @link http://www.flipcode.com/documents/matrfaq.html#Q55 */
QQuaternion quaternionFromMatrix3x3(const QMatrix3x3 &mat)
{
    QVector3D vector;
    float scalar;
    float t = mat(0, 0) + mat(1, 1) + mat(2, 2);
    if (t > 0.0f)
    {
        t = sqrtf(t + 1.0f);
        scalar = 0.5f * t;
        t = 0.5f / t;
        vector[0] = (mat(2, 1) - mat(1, 2)) * t;
        vector[1] = (mat(0, 2) - mat(2, 0)) * t;
        vector[2] = (mat(1, 0) - mat(0, 1)) * t;
    }
    else
    {
        int i = 0;
        if (mat(1, 1) > mat(0, 0))
            i = 1;
        if (mat(2, 2) > mat(i, i))
            i = 2;
        int j = (i + 1) % 3;
        int k = (j + 1) % 3;

        t = sqrtf(mat(i, i) - mat(j, j) - mat(k, k) + 1.0f);
        vector[i] = 0.5f * t;
        t = 0.5f / t;
        scalar = (mat(k, j) - mat(j, k)) * t;
        vector[j] = (mat(j, i) + mat(i, j)) * t;
        vector[k] = (mat(k, i) + mat(i, k)) * t;
    }
    return QQuaternion(scalar, vector);
}
