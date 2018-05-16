#include <stdio.h>
#include <string>
#include <iostream>
#include <algorithm>
#include <cmath>

using namespace std;


int main(int argc, char *argv[]) {

    std::cout << [](float f) { return abs(f); } (-3.5) << endl;

    return 0;
}