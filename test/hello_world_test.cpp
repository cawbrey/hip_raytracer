#include <gtest/gtest.h>
#include <sstream>
#include <iostream>
#include "../src/hello_world.cpp"


TEST(HelloWorldExecutable, PrintsHelloWorld) {
    // Save original buffer
    std::streambuf *orig_buf = std::cout.rdbuf();

    // Redirect cout to our buffer
    std::ostringstream captured;
    std::cout.rdbuf(captured.rdbuf());

    print_hello_world();

    // Restore original buffer
    std::cout.rdbuf(orig_buf);

    EXPECT_EQ(captured.str(), "Hello World!\n");
}