#ifndef IFST_H
#define IFST_H

#include <vector>
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <memory>

namespace ifst {

    struct vertex_data {
        long time;
    };

    struct edge_data {
        int tail;
        int head;
        double weight;
        int input;
        int output;
    };

    bool operator==(vertex_data const& v1, vertex_data const& v2);
    bool operator==(edge_data const& e1, edge_data const& e2);

    struct fst_data {
        std::string name;

        std::shared_ptr<std::unordered_map<std::string, int>> symbol_id;
        std::shared_ptr<std::vector<std::string>> id_symbol;

        std::vector<int> initials;
        std::vector<int> finals;
    
        std::vector<int> vertex_indices;
        std::vector<int> edge_indices;

        std::unordered_set<int> vertex_set;
        std::unordered_set<int> edge_set;

        std::vector<vertex_data> vertices;
        std::vector<edge_data> edges;

        std::vector<std::vector<int>> in_edges;
        std::vector<std::vector<int>> out_edges;

        std::vector<std::unordered_map<int, std::vector<int>>> in_edges_input_map;
        std::vector<std::unordered_map<int, std::vector<int>>> in_edges_output_map;
        std::vector<std::unordered_map<int, std::vector<int>>> out_edges_input_map;
        std::vector<std::unordered_map<int, std::vector<int>>> out_edges_output_map;

        std::vector<std::vector<std::pair<std::string, std::string>>> vertex_attrs;
        std::vector<std::vector<std::pair<std::string, std::string>>> edge_attrs;

        std::vector<std::vector<double>> feats;

    };

    void add_vertex(fst_data& data, int v, vertex_data v_data);
    void add_edge(fst_data& data, int e, edge_data e_data);

    /*
     * The class `fst_data` is separated instead of inlined in `fst`,
     * because we want to separate data (`fst_data`) that can be manipulated
     * and interface (`fst`) for accessing the data.
     *
     * We make `data` a `shared_ptr` to avoid copying everything when `fst`
     * is copied.
     *
     */
    struct fst {

        using vertex = int;
        using edge = int;
        using input_symbol = int;
        using output_symbol = int;

        std::shared_ptr<fst_data> data;

        std::vector<int> const& vertices() const;
        std::vector<int> const& edges() const;
        double weight(int e) const;
        std::vector<int> const& in_edges(int v) const;
        std::vector<int> const& out_edges(int v) const;
        int tail(int e) const;
        int head(int e) const;
        std::vector<int> const& initials() const;
        std::vector<int> const& finals() const;
        int const& input(int e) const;
        int const& output(int e) const;

        long time(int v) const;

        std::unordered_map<int, std::vector<int>> const& in_edges_input_map(int v) const;
        std::unordered_map<int, std::vector<int>> const& in_edges_output_map(int v) const;
        std::unordered_map<int, std::vector<int>> const& out_edges_input_map(int v) const;
        std::unordered_map<int, std::vector<int>> const& out_edges_output_map(int v) const;

    };

    fst add_eps_loops(fst f, int label=0);

}

#endif
