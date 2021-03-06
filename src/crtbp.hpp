//
//  crtbp.cpp
//
//
//  Created by Protoss Probe on 2017/06/07.
//  Copyright � 2016-2017 probe. All rights reserved.
//

#ifndef _CRTBP_HPP_
#define _CRTBP_HPP_

#include "utility.hpp"
#include <boost/array.hpp>
#include <boost/numeric/odeint.hpp>
#include <cmath>
#include <fstream>
#include <iostream>
#include <libiomp/omp.h>
#include <string>

// define some handy constants
const double pi = M_PI;
const double pi2 = 2 * M_PI;
const double pi180 = pi / 180;
const double pi_180 = 180 / pi;

/**
 * with the assumption of the total mass of two bodies is equal to 1,
 * we define:
 *      mu - mass of the planet (a.k.a the secondaty);
 *      mu_sun - mass of the center body (a.k.a the primary);
 *      year - days in one sidereal year (to convert unit between real model and
        dimensionless model);
 *      muu - 1 - mu;
 *      MAX_NUMBER - max number of particles can be handle in this program;
*/

const size_t PLANET_INDEX = 3;

const double mu_y = 1.66013679527193009e-7;
const double mu_v = 2.44783833966454430e-6;
const double mu_e = 3.04043264264672381e-6;
const double mu_m = 3.22715144505386530e-7;
const double mu_j = 9.54791938424326609e-4;
const double mu_s = 2.85885980666130812e-4;
const double mu_u = 4.36624404335156298e-5;
const double mu_n = 5.15138902046611451e-5;

const double mu_group[] = {mu_y, mu_v, mu_e, mu_m, mu_j, mu_s, mu_u, mu_n};

const double d_y = 0.387;
const double d_v = 0.723;
const double d_e = 1;
const double d_m = 1.52;
const double d_j = 5.20;
const double d_s = 9.58;
const double d_u = 19.20;
const double d_n = 30.05;

const double d_group[] = {d_y, d_v, d_e, d_m, d_j, d_s, d_u, d_n};

const double mu = mu_group[PLANET_INDEX-1];
const double PLANET_DISTANCE = d_group[PLANET_INDEX-1];
const double mu_sun = 1 - mu;

const double year = 365.25;
const double muu = mu_sun;
const size_t MAX_NUMBER = 100;

// ouput file location
const std::string GLOBAL_OUTPUT_LOCATION = "assets/_output/";
const std::string GLOBAL_LOCATION = "assets/";

// define some handy vector types
typedef boost::array<double, 2> vec2;
typedef boost::array<double, 3> vec3;
typedef boost::array<double, 4> vec4;
typedef boost::array<double, 5> vec5;
typedef boost::array<double, 6> vec6;
typedef boost::array<double, 7> vec7;
typedef boost::array<double, 12> vec12;

// orbit3d class
class crtbp;
class orbit3d {
    friend class crtbp;

  private:
    double div = 1e-8;
    double megno_temp = 0;
    double megno_fast = 0;
    double megno_sum = 0;
    double ticktime = 0.0;
    size_t new_steps = 0;

    vec2 deltaNormCal();
    void updateInerState();
    void updateJacobi();
    void updateMEGNO();
    void updateElements();

  public:
    // name of this orbit
    std::string name = "default";

    // initial jacobi constant
    double jacobi0 = 0;

    // current jacobi constant
    double jacobi = 0;

    // current relative jacobi error
    double jacobi_err = 0;

    // current time
    double current_t = 0;

    // current time interval
    double dt = 0.001;

    // current average megno (not yet calculated until time > ticktime)
    double megno = 0;

    // maximum megno value during the ingegration
    double megno_max = 0;

    // current number of steps;
    size_t steps = 0;

    // assigned file for output;
    std::ofstream outputfile;

    // a vector composed of a state vector (in the rotating frame) and a
    // variational vector
    vec12 vec = {{0, 0, 0, 0, 0, 0, div, div, div, div, div, div}};

    // an orbital elements vector {{a,e,I,g,n,f}} or {{a,e,I,g,n,m}}
    // a = semi-major axis (in AU)
    // e = eccentricity
    // I = inclination (degrees)
    // g = argument of pericentre (degrees)
    // n = longitude of the ascending node (degrees)
    // f = true anomaly (degrees)
    // m = mean anomaly (degrees)
    vec6 ele = {{1, 0, 0, 0, 0, 0}};

    // a state vector in the inertial frame
    vec6 vec_inertial = {{0, 0, 0, 0, 0, 0}};

    // get & set functions
    // declared and defined in this header file
    double getTime() const { return current_t; }
    double getTimeYear() const { return current_t / pi2; }
    vec6 getState() const {
        return {{vec[0], vec[1], vec[2], vec[3], vec[4], vec[5]}};
    }
    vec6 getDelta() const {
        return {{vec[6], vec[7], vec[8], vec[9], vec[10], vec[11]}};
    }
    vec3 getPos() const { return {{vec[0], vec[1], vec[2]}}; }
    vec3 getVel() const { return {{vec[3], vec[4], vec[5]}}; }
    vec6 getInerState() const { return vec_inertial; }
    vec3 getInerPos() const {
        return {{vec_inertial[0], vec_inertial[1], vec_inertial[2]}};
    }
    vec3 getInerVel() const {
        return {{vec_inertial[3], vec_inertial[4], vec_inertial[5]}};
    }
    std::string getName() const { return name; };
    vec6 getElements() const { return ele; };
    double getJacobi() const { return jacobi; };
    double getJacobiError() const { return jacobi_err; };
    double getMEGNO() const { return megno; };
    double getMEGNOMax() const { return megno_max; };
    inline orbit3d &setState(const vec6 &state) {
        for (size_t i = 0; i < state.size(); i++)
            vec[i] = state[i];
        return *this;
    };
    inline orbit3d &setElement(vec6 element) {
        ele = element;
        return *this;
    };
    inline orbit3d &setName(std::string newname) {
        name = newname;
        return *this;
    };
    inline orbit3d &setDt(double t) {
        dt = t;
        return *this;
    };
    inline orbit3d &setOutputFile() {
        std::string location = GLOBAL_OUTPUT_LOCATION + "Ast_" + getName();
        outputfile.open(location + ".txt");
        return *this;
    };
    inline orbit3d &setTickTime(double tick) {
        ticktime = tick;
        return *this;
    }
    orbit3d &setInitial(vec6 elements, double dtt, double endt, std::string na);
    void closeOutputFile() { outputfile.close(); };

    // declared here, defined in crtbp.cpp
    double updatePerStep(const double t);
    void updatePerOutput();
    double getLCN();
};

static void printInteData(orbit3d &orb) {
    std::cout << "State:  ";
    for (double j : orb.getPos())
        std::cout << std::setw(12) << j << " ";
    std::cout << "  |   "
              << "Time: " << std::setw(6) << orb.getTime()
              << "   |   Jacobi: " << std::setw(8) << orb.getJacobi()
              << "   |   LCN: " << std::setw(8) << orb.getLCN()
              << "   |   MEGNO: " << std::setw(8) << orb.getMEGNO()
              << std::endl;
}

static void writeElemData(orbit3d &orb) {
    orb.outputfile << std::setw(8) << orb.getTimeYear() << '\t';
    for (double j : orb.getElements())
        orb.outputfile << std::setw(10) << j << '\t';
    orb.outputfile << std::setw(10) << orb.getJacobiError() << '\t'
                   << std::setw(10) << orb.getMEGNO() << std::endl;
}

static void writePosData(orbit3d &orb) {
    orb.outputfile << std::setw(8) << orb.getTimeYear() << '\t';
    for (double j : orb.getInerPos())
        orb.outputfile << std::setw(10) << j << '\t';
    orb.outputfile << std::setw(10) << orb.getJacobiError() << '\t'
                   << std::setw(10) << orb.getMEGNO() << std::endl;
}

static void writeElemPosData(orbit3d &orb) {
    orb.outputfile << std::setw(8) << orb.getTimeYear() << '\t';
    for (double j : orb.getPos())
        orb.outputfile << std::setw(10) << j << '\t';
    for (double j : orb.getElements())
        orb.outputfile << std::setw(10) << j << '\t';
    orb.outputfile << std::setw(10) << orb.getJacobiError() << '\t'
                   << std::setw(10) << orb.getMEGNO() << std::endl;
}

struct observer {
    orbit3d *orb;
    size_t jump;
    observer(orbit3d &orbit, size_t jump) : orb(&orbit), jump(jump) {}
    void operator()(const vec12 &x, double t) {
        if (t > 0) {
            auto megno = (*orb).updatePerStep(t);
            if (megno >= 8) {
                throw 8.0;
                return;
            }
        }
        if ((*orb).steps % jump == 0) {
            (*orb).updatePerOutput();
            writeElemPosData(*orb);
            // writeElemData(*orb);
            // writePosData(*orb);
        }
    }
};

struct observer_default {
    void operator()(const vec6 &x, double t) {
        std::cout << "Time: " << t << '\t';
        for (auto var : x) {
            std::cout << "State: " << var << '\t';
        }
        std::cout << std::endl;
    }
};

class crtbp {
  public:
    crtbp() = default;
    ~crtbp() = default;
    std::ofstream summary;
    void inteSingleAdaptive(orbit3d &orbit, double endtime, size_t jump);
    void inteSingle(orbit3d &orbit, double endtime, size_t jump);
    void inteNbody(orbit3d orbits[], size_t n, double endtime, size_t jump);
    void writeSummary(orbit3d &orbit);
    static double jacobiConstant(const vec6 &x);
    static void eqOfMotion(const vec6 &x, vec6 &dxdt);
    static void eqOfVariation(const vec6 &x, vec6 &dxdt, const vec6 &p);
    static void eqOfTwoBody(const vec6 &x, vec6 &dxdt);
    static vec6 elementsToState(const vec6 &in);
    static vec6 stateToElements(const vec6 &in, const char option = 'f');
    static vec6 inertialToRot(const vec6 &x, const double t);
    static vec6 rotToInertial(const vec6 &x, const double t);
    static vec6 elementsToRot(const vec6 &x, const double t);
    static double mean2true(const double M, const double e, const double acc);
    static double true2mean(double theta, double e);

  private:
    class crtbp_ode {
      public:
        void operator()(const vec6 &x, vec6 &dxdt, double t);
    };

    class crtbp_ode_variation {
      public:
        void operator()(const vec12 &x, vec12 &dxdt, double t);
    };

    class crtbp_two_body {
      public:
        void operator()(const vec6 &x, vec6 &dxdt, double t);
    };

    static double keplerIteration(double E, double e, double M);
    static vec6 uxxMatrix(const vec3 &x);
};

static std::vector<vec6> readInputFromTxt(const std::string &inputstring) {
    std::ifstream inputfile;
    inputfile.open(inputstring);
    std::vector<vec6> inputmatrix;
    std::string name;
    vec6 inputarray;
    if (inputfile.is_open()) {
        while (!inputfile.eof()) {
            for (size_t i = 0; i < 6; i++) {
                inputfile >> inputarray[i];
            }
            inputmatrix.push_back(inputarray);
        }
    }
    return inputmatrix;
}

static vec3 readInfoFromTxt(const std::string &infostring) {
    std::ifstream infofile;
    infofile.open(infostring);
    vec3 infoarray;
    if (infofile.is_open()) {
        while (!infofile.eof()) {
            for (size_t i = 0; i < infoarray.size(); i++) {
                infofile >> infoarray[i];
            }
        }
    }
    return infoarray;
}

#endif