#ifndef SHITTYRAYTRACER_HIPHELPER_H
#define SHITTYRAYTRACER_HIPHELPER_H

#include <hip/hip_runtime.h>
#include <iostream>

#define checkHipErrors(val) hipErrCheck( (val), #val, __FILE__, __LINE__ )
void hipErrCheck(hipError_t result, char const *const func, const char *const file, int const line) {
    if (result) {
        std::cerr << "HIP error = " << static_cast<unsigned int>(result) << " at " <<
                  file << ":" << line << " '" << func << "' \n";
        // Make sure we call HIP Device Reset before exiting
        std::cerr << "Calling hipDeviceReset: " << hipDeviceReset() << std::endl;
        exit(99);
    }
}

#endif //SHITTYRAYTRACER_HIPHELPER_H
