#ifndef FST_H
#define FST_H

#include <unordered_map>
#include <tuple>
#include <vector>
#include <limits>
#include <algorithm>
#include <memory>
#include "ebt/ebt.h"

namespace fst {

    template <class vertex, class edge, class input_symbol, class output_symbol>
    struct fst {
        virtual ~fst()
        {}

        virtual std::vector<vertex> const& vertices() const = 0;
        virtual std::vector<vertex> const& edges() const = 0;
        virtual vertex tail(edge e) const = 0;
        virtual vertex head(edge e) const = 0;
        virtual std::vector<edge> const& in_edges(vertex v) const = 0;
        virtual std::vector<edge> const& out_edges(vertex v) const = 0;
        virtual std::vector<vertex> const& initials() const = 0;
        virtual std::vector<vertex> const& finals() const = 0;
        virtual double weight(edge e) const = 0;
        virtual input_symbol const& input(edge e) const = 0;
        virtual output_symbol const& output(edge e) const = 0;
    };

    template <class vertex>
    struct timed {
        virtual long time(vertex v) const = 0;
    };

    template <class vertex, class edge, class input_symbol, class output_symbol>
    struct adj_indexed {

        virtual std::unordered_map<input_symbol, std::vector<edge>> const&
        in_edges_input_map(vertex v) const = 0;

        virtual std::unordered_map<output_symbol, std::vector<edge>> const&
        in_edges_output_map(vertex v) const = 0;

        virtual std::unordered_map<input_symbol, std::vector<edge>> const&
        out_edges_input_map(vertex v) const = 0;

        virtual std::unordered_map<output_symbol, std::vector<edge>> const&
        out_edges_output_map(vertex v) const = 0;

    };

    /*
     * The class `symbol_trait` is useful for defining `eps`
     * and other special symbols.
     *
     */
    template <class symbol>
    struct symbol_trait;

    template <>
    struct symbol_trait<std::string> {
        static std::string eps;
    };

    /*
     * The class `edge_trait` is usefule for creating null edges.
     * Null edges are used, for example, in tracking back
     * the shortest path.
     *
     */
    template <class edge>
    struct edge_trait;

}

#endif
