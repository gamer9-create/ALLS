//
// Created by lalit on 11/30/2025.
//

#include "Utils.h"

using namespace std;
using namespace chrono;
double GetTimeUtils() {
    double da_time = 0;

    high_resolution_clock::time_point t1 = high_resolution_clock::now();
    da_time = duration<double>(t1.time_since_epoch()).count();

    return da_time;
}