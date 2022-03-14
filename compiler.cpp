#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <math.h>

#include "Function.h"

#define CATCH_CONFIG_MAIN  // This tells Catch to provide a main() - only do this in one cpp file
#define CATCH_CONFIG_ENABLE_BENCHMARKING
#include <catch2/catch_all.hpp>

TEST_CASE( "SimpleFunction2var", "[simfun]" )
{
    Function *f = new Function("f(x,y) = (x+y)/3");
	f->SetVariable("x",3);
    f->SetVariable("y",3);

    double cc=f->Calculate(nullptr);
    REQUIRE(cc == 2.0);
    REQUIRE(f->get_name() == "f");
    delete f;

}

TEST_CASE( "SimpleFunction1var", "[simfun]" )
{
    Function *f = new Function("f(x) = x*x + 3*x + 2");
    f->SetVariable("x", 1);

    double cc=f->Calculate(nullptr);
    REQUIRE(cc == 6.0);
    REQUIRE(f->get_name() == "f");
    delete f;

}


TEST_CASE( "SimpleNumber", "[simfun]" )
{
    Function *f = new Function("f(x) = 3");
    f->SetVariable("x", 1);

    double cc=f->Calculate(nullptr);
    REQUIRE(cc == 3);
    REQUIRE(f->get_name() == "f");
    delete f;

}




TEST_CASE( "Divide", "[simfun]" )
{
    Function *f = new Function("f(x) = (x-1)/(x+1)");
    f->SetVariable("x", 1);

    double cc=f->Calculate(nullptr);
    REQUIRE(cc == 0);
    REQUIRE(f->get_name() == "f");
    delete f;

}
