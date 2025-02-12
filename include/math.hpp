struct vec3 {
	float x{0}, y{0}, z{0};

	vec3 operator+(const vec3& other) const {
		return vec3(x + other.x, y + other.y, z + other.z);
	}

	vec3 operator*(float scalar) const {
		return vec3(x * scalar, y * scalar, z * scalar);
	}

	vec3& operator*=(float scalar) {
		x *= scalar;
		y *= scalar;
		z *= scalar;
		return *this;
	}

	vec3& operator+=(const vec3& other) {
		x += other.x;
		y += other.y;
		z += other.z;
		return *this;
	}
};

typedef float mat4[4][4];

struct Rotation {
	float yaw;
	float pitch;
	float roll;
};
