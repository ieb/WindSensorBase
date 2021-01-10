
#pragma once

#include <Arduino.h>

#include <unity.h>
#include <iostream>

using namespace fakeit;


/**
 * When using this object make sure it stays in scope otherwise the lambdas will be called with 
 * references to memory that has gone and wierd things will happen.
 */ 
class MockStreamLoader {
    public:
       MockStreamLoader() {
        }
        void load(const char * _input) {
           input = _input;
           std::cout << "Input set to " << input  << "Length " << strlen(input) <<  std::endl;
           n = 0;
            When(Method(ArduinoFake(Stream), available)).AlwaysDo(
            [=]()->int{
                return this->available();
            });
            When(Method(ArduinoFake(Stream), read)).AlwaysDo(
            [=]()->char{
                return this->read();
            });
       }
       int available() {
           return input[n] != '\0';
       }
       char read() {
           char c = input[n];
           if ( c != '\0' ) {
               n++;
               return c;
           } else {
               return -1;
           }
       }
    private:
       const char * input;
       int n;

    
};




void setupStream() {

    When(OverloadedMethod(ArduinoFake(Stream), print, size_t(const char *))).AlwaysDo(
        [](const char * out)->int{ 
            std::cout << out;
            return 0;
        });
    When(OverloadedMethod(ArduinoFake(Stream), println, size_t(const char *))).AlwaysDo(
        [](const char * out)->int{ 
            std::cout << out << "[Serial]" << std::endl; 
            return 0;
        });
    When(OverloadedMethod(ArduinoFake(Stream), print, size_t(char))).AlwaysDo(
        [](char out)->int{ 
            std::cout << out; 
            return 0;
        });
    When(OverloadedMethod(ArduinoFake(Stream), println, size_t(char))).AlwaysDo(
        [](char out)->int{ 
            std::cout << out << "[Serial]" << std::endl; 
            return 0;
        });
    When(OverloadedMethod(ArduinoFake(Stream), println, size_t(int, int))).AlwaysDo(
        [](int out, int base)->int{ 
            std::cout << out << "[Serial]" << std::endl; 
            return 0;
        });
}
