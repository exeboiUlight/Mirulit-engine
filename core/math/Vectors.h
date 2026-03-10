#pragma once

#include <cmath>
#include <iostream>

namespace MirulitMath {

template<typename T>
class Vector2 {
public:
    T x, y;
    
    Vector2() : x(0), y(0) {}
    Vector2(T x, T y) : x(x), y(y) {}
    
    Vector2 operator+(const Vector2& other) const {
        return Vector2(x + other.x, y + other.y);
    }
    
    Vector2 operator-(const Vector2& other) const {
        return Vector2(x - other.x, y - other.y);
    }
    
    Vector2 operator*(T scalar) const {
        return Vector2(x * scalar, y * scalar);
    }
    
    Vector2 operator/(T scalar) const {
        return Vector2(x / scalar, y / scalar);
    }
    
    Vector2& operator+=(const Vector2& other) {
        x += other.x;
        y += other.y;
        return *this;
    }
    
    Vector2& operator-=(const Vector2& other) {
        x -= other.x;
        y -= other.y;
        return *this;
    }
    
    Vector2& operator*=(T scalar) {
        x *= scalar;
        y *= scalar;
        return *this;
    }
    
    Vector2& operator/=(T scalar) {
        x /= scalar;
        y /= scalar;
        return *this;
    }
    
    bool operator==(const Vector2& other) const {
        return x == other.x && y == other.y;
    }
    
    bool operator!=(const Vector2& other) const {
        return !(*this == other);
    }
    
    T length() const {
        return std::sqrt(x * x + y * y);
    }
    
    T dot(const Vector2& other) const {
        return x * other.x + y * other.y;
    }
    
    Vector2 normalized() const {
        T len = length();
        if (len > 0) {
            return Vector2(x / len, y / len);
        }
        return *this;
    }
    
    void normalize() {
        T len = length();
        if (len > 0) {
            x /= len;
            y /= len;
        }
    }
};

template<typename T>
Vector2<T> operator*(T scalar, const Vector2<T>& v) {
    return Vector2<T>(v.x * scalar, v.y * scalar);
}

template<typename T>
std::ostream& operator<<(std::ostream& os, const Vector2<T>& v) {
    os << "(" << v.x << ", " << v.y << ")";
    return os;
}

using Vector2f = Vector2<float>;
using Vector2d = Vector2<double>;
using Vector2i = Vector2<int>;

template<typename T>
class Vector3 {
public:
    T x, y, z;
    
    Vector3() : x(0), y(0), z(0) {}
    Vector3(T x, T y, T z) : x(x), y(y), z(z) {}
    
    Vector3 operator+(const Vector3& other) const {
        return Vector3(x + other.x, y + other.y, z + other.z);
    }
    
    Vector3 operator-(const Vector3& other) const {
        return Vector3(x - other.x, y - other.y, z - other.z);
    }
    
    Vector3 operator*(T scalar) const {
        return Vector3(x * scalar, y * scalar, z * scalar);
    }
    
    Vector3 operator/(T scalar) const {
        return Vector3(x / scalar, y / scalar, z / scalar);
    }
    
    Vector3& operator+=(const Vector3& other) {
        x += other.x;
        y += other.y;
        z += other.z;
        return *this;
    }
    
    Vector3& operator-=(const Vector3& other) {
        x -= other.x;
        y -= other.y;
        z -= other.z;
        return *this;
    }
    
    Vector3& operator*=(T scalar) {
        x *= scalar;
        y *= scalar;
        z *= scalar;
        return *this;
    }
    
    Vector3& operator/=(T scalar) {
        x /= scalar;
        y /= scalar;
        z /= scalar;
        return *this;
    }
    
    bool operator==(const Vector3& other) const {
        return x == other.x && y == other.y && z == other.z;
    }
    
    bool operator!=(const Vector3& other) const {
        return !(*this == other);
    }
    
    T length() const {
        return std::sqrt(x * x + y * y + z * z);
    }
    
    T dot(const Vector3& other) const {
        return x * other.x + y * other.y + z * other.z;
    }
    
    Vector3 cross(const Vector3& other) const {
        return Vector3(
            y * other.z - z * other.y,
            z * other.x - x * other.z,
            x * other.y - y * other.x
        );
    }
    
    Vector3 normalized() const {
        T len = length();
        if (len > 0) {
            return Vector3(x / len, y / len, z / len);
        }
        return *this;
    }
    
    void normalize() {
        T len = length();
        if (len > 0) {
            x /= len;
            y /= len;
            z /= len;
        }
    }
};

template<typename T>
Vector3<T> operator*(T scalar, const Vector3<T>& v) {
    return Vector3<T>(v.x * scalar, v.y * scalar, v.z * scalar);
}

template<typename T>
std::ostream& operator<<(std::ostream& os, const Vector3<T>& v) {
    os << "(" << v.x << ", " << v.y << ", " << v.z << ")";
    return os;
}

using Vector3f = Vector3<float>;
using Vector3d = Vector3<double>;
using Vector3i = Vector3<int>;

template<typename T>
class Quaternion {
public:
    T w, x, y, z;
    
    Quaternion() : w(1), x(0), y(0), z(0) {}
    Quaternion(T w, T x, T y, T z) : w(w), x(x), y(y), z(z) {}
    
    Quaternion operator+(const Quaternion& other) const {
        return Quaternion(w + other.w, x + other.x, y + other.y, z + other.z);
    }
    
    Quaternion operator-(const Quaternion& other) const {
        return Quaternion(w - other.w, x - other.x, y - other.y, z - other.z);
    }
    
    Quaternion operator*(const Quaternion& other) const {
        return Quaternion(
            w * other.w - x * other.x - y * other.y - z * other.z,
            w * other.x + x * other.w + y * other.z - z * other.y,
            w * other.y - x * other.z + y * other.w + z * other.x,
            w * other.z + x * other.y - y * other.x + z * other.w
        );
    }
    
    Quaternion operator*(T scalar) const {
        return Quaternion(w * scalar, x * scalar, y * scalar, z * scalar);
    }
    
    Quaternion operator/(T scalar) const {
        return Quaternion(w / scalar, x / scalar, y / scalar, z / scalar);
    }
    
    Quaternion& operator+=(const Quaternion& other) {
        w += other.w;
        x += other.x;
        y += other.y;
        z += other.z;
        return *this;
    }
    
    Quaternion& operator-=(const Quaternion& other) {
        w -= other.w;
        x -= other.x;
        y -= other.y;
        z -= other.z;
        return *this;
    }
    
    Quaternion& operator*=(const Quaternion& other) {
        *this = *this * other;
        return *this;
    }
    
    Quaternion& operator*=(T scalar) {
        w *= scalar;
        x *= scalar;
        y *= scalar;
        z *= scalar;
        return *this;
    }
    
    Quaternion conjugated() const {
        return Quaternion(w, -x, -y, -z);
    }
    
    T length() const {
        return std::sqrt(w * w + x * x + y * y + z * z);
    }
    
    Quaternion normalized() const {
        T len = length();
        if (len > 0) {
            return Quaternion(w / len, x / len, y / len, z / len);
        }
        return *this;
    }
    
    void normalize() {
        T len = length();
        if (len > 0) {
            w /= len;
            x /= len;
            y /= len;
            z /= len;
        }
    }
    
    Quaternion inverse() const {
        T lenSq = w * w + x * x + y * y + z * z;
        if (lenSq > 0) {
            Quaternion conj = conjugated();
            return Quaternion(conj.w / lenSq, conj.x / lenSq, conj.y / lenSq, conj.z / lenSq);
        }
        return *this;
    }
    
    Vector3<T> rotate(const Vector3<T>& v) const {
        Quaternion p(0, v.x, v.y, v.z);
        Quaternion rotated = *this * p * this->conjugated();
        return Vector3<T>(rotated.x, rotated.y, rotated.z);
    }
    
    static Quaternion fromAxisAngle(const Vector3<T>& axis, T angle) {
        T halfAngle = angle * 0.5;
        T sinHalf = std::sin(halfAngle);
        Vector3<T> normAxis = axis.normalized();
        return Quaternion(
            std::cos(halfAngle),
            normAxis.x * sinHalf,
            normAxis.y * sinHalf,
            normAxis.z * sinHalf
        );
    }
};

template<typename T>
Quaternion<T> operator*(T scalar, const Quaternion<T>& q) {
    return Quaternion<T>(q.w * scalar, q.x * scalar, q.y * scalar, q.z * scalar);
}

template<typename T>
std::ostream& operator<<(std::ostream& os, const Quaternion<T>& q) {
    os << "(" << q.w << ", " << q.x << ", " << q.y << ", " << q.z << ")";
    return os;
}

using Quaternionf = Quaternion<float>;
using Quaterniond = Quaternion<double>;

}