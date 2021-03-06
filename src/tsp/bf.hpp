/// Contains methods for brute-forcing TSP.
//
// Given the matrix:
//
//  X ----------------------------->
// Y
// |    0,   64,  378, 519, 434, 200,
// |    64,  0,   318, 455, 375, 164,
// |    378, 318, 0,   170, 265, 344,
// |    519, 455, 170, 0,   223, 428,
// |    434, 375, 265, 223, 0,   273,
// V    200, 164, 344, 428, 273, 0
//
// We need to visit each Y only once, to do that we can generate all
// *unique* visitations orders using std::next_permutation and
// limiting the first number to 0, yielding the following:
//
//     {0, 1, 2, 3, 4, 5}
//     {0, 1, 2, 3, 5, 4}
//     {0, 1, 2, 4, 3, 5}
//             ...
//     {0, 5, 4, 2, 3, 1}
//     {0, 5, 4, 3, 1, 2}
//     {0, 5, 4, 3, 2, 1}
//
// > We fix the first element to 0 since we have to visit *every*
// > node it doens't matter where we start, as long as we end on
// > the same spot. Limiting it makes sure we won't double check
// > some path we tried before.
//
// We add a 0, to the end of each permutation and then we can do the
// following transformation to get the {X, Y}s we should visit:
//
//     {0, 1, 2, 3, 4, 5, 0}
//      |  |  |  |  |  |  |
//      \  /\ |\ \  |  |  |
//       \/  \| \ \   ...
//       /\   \  \ \.
//      /  \  |\  \ \.
//      |  |  | \  \|
// {    V  V  |  |  |\.
//     {1, 0},V  V  | \.
//           {2, 1},V  V
//                 {3, 2},
//                       {4, 3},
//                             {5, 4},
//                                   {0, 5}
// }
//
// > Hopefully that was somewhat helpful.
//
// As we visit these points we add the values we get and keep the
// lowest sum.
//
// Et voilà.
#pragma once

#include <algorithm> // For std::next_permutation and std::generate.
#include <optional>
#include <limits> // For std::numeric_limits.
#include <thread>
#include <tuple>
#include <array>

#include "utils.hpp"

namespace tsp
{
    // As we'll see later, the fix is needed to make this usable by the parallel implementation.
    template<typename T, std::size_t Cells>
    inline auto seq_brute_force(
        const utils::bidimensional_access<T, Cells>& mat,
        const std::optional<int> fix1 = {}
    ) -> std::tuple< std::size_t, std::array<std::size_t, utils::ct_sqrt(Cells) + 1> >;

    template<typename T, std::size_t Cells>
    inline auto par_brute_force(
        const utils::bidimensional_access<T, Cells>& mat
    ) -> std::tuple< std::size_t, std::array<std::size_t, utils::ct_sqrt(Cells) + 1> >;
}

// This is a single threaded (sequential) brute_force, where we can add a fix (fix1),
// which controls the second element of the permutation array.
//
// Setting a fix to the first position makes the function avoid redundant work.
//
// Setting a fix to the second position (fix1) will make the function only check
// for the combinations beginning in {fix0, fix1, ...}, this is useful for splitting
// up the work between multiple threads, by default it is set to nullopt (not present).
// Setting it to some number will enable it.
template<typename T, std::size_t Cells>
inline auto tsp::seq_brute_force(
    const utils::bidimensional_access<T, Cells>& mat,
    const std::optional<int> fix1
) -> std::tuple< std::size_t, std::array<std::size_t, utils::ct_sqrt(Cells) + 1> >
{
    auto smallest_cicle = std::array<std::size_t, utils::ct_sqrt(Cells) + 1 >{};
    auto v = std::array<
        std::size_t,
        utils::ct_sqrt(Cells) + 1 // + 1 since we want an additional 0 at the end.
    >{};
    v[0] = 0;
    v[1] = fix1.value_or(1);

    // This is basically std::iota but skipping a number.
    std::generate(
        v.begin() + 2, v.end() - 1, // - 1 since we want the 0 at the end to stay.
        [current = std::size_t{1}, skip = v[1]]() mutable {
            if(current == skip) ++current;
            return current++;
        }
    );

    auto lowest_cost = std::numeric_limits<std::size_t>::max();

    const auto fix_index = int{ !!fix1 };
    while( std::next_permutation(v.begin() + 1 + fix_index, v.end() - 1) )
    {
        auto current_cost = decltype(lowest_cost){0};
        for(std::size_t i = 1; i < v.size(); ++i) {
            current_cost += mat[ {static_cast<std::size_t>( v[i] ), static_cast<std::size_t>( v[i - 1] )} ];
            if(current_cost >= lowest_cost) break;
        }

        if(current_cost < lowest_cost) {
            lowest_cost = current_cost;
            smallest_cicle = v;
        }
    }

    return {lowest_cost, smallest_cicle};
}

// Here we will spawn sqrt(Cells) - 1 threads and each of them will be responsible
// for a different fix1.
// For example:
//     Thread 1 is responsible for {0, 1, 2, 3, 4, 5} to {0, 1, 5, 4, 3, 2}
//     Thread 2 is responsible for {0, 2, 1, 3, 4, 5} to {0, 2, 5, 4, 3, 1}
//     ...
//     Thread 5 is responsible for {0, 5, 1, 2, 3, 4} to {0, 5, 4, 3, 2, 1}
// And so on.
template<typename T, std::size_t Cells>
inline auto tsp::par_brute_force(const utils::bidimensional_access<T , Cells>& mat)
-> std::tuple< std::size_t, std::array<std::size_t, utils::ct_sqrt(Cells) + 1> >
{
    auto workers = std::array<
        std::thread,
        utils::ct_sqrt(Cells) - 1
    >{};
    auto results = std::array<
        std::tuple< std::size_t, std::array<std::size_t, utils::ct_sqrt(Cells) + 1> >,
        utils::ct_sqrt(Cells) - 1
    >{};

    for(decltype(results.size()) i = 0; i < results.size(); ++i)
    {
        workers[i] = std::thread{
            [&, i, fix1 = i + 1]{ results[i] = seq_brute_force(mat, fix1); }
        };
    }
    for(auto& worker : workers) { worker.join(); }

    auto lowest_i = 0;
    for(std::size_t i = 0 ; i < results.size(); ++i) {
        if( std::get<0>(results[i]) < std::get<0>(results[lowest_i]) ) {
            lowest_i = i;
        }
    }
    return results[lowest_i];
}
