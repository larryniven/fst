namespace fst {

    template <class fst1_type, class fst2_type>
    lazy_pair_fst<fst1_type, fst2_type>::lazy_pair_fst(fst1_type fst1, fst2_type fst2)
        : fst1_(fst1), fst2_(fst2)
    {}

    template <class fst1_type, class fst2_type>
    std::vector<typename lazy_pair_fst<fst1_type, fst2_type>::vertex> const&
    lazy_pair_fst<fst1_type, fst2_type>::vertices() const
    {
        if (vertices_cache == nullptr) {
            std::vector<typename lazy_pair_fst<fst1_type, fst2_type>::vertex> vertices;

            for (auto& v1: fst1_.vertices()) {
                for (auto& v2: fst2_.vertices()) {
                    vertices.push_back(std::make_tuple(v1, v2));
                }
            }

            vertices_cache = std::make_shared<std::vector<
                typename lazy_pair_fst<fst1_type, fst2_type>::vertex>>(vertices);
        }

        return *vertices_cache;
    }

    template <class fst1_type, class fst2_type>
    std::vector<typename lazy_pair_fst<fst1_type, fst2_type>::edge> const&
    lazy_pair_fst<fst1_type, fst2_type>::edges() const
    {
        if (edges_cache == nullptr) {
            std::vector<typename lazy_pair_fst<fst1_type, fst2_type>::vertex> edges;

            for (auto& v1: fst1_.edges()) {
                for (auto& v2: fst2_.edges()) {
                    edges.push_back(std::make_tuple(v1, v2));
                }
            }

            edges_cache = std::make_shared<std::vector<
                typename lazy_pair_fst<fst1_type, fst2_type>::edge>>(edges);
        }

        return *edges_cache;
    }

    template <class fst1_type, class fst2_type>
    typename lazy_pair_fst<fst1_type, fst2_type>::vertex
    lazy_pair_fst<fst1_type, fst2_type>::tail(
        typename lazy_pair_fst<fst1_type, fst2_type>::edge e) const
    {
        return std::make_tuple(fst1_.tail(std::get<0>(e)), fst2_.tail(std::get<1>(e)));
    }

    template <class fst1_type, class fst2_type>
    typename lazy_pair_fst<fst1_type, fst2_type>::vertex
    lazy_pair_fst<fst1_type, fst2_type>::head(
        typename lazy_pair_fst<fst1_type, fst2_type>::edge e) const
    {
        return std::make_tuple(fst1_.head(std::get<0>(e)), fst2_.head(std::get<1>(e)));
    }

    template <class fst1_type, class fst2_type>
    std::vector<typename lazy_pair_fst<fst1_type, fst2_type>::edge> const&
    lazy_pair_fst<fst1_type, fst2_type>::in_edges(
        typename lazy_pair_fst<fst1_type, fst2_type>::vertex v) const
    {
        if (in_edges_cache == nullptr || *in_edges_vertex != v) {
            std::vector<typename lazy_pair_fst<fst1_type, fst2_type>::edge> in_edges;

            auto edges1 = fst1_.in_edges(std::get<0>(v));
            auto edges2 = fst2_.in_edges(std::get<1>(v));

            for (auto& e1: edges1) {
                for (auto& e2: edges2) {
                    if (fst1_.output(e1) == fst2_.input(e2)) {
                        in_edges.push_back(std::make_tuple(e1, e2));
                    }
                }
            }

            in_edges_vertex = std::make_shared<
                typename lazy_pair_fst<fst1_type, fst2_type>::vertex>(v);

            in_edges_cache = std::make_shared<
                std::vector<typename lazy_pair_fst<fst1_type, fst2_type>::edge>>(in_edges);
        }

        return *in_edges_cache;
    }

    template <class fst1_type, class fst2_type>
    std::vector<typename lazy_pair_fst<fst1_type, fst2_type>::edge> const&
    lazy_pair_fst<fst1_type, fst2_type>::out_edges(
        typename lazy_pair_fst<fst1_type, fst2_type>::vertex v) const
    {
        if (out_edges_cache == nullptr || *out_edges_vertex != v) {
            std::vector<typename lazy_pair_fst<fst1_type, fst2_type>::edge> out_edges;

            auto edges1 = fst1_.out_edges(std::get<0>(v));
            auto edges2 = fst2_.out_edges(std::get<1>(v));

            for (auto& e1: edges1) {
                for (auto& e2: edges2) {
                    if (fst1_.output(e1) == fst2_.input(e2)) {
                        out_edges.push_back(std::make_tuple(e1, e2));
                    }
                }
            }

            out_edges_vertex = std::make_shared<
                typename lazy_pair_fst<fst1_type, fst2_type>::vertex>(v);

            out_edges_cache = std::make_shared<
                std::vector<typename lazy_pair_fst<fst1_type, fst2_type>::edge>>(out_edges);
        }

        return *out_edges_cache;
    }

    template <class fst1_type, class fst2_type>
    std::vector<typename lazy_pair_fst<fst1_type, fst2_type>::vertex> const&
    lazy_pair_fst<fst1_type, fst2_type>::initials() const
    {
        if (initials_cache == nullptr) {
            std::vector<typename lazy_pair_fst<fst1_type, fst2_type>::vertex> initials;

            for (auto& i1: fst1_.initials()) {
                for (auto& i2: fst2_.initials()) {
                    initials.push_back(std::make_tuple(i1, i2));
                }
            }

            initials_cache = std::make_shared<
                std::vector<typename lazy_pair_fst<fst1_type, fst2_type>::vertex>>(initials);
        }

        return *initials_cache;
    }

    template <class fst1_type, class fst2_type>
    std::vector<typename lazy_pair_fst<fst1_type, fst2_type>::vertex> const&
    lazy_pair_fst<fst1_type, fst2_type>::finals() const
    {
        if (finals_cache == nullptr) {
            std::vector<typename lazy_pair_fst<fst1_type, fst2_type>::edge> finals;

            for (auto& f1: fst1_.finals()) {
                for (auto& f2: fst2_.finals()) {
                    finals.push_back(std::make_tuple(f1, f2));
                }
            }

            finals_cache = std::make_shared<
                std::vector<typename lazy_pair_fst<fst1_type, fst2_type>::edge>>(finals);
        }

        return *finals_cache;
    }

    template <class fst1_type, class fst2_type>
    double lazy_pair_fst<fst1_type, fst2_type>::weight(
        typename lazy_pair_fst<fst1_type, fst2_type>::edge e) const
    {
        return fst1_.weight(std::get<0>(e)) + fst2_.weight(std::get<1>(e));
    }

    template <class fst1_type, class fst2_type>
    typename lazy_pair_fst<fst1_type, fst2_type>::input_symbol const&
    lazy_pair_fst<fst1_type, fst2_type>::input(
        typename lazy_pair_fst<fst1_type, fst2_type>::edge e) const
    {
        return fst1_.input(std::get<0>(e));
    }

    template <class fst1_type, class fst2_type>
    typename lazy_pair_fst<fst1_type, fst2_type>::output_symbol const&
    lazy_pair_fst<fst1_type, fst2_type>::output(
        typename lazy_pair_fst<fst1_type, fst2_type>::edge e) const
    {
        return fst2_.output(std::get<1>(e));
    }

    template <class fst1_type, class fst2_type>
    fst1_type& lazy_pair_fst<fst1_type, fst2_type>::fst1()
    {
        return fst1_;
    }

    template <class fst1_type, class fst2_type>
    fst1_type const& lazy_pair_fst<fst1_type, fst2_type>::fst1() const
    {
        return fst1_;
    }

    template <class fst1_type, class fst2_type>
    fst2_type& lazy_pair_fst<fst1_type, fst2_type>::fst2()
    {
        return fst2_;
    }

    template <class fst1_type, class fst2_type>
    fst2_type const& lazy_pair_fst<fst1_type, fst2_type>::fst2() const
    {
        return fst2_;
    }

    template <class fst1_type, class fst2_type>
    lazy_pair_mode1_fst<fst1_type, fst2_type>::lazy_pair_mode1_fst(fst1_type fst1, fst2_type fst2)
        : lazy_pair_fst<fst1_type, fst2_type>(fst1, fst2)
    {}

    template <class fst1_type, class fst2_type>
    std::vector<typename lazy_pair_mode1_fst<fst1_type, fst2_type>::edge> const&
    lazy_pair_mode1_fst<fst1_type, fst2_type>::in_edges(
        lazy_pair_mode1_fst<fst1_type, fst2_type>::vertex v) const
    {
        if (this->in_edges_cache == nullptr || *this->in_edges_vertex != v) {
            std::vector<typename lazy_pair_fst<fst1_type, fst2_type>::edge> in_edges;

            auto edges1_map = this->fst1_.in_edges_output_map(std::get<0>(v));
            auto edges2 = this->fst2_.in_edges(std::get<1>(v));

            for (auto& e2: edges2) {
                if (!ebt::in(this->fst2_.input(e2), edges1_map)) {
                    continue;
                }

                for (auto& e1: edges1_map.at(this->fst2_.input(e2))) {
                    in_edges.push_back(std::make_tuple(e1, e2));
                }
            }

            this->in_edges_vertex = std::make_shared<
                typename lazy_pair_fst<fst1_type, fst2_type>::vertex>(v);

            this->in_edges_cache = std::make_shared<
                std::vector<typename lazy_pair_fst<fst1_type, fst2_type>::edge>>(in_edges);
        }

        return *this->in_edges_cache;
    }

    template <class fst1_type, class fst2_type>
    std::vector<typename lazy_pair_mode1_fst<fst1_type, fst2_type>::edge> const&
    lazy_pair_mode1_fst<fst1_type, fst2_type>::out_edges(
        typename lazy_pair_mode1_fst<fst1_type, fst2_type>::vertex v) const
    {
        if (this->out_edges_cache == nullptr || *this->out_edges_vertex != v) {
            std::vector<typename lazy_pair_fst<fst1_type, fst2_type>::edge> out_edges;

            auto edges1_map = this->fst1_.out_edges_output_map(std::get<0>(v));
            auto edges2 = this->fst2_.out_edges(std::get<1>(v));

            for (auto& e2: edges2) {
                if (!ebt::in(this->fst2_.input(e2), edges1_map)) {
                    continue;
                }

                for (auto& e1: edges1_map.at(this->fst2_.input(e2))) {
                    out_edges.push_back(std::make_tuple(e1, e2));
                }
            }

            this->out_edges_vertex = std::make_shared<
                typename lazy_pair_fst<fst1_type, fst2_type>::vertex>(v);

            this->out_edges_cache = std::make_shared<
                std::vector<typename lazy_pair_fst<fst1_type, fst2_type>::edge>>(out_edges);
        }

        return *this->out_edges_cache;
    }

    template <class fst1_type, class fst2_type>
    lazy_pair_mode2_fst<fst1_type, fst2_type>::lazy_pair_mode2_fst(fst1_type fst1, fst2_type fst2)
        : lazy_pair_fst<fst1_type, fst2_type>(fst1, fst2)
    {}

    template <class fst1_type, class fst2_type>
    std::vector<typename lazy_pair_mode2_fst<fst1_type, fst2_type>::edge> const&
    lazy_pair_mode2_fst<fst1_type, fst2_type>::in_edges(
        lazy_pair_mode2_fst<fst1_type, fst2_type>::vertex v) const
    {
        if (this->in_edges_cache == nullptr || *this->in_edges_vertex != v) {
            std::vector<typename lazy_pair_fst<fst1_type, fst2_type>::edge> in_edges;

            auto edges1 = this->fst1_.in_edges(std::get<0>(v));
            auto edges2_map = this->fst2_.in_edges_input_map(std::get<1>(v));

            for (auto& e1: edges1) {
                if (!ebt::in(this->fst1_.output(e1), edges2_map)) {
                    continue;
                }

                for (auto& e2: edges2_map.at(this->fst1_.output(e1))) {
                    in_edges.push_back(std::make_tuple(e1, e2));
                }
            }

            this->in_edges_vertex = std::make_shared<
                typename lazy_pair_fst<fst1_type, fst2_type>::vertex>(v);

            this->in_edges_cache = std::make_shared<
                std::vector<typename lazy_pair_fst<fst1_type, fst2_type>::edge>>(in_edges);
        }

        return *this->in_edges_cache;
    }

    template <class fst1_type, class fst2_type>
    std::vector<typename lazy_pair_mode2_fst<fst1_type, fst2_type>::edge> const&
    lazy_pair_mode2_fst<fst1_type, fst2_type>::out_edges(
        typename lazy_pair_mode2_fst<fst1_type, fst2_type>::vertex v) const
    {
        if (this->out_edges_cache == nullptr || *this->out_edges_vertex != v) {
            std::vector<typename lazy_pair_fst<fst1_type, fst2_type>::edge> out_edges;

            auto edges1 = this->fst1_.out_edges(std::get<0>(v));
            auto edges2_map = this->fst2_.out_edges_input_map(std::get<1>(v));

            for (auto& e1: edges1) {
                if (!ebt::in(this->fst1_.output(e1), edges2_map)) {
                    continue;
                }

                for (auto& e2: edges2_map.at(this->fst1_.output(e1))) {
                    out_edges.push_back(std::make_tuple(e1, e2));
                }
            }

            this->out_edges_vertex = std::make_shared<
                typename lazy_pair_fst<fst1_type, fst2_type>::vertex>(v);

            this->out_edges_cache = std::make_shared<
                std::vector<typename lazy_pair_fst<fst1_type, fst2_type>::edge>>(out_edges);
        }

        return *this->out_edges_cache;
    }

}
