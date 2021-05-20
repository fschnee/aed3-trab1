// For our approximative algorithm we'll use prim's algorithm
// and do a MSP Tour, this method is 2-approximative.
#pragma once

#include <algorithm>
#include <numeric>
#include <array>
#include <set>

#include "utils.hpp"

namespace tsp
{
    // Forward decls.
    template<typename T, std::size_t Cells>
    inline auto prims(const utils::bidimensional_access<T, Cells>& mat) -> std::array<T, Cells>;
    template<typename T, std::size_t Cells, typename It>
    inline auto tour_mst(const utils::bidimensional_access<T, Cells>& mst, It&& visited_it, std::size_t node = 0) -> void;

    template<typename T, std::size_t Cells>
    inline auto approx(
        const utils::bidimensional_access<T, Cells>& mat
    ) -> std::tuple< std::size_t, std::array<std::size_t, utils::ct_sqrt(Cells) + 1> >
    {
        constexpr auto nodes = utils::ct_sqrt(Cells);

        const auto mst = prims(mat);

        auto visit_order = std::array<std::size_t, nodes + 1>{0}; // + 1 since we want a 0 at the end.
        tour_mst(utils::bidimensional_access{ mst }, visit_order.begin() + 1); // + 1 since we want to skip the first 0.

        auto cost = 0;
        for(std::size_t i = 1; i < visit_order.size(); ++i) {
            cost += mat[{
                static_cast<std::size_t>( visit_order[i] ),
                static_cast<std::size_t>( visit_order[i - 1] )}
            ];
        }
        return {cost, visit_order};
    }

    template<typename T, std::size_t Cells>
    inline auto prims(const utils::bidimensional_access<T, Cells>& mat) -> std::array<T, Cells>
    {
        auto mst = std::array<T, Cells>{0};

        auto visited_nodes = std::set<std::size_t>{};
        visited_nodes.insert(0);
        auto unvisited_nodes = std::set<std::size_t>{};
        for(auto i = 1; i < utils::ct_sqrt(Cells); ++i) { unvisited_nodes.insert(i); }

        while(!unvisited_nodes.empty())
        {
            auto smallest_edge        = std::numeric_limits<T>::max();
            auto smallest_edge_coords = std::tuple<std::size_t, std::size_t>{0, 0};

            // Find the smallest edge connecting to an unvisited node.
            for(auto visited_node : visited_nodes)
            {
                for(auto unvisited_node : unvisited_nodes)
                {
                    if(mat[{visited_node, unvisited_node}] < smallest_edge) {
                        smallest_edge        = mat[{visited_node, unvisited_node}];
                        smallest_edge_coords = {visited_node, unvisited_node};
                    }
                }
            }

            visited_nodes.insert( std::get<1>(smallest_edge_coords) );
            unvisited_nodes.erase( std::get<1>(smallest_edge_coords) );

            mst[
                std::get<0>(smallest_edge_coords) +
                std::get<1>(smallest_edge_coords) * utils::ct_sqrt(Cells)
            ] = smallest_edge;
        }

        return mst;
    }

    // Basically a BFS.
    template<typename T, std::size_t Cells, typename It>
    inline auto tour_mst(const utils::bidimensional_access<T, Cells>& mst, It&& visited_it, std::size_t node) -> void
    {
        // For each edge connected to node.
        for(std::size_t i = 0; i < utils::ct_sqrt(Cells); ++i)
        {
            if(!mst[{node, i}]) continue;
            // Visit it and their children.
            *(visited_it++) = i;
            tour_mst(mst, visited_it, i);
        }
    }
}