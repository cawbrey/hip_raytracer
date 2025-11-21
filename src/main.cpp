#include <SDL.h>
#include <iostream>

#include "math.cpp"
#include "camera.cpp"
#include "world.cpp"
#include "parallel.cpp"


int main(int argc, char *argv[]) {
    constexpr auto WIDTH = 1280;
    constexpr auto HEIGHT = 720;
    const auto start_time = std::chrono::system_clock::now();

    const Camera camera({{0,2,0},{0, 0, -0.0871557,0.9961947}}, 1);

    const World world{std::vector{
        Sphere{{0, 790'400'000, 0}, 693'000'000, {0.9, 0.9, 0.9}, {0.95, 0.95, 0.95}},
        Sphere{{0, -6'300'000, 0}, 6'300'000, {0.25, 0.25, 0.25}, {0, 0, 0}},
        Sphere{{-1.75, 1.25, 5}, 1, {0.9, 0, 0}, {0, 0, 0}},
        Sphere{{1.25, 1, 5}, 1, {0.9, 0.9, 0}, {0, 0, 0}},
    }};

    auto image = std::make_unique<std::array<std::array<vec3, HEIGHT>, WIDTH> >();

    constexpr auto AR = (flt)WIDTH / HEIGHT;
    constexpr auto HALF_WIDTH_SPAN = AR / 2;
    constexpr auto HALF_HEIGHT_SPAN = 1.0 / 2;


    parallel_for_rows(0, HEIGHT, HEIGHT, [&](int y) {
        for (int x = 0; x < WIDTH; ++x) {

            vec3 color(0.0f);

            const uint samples = 50;

            for (uint i = 0; i < samples; ++i) {
                float u = (2.0f * x / WIDTH - 1.0f) * HALF_WIDTH_SPAN;
                float v = (2.0f * y / HEIGHT - 1.0f) * HALF_HEIGHT_SPAN;

                auto cameraRay = camera.get_ray(u, v);

                color += world.trace_ray(cameraRay);
            }

            color /= float(samples);

            image->at(x).at(y) = color;
        }
    });

    std::cout << "Time taken: " << std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now() - start_time).count() / 1000.0F << "Seconds" << std::endl;


    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        std::cerr << "SDL could not initialize! " << SDL_GetError() << std::endl;
        return 1;
    }

    SDL_Window *window = SDL_CreateWindow("SDL2 Pixel Drawing",
                                          SDL_WINDOWPOS_CENTERED,
                                          SDL_WINDOWPOS_CENTERED,
                                          WIDTH, HEIGHT,
                                          SDL_WINDOW_SHOWN);

    if (!window) {
        std::cerr << "Window could not be created! " << SDL_GetError() << std::endl;
        SDL_Quit();
        return 1;
    }

    SDL_Renderer *renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
    if (!renderer) {
        std::cerr << "Renderer could not be created! " << SDL_GetError() << std::endl;
        SDL_DestroyWindow(window);
        SDL_Quit();
        return 1;
    }

    // Create a texture we can write pixels to
    SDL_Texture *texture = SDL_CreateTexture(renderer,
                                             SDL_PIXELFORMAT_ARGB8888,
                                             SDL_TEXTUREACCESS_STREAMING,
                                             WIDTH, HEIGHT);

    if (!texture) {
        std::cerr << "Texture could not be created! " << SDL_GetError() << std::endl;
        SDL_DestroyRenderer(renderer);
        SDL_DestroyWindow(window);
        SDL_Quit();
        return 1;
    }


    void *pixels;
    int pitch;
    SDL_LockTexture(texture, nullptr, &pixels, &pitch);

    uint32_t *pixels32 = static_cast<uint32_t *>(pixels);

    for (int y = 0; y < HEIGHT; ++y) {
        for (int x = 0; x < WIDTH; ++x) {
            // Clamp and convert color to 0-255
            auto col = (*image)[x][y];
            uint8_t r = col.r * SDL_MAX_UINT8;
            uint8_t g = col.g * SDL_MAX_UINT8;
            uint8_t b = col.b * SDL_MAX_UINT8;
            uint8_t a = 255;

            pixels32[y * (pitch / 4) + x] = (a << 24) | (r << 16) | (g << 8) | b;
        }
    }

    SDL_UnlockTexture(texture);
    SDL_RenderClear(renderer);
    SDL_RenderCopy(renderer, texture, nullptr, nullptr);
    SDL_RenderPresent(renderer);


    bool running = true;
    SDL_Event event;

    while (running) {
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_QUIT)
                running = false;
            else if (event.type == SDL_MOUSEBUTTONDOWN) {
                if (event.button.button == SDL_BUTTON_LEFT) {
                    std::cout << "Clicked at position: (" << event.button.x << ", " << event.button.y << ")" <<
                            std::endl;
                }
            }
        }
    }

    SDL_DestroyTexture(texture);
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
    return 0;
};
