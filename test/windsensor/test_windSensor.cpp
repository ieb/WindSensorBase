#include "../testsupport.h"
#include <windsensor.h>



using namespace fakeit;

Stream* debug = ArduinoFakeMock(Stream);

void setUp(void) {
    ArduinoFakeReset();
    setupStream();
    When(Method(ArduinoFake(), millis)).Return(1001,1002,1003,1004);
    std::cout << "Setup done" << std::endl; 
}


void test_parse_compact_line() {

    WindSensor windSensor(debug);
    char data[] = "1,203,3.456";
    TEST_ASSERT_TRUE(windSensor.processData(data));
    std::cout << "AWS:" << windSensor.getWindSpeed() << " AWA:" << windSensor.getWindAngle() << std::endl;
}

void test_parse_compact_line_bad_commas1() {

    WindSensor windSensor(debug);
    char data[] = "1,,3.456";
    TEST_ASSERT_TRUE(windSensor.processData(data));
    std::cout << "AWS:" << windSensor.getWindSpeed() << " AWA:" << windSensor.getWindAngle() << std::endl;
}
void test_parse_compact_line_bad_commas2() {

    WindSensor windSensor(debug);
    char data[] = ",,3.456";
    TEST_ASSERT_TRUE(windSensor.processData(data));
    std::cout << "AWS:" << windSensor.getWindSpeed() << " AWA:" << windSensor.getWindAngle() << std::endl;
}

void test_parse_debug_line() {

    WindSensor windSensor(debug);
    char data[] = "1,2,3,4,5,6,7.1,8,9,10";
    TEST_ASSERT_TRUE(windSensor.processData(data));
    std::cout << "AWS:" << windSensor.getWindSpeed() << " AWA:" << windSensor.getWindAngle() << std::endl;
}

void test_parse_debug_line_bad_commas() {

    WindSensor windSensor(debug);
    char data[] = "1,,,,5,6,7.1,8,9,10";
    TEST_ASSERT_TRUE(windSensor.processData(data));
    std::cout << "AWS:" << windSensor.getWindSpeed() << " AWA:" << windSensor.getWindAngle() << std::endl;
}


void test_parse_short_line() {

    WindSensor windSensor(debug);
    char data[] = "1,2";
    TEST_ASSERT_FALSE(windSensor.processData(data));
}

void test_parse_long_line() {

    WindSensor windSensor(debug);
    char data[] = "1,2,1,2,3,4,5,6,7.1,8,9,10,1,2,3,4,5,6,7.1,8,9,10,1,2,3,4,5,6,7.1,8,9,10";
    TEST_ASSERT_FALSE(windSensor.processData(data));
}

void test_bad_commas_long_line() {

    WindSensor windSensor(debug);
    char data[] = "1,,,,2,1,2,3,4,5,6,7.1,8,9,10,1,2,3,4,5,6,7.1,8,9,10,1,2,3,4,5,6,7.1,8,9,10";
    TEST_ASSERT_FALSE(windSensor.processData(data));
}

int main(int argc, char **argv) { 
    try {
        UNITY_BEGIN();
        RUN_TEST(test_parse_compact_line);
        RUN_TEST(test_parse_debug_line);
        RUN_TEST(test_parse_short_line);
        RUN_TEST(test_parse_long_line);
        RUN_TEST(test_bad_commas_long_line);
        RUN_TEST(test_parse_debug_line_bad_commas);
        RUN_TEST(test_parse_compact_line_bad_commas1);
        RUN_TEST(test_parse_compact_line_bad_commas2);
        return UNITY_END();
    } catch( UnexpectedMethodCallException e) {
            std::cout << "Exception:" << e << std::endl;

    }
}