#include "fmt/core.h"
#include "fmt/ranges.h"

#include "tsp/data.hpp"
#include "tsp/bf.hpp"
#include "tsp/approx.hpp"

#include <chrono>
#include <cstring> // For strcmp.

namespace ch = std::chrono;
using sc = ch::system_clock;
using us = ch::microseconds;
using ms = ch::milliseconds;
using s  = ch::seconds;
template< typename T, typename U>
constexpr auto to(U&& t) { return ch::duration_cast<T>(t).count(); }

int main(int argc, char* argv[])
{
    // Defaults
    auto p1_bf_st = true;
    auto p2_bf_st = true;
    auto p3_bf_st = true;
    auto p4_bf_st = false;
    auto p5_bf_st = false;

    auto p1_bf_mt = true;
    auto p2_bf_mt = true;
    auto p3_bf_mt = true;
    auto p4_bf_mt = false;
    auto p5_bf_mt = false;

    auto p1_approx = true;
    auto p2_approx = true;
    auto p3_approx = true;
    auto p4_approx = true;
    auto p5_approx = true;

    #define CHECK_ARG(argstr, argname, enableflag, disableflag)     \
        if     ( strcmp(argstr, enableflag) == 0)  { argname = 1; } \
        else if( strcmp(argstr, disableflag) == 0) { argname = 0; }

    for(auto arg = 1; arg < argc; ++arg)
    {
        CHECK_ARG(argv[arg], p1_bf_st, "--p1:seq-brute-force", "--p1:no-seq-brute-force");
        CHECK_ARG(argv[arg], p2_bf_st, "--p2:seq-brute-force", "--p2:no-seq-brute-force");
        CHECK_ARG(argv[arg], p3_bf_st, "--p3:seq-brute-force", "--p3:no-seq-brute-force");
        CHECK_ARG(argv[arg], p4_bf_st, "--p4:seq-brute-force", "--p4:no-seq-brute-force");
        CHECK_ARG(argv[arg], p5_bf_st, "--p5:seq-brute-force", "--p5:no-seq-brute-force");
        CHECK_ARG(argv[arg], p1_bf_mt, "--p1:par-brute-force", "--p1:no-par-brute-force");
        CHECK_ARG(argv[arg], p2_bf_mt, "--p2:par-brute-force", "--p2:no-par-brute-force");
        CHECK_ARG(argv[arg], p3_bf_mt, "--p3:par-brute-force", "--p3:no-par-brute-force");
        CHECK_ARG(argv[arg], p4_bf_mt, "--p4:par-brute-force", "--p4:no-par-brute-force");
        CHECK_ARG(argv[arg], p5_bf_mt, "--p5:par-brute-force", "--p5:no-par-brute-force");
        CHECK_ARG(argv[arg], p1_approx, "--p1:approx", "--p1:no-approx");
        CHECK_ARG(argv[arg], p2_approx, "--p2:approx", "--p2:no-approx");
        CHECK_ARG(argv[arg], p3_approx, "--p3:approx", "--p3:no-approx");
        CHECK_ARG(argv[arg], p4_approx, "--p4:approx", "--p4:no-approx");
        CHECK_ARG(argv[arg], p5_approx, "--p5:approx", "--p5:no-approx");
    }

    #define TIME_IF_SET(varname, problem, method, timeunit)                                                  \
        if(varname) {                                                                                        \
            auto start  = sc::now();                                                                         \
            auto answer = tsp::method(tsp::data::problem);                                                   \
            auto end    = sc::now();                                                                         \
            fmt::print(                                                                                      \
                #problem ": tsp::" #method ": {}" #timeunit " cost(answer vs min) = {} vs {}, cicle = {}\n", \
                to<timeunit>(end - start),                                                                   \
                std::get<0>(answer),                                                                         \
                tsp::data::problem##_answer,                                                                 \
                std::get<1>(answer)                                                                          \
            );                                                                                               \
        } else {fmt::print( #problem ": tsp::" #method ": SKIPPED\n");}

    TIME_IF_SET(p1_approx, p1, approx, us);
    TIME_IF_SET(p2_approx, p2, approx, us);
    TIME_IF_SET(p3_approx, p3, approx, us);
    TIME_IF_SET(p4_approx, p4, approx, us);
    TIME_IF_SET(p5_approx, p5, approx, us);

    TIME_IF_SET(p1_bf_mt, p1, par_brute_force, ms);
    TIME_IF_SET(p2_bf_mt, p2, par_brute_force, us);
    TIME_IF_SET(p3_bf_mt, p3, par_brute_force, s);
    TIME_IF_SET(p4_bf_mt, p4, par_brute_force, s);
    TIME_IF_SET(p5_bf_mt, p5, par_brute_force, s);

    TIME_IF_SET(p1_bf_st, p1, seq_brute_force, ms);
    TIME_IF_SET(p2_bf_st, p2, seq_brute_force, us);
    TIME_IF_SET(p3_bf_st, p3, seq_brute_force, s);
    TIME_IF_SET(p4_bf_st, p4, seq_brute_force, s);
    TIME_IF_SET(p5_bf_st, p5, seq_brute_force, s);

    return 0;
}
