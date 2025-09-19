#ifndef MATERIALH
#define MATERIALH

#include <hip/hip_math.h>
#include <hiprand/hiprand_kernel.h>

#include "Hittable.h"
#include "Texture.h"
#include "vec3.h"

struct HitRecord;

// TODO - ur mom
__device__ float schlick( float cosine, float ref_idx ) {
    float r0 = ( 1.0f - ref_idx ) / ( 1.0f + ref_idx );
    r0 = r0 * r0;
    return r0 + ( 1.0f - r0 ) * pow( ( 1.0f - cosine ), 5.0f );
}

// TODO - ur mom
__device__ bool refract( const vec3& v, const vec3& n, float ni_over_nt, vec3& refracted ) {
    vec3 uv = unit_vector( v );
    float dt = dot( uv, n );
    float discriminant = 1.0f - ni_over_nt * ni_over_nt * ( 1 - dt * dt );
    if ( discriminant > 0 ) {
        refracted = ni_over_nt * ( uv - n * dt ) - n * sqrt( discriminant );
        return true;
    } else
        return false;
}

class Material {
   public:
    __device__ virtual ~Material() = default;

    __device__ virtual bool scatter( const ray3& r_in,
                                     const HitRecord& rec,
                                     Color& attenuation,
                                     ray3& scattered,
                                     hiprandState* local_rand_state ) const {
        return false;
    }

    __device__ virtual bool emit( float u, float v, Color& emittance ) const {
        return false;
    }
};

class Lambertion : public Material {
    Color albedo;

   public:
    __device__ explicit Lambertion( const Color& a ) : albedo( a ) {
    }

    __device__ bool scatter( const ray3& r_in,
                             const HitRecord& rec,
                             Color& attenuation,
                             ray3& scattered,
                             hiprandState* local_rand_state ) const override {
        vec3 target = rec.p + rec.normal + randomUnitVec3( local_rand_state );
        scattered = ray3( { rec.p, target - rec.p } );
        attenuation = albedo;
        return true;
    }
};

class Metal : public Material {
    Color albedo;
    float fuzz;

   public:
    __device__ Metal( const Color& albedo, float fuzz )
        : albedo( albedo ), fuzz( min( fuzz, 1.0f ) ) {
    }

    __device__ bool scatter( const ray3& r_in,
                             const HitRecord& rec,
                             Color& attenuation,
                             ray3& scattered,
                             hiprandState* local_rand_state ) const override {
        vec3 reflected = reflect( unit_vector( r_in.direction ), rec.normal );
        scattered = ray3( { rec.p, reflected + fuzz * randomUnitVec3( local_rand_state ) } );
        attenuation = albedo;
        return ( dot( scattered.direction, rec.normal ) > 0.0f );
    }
};

class Dielectric : public Material {
    float ref_idx;

   public:
    __device__ explicit Dielectric( float ri ) : ref_idx( ri ) {
    }

    __device__ bool scatter( const ray3& r_in,
                             const HitRecord& rec,
                             vec3& attenuation,
                             ray3& scattered,
                             hiprandState* local_rand_state ) const override {
        vec3 outward_normal;
        vec3 reflected = reflect( r_in.direction, rec.normal );
        float ni_over_nt;
        attenuation = vec3( 1.0, 1.0, 1.0 );
        vec3 refracted;
        float reflect_prob;
        float cosine;
        if ( dot( r_in.direction, rec.normal ) > 0.0f ) {
            outward_normal = -rec.normal;
            ni_over_nt = ref_idx;
            cosine = dot( r_in.direction, rec.normal ) / length( r_in.direction );
            cosine = sqrt( 1.0f - ref_idx * ref_idx * ( 1 - cosine * cosine ) );
        } else {
            outward_normal = rec.normal;
            ni_over_nt = 1.0f / ref_idx;
            cosine = -dot( r_in.direction, rec.normal ) / length( r_in.direction );
        }
        if ( refract( r_in.direction, outward_normal, ni_over_nt, refracted ) )
            reflect_prob = schlick( cosine, ref_idx );
        else
            reflect_prob = 1.0f;
        if ( hiprand_uniform( local_rand_state ) < reflect_prob )
            scattered = ray3( { rec.p, reflected } );
        else
            scattered = ray3( { rec.p, refracted } );
        return true;
    }
};

class DiffuseLight : public Material {
    Color color;

   public:
    __device__ explicit DiffuseLight( const Color& color ) : color( color ) {
    }

    __device__ bool emit( float u, float v, Color& emittance ) const override {
        emittance = color;
        return true;
    }
};

class TexturedLambertion : public Material {
    Texture* texture;

   public:
    __device__ explicit TexturedLambertion( Texture* tex ) : texture( tex ) {
    }

    __device__ bool scatter( const ray3& r_in,
                             const HitRecord& rec,
                             Color& attenuation,
                             ray3& scattered,
                             hiprandState* local_rand_state ) const override {
        vec3 target = rec.p + rec.normal + randomUnitVec3( local_rand_state );
        scattered = ray3( { rec.p, target - rec.p } );
        attenuation = texture->value( rec.u, rec.v );
        return true;
    }
};

class TexturedMetal : public Material {
    Texture* texture;
    float fuzz;

   public:
    __device__ TexturedMetal( Texture* tex, float fuzz )
        : texture( tex ), fuzz( min( fuzz, 1.0f ) ) {
    }

    __device__ bool scatter( const ray3& r_in,
                             const HitRecord& rec,
                             Color& attenuation,
                             ray3& scattered,
                             hiprandState* local_rand_state ) const override {
        vec3 reflected = reflect( unit_vector( r_in.direction ), rec.normal );
        scattered = ray3( { rec.p, reflected + fuzz * randomUnitVec3( local_rand_state ) } );
        attenuation = texture->value( rec.u, rec.v );
        return ( dot( scattered.direction, rec.normal ) > 0.0f );
    }
};

class TexturedDielectric : public Material {
    Texture* texture;
    float ref_idx;

   public:
    __device__ TexturedDielectric( Texture* tex, float ri ) : texture( tex ), ref_idx( ri ) {
    }

    __device__ bool scatter( const ray3& r_in,
                             const HitRecord& rec,
                             vec3& attenuation,
                             ray3& scattered,
                             hiprandState* local_rand_state ) const override {
        vec3 outward_normal;
        vec3 reflected = reflect( r_in.direction, rec.normal );
        float ni_over_nt;
        attenuation = texture->value( rec.u, rec.v );
        vec3 refracted;
        float reflect_prob;
        float cosine;
        if ( dot( r_in.direction, rec.normal ) > 0.0f ) {
            outward_normal = -rec.normal;
            ni_over_nt = ref_idx;
            cosine = dot( r_in.direction, rec.normal ) / length( r_in.direction );
            cosine = sqrt( 1.0f - ref_idx * ref_idx * ( 1 - cosine * cosine ) );
        } else {
            outward_normal = rec.normal;
            ni_over_nt = 1.0f / ref_idx;
            cosine = -dot( r_in.direction, rec.normal ) / length( r_in.direction );
        }
        if ( refract( r_in.direction, outward_normal, ni_over_nt, refracted ) )
            reflect_prob = schlick( cosine, ref_idx );
        else
            reflect_prob = 1.0f;
        if ( hiprand_uniform( local_rand_state ) < reflect_prob )
            scattered = ray3( { rec.p, reflected } );
        else
            scattered = ray3( { rec.p, refracted } );
        return true;
    }
};

class TexturedDiffuseLight : public Material {
    Texture* texture;

   public:
    __device__ explicit TexturedDiffuseLight( Texture* tex ) : texture( tex ) {
    }

    __device__ bool emit( float u, float v, Color& emittance ) const override {
        emittance = texture->value( u, v );
        return true;
    }
};

#endif