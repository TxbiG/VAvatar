#pragma once


struct Curve : public Variant{
    TArray<Vector2> values;

    static const int MIN_X = 0.f;
    static const int MAX_X = 1.f;

    float m_bake_interval;
    int m_pointCount;
    int bake_resolution = 100;




    struct Point {
        Vector2 position;
        float left_tangent = 0.0;
        float right_tangent = 0.0;
        TangentMode left_mode = TANGENT_FREE;
        TangentMode right_mode = TANGENT_FREE;

        Point() {
        }

        Point(const Vector2& p_position,
            real_t p_left = 0.0,
            real_t p_right = 0.0,
            TangentMode p_left_mode = TANGENT_FREE,
            TangentMode p_right_mode = TANGENT_FREE) {
            position = p_position;
            left_tangent = p_left;
            right_tangent = p_right;
            left_mode = p_left_mode;
            right_mode = p_right_mode;
        }
    };
    // Add a value to the curve
    void add_value(float value) {
        values.push_back(value);
    }

    void add_point(Vector2 position, float left_tangent = 0, float right_tangent = 0, TangentMode left_mode = 0, TangentMode right_mode = 0);

    // Linear interpolation
    Vector2 interpolate_linear(float t) const {
        if (values.size() < 2) {
            return Vector2::ZERO();
        }

        size_t segment = static_cast<size_t>(t * (values.size() - 1));
        float local_t = (t * (values.size() - 1)) - segment;

        float v0 = values[segment];
        float v1 = values[segment + 1];

        return v0 + local_t * (v1 - v0);
    }

    // Add a method to clear values
    void clear() {
        values.clear();
    }

    float get_point_left_tangent(int index) const {}

    Vector2 get_point_position(int index) const {}

    TangentMode get_point_right_mode(int index) const {}

    float get_point_right_tangent(int index) const {}

    void remove_point(int index) {}

    void set_point_left_mode(int index, TangentMode mode) {}

    void set_point_left_tangent(int index, float tangent) {}

    int set_point_offset(int index, float offset) {}

    void set_point_right_mode(int index, TangentMode mode) {}

    void set_point_right_tangent(int index, float tangent) {}

    void set_point_value(int index, float y) {}
};

struct Curve2D : public Variant
{
    // Add a point to the curve
    void add_point(const Vector2& point) {
        points.push_back(point);
    }
    void add_point(Vector2 position, Vector2 in = Vector2(0, 0), Vector2 out = Vector2(0, 0), int index = -1);

    // Linear interpolation
    Vector2 interpolate_linear(float t) const {
        if (points.size() < 2) {
            return Vector2(0.0f, 0.0f);
        }

        size_t segment = static_cast<size_t>(t * (points.size() - 1));
        float local_t = (t * (points.size() - 1)) - segment;

        const Vector2& p0 = points[segment];
        const Vector2& p1 = points[segment + 1];

        return Vector2(p0.x + local_t * (p1.x - p0.x), p0.y + local_t * (p1.y - p0.y));
    }

    // Add a method to clear points
    void clear() {
        points.clear();
    }


    float get_baked_length() const;

    TArray<Vector2> get_baked_points() const;

    float get_closest_offset(Vector2 to_point) const;

    Vector2  get_closest_point(Vector2 to_point) const;

    Vector2 get_point_in(int idx) const;

    Vector2 get_point_out(int idx) const;
    
    Vector2 get_point_position(int idx) const;

    void remove_point(int idx);

    Vector2 sample(int idx, float t) const;

    Vector2 sample_baked(float offset = 0.0, bool cubic = false) const;

    Transform2D sample_baked_with_rotation(float offset = 0.0, bool cubic = false) const;

    Vector2 samplef(float fofs) const;

    void set_point_in(int idx, Vector2 position);

    void set_point_out(int idx, Vector2 position);

    void set_point_position(int idx, Vector2 position);

    TArray<Vector2> tessellate(int max_stages = 5, float tolerance_degrees = 4) const;

    TArray<Vector2> tessellate_even_length(int max_stages = 5, float tolerance_length = 20.0) const;

    TArray<Vector2> points;
    float bake_interval;
    int m_pointCount;
};

struct Curve3D : public Variant
{
    // Add a point to the curve
    void add_point(const Vector3& point) {
        points.push_back(point);
    }

    add_point(Vector3 position, Vector3 in = Vector3(0, 0, 0), Vector3 out = Vector3(0, 0, 0), int index = -1)

    // Linear interpolation
    Vector3 interpolate_linear(float t) const {
        if (points.size() < 2) {
            return Vector3(0.0f, 0.0f, 0.0f);
        }

        size_t segment = static_cast<size_t>(t * (points.size() - 1));
        float local_t = (t * (points.size() - 1)) - segment;

        const Vector3& p0 = points[segment];
        const Vector3& p1 = points[segment + 1];

        return Vector3(p0.x + local_t * (p1.x - p0.x), p0.y + local_t * (p1.y - p0.y), p0.z + local_t * (p1.z - p0.z));
    }

    // Add a method to clear points
    void clear() {
        points.clear();
    }

    float get_baked_length() const;

    TArray<Vector3> get_baked_points() const;

    TArray<Vector3> get_baked_tilts() const;

    TArray<Vector3> get_baked_up_vectors() const;

    float get_closest_offset(Vector3 to_point) const;

    Vector3 get_closest_point(Vector3 to_point) const;

    Vector3 get_point_in(int idx) const;

    Vector3 get_point_out(int idx) const;

    Vector3 get_point_position(int idx) const;

    float get_point_tilt(int idx) const;

    void remove_point(int idx);

    Vector3 sample(int idx, float t) const;

    Vector3 sample_baked(float offset = 0.0, bool cubic = false) const;

    Vector3 sample_baked_up_vector(float offset, bool apply_tilt = false) const;

    Transform3D sample_baked_with_rotation(float offset = 0.0, bool cubic = false, bool apply_tilt = false) const;

    Vector3 samplef(float fofs) const;

    void set_point_in(int idx, Vector3 position);

    void set_point_out(int idx, Vector3 position);

    void set_point_position(int idx, Vector3 position);

    void set_point_tilt(int idx, float tilt);

    TArray<Vector3> tessellate(int max_stages = 5, float tolerance_degrees = 4) const;

    TArray<Vector3> tessellate_even_length(int max_stages = 5, float tolerance_length = 0.2) const;

    TArray<Vector3> points;
    float bake_interval;
    int m_pointCount;
    bool up_vector_enabled;
};