#include "Vector2.hpp"

Vector2::Vector2(): x(0), y(0) {}
Vector2::Vector2(int p_x, int p_y): x(p_x), y(p_y) {}

Vector2& Vector2::operator+=(const Vector2 &right) {
  x += right.x;
  y += right.y;
  return *this;
}
Vector2& Vector2::operator-=(const Vector2 &right) {
  x -= right.x;
  y -= right.y;
  return *this;
}
bool operator==(const Vector2 &left, const Vector2 &right) {
  return (left.x == right.x && left.y == right.y);
}
bool operator!=(const Vector2 &left, const Vector2 &right) {
  return !(operator==(left, right));
}
Vector2 operator+(Vector2 left, const Vector2 &right) {
  left += right;
  return left;
}
Vector2 operator-(Vector2 left, const Vector2 &right) {
  left -= right;
  return left;
}