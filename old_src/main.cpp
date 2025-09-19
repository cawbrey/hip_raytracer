#include <hiprand/hiprand_kernel.h>
#include <thrust/device_malloc.h>
#include <thrust/device_ptr.h>

#include <ctime>
#include <iostream>

#include "Bvh.h"
#include "HipHelper.h"
#include "Hittable.h"
#include "Image.h"
#include "Material.h"
#include "Texture.h"
#include "Triangle.h"
#include "camera.h"
#include "hip/hip_runtime.h"
#include "sphere.h"
#include "vec3.h"

__device__ Color color_ray( const ray3& r, Hittable** world, hiprandState* local_rand_state ) {
    ray3 cur_ray = r;
    Color totalAttentuation = Color( 1.0, 1.0, 1.0 );
    Color totalEmission = Color( 0, 0, 0 );

    for ( int i = 0; i < 50; i++ ) {
        HitRecord rec{};
        if ( !( *world )->hit( cur_ray, 0.001f, MAXFLOAT, rec ) )
            break;

        ray3 scattered;
        Color tempAtt = Color( 1.0, 1.0, 1.0 );
        Color tempEm = Color( 0, 0, 0 );

        if ( rec.mat_ptr->emit( 0, 0, tempEm ) ) {
            // totalEmission += totalAttentuation * tempEm;
            totalEmission += tempEm;
            // break;
        }

        if ( rec.mat_ptr->scatter( cur_ray, rec, tempAtt, scattered, local_rand_state ) ) {
            totalAttentuation *= tempAtt;
            cur_ray = scattered;
        } else {
            break;
        }
    }

    return totalEmission * totalAttentuation;
}

__global__ void rand_init( hiprandState* rand_state ) {
    if ( threadIdx.x == 0 && blockIdx.x == 0 ) {
        hiprand_init( 1984, 0, 0, rand_state );
    }
}

__global__ void render_init( uint max_x, uint max_y, hiprandState* rand_state ) {
    uint i = threadIdx.x + blockIdx.x * blockDim.x;
    uint j = threadIdx.y + blockIdx.y * blockDim.y;
    if ( ( i >= max_x ) || ( j >= max_y ) )
        return;
    uint pixel_index = j * max_x + i;
    // Original: Each thread gets same seed, a different sequence number, no
    // offset hiprand_init(1984, pixel_index, 0, &rand_state[pixel_index]);
    // BUGFIX, see Issue#2: Each thread gets different seed, same sequence for
    // performance improvement of about 2x!
    hiprand_init( 1984 + pixel_index, 0, 0, &rand_state[pixel_index] );
}

__global__ void render( vec3* fb,
                        uint max_x,
                        uint max_y,
                        uint ns,
                        camera** cam,
                        Hittable** world,
                        hiprandState* rand_state ) {
    uint x = threadIdx.x + blockIdx.x * blockDim.x;
    uint y = threadIdx.y + blockIdx.y * blockDim.y;

    if ( ( x >= max_x ) || ( y >= max_y ) )
        return;

    uint pixel_index = ( ( ( max_y - y + 1 ) * max_x ) - ( max_x - x ) );

    hiprandState local_rand_state = rand_state[pixel_index];
    Color col( 0, 0, 0 );
    for ( uint s = 0; s < ns; s++ ) {
        float u = ( (float)x + hiprand_uniform( &local_rand_state ) ) / float( max_x );
        float v = ( (float)y + hiprand_uniform( &local_rand_state ) ) / float( max_y );
        ray3 r = ( *cam )->get_ray( u, v, &local_rand_state );
        col += color_ray( r, world, &local_rand_state );
    }
    rand_state[pixel_index] = local_rand_state;
    col /= float( ns );
    // gamma correction
    fb[pixel_index] = sqrt( col );
}

#define RND ( hiprand_uniform( &local_rand_state ) )

__global__ void create_world( Hittable** d_list,
                              Hittable** d_world,
                              camera** d_camera,
                              int nx,
                              int ny,
                              Color* d_earth_img ) {
    if ( threadIdx.x == 0 && blockIdx.x == 0 ) {
        // Create Materials
        Material* sun = new DiffuseLight( { 1.0, 0.98, 0.96 } );
        Material* ground = new Lambertion( { 0.25, 0.25, 0.25 } );
        Material* blue = new Lambertion( { 0.0, 0.0, 1.0 } );
        Material* green = new Lambertion( { 0.0, 1.0, 0.0 } );
        Material* red = new Lambertion( { 1.0, 0.0, 0.0 } );
        Material* white = new Lambertion( { 1.0, 1.0, 1.0 } );
        Material* metal = new Metal( { 0.90, 0.95, 0.95 }, 0.0 );
        Material* glass = new Dielectric( 1.5 );
        Material* checkerBoard =
            new TexturedLambertion( new CheckerBoardTexture( { 1, 0, 1 }, { 0, 0, 0 }, 10.0 ) );
        Material* earth = new TexturedLambertion(
            new ImageTexture( new Image::Bitmap( d_earth_img, 2000, 1000 ) ) );

        // Create World
        d_list[0] = new sphere( vec3( 0, 1500, 0 ), 1000, sun );
        d_list[1] = new sphere( vec3( 0, -1000.0, -1 ), 1000, ground );
        d_list[2] = new sphere( vec3( -4, 1, 5 ), 1.0, blue );
        d_list[3] = new sphere( vec3( 0, 1, 5 ), 1.0, metal );
        d_list[4] = new sphere( vec3( 4, 1, 5 ), 1.0, glass );

        d_list[5] = new sphere( { 2, 3, 0 }, 0.5, red );
        d_list[6] = new sphere( { 0, 5, 0 }, 0.5, green );
        d_list[7] = new sphere( { 0, 3, 2 }, 0.5, blue );
        d_list[8] = new sphere( { 0, 3, 0 }, 0.5, white );

        d_list[9] = new sphere( { 5, 1, 0 }, 0.75, checkerBoard );
        d_list[10] = new sphere( { -5, 1, 0 }, 0.75, earth );

#define NUM_OBJECTS 11

        // Create the BVH
        *d_world = new Bvh( d_list, 0, NUM_OBJECTS );

        // Create Camera
        *d_camera = new camera( { 0, 5, -10 }, { 0, 1, 5 }, { 0, 1, 0 }, 50.0,
                                float( nx ) / float( ny ), 0.0001, 10.0 );
    }
}

__global__ void free_world( Hittable** d_list, Hittable** d_world, camera** d_camera ) {
    for ( int i = 0; i < NUM_OBJECTS; i++ ) {
        delete ( (sphere*)d_list[i] )->mat_ptr;
        delete d_list[i];
    }
    delete *d_world;
    delete *d_camera;
}

int main() {
    int width = 1920;
    int height = 1080;
    int numSamples = 50;

    int threadsX = 16;
    int threadsY = 16;

    std::cerr << "Rendering a " << width << "x" << height << " image with " << numSamples
              << " samples per pixel ";
    std::cerr << "in " << threadsX << "x" << threadsY << " blocks.\n";

    int num_pixels = width * height;
    size_t fb_size = num_pixels * sizeof( vec3 );

    // allocate FB
    Color* fb;
    checkHipErrors( hipMallocManaged( (void**)&fb, fb_size ) );

    // allocate random state
    hiprandState* d_rand_state;
    checkHipErrors( hipMalloc( (void**)&d_rand_state, num_pixels * sizeof( hiprandState ) ) );

    // make our world of hitables & the camera
    Hittable** d_objects;
    checkHipErrors( hipMalloc( (void**)&d_objects, NUM_OBJECTS * sizeof( Hittable* ) ) );
    Hittable** d_world;
    checkHipErrors( hipMalloc( (void**)&d_world, sizeof( Hittable* ) ) );
    camera** d_camera;
    checkHipErrors( hipMalloc( (void**)&d_camera, sizeof( camera* ) ) );

    // load texture
    Image::Bitmap* h_earth_img = Image::readPNG( "resources/earth.png" );
    thrust::device_ptr<Color> d_earth_img =
        thrust::device_malloc<Color>( h_earth_img->width * h_earth_img->height );
    for ( int i = 0; i < h_earth_img->width * h_earth_img->height; i++ ) {
        d_earth_img[i] = h_earth_img->data[i];
    }

    checkHipErrors( hipGetLastError() );
    checkHipErrors( hipDeviceSynchronize() );

    // build the world
    create_world<<<1, 1>>>( d_objects, d_world, d_camera, width, height,
                            thrust::raw_pointer_cast( d_earth_img ) );
    checkHipErrors( hipGetLastError() );
    checkHipErrors( hipDeviceSynchronize() );
    std::cout << "World created" << std::endl;

    clock_t start, stop;
    start = clock();
    // Render our buffer
    dim3 blocks( width / threadsX + 1, height / threadsY + 1 );
    dim3 threads( threadsX, threadsY );
    render_init<<<blocks, threads>>>( width, height, d_rand_state );
    checkHipErrors( hipGetLastError() );
    checkHipErrors( hipDeviceSynchronize() );
    render<<<blocks, threads>>>( fb, width, height, numSamples, d_camera, d_world, d_rand_state );
    checkHipErrors( hipGetLastError() );
    checkHipErrors( hipDeviceSynchronize() );
    stop = clock();
    double timer_seconds = ( (double)( stop - start ) ) / CLOCKS_PER_SEC;
    std::cerr << "took " << timer_seconds << " seconds.\n";

    // Output FB as Image
    Image::writePNG( fb, width, height, "out.png" );

    // clean up
    checkHipErrors( hipDeviceSynchronize() );
    free_world<<<1, 1>>>( d_objects, d_world, d_camera );
    checkHipErrors( hipGetLastError() );
    checkHipErrors( hipFree( d_camera ) );
    checkHipErrors( hipFree( d_world ) );
    checkHipErrors( hipFree( d_objects ) );
    checkHipErrors( hipFree( d_rand_state ) );
    checkHipErrors( hipFree( fb ) );
    checkHipErrors( hipDeviceReset() );
}