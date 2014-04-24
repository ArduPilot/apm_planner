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
 * against external library includes like alglib and Eigen. We can't use the vector supplied by Qt (QVector3D), because
 * that one is made out of floats. We need the extra precision for expressing latitude/longitude, for example.
 */

class Vector3D;
typedef QList<Vector3D> Vector3DList;

class Vector3D
{
public:
    /** @brief Constructs an empty, null vector */
    Vector3D(void) : m_x(0.0), m_y(0.0), m_z(0.0) {}

    /** @brief Constructs a vector with coordinates (x, y, z) */
    Vector3D(double x, double y, double z) : m_x(x), m_y(y), m_z(z) {}

    /** @brief Construct a vector initialized with the given vector */
    Vector3D(const Vector3D &rhs) : m_x(rhs.m_x), m_y(rhs.m_y), m_z(rhs.m_z) {}

    /** @brief Construct a vector initialized with the given QVector3D */
    Vector3D(const QVector3D &rhs) : m_x(rhs.x()), m_y(rhs.y()), m_z(rhs.z()) {}

    /** @brief Sets the fields of this vector to the values of the given vector */
    const Vector3D &operator=(const Vector3D &rhs);

    /** @brief Adds the given vector to this vector */
    Vector3D &operator+=(const Vector3D &rhs);

    /** @brief Subtracts the given vector from this vector */
    Vector3D &operator-=(const Vector3D &rhs);

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
    bool isEqualTo(const Vector3D &other) const;

    /** @brief returns the cartesian length of this vector */
    double length(void) const;

    /** @brief returns the squared length of this vector */
    double lengthSquared(void) const;

    /** @brief calculates the approximate center of a sphere, given the points on the sphere.
      * @returns return true, if the method converged on a value. returns false, if there not enough points.
      * @param pointsOnSphere a list of points that are on the sphere
     **/
    bool setToLeastSquaresSphericalCenter(const Vector3DList &pointsOnSphere);

    /** @brief returns a QVector3D for this vector.
      * (Note: QVector3D uses floats to store the fields, so some rounding will occur.)
     **/
    QVector3D toQVector3D(void) const;

    /** @brief calculates the dot product of this vector with the given vector */
    double dot(const Vector3D &v) const;

    /** @brief calcuates the dot product of the two given vectors */
    static double dotProduct(const Vector3D &v1, const Vector3D &v2);

    /** @brief calculates and stores the cross product of this vector with the given vector */
    Vector3D &cross(const Vector3D &v);

    /** @brief returns the cross product of the two given vectors */
    static Vector3D crossProduct(const Vector3D &lhs, const Vector3D &rhs);

    /** @brief rotates the current vector with the given quaternion. */
    Vector3D &rotateWithQuaternion(const QQuaternion &q);

    /** @brief returns the vector, rotated by the given quaternion */
    static Vector3D rotateVectorWithQuaternion(const Vector3D &v, const QQuaternion &q);

protected:
    double m_x;
    double m_y;
    double m_z;
};

const Vector3D crossProduct(const Vector3D &lhs, const Vector3D &rhs);
bool operator==(const Vector3D &lhs, const Vector3D &rhs);
bool operator!=(const Vector3D &lhs, const Vector3D &rhs);
const Vector3D operator+(const Vector3D &v1, const Vector3D &v2);
const Vector3D operator-(const Vector3D &v1, const Vector3D &v2);
const Vector3D operator*(double scalar, const Vector3D &v);
const Vector3D operator*(const Vector3D &v, double scalar);

/** Convert a rotation matrix to a quaternion */
QQuaternion quaternionFromMatrix3x3(const QMatrix3x3 &mat);

/**
 * Converting from latitude / longitude to tangent on earth surface
 * @link http://psas.pdx.edu/CoordinateSystem/Latitude_to_LocalTangent.pdf
 * @link http://dspace.dsto.defence.gov.au/dspace/bitstream/1947/3538/1/DSTO-TN-0432.pdf
 */
//void LatLonToENU(double lat, double lon, double alt, double originLat, double originLon, double originAlt, double* x, double* y, double* z)
//{

//}

#endif // QGCGEO_H
