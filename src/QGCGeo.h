#ifndef QGCGEO_H
#define QGCGEO_H

#include <QList>
#include <QVector3D>
#include <QQuaternion>
#include <QMatrix3x3>


#define DEG2RAD (M_PI/180.0)

/* Safeguard for systems lacking sincos (e.g. Mac OS X Leopard) */
#ifndef sincos
#define sincos(th,x,y) { (*(x))=sin(th); (*(y))=cos(th); }
#endif

/** @brief A class to hold a 3D vector made with doubles. This class is meant to help isolate the code
 * against external library includes like alglib and Eigen. We can't use the vector supplied by Qt (QVector3d), because
 * that one is made out of floats. We need the extra precision for expressing latitude/longitude, for example.
 */

class Vector3d
{
public:
    /** @brief Constructs an empty, null vector */
    Vector3d(void) : m_x(0.0), m_y(0.0), m_z(0.0) {}

    /** @brief Constructs a vector with coordinates (x, y, z) */
    Vector3d(double x, double y, double z) : m_x(x), m_y(y), m_z(z) {}

    /** @brief Construct a vector initialized with the given vector */
    Vector3d(const Vector3d &rhs) : m_x(rhs.m_x), m_y(rhs.m_y), m_z(rhs.m_z) {}

    /** @brief Construct a vector initialized with the given QVector3D */
    Vector3d(const QVector3D &rhs) : m_x(rhs.x()), m_y(rhs.y()), m_z(rhs.z()) {}

    /** @brief Sets the fields of this vector to the values of the given vector */
    const Vector3d &operator=(const Vector3d &rhs);

    /** @brief Adds the given vector to this vector */
    Vector3d &operator+=(const Vector3d &rhs);

    /** @brief Subtracts the given vector from this vector */
    Vector3d &operator-=(const Vector3d &rhs);

    /** @brief returns the individual coordinates */
    double x(void) const { return m_x; }
    double y(void) const { return m_y; }
    double z(void) const { return m_z; }
    double operator[](int i) const;

    /** @brief sets the individual coordinates */
    void set(double x, double y, double z) { m_x = x; m_y = y; m_z = z; }
    void setX(double x) { m_x = x; }
    void setY(double y) { m_y = y; }
    void setZ(double z) { m_z = z; }
    double &operator[](int i);

    /** @brief returns true, if the x, y and z field is exactly 0.0. Returns false otherwise */
    bool isNull(void) const;

    /** @brief returns true, if this vector is the same as the given vector */
    bool isEqualTo(const Vector3d &other) const;

    /** @brief returns the cartesian length of this vector */
    double length(void) const;

    /** @brief returns the squared length of this vector */
    double lengthSquared(void) const;

    /** @brief calculates the approximate center of a sphere, given the points on the sphere.
      * @returns return true, if the method converged on a value. returns false, if there not enough points.
      * @param pointsOnSphere a list of points that are on the sphere
     **/
    bool setToLeastSquaresSphericalCenter(const QVector<Vector3d> &pointsOnSphere);

    /** @brief returns a QVector3d for this vector.
      * (Note: QVector3d uses floats to store the fields, so some rounding will occur.)
     **/
    QVector3D toQVector3D(void) const;

    /** @brief calculates the dot product of this vector with the given vector */
    double dot(const Vector3d &v) const;

    /** @brief calcuates the dot product of the two given vectors */
    static double dotProduct(const Vector3d &v1, const Vector3d &v2);

    /** @brief calculates and stores the cross product of this vector with the given vector */
    Vector3d &cross(const Vector3d &v);

    /** @brief returns the cross product of the two given vectors */
    static Vector3d crossProduct(const Vector3d &lhs, const Vector3d &rhs);

    /** @brief rotates the current vector with the given quaternion. */
    Vector3d &rotateWithQuaternion(const QQuaternion &q);

    /** @brief returns the vector, rotated by the given quaternion */
    static Vector3d rotateVectorWithQuaternion(const Vector3d &v, const QQuaternion &q);

protected:
    double m_x;
    double m_y;
    double m_z;
};

const Vector3d crossProduct(const Vector3d &lhs, const Vector3d &rhs);
bool operator==(const Vector3d &lhs, const Vector3d &rhs);
bool operator!=(const Vector3d &lhs, const Vector3d &rhs);
const Vector3d operator+(const Vector3d &v1, const Vector3d &v2);
const Vector3d operator-(const Vector3d &v1, const Vector3d &v2);
const Vector3d operator*(double scalar, const Vector3d &v);
const Vector3d operator*(const Vector3d &v, double scalar);

/** Convert a rotation matrix to a quaternion */
QQuaternion quaternionFromMatrix3x3(const QMatrix3x3 &mat);

#endif // QGCGEO_H
