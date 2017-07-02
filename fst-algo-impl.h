namespace fst {

    template <class fst>
    std::vector<typename fst::vertex> topo_order(fst const& f)
    {
        enum class action_t {
            color_grey,
            color_black
        };

        std::vector<std::pair<action_t, typename fst::vertex>> stack;
        std::unordered_set<typename fst::vertex> traversed;

        std::vector<typename fst::vertex> order;

        for (auto& v: f.initials()) {
            stack.push_back(std::make_pair(action_t::color_grey, v));
        }

        while (stack.size() > 0) {
            action_t a;
            typename fst::vertex v;
            std::tie(a, v) = stack.back();
            stack.pop_back();

            if (a == action_t::color_grey) {
                if (ebt::in(v, traversed)) {
                    continue;
                }

                traversed.insert(v);

                stack.push_back(std::make_pair(action_t::color_black, v));

                for (auto& e: f.out_edges(v)) {
                    auto u = f.head(e);

                    if (!ebt::in(u, traversed)) {
                        stack.push_back(std::make_pair(action_t::color_grey, u));
                    }
                }

            } else if (a == action_t::color_black) {
                order.push_back(v);
            } else {
                std::cerr << "unknown action " << int(a) << std::endl;
                exit(1);
            }
        }

        std::reverse(order.begin(), order.end());

        return order;
    }

    template <class fst>
    void forward_one_best<fst>::merge(fst const& f, std::vector<typename fst::vertex> const& order)
    {
        double inf = std::numeric_limits<double>::infinity();

        auto get_value = [&](vertex v) {
            if (!ebt::in(v, extra)) {
                return -inf;
            } else {
                return extra.at(v).value;
            }
        };

        for (auto& u: order) {
            double max = get_value(u);
            typename fst::edge argmax;
            bool update = false;

            std::vector<edge> edges = f.in_edges(u);
            std::vector<double> candidate_value;
            candidate_value.resize(edges.size());

            for (int i = 0; i < edges.size(); ++i) {
                typename fst::edge& e = edges[i];
                typename fst::vertex v = f.tail(e);
                candidate_value[i] = get_value(v) + f.weight(e);
            }

            for (int i = 0; i < edges.size(); ++i) {
                if (candidate_value[i] > max) {
                    max = candidate_value[i];
                    argmax = edges[i];
                    update = true;
                }
            }

            if (update) {
                extra[u] = extra_data { argmax, max };
            }
        }
    }

    template <class fst>
    std::vector<typename fst::edge> forward_one_best<fst>::best_path(fst const& f)
    {
        double inf = std::numeric_limits<double>::infinity();
        double max = -inf;
        typename fst::vertex argmax;

        for (auto v: f.finals()) {
            if (ebt::in(v, extra) && extra.at(v).value > max) {
                max = extra.at(v).value;
                argmax = v;
            }
        }

        std::vector<typename fst::edge> result;

        if (max == -inf) {
            return result;
        }

        vertex u = argmax;

        std::vector<typename fst::vertex> const& initials = f.initials();
        std::unordered_set<typename fst::vertex> initial_set { initials.begin(), initials.end() };

        while (!ebt::in(u, initial_set)) {
            edge e = extra.at(u).pi;
            vertex v = f.tail(e);
            result.push_back(e);
            u = v;
        }

        std::reverse(result.begin(), result.end());

        return result;
    }

    template <class fst>
    void backward_one_best<fst>::merge(fst const& f, std::vector<typename fst::vertex> const& order)
    {
        double inf = std::numeric_limits<double>::infinity();

        auto get_value = [&](vertex v) {
            if (!ebt::in(v, extra)) {
                return -inf;
            } else {
                return extra.at(v).value;
            }
        };

        auto rev_order = order;
        std::reverse(rev_order.begin(), rev_order.end());

        for (auto& u: rev_order) {
            double max = get_value(u);
            typename fst::edge argmax;
            bool update = false;

            std::vector<edge> edges = f.out_edges(u);
            std::vector<double> candidate_value;
            candidate_value.resize(edges.size());

            for (int i = 0; i < edges.size(); ++i) {
                typename fst::edge& e = edges[i];
                typename fst::vertex v = f.head(e);
                candidate_value[i] = get_value(v) + f.weight(e);
            }

            for (int i = 0; i < edges.size(); ++i) {
                if (candidate_value[i] > max) {
                    max = candidate_value[i];
                    argmax = edges[i];
                    update = true;
                }
            }

            if (update) {
                extra[u] = extra_data { argmax, max };
            }
        }
    }

    template <class fst>
    std::vector<typename fst::edge> backward_one_best<fst>::best_path(fst const& f)
    {
        double inf = std::numeric_limits<double>::infinity();
        double max = -inf;
        typename fst::vertex argmax;

        for (auto v: f.initials()) {
            if (ebt::in(v, extra) && extra.at(v).value > max) {
                max = extra.at(v).value;
                argmax = v;
            }
        }

        std::vector<typename fst::edge> result;

        if (max == -inf) {
            return result;
        }

        vertex u = argmax;

        std::vector<typename fst::vertex> const& finals = f.finals();
        std::unordered_set<typename fst::vertex> final_set { finals.begin(), finals.end() };

        while (!ebt::in(u, final_set)) {
            edge e = extra.at(u).pi;
            vertex v = f.head(e);
            result.push_back(e);
            u = v;
        }

        return result;
    }

    template <class fst>
    void forward_k_best<fst>::first_best(fst const& f, std::vector<typename fst::vertex> const& order)
    {
        // After running 1-best, each edge has exactly one card,
        // and top is pointing at that card.
        // 
        // When extra[v].deck.size() == 0, v is a deadend.

        for (auto& v: f.initials()) {
            vertex_extra[v].deck.push_back(std::make_tuple(edge_trait<typename fst::edge>::null, -1, 0));
            vertex_extra[v].bottom_out = true;
        }

        for (auto& v: order) {
            auto& in_edges = f.in_edges(v);

            typename fst::edge argmax = edge_trait<typename fst::edge>::null;
            double max = -std::numeric_limits<double>::infinity();

            for (auto& e: in_edges) {
                if (vertex_extra[f.tail(e)].deck.size() == 0) {
                    continue;
                }

                double value = f.weight(e) + std::get<2>(vertex_extra[f.tail(e)].deck[0]);

                if (value > max) {
                    max = value;
                    argmax = e;
                }
            }

            if (argmax == edge_trait<typename fst::edge>::null) {
                vertex_extra[v].bottom_out = true;
            } else {
                vertex_extra[v].deck.push_back(std::make_tuple(argmax, 0, max));
                edge_extra[argmax].top = 0;
            }
        }
    }

    template <class fst>
    void forward_k_best<fst>::next_best(fst const& f, typename fst::vertex const& final, int k)
    {
        std::vector<typename fst::vertex> stack;

        typename fst::vertex v = final;
        int m = k;

        if (k > vertex_extra[v].deck.size()) {
            std::cout << "unable to find " << k << "-th path" << std::endl;
            exit(1);
        } else if (k < vertex_extra[v].deck.size()) {
            return;
        }

        while (1) {
            typename fst::edge e = std::get<0>(vertex_extra[v].deck[m - 1]);

            if (e == edge_trait<typename fst::edge>::null) {
                break;
            }

            int i = std::get<1>(vertex_extra[v].deck[m - 1]);
            typename fst::vertex tail = f.tail(e);

            if (vertex_extra[tail].deck.size() == 0) {
                break;
            } else if (i == vertex_extra[tail].deck.size() - 1 && vertex_extra[tail].bottom_out) {
                stack.push_back(v);
                break;
            } else if (i <= vertex_extra[tail].deck.size() - 1) {
                stack.push_back(v);
                v = tail;
                m = i + 1;
            }
        }

        auto get_top = [&](typename fst::edge const& e) {
            if (ebt::in(e, edge_extra)) {
                return edge_extra.at(e).top;
            } else {
                return -1;
            }
        };

        while (stack.size() > 0) {
            v = stack.back();
            stack.pop_back();

            typename fst::edge argmax = edge_trait<typename fst::edge>::null;
            double max = -std::numeric_limits<double>::infinity();

            for (auto& e: f.in_edges(v)) {

                if (vertex_extra[f.tail(e)].deck.size() == 0) {
                    continue;
                }

                if (get_top(e) + 1 >= vertex_extra[f.tail(e)].deck.size()) {
                    continue;
                }

                double value = f.weight(e) + std::get<2>(vertex_extra[f.tail(e)].deck[get_top(e) + 1]);

                if (value > max) {
                    max = value;
                    argmax = e;
                }

            }

            if (argmax == edge_trait<typename fst::edge>::null) {
                vertex_extra[v].bottom_out = true;
            } else {
                int top = get_top(argmax) + 1;
                edge_extra[argmax].top = top;
                vertex_extra[v].deck.push_back(std::make_tuple(argmax, top, max));
            }
        }
    }

    template <class fst>
    std::vector<typename fst::edge> forward_k_best<fst>::best_path(
        fst const& f, typename fst::vertex const& final, int k)
    {
        std::vector<typename fst::edge> result;

        vertex u = final;
        int i = k;

        std::vector<typename fst::vertex> const& initials = f.initials();
        std::unordered_set<typename fst::vertex> initial_set { initials.begin(), initials.end() };

        while (!ebt::in(u, initial_set)) {
            edge e = std::get<0>(vertex_extra.at(u).deck[i]);
            i = std::get<1>(vertex_extra.at(u).deck[i]);
            result.push_back(e);
            u = f.tail(e);
        }

        std::reverse(result.begin(), result.end());

        return result;
    }

    template <class fst>
    void forward_log_sum<fst>::merge(fst const& f, std::vector<typename fst::vertex> const& order)
    {
        for (auto& v: f.initials()) {
            extra[v] = 0;
        }

        double inf = std::numeric_limits<double>::infinity();

        auto get_value = [&](vertex v) {
            if (!ebt::in(v, extra)) {
                return -inf;
            } else {
                return extra.at(v);
            }
        };

        for (auto& u: order) {
#if OMP_SAFE
            double u_value = get_value(u);

            if (u_value == -inf) {
                continue;
            }

            std::unordered_map<input_symbol, std::vector<edge>> const& map = f.out_edges_input_map(u);

            for (auto& p: map) {

                for (int i = 0; i < p.second.size(); ++i) {
                    auto e = p.second[i];
                    vertex v = f.head(e);
                    double s = get_value(v);
                    extra[v] = s;
                }

                #pragma omp parallel for
                for (int i = 0; i < p.second.size(); ++i) {
                    auto e = p.second[i];
                    vertex v = f.head(e);
                    double s = ebt::log_add(get_value(v), u_value + f.weight(e));
                    extra.at(v) = s;
                }
            }
#else
            double s = get_value(u);

            std::vector<edge> edges = f.in_edges(u);
            std::vector<double> candidate_value;
            candidate_value.resize(edges.size());

            for (int i = 0; i < edges.size(); ++i) {
                edge& e = edges[i];
                vertex v = f.tail(e);
                candidate_value[i] = get_value(v) + f.weight(e);
            }

            for (int i = 0; i < edges.size(); ++i) {
                if (candidate_value[i] != -inf) {
                    s = ebt::log_add(s, candidate_value[i]);
                }
            }

            extra[u] = s;

#endif
        }
    }

    template <class fst>
    void backward_log_sum<fst>::merge(fst const& f, std::vector<typename fst::vertex> const& order)
    {
        for (auto& v: f.finals()) {
            extra[v] = 0;
        }

        double inf = std::numeric_limits<double>::infinity();

        auto get_value = [&](vertex v) {
            if (!ebt::in(v, extra)) {
                return -inf;
            } else {
                return extra.at(v);
            }
        };

        for (auto& u: order) {
#if OMP_SAFE
            double u_value = get_value(u);

            if (u_value == -inf) {
                continue;
            }

            std::unordered_map<input_symbol, std::vector<edge>> const& map = f.in_edges_input_map(u);

            for (auto& p: map) {

                for (int i = 0; i < p.second.size(); ++i) {
                    auto e = p.second[i];
                    vertex v = f.tail(e);
                    double s = get_value(v);
                    extra[v] = s;
                }

                #pragma omp parallel for
                for (int i = 0; i < p.second.size(); ++i) {
                    auto e = p.second[i];
                    vertex v = f.tail(e);
                    double s = ebt::log_add(get_value(v), u_value + f.weight(e));
                    extra.at(v) = s;
                }
            }
#else
            double s = get_value(u);

            std::vector<edge> edges = f.out_edges(u);
            std::vector<double> candidate_value;
            candidate_value.resize(edges.size());

            for (int i = 0; i < edges.size(); ++i) {
                typename fst::edge& e = edges[i];
                typename fst::vertex v = f.head(e);
                candidate_value[i] = get_value(v) + f.weight(e);
            }

            for (int i = 0; i < edges.size(); ++i) {
                if (candidate_value[i] != -inf) {
                    s = ebt::log_add(s, candidate_value[i]);
                }
            }

            extra[u] = s;
#endif
        }
    }

    template <class fst_type>
    std::vector<typename fst_type::edge> shortest_path(fst_type const& f,
        std::vector<typename fst_type::vertex> const& topo_order)
    {
        forward_one_best<fst_type> one_best;

        for (auto& v: f.initials()) {
            one_best.extra[v] = { edge_trait<typename fst_type::edge>::null, 0 };
        }
        one_best.merge(f, topo_order);

        return one_best.best_path(f);
    }

    template <class fst_type>
    void beam_search<fst_type>::merge(fst_type const& f,
        std::vector<typename fst_type::vertex> const& order, double alpha)
    {
        for (auto& v: f.initials()) {
            retained_vertices.insert(v);
        }

        for (auto& v: order) {
            if (ebt::in(v, retained_vertices)) {
                std::vector<std::tuple<typename fst_type::edge, double>> out;

                double inf = std::numeric_limits<double>::infinity();

                double min = inf;
                typename fst_type::edge argmin = edge_trait<typename fst_type::edge>::null;

                double max = -inf;
                typename fst_type::edge argmax = edge_trait<typename fst_type::edge>::null;

                for (auto& e: f.out_edges(v)) {
                    double w = f.weight(e);

                    out.push_back(std::make_tuple(e, w));

                    if (w > max) {
                        max = w;
                        argmax = e;
                    }

                    if (w < min) {
                        min = w;
                        argmin = e;
                    }
                }

                double cutoff = min + (max - min) * alpha;

                for (auto& t: out) {
                    typename fst_type::edge e;
                    double weight;

                    std::tie(e, weight) = t;

                    if (weight > cutoff) {
                        retained_edges.push_back(e);
                        retained_vertices.insert(f.head(e));
                    }
                }
            }
        }
    }

}

