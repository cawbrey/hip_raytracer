#include <SDL.h>
#include <iostream>
#include "math.cpp"
#include "camera.cpp"
#include "world.cpp"


int main(int argc, char *argv[]) {
    const int WIDTH = 1920;
    const int HEIGHT = 1080;
    const auto start_time = std::chrono::system_clock::now();

    const Camera camera({{0,0,0},{0,0,0,1}}, 1);

    const World world{std::vector{
        Sphere{{0, 0, 5}, 1, {0, 255, 0}},
        Sphere{{3, 0, 5}, 1, {255, 255, 0}},
        Sphere{{-3, 0, 5}, 1, {255, 0, 0}},
        Sphere{{0, 3, 5}, 1, {0, 0, 255}},
        Sphere{{0, -3, 5}, 1, {255, 0, 255}},
    }};

    auto image = std::make_unique<std::array<std::array<glm::u8vec3, HEIGHT>, WIDTH> >();

    const float AR = (float)WIDTH / (float)HEIGHT;
    const float HALF_WIDTH_SPAN = AR / 2;
    const float HALF_HEIGHT_SPAN = 1.0F / 2;

    for (int y = 0; y < HEIGHT; ++y) {
        for (int x = 0; x < WIDTH; ++x) {
            const float x_norm = x / (float)WIDTH;
            const float y_norm = y / (float)HEIGHT;

            const float u = (2.0F * x_norm - 1.0F) * HALF_WIDTH_SPAN;
            const float v = (2.0F * y_norm - 1.0F) * HALF_HEIGHT_SPAN;

            const auto cameraRay = camera.get_ray(u, v);
            const auto worldHit = world.cast_ray(cameraRay);

            if (worldHit == std::nullopt) {
                image->at(x).at(y) = {0,0,0};
                continue;
            }

            image->at(x).at(y) = worldHit.value().first->color;
        }
    }

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
            uint8_t r = static_cast<uint8_t>(std::min(std::max(col.x * 255.0f, 0.0f), 255.0f));
            uint8_t g = static_cast<uint8_t>(std::min(std::max(col.y * 255.0f, 0.0f), 255.0f));
            uint8_t b = static_cast<uint8_t>(std::min(std::max(col.z * 255.0f, 0.0f), 255.0f));
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
        }
    }

    SDL_DestroyTexture(texture);
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
    return 0;
};
