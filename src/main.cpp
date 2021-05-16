#include "fmt/core.h"

#include "tsp/data.hpp"
#include "tsp/bf.hpp"

#include <chrono>

namespace ch = std::chrono;
using sc = ch::system_clock;
using ns = ch::nanoseconds;
using ms = ch::milliseconds;
using s  = ch::seconds;
template< typename T, typename U>
constexpr auto to(U&& t) { return ch::duration_cast<T>(t).count(); }

int main()
{
    auto answer = 0;
    decltype(sc::now()) start;
    decltype(sc::now()) end;

    start  = sc::now();
    answer = tsp::par_brute_force(tsp::data::p1);
    end    = sc::now();
    fmt::print("p1: {}ms => {}, {} expected\n", to<ms>(end - start), answer, tsp::data::p1_answer);

    start  = sc::now();
    answer = tsp::par_brute_force(tsp::data::p2);
    end    = sc::now();
    fmt::print("p2: {}ns => {}, {} expected\n", to<ns>(end - start), answer, tsp::data::p2_answer);

    start  = sc::now();
    answer = tsp::par_brute_force(tsp::data::p3);
    end    = sc::now();
    fmt::print("p3: {}s => {}, {} expected\n", to<s>(end - start), answer, tsp::data::p3_answer);

    start  = sc::now();
    answer = tsp::par_brute_force(tsp::data::p4);
    end    = sc::now();
    fmt::print("p4: {}s => {}, {} expected\n", to<s>(end - start), answer, tsp::data::p4_answer);

    start  = sc::now();
    answer = tsp::par_brute_force(tsp::data::p5);
    end    = sc::now();
    fmt::print("p5: {}s => {}, {} expected\n", to<s>(end - start), answer, tsp::data::p5_answer);

    return 0;
}
