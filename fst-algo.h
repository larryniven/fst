#ifndef FST_ALGO_H
#define FST_ALGO_H

#incluce "fst/fst.h"

namespace fst {

    template <class fst>
    std::vector<typename fst::vertex> topo_order(fst const& f);

    template <class fst>
    struct forward_one_best {

        using vertex = typename fst::vertex;
        using edge = typename fst::edge;

        struct extra_data {
            edge pi;
            double value;
        };

        std::unordered_map<vertex, extra_data> extra;

        void merge(fst const& f, std::vector<vertex> const& order);

        std::vector<typename fst::edge> best_path(fst const& f);

    };

    template <class fst>
    struct backward_one_best {

        using vertex = typename fst::vertex;
        using edge = typename fst::edge;

        struct extra_data {
            edge pi;
            double value;
        };

        std::unordered_map<vertex, extra_data> extra;

        void merge(fst const& f, std::vector<vertex> const& order);

        std::vector<typename fst::edge> best_path(fst const& f);

    };

    template <class fst>
    struct forward_k_best {

        using vertex = typename fst::vertex;
        using edge = typename fst::edge;

        struct vertex_data {
            std::vector<std::tuple<edge, int, double>> deck;
            bool bottom_out;
        };

        struct edge_data {
            int top;
        };

        std::unordered_map<edge, edge_data> edge_extra;
        std::unordered_map<vertex, vertex_data> vertex_extra;

        void first_best(fst const& f, std::vector<vertex> const& order);
        void next_best(fst const& f, vertex const& final, int k);
        std::vector<edge> best_path(fst const& f, vertex const& final, int k);

    };

    template <class fst>
    struct forward_log_sum {

        using vertex = typename fst::vertex;
        using edge = typename fst::edge;

        std::unordered_map<vertex, double> extra;

        void merge(fst const& f, std::vector<vertex> const& order);

    };

    template <class fst>
    struct backward_log_sum {

        using vertex = typename fst::vertex;
        using edge = typename fst::edge;

        std::unordered_map<vertex, double> extra;

        void merge(fst const& f, std::vector<vertex> const& order);

    };

}

#include "fst/fst-algo-impl.h"

#endif
