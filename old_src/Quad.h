#ifndef SHITTYRAYTRACER_QUAD_H
#define SHITTYRAYTRACER_QUAD_H

#include "Hittable.h"

class Quad : public Hittable {
   public:
    __device__ Quad( const point3& q, const point3& u, const point3& v, Material* mat )
        : q( q ), u( u ), v( v ) {
        mat_ptr = mat;

        point3 n = cross( u, v );
        normal = unit_vector( n );
        d = dot( normal, q );
        w = n / dot( n, n );

        // w = {1,2,10};

        //        point3 qq = q + u + v;
        //
        //        float min_x = std::min({q.x, qq.x}) - 0.001;
        //        float min_y = std::min({q.y, qq.y}) - 0.001;
        //        float min_z = std::min({q.z, qq.z}) - 0.001;
        //
        //        float max_x = std::max({q.x, qq.x}) + 0.001;
        //        float max_y = std::max({q.y, qq.y}) + 0.001;
        //        float max_z = std::max({q.z, qq.z}) + 0.001;
        //
        //        boundingBox = std::make_shared<BoundingBox>( *new
        //        BoundingBox(point3(min_x, min_y, min_z),
        //                point3(max_x, max_y, max_z)));

        boundingBox = BoundingBox( BoundingBox( ( q - 0.1 ), ( q + u + v ) + 0.1 ),
                                   BoundingBox( { ( q + u ) - 0.1, ( q + v ) + 0.1 } ) );
    }

    __device__ bool hit( const ray3& r, float tmin, float tmax, HitRecord& rec ) const override {
        // Lalg3D::Ray normalized_r = r.normalized();
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
        if ( alpha < 0 || alpha > 1 || beta < 0 || beta > 1 )
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

#endif  // SHITTYRAYTRACER_QUAD_H