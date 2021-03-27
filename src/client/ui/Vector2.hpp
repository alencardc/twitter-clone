#ifndef __Vector2_hpp__
#define __Vector2_hpp__

class Vector2 {
  public: 
    int x;
    int y;

    Vector2();
    Vector2(int p_x, int p_y);

    Vector2 &operator+=(const Vector2 &right);
    Vector2 &operator-=(const Vector2 &right);
};

bool operator==(const Vector2 &left, const Vector2 &right);
bool operator!=(const Vector2 &left, const Vector2 &right);
Vector2 operator+(Vector2 left, const Vector2 &right);
Vector2 operator-(Vector2 left, const Vector2 &right);

#endif // __Vector2_hpp__