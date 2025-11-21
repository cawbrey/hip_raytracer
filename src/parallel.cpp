#pragma once

#include <vector>
#include <future>

template<typename F>
void parallel_for_rows(int begin, int end, int workers, F func) {
    std::vector<std::future<void>> tasks;
    tasks.reserve(workers);

    int rows = end - begin;
    int chunk = (rows + workers - 1) / workers;

    for (int w = 0; w < workers; ++w) {
        int start = begin + w * chunk;
        int stop  = std::min(start + chunk, end);

        if (start >= stop) {
            break;
        }

        tasks.emplace_back(std::async(std::launch::async, [=, &func] {
            for (int y = start; y < stop; ++y) {
                func(y);
            }
        }));
    }

    for (auto& t : tasks) {
        t.get();
    }
}
