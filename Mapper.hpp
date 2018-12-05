/** Copyright &copy; 2017, rostislav.vel@gmail.com.
 * \brief  Класс реализующий постоение таблици с оптимальными маршрутами.
 * \author Величко Ростислав
 * \date   21.12.2017
 */

#pragma once

#include <string>
#include <unordered_map>

#include <boost/graph/adjacency_list.hpp>
#include <boost/graph/directed_graph.hpp> 
#include <boost/graph/dijkstra_shortest_paths.hpp>
#include <boost/property_map/property_map.hpp>

namespace mapper {

typedef boost::vertex_name_t GVertexNameT;
typedef boost::edge_weight_t GEdgeWeightT;
typedef boost::property<GVertexNameT, std::string> GVertexProperty;
typedef boost::property<GEdgeWeightT, double> GEdgeWeightProperty;
typedef boost::adjacency_list<boost::setS, boost::vecS, boost::undirectedS, GVertexProperty, GEdgeWeightProperty> UndirectedGraph;
typedef boost::graph_traits<UndirectedGraph>::vertex_descriptor GVertex;
typedef boost::graph_traits<UndirectedGraph>::vertex_iterator GVertexIter;
typedef boost::graph_traits<UndirectedGraph>::edge_descriptor GEdge;
typedef boost::graph_traits<UndirectedGraph>::edge_iterator GEdgeIter;
typedef boost::graph_traits<UndirectedGraph>::adjacency_iterator GAdjacencyIter;
typedef boost::property_map<UndirectedGraph, GVertexNameT>::type GVertexNameType;
typedef boost::property_map<UndirectedGraph, GEdgeWeightT>::type GEdgeWeightType;
typedef std::unordered_map<std::string, GVertex> GNameVertexMap;
typedef GNameVertexMap::iterator GNvMapIter; 


class Mapper { 
protected:
    GNameVertexMap _actors; ///< Хранилище ассоциаций адресов с векторами графа.
    UndirectedGraph _graph; ///< Не ориентированный граф загруженной сетевой структуры.
   
public:
    Mapper();
    virtual ~Mapper();

    /**
     * \brief Метод выполняет загрузку сетевых узлов в граф.
     * \param addr_from_ Исходящий сетевой узел.
     * \param addr_to_ Входящий сетевой узел.
     * \param weight_ Вес данной связи.
     */
    void insert(const std::string &addr_from_, const std::string &addr_to_, double weight_);

    /**
     * \brief Метод выполняет поиск следующего оптимального узла м/у двумя переданными.
     * \param addr_from_ Исходящий сетевой узел.
     * \param addr_to_ Конечный сетевой узел.
     * \return Адрес искомого узла.
     */
    std::string getNextNode(const std::string &addr_from_, const std::string &addr_to_);
};
} /// mapper
