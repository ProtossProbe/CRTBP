//
//  crtbp.cpp
//
//
//  Created by Protoss Probe on 2017/06/07.
//  Copyright © 2016-2017年 probe. All rights reserved.
//

#include "crtbp.hpp"
#include <boost/array.hpp>
#include <boost/numeric/odeint.hpp>
#include <cmath>
#include <fstream>
#include <iostream>
#include <sstream>
#include <string>

using namespace std;
using namespace boost::math;
using namespace boost::numeric::odeint;

int main(int argc, char *argv[]) {
    string inputstring(argv[1]);
    auto input = readInputFromTxt(inputstring);

    outputfile.open("assets/rot.txt");
    crtbp system;
    orbit3d orbit;
    orbit.setState(input[0]);
    system.inteSingle(orbit, 1000, 0.01);
    outputfile.close();
}