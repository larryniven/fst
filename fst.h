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

    template <class fst>
    struct fst_trait;

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

    template <class vertex_, class edge_, class input_symbol_, class output_symbol_>
    struct fst_trait<fst<vertex_, edge_, input_symbol_, output_symbol_>> {
        using vertex = vertex_;
        using edge = edge_;
        using input_symbol = input_symbol_;
        using output_symbol = output_symbol_;
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

    template <class fst1_type, class fst2_type>
    struct pair_fst {

        using vertex = std::tuple<typename fst1_type::vertex, typename fst2_type::vertex>;
        using edge = std::tuple<typename fst1_type::edge, typename fst2_type::edge>;
        using input_symbol = typename fst1_type::input_symbol;
        using output_symbol = typename fst2_type::output_symbol;

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

        virtual std::unordered_map<input_symbol, std::vector<edge>> const&
        in_edges_input_map(vertex v) const = 0;

        virtual std::unordered_map<output_symbol, std::vector<edge>> const&
        in_edges_output_map(vertex v) const = 0;

        virtual std::unordered_map<input_symbol, std::vector<edge>> const&
        out_edges_input_map(vertex v) const = 0;

        virtual std::unordered_map<output_symbol, std::vector<edge>> const&
        out_edges_output_map(vertex v) const = 0;

        virtual fst1_type& fst1() = 0;
        virtual fst1_type const& fst1() const = 0;
        virtual fst2_type& fst2() = 0;
        virtual fst2_type const& fst2() const = 0;

    };

    template <class fst1, class fst2>
    struct fst_trait<pair_fst<fst1, fst2>> {
        using vertex = typename pair_fst<fst1, fst2>::vertex;
        using edge = typename pair_fst<fst1, fst2>::edge;
        using input_symbol = typename pair_fst<fst1, fst2>::input_symbol;
        using output_symbol = typename pair_fst<fst1, fst2>::output_symbol;
    };

    template <class fst1_type, class fst2_type>
    struct lazy_pair_fst
        : public pair_fst<fst1_type, fst2_type> {

        fst1_type fst1_;
        fst2_type fst2_;

        using typename pair_fst<fst1_type, fst2_type>::vertex;
        using typename pair_fst<fst1_type, fst2_type>::edge;
        using typename pair_fst<fst1_type, fst2_type>::input_symbol;
        using typename pair_fst<fst1_type, fst2_type>::output_symbol;

        mutable std::shared_ptr<std::vector<vertex>> vertices_cache;
        mutable std::shared_ptr<std::vector<edge>> edges_cache;
        mutable std::shared_ptr<std::vector<vertex>> initials_cache;
        mutable std::shared_ptr<std::vector<vertex>> finals_cache;
        mutable std::shared_ptr<vertex> in_edges_vertex;
        mutable std::shared_ptr<std::vector<edge>> in_edges_cache;
        mutable std::shared_ptr<vertex> out_edges_vertex;
        mutable std::shared_ptr<std::vector<edge>> out_edges_cache;

        mutable std::shared_ptr<vertex> in_edges_input_map_vertex;
        mutable std::shared_ptr<std::unordered_map<input_symbol,
            std::vector<edge>>> in_edges_input_map_cache;

        mutable std::shared_ptr<vertex> in_edges_output_map_vertex;
        mutable std::shared_ptr<std::unordered_map<output_symbol,
            std::vector<edge>>> in_edges_output_map_cache;

        mutable std::shared_ptr<vertex> out_edges_input_map_vertex;
        mutable std::shared_ptr<std::unordered_map<input_symbol,
            std::vector<edge>>> out_edges_input_map_cache;

        mutable std::shared_ptr<vertex> out_edges_output_map_vertex;
        mutable std::shared_ptr<std::unordered_map<output_symbol,
            std::vector<edge>>> out_edges_output_map_cache;

        lazy_pair_fst(fst1_type fst1, fst2_type fst2);

        virtual std::vector<vertex> const& vertices() const override;
        virtual std::vector<edge> const& edges() const override;
        virtual vertex tail(edge e) const override;
        virtual vertex head(edge e) const override;
        virtual std::vector<edge> const& in_edges(vertex v) const override;
        virtual std::vector<edge> const& out_edges(vertex v) const override;
        virtual std::vector<vertex> const& initials() const override;
        virtual std::vector<vertex> const& finals() const override;
        virtual double weight(edge e) const override;
        virtual input_symbol const& input(edge e) const override;
        virtual output_symbol const& output(edge e) const override;

        virtual std::unordered_map<input_symbol, std::vector<edge>> const&
        in_edges_input_map(vertex v) const;

        virtual std::unordered_map<output_symbol, std::vector<edge>> const&
        in_edges_output_map(vertex v) const;

        virtual std::unordered_map<input_symbol, std::vector<edge>> const&
        out_edges_input_map(vertex v) const;

        virtual std::unordered_map<output_symbol, std::vector<edge>> const&
        out_edges_output_map(vertex v) const;

        virtual fst1_type& fst1() override;
        virtual fst1_type const& fst1() const override;
        virtual fst2_type& fst2() override;
        virtual fst2_type const& fst2() const override;

    };

    template <class fst1, class fst2>
    struct fst_trait<lazy_pair_fst<fst1, fst2>> {
        using vertex = typename lazy_pair_fst<fst1, fst2>::vertex;
        using edge = typename lazy_pair_fst<fst1, fst2>::edge;
        using input_symbol = typename lazy_pair_fst<fst1, fst2>::input_symbol;
        using output_symbol = typename lazy_pair_fst<fst1, fst2>::output_symbol;
    };

    template <class fst1_type, class fst2_type>
    struct lazy_pair_mode1_fst
        : public lazy_pair_fst<fst1_type, fst2_type> {

        using typename pair_fst<fst1_type, fst2_type>::vertex;
        using typename pair_fst<fst1_type, fst2_type>::edge;
        using typename pair_fst<fst1_type, fst2_type>::input_symbol;
        using typename pair_fst<fst1_type, fst2_type>::output_symbol;

        lazy_pair_mode1_fst(fst1_type fst1, fst2_type fst2);

        virtual std::vector<edge> const& in_edges(vertex v) const override;
        virtual std::vector<edge> const& out_edges(vertex v) const override;
        
        virtual std::vector<edge> const& edges() const override;

        virtual std::unordered_map<input_symbol, std::vector<edge>> const&
        in_edges_input_map(vertex v) const override;

        virtual std::unordered_map<output_symbol, std::vector<edge>> const&
        in_edges_output_map(vertex v) const override;

        virtual std::unordered_map<input_symbol, std::vector<edge>> const&
        out_edges_input_map(vertex v) const override;

        virtual std::unordered_map<output_symbol, std::vector<edge>> const&
        out_edges_output_map(vertex v) const override;

    };

    template <class fst1, class fst2>
    struct fst_trait<lazy_pair_mode1_fst<fst1, fst2>> {
        using vertex = typename lazy_pair_mode1_fst<fst1, fst2>::vertex;
        using edge = typename lazy_pair_mode1_fst<fst1, fst2>::edge;
        using input_symbol = typename lazy_pair_mode1_fst<fst1, fst2>::input_symbol;
        using output_symbol = typename lazy_pair_mode1_fst<fst1, fst2>::output_symbol;
    };

    template <class fst1_type, class fst2_type>
    struct lazy_pair_mode2_fst
        : public lazy_pair_fst<fst1_type, fst2_type> {

        using typename pair_fst<fst1_type, fst2_type>::vertex;
        using typename pair_fst<fst1_type, fst2_type>::edge;
        using typename pair_fst<fst1_type, fst2_type>::input_symbol;
        using typename pair_fst<fst1_type, fst2_type>::output_symbol;

        lazy_pair_mode2_fst(fst1_type fst1, fst2_type fst2);

        virtual std::vector<edge> const& in_edges(vertex v) const override;
        virtual std::vector<edge> const& out_edges(vertex v) const override;
        
        virtual std::vector<edge> const& edges() const override;

        virtual std::unordered_map<input_symbol, std::vector<edge>> const&
        in_edges_input_map(vertex v) const override;

        virtual std::unordered_map<output_symbol, std::vector<edge>> const&
        in_edges_output_map(vertex v) const override;

        virtual std::unordered_map<input_symbol, std::vector<edge>> const&
        out_edges_input_map(vertex v) const override;

        virtual std::unordered_map<output_symbol, std::vector<edge>> const&
        out_edges_output_map(vertex v) const override;

    };

    template <class fst1, class fst2>
    struct fst_trait<lazy_pair_mode2_fst<fst1, fst2>> {
        using vertex = typename lazy_pair_mode2_fst<fst1, fst2>::vertex;
        using edge = typename lazy_pair_mode2_fst<fst1, fst2>::edge;
        using input_symbol = typename lazy_pair_mode2_fst<fst1, fst2>::input_symbol;
        using output_symbol = typename lazy_pair_mode2_fst<fst1, fst2>::output_symbol;
    };

}

#include "fst/fst-impl.h"

#endif
