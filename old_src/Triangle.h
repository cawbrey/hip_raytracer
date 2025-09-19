#ifndef SHITTYRAYTRACER_TRIANGLE_H
#define SHITTYRAYTRACER_TRIANGLE_H

#include "Hittable.h"

class Triangle : public Hittable {
   public:
    __device__ Triangle( const point3& a, const point3& b, const point3& c, Material* mat )
        : q( b ), u( c - b ), v( a - b ) {
        mat_ptr = mat;

        point3 n = cross( u, v );
        normal = unit_vector( n );
        d = dot( normal, q );
        w = n / dot( n, n );

        float min_x = fmin( a.x, fmin( b.x, c.x ) );
        float min_y = fmin( a.y, fmin( b.y, c.y ) );
        float min_z = fmin( a.z, fmin( b.z, c.z ) );

        float max_x = fmin( a.x, fmin( b.x, c.x ) );
        float max_y = fmin( a.y, fmin( b.y, c.y ) );
        float max_z = fmin( a.z, fmin( b.z, c.z ) );

        boundingBox = BoundingBox( point3( min_x, min_y, min_z ), point3( max_x, max_y, max_z ) );
    }

    __device__ bool hit( const ray3& r, float tmin, float tmax, HitRecord& rec ) const override {
        // ray3 normalized_r = r.normalized();
        // normalized_r = r;

        float denominator = dot( normal, r.direction );

        // No hit if the ray is parallel to the plane.
        if ( std::abs( denominator ) < 1e-8 )
            return false;

        float numerator = d - dot( normal, r.origin );

        float t = numerator / denominator;

        // Return false if the hit point parameter t is outside the ray interval.
        if ( t < tmin || t > tmax )
            return false;
        point3 intersection = r( t );

        point3 planar_hitpt_vector = intersection - q;
        float alpha = dot( w, cross( planar_hitpt_vector, v ) );
        float beta = dot( w, cross( u, planar_hitpt_vector ) );

        // Check that the ray actually hit this plane
        if ( alpha < 0 || alpha > 1 || beta < 0 || beta > 1 || alpha + beta > 1 )
            return false;

        // Ray hits the 2D shape; set the rest of the hit record and return true.
        rec.t = t;
        rec.p = intersection;
        rec.u = alpha;
        rec.v = beta;
        // rec.set_face_normal(r, normal);

        return true;
    }

   private:
    point3 q;
    point3 u;
    point3 v;
    point3 w;

    // Cached Values
    point3 normal;
    float d;
};

#endif  // SHITTYRAYTRACER_TRIANGLE_H