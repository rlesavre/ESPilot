#include <unity.h>
#ifndef PIO_UNIT_TESTING
    #include <Wire.h>
    #include <SPI.h>
#endif
//#include "Observer.h"

void setUp(void) {
    // set stuff up here
}

void tearDown(void) {
    // clean stuff up here
}

void test_embedded_function(void) {
    //test stuff
    UNITY_TEST_ASSERT(1==1, 14, "OK");
}

// not needed when using generate_test_runner.rb
int main(void) {
    UNITY_BEGIN();
    RUN_TEST(test_embedded_function);
    return UNITY_END();
}