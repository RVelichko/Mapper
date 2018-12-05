#include <iostream>
#include <exception>
#include <functional>
#include <limits>
#include <map>
#include <vector>

#include "Log.hpp"
#include "Mapper.hpp"


using namespace mapper;

typedef std::vector<GVertex> GVertexRate;
typedef std::pair<double, GVertexRate> GRate;
typedef std::map<double, GVertexRate> GRates;
typedef GRates::iterator GRateIter;
typedef std::vector<double> GEdgeWeights;
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


Mapper::Mapper() { 
    LOG(DEBUG);
}


Mapper::~Mapper() { 
    LOG(DEBUG);
}

    
void Mapper::insert(const std::string &addr_from_, const std::string &addr_to_, double weight_) { 
    /// Добавление исходящего сервера.
    bool inserted;
    GVertexNameType actor_name = get(boost::vertex_name, _graph);
    GNvMapIter pos;
    GVertex from;
    boost::tie(pos, inserted) = _actors.insert(std::make_pair(addr_from_, GVertex()));
    if (inserted) {
        from = add_vertex(_graph);
        actor_name[from] = addr_from_;
        pos->second = from;
    } else {
        from = pos->second;
    }
    /// Добавление входящего сервера.
    GVertex to;
    boost::tie(pos, inserted) = _actors.insert(std::make_pair(addr_to_, GVertex()));
    if (inserted) {
        to = add_vertex(_graph);
        actor_name[to] = addr_to_;
        pos->second = to;
    } else {
        to = pos->second;
    }
    /// Добавление связи.
    if (addr_from_ not_eq addr_to_) {
        GEdge edge;
        boost::tie(edge, inserted) = add_edge(from, to, GEdgeWeightProperty(weight_), _graph);
        LOG(DEBUG) << "[" << addr_from_ << " <-> " << addr_to_ << "]: " << weight_;
    } else {
        LOG(WARNING) << "Invalid edge: " << addr_from_ << " <->" << addr_to_;
    }
}


std::string Mapper::getNextNode(const std::string &addr_from_, const std::string &addr_to_) { 
    std::string res = addr_to_;
    GNvMapIter from_iter = _actors.find(addr_from_);
    GNvMapIter to_titer = _actors.find(addr_to_);
    if (from_iter not_eq _actors.end() and to_titer not_eq _actors.end()) {
        GVertex from = from_iter->second;
        /// Вызов функции поиска кратчайших путей.
        GVertexRate p(num_vertices(_graph));
        GEdgeWeights d(num_vertices(_graph));
        boost::dijkstra_shortest_paths(_graph, from, boost::predecessor_map(&p[0]).distance_map(&d[0]));
        /// Построение мршрутов.
        GVertexIter vi, vend;
        GRates rates;
        for (boost::tie(vi, vend) = vertices(_graph); vi not_eq vend; ++vi) {
            double weight = d[*vi];
            if (weight) {
                GVertexRate vrate;
                GVertex vert = p[*vi];
                vrate.push_back(*vi);
                while (vert not_eq from) {
                    vrate.push_back(vert);
                    vert = p[vert];
                } 
                /// Т.к. алгоритм возвращает список от конечного узла в цепочке - массив необходимо перевернуть.
                std::reverse(std::begin(vrate), std::end(vrate));
                rates.insert(std::make_pair(weight, vrate));
            }
        }
        /// Поиск пути м/у переданными начальным и конечным узлами.
        GVertexNameType actor_name = get(boost::vertex_name_t(), _graph);
        bool is_complette = false;
        for (GRateIter riter = rates.begin(); riter not_eq rates.end() and not is_complette; ++riter ) {
            std::stringstream ss;
            /// Вернуть следующий за исходным узел в цепочке.
            res = actor_name[*(riter->second.begin())];
            for (GVertex &vert : riter->second) {
                if (actor_name[vert] == addr_to_) {
                    ss << "->(" << vert << ")";
                    /// Принудительно завершить цикл верхнего уровня.
                    is_complette = true;
                    break;
                } else {
                    ss << "->" << vert;
                }
            }
            LOG(TEST) << _actors[addr_from_] << ss.str();
        }
    } else {
        LOG(ERROR) << "Can`t find node: \"" << addr_from_ << "\" or \"" << addr_to_ << "\"";
    }
    return res;
}
