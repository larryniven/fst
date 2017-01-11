#include "fst/ifst.h"
#include "ebt/ebt.h"
#include <cassert>

namespace ifst {

    bool operator==(vertex_data const& v1, vertex_data const& v2)
    {
        return v1.time == v2.time;
    }

    bool operator==(edge_data const& e1, edge_data const& e2)
    {
        return e1.input == e2.input && e1.output == e2.output
            && e1.tail == e2.tail && e1.head == e2.head
            && e1.weight == e2.weight;
    }

    void add_vertex(fst_data& data, int v, vertex_data v_data)
    {
        if (!ebt::in(v, data.vertex_set)) {
            data.vertex_set.insert(v);
            data.vertex_indices.push_back(v);

            int size = std::max<int>(v + 1, data.vertices.size());

            data.vertices.resize(size);
            data.vertices[v] = v_data;
            data.in_edges.resize(size);
            data.out_edges.resize(size);
            data.in_edges_input_map.resize(size);
            data.in_edges_output_map.resize(size);
            data.out_edges_input_map.resize(size);
            data.out_edges_output_map.resize(size);
            data.vertex_attrs.resize(size);
        } else {
            assert(data.vertices[v] == v_data);
        }
    }

    void add_edge(fst_data& data, int e, edge_data e_data)
    {
        assert(ebt::in(e_data.head, data.vertex_set));
        assert(ebt::in(e_data.tail, data.vertex_set));

        if (!ebt::in(e, data.edge_set)) {
            data.edge_set.insert(e);
            data.edge_indices.push_back(e);

            int size = std::max<int>(e + 1, data.edges.size());

            data.edges.resize(size);
            data.edges[e] = e_data;
            data.in_edges[e_data.head].push_back(e);
            data.out_edges[e_data.tail].push_back(e);
            data.in_edges_input_map[e_data.head][e_data.input].push_back(e);
            data.in_edges_output_map[e_data.head][e_data.output].push_back(e);
            data.out_edges_input_map[e_data.tail][e_data.input].push_back(e);
            data.out_edges_output_map[e_data.tail][e_data.output].push_back(e);
            data.edge_attrs.resize(size);
            data.feats.resize(size);
        } else {
            assert(data.edges[e] == e_data);
        }
    }

    std::vector<int> const& fst::vertices() const
    {
        return data->vertex_indices;
    }

    std::vector<int> const& fst::edges() const
    {
        return data->edge_indices;
    }

    double fst::weight(int e) const
    {
        return data->edges.at(e).weight;
    }

    std::vector<int> const& fst::in_edges(int v) const
    {
        return data->in_edges.at(v);
    }

    std::vector<int> const& fst::out_edges(int v) const
    {
        return data->out_edges.at(v);
    }

    std::unordered_map<int, std::vector<int>> const& fst::in_edges_input_map(int v) const
    {
        return data->in_edges_input_map.at(v);
    }

    std::unordered_map<int, std::vector<int>> const& fst::in_edges_output_map(int v) const
    {
        return data->in_edges_output_map.at(v);
    }

    std::unordered_map<int, std::vector<int>> const& fst::out_edges_input_map(int v) const
    {
        return data->out_edges_input_map.at(v);
    }

    std::unordered_map<int, std::vector<int>> const& fst::out_edges_output_map(int v) const
    {
        return data->out_edges_output_map.at(v);
    }

    int fst::tail(int e) const
    {
        return data->edges.at(e).tail;
    }

    int fst::head(int e) const
    {
        return data->edges.at(e).head;
    }

    std::vector<int> const& fst::initials() const
    {
        return data->initials;
    }

    std::vector<int> const& fst::finals() const
    {
        return data->finals;
    }

    int const& fst::input(int e) const
    {
        return data->edges.at(e).input;
    }

    int const& fst::output(int e) const
    {
        return data->edges.at(e).output;
    }

    long fst::time(int v) const
    {
        return data->vertices.at(v).time;
    }

    fst add_eps_loops(fst f, int label)
    {
        fst_data& data = *(f.data);

        for (int i = 0; i < data.vertices.size(); ++i) {
            int e = int(data.edges.size());
            add_edge(data, e, edge_data {i, i, 0, label, label});
        }

        return f;
    }

}
