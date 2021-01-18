#include "../testsupport.h"
#include <windconfig.h>



using namespace fakeit;

Stream * io = ArduinoFakeMock(Stream);
WindSensor windSensor(io);
WindConfig * windConfig;

void setUp(void) {
    ArduinoFakeReset();
    setupStream();
    windConfig = new WindConfig(&windSensor, io);
    windConfig->begin();
}

void tearDown(void) {
    delete windConfig;
}


void test_help() {
    MockStreamLoader loader;
    loader.load("help\n");
    windConfig->process();
}

void test_dump() {
    MockStreamLoader loader;
    loader.load("dump\n");
    windConfig->process();
}

void test_angle_max() {
    TEST_ASSERT_EQUAL_INT(4096,windConfig->config->maxAngle);
    MockStreamLoader loader;
    loader.load("angle max 3096\n");
    windConfig->process();
    TEST_ASSERT_EQUAL_INT(3096,windConfig->config->maxAngle);

}
void test_angle_correction() {
    TEST_ASSERT_EQUAL_INT(0,windConfig->config->angleCorrection);
    MockStreamLoader loader;
    loader.load("angle correction 15\n");
    windConfig->process();
    TEST_ASSERT_EQUAL_INT(15,windConfig->config->angleCorrection);
    loader.load("ac -2596\n");
    windConfig->process();
    TEST_ASSERT_EQUAL_INT(-2596,windConfig->config->angleCorrection);

}
void test_angle_direction() {
    TEST_ASSERT_EQUAL_INT(1,windConfig->config->signCorrection);
    MockStreamLoader loader;
    loader.load("angle dir 0\n");
    windConfig->process();
    TEST_ASSERT_EQUAL_INT(-1,windConfig->config->signCorrection);
    loader.load("angle dir 1\n");
    windConfig->process();
}

void test_angle_configuration() {
    MockStreamLoader loader;
    loader.load("angle size 36\n");
    windConfig->process(); 
    loader.load("angles 0,100,1,101,2,102,3,103,4,104,5,105,6,106,7,107,8,108,9,109\n");
    windConfig->process(); 
    loader.load("angles 10,110,11,111,12,112,13,113,14,114,15,115,16,116,17,117,18,118,19,119\n");
    windConfig->process(); 
    loader.load("a 20,120,21,121,22,122,23,123,24,124,25,125,26,126,27,127,28,128,29,129\n");
    windConfig->process(); 
    loader.load("angles 30,130,31,131,32,132,33,133,34,134,35,135\n");
    windConfig->process(); 
    TEST_ASSERT_EQUAL_INT(36,windConfig->config->angleTableSize);
    for (int i = 0; i < 36; i++) {
        TEST_ASSERT_EQUAL_INT16(i+100,windConfig->config->angleTable[i]);
    }
}

void test_speed_configuration() {
    MockStreamLoader loader;
    loader.load("speed size 5\n");
    windConfig->process(); 
    loader.load("speed table 0,5.5,20.5,40.6,45.6\n");
    windConfig->process(); 
    loader.load("speed values 0,1.5,2.5,4.6,6.7\n");
    windConfig->process(); 
    TEST_ASSERT_EQUAL_INT(5,windConfig->config->speedTableSize);
    double speedTable[] = {0,5.5,20.5,40.6,45.6};
    double speed[] = {0,1.5,2.5,4.6,6.7};
    for (int i = 0; i < 5; i++ ) {
        if ( fabs(speedTable[i]-windConfig->config->speedTable[i]) > 0.001 || fabs(speed[i]-windConfig->config->speed[i]) > 0.001 ) {
        std::cout << "FAIL" << i << "," << speedTable[i]  << "," << speed[i] << std::endl;
        std::cout << "FAIL" << i << "," << windConfig->config->speedTable[i]  << "," << windConfig->config->speed[i] << std::endl;
        std::cout << "FAIL" << i << "," << speedTable[i]-windConfig->config->speedTable[i]  << "," << speed[i]-windConfig->config->speed[i] << std::endl;

        }
        TEST_ASSERT_TRUE(fabs(speedTable[i]-windConfig->config->speedTable[i]) < 0.001);
        TEST_ASSERT_TRUE(fabs(speed[i]-windConfig->config->speed[i]) < 0.001);
    }
}

/*
 io->println("Commands:");
        io->println("help|? - this help");
        io->println("as|angle size <n>         - set the number of angle caibrations.");
        io->println("a|angle <n>,<v>,<n1>,<v1> - set the angle calibrations, where n and v are ints. n = slot (0-slot), v = offset (+-)");
        io->println("st|speed table <n>,<n>    - Set the speed table up, where n is the puse frequency in Hz ");
        io->println("sv|speed values <n>,<n>   - Set the speed values up matching the speed table where n is the speed in m/s");
        io->println("dump                      - Dumps the current configuration");
        io->println("save                      - Saves the current configuration and makes it active.");
        io->print("$>")
*/
void test_config() {
    MockStreamLoader loader;

    loader.load("help\n");
    windConfig->process(); 
    loader.load("angle size 36\n");
    windConfig->process(); 
    loader.load("angles 0,100,1,101,2,102,3,103,4,104,5,105,6,106,7,107,8,108,9,109\n");
    windConfig->process(); 
    loader.load("angles 10,110,11,111,12,112,13,113,14,114,15,115,16,116,17,117,18,118,19,119\n");
    windConfig->process(); 
    loader.load("a 20,120,21,121,22,122,23,123,24,124,25,125,26,126,27,127,28,128,29,129\n");
    windConfig->process(); 
    loader.load("angles 30,130,31,131,32,132,33,133,34,134,35,135\n");
    windConfig->process(); 
    loader.load("speed size 5\n");
    windConfig->process(); 
    loader.load("speed table 0,5.5,20.5,40.6,45.6\n");
    windConfig->process(); 
    loader.load("speed values 0,1.5,2.5,4.6,6.7\n");
    windConfig->process(); 
    loader.load("dump\n");
    windConfig->process(); 
    loader.load("help\n");
    windConfig->process(); 
}



int main(int argc, char **argv) { 
    try {
        UNITY_BEGIN();
        RUN_TEST(test_help);
        RUN_TEST(test_dump);
        RUN_TEST(test_angle_max);
        RUN_TEST(test_angle_correction);
        RUN_TEST(test_angle_direction);
        RUN_TEST(test_angle_configuration);
        RUN_TEST(test_speed_configuration);
        RUN_TEST(test_config);
        return UNITY_END();
    } catch( UnexpectedMethodCallException e) {
            std::cout << "Exception:" << e << std::endl;

    }
}