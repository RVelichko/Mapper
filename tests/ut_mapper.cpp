#ifndef BOOST_STATIC_LINK
#   define BOOST_TEST_DYN_LINK
#endif // BOOST_STATIC_LINK

#define BOOST_TEST_MODULE CppExamples
#define BOOST_AUTO_TEST_MAIN

#include <unordered_set>
#include <unordered_map>
#include <utility>
#include <vector>
#include <iostream>

#include <boost/test/unit_test.hpp>
#include <boost/test/output_test_stream.hpp>

#include "Log.hpp"
#include "LinksGenerator.hpp"
#include "Mapper.hpp"

namespace test {

typedef mapper::Mapper Mapper;
typedef std::shared_ptr<Mapper> PMapper;
typedef mapper::UndirectedGraph UndirectedGraph;
typedef mapper::GVertexIter GVertexIter;
typedef std::pair<GVertexIter, GVertexIter> GvPair;
typedef mapper::GEdgeIter GEdgeIter;
typedef std::pair<GEdgeIter, GEdgeIter> GePair;
typedef mapper::GEdgeWeightProperty GEdgeWeightProperty;
typedef mapper::GEdgeWeightType GEdgeWeightType;
typedef mapper::GVertexProperty GVertexProperty;
typedef mapper::GAdjacencyIter GAdjacencyIter;
typedef std::pair<GAdjacencyIter, GAdjacencyIter> AiPair;
typedef mapper::GVertex GVertex;
typedef mapper::GEdge GEdge;
typedef mapper::GVertexNameType GVertexNameType;
typedef mapper::GNameVertexMap GNameVertexMap; 

const GraphLink __test_gl[] = {
    GraphLink{ "65.133.217.202:52164", "237.67.28.35:61332",   26.6 },
    GraphLink{ "65.133.217.202:52164", "170.194.76.149:53329", 56 },
    GraphLink{ "65.133.217.202:52164", "186.63.121.235:65184", 17 },
    GraphLink{ "65.133.217.202:52164", "229.117.57.122:52282", 27.1 },
    GraphLink{ "186.63.121.235:65184", "170.194.76.149:53329", 0.7 },
    GraphLink{ "186.63.121.235:65184", "237.67.28.35:61332",   98 },
    GraphLink{ "186.63.121.235:65184", "229.117.57.122:52282", 78 },
    GraphLink{ "170.194.76.149:53329", "237.67.28.35:61332",   63 },
    GraphLink{ "170.194.76.149:53329", "229.117.57.122:52282", 11 }
};


const std::string __test_results[] = {
    "186.63.121.235:65184",
    "65.133.217.202:52164",
    "186.63.121.235:65184",
    "170.194.76.149:53329",
    "170.194.76.149:53329"
}; 


class TestMapper : public Mapper {
public:
    UndirectedGraph& getGraph() {
        return _graph;
    }

    GNameVertexMap& getActors() {
        return _actors;
    }
};
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


BOOST_AUTO_TEST_CASE(TestMapperLow) {
    LOG_TO_STDOUT;
    /// Загрузка .
    PMapper tmapper = std::make_shared<TestMapper>();
    for (GraphLink gl : __test_gl) {
        tmapper->insert(gl._addr_from, gl._addr_to, gl._weigth);
    }
    /// Проверка полученного графа.
    UndirectedGraph g = dynamic_cast<TestMapper*>(tmapper.get())->getGraph();
    GVertexNameType actor_name = get(boost::vertex_name_t(), g);
    /// Проверка вершин.
    size_t count = 0;
    std::vector<GVertex> verts;
    GVertexIter vi, viend;
    for (boost::tie(vi, viend) = vertices(g); vi not_eq viend; ++vi) {
        LOG(TEST) << "[" << count++ << "]: " << actor_name[*vi];
        verts.push_back(*vi);
    }
    /// Проверка весов.
    count = 0;
    GEdgeWeightType ewm = get(boost::edge_weight_t(), g);
    for(GePair gep = edges(g); gep.first not_eq gep.second; ++gep.first) {
        double weight = ewm[*gep.first];
        BOOST_CHECK(__test_gl[count]._weigth == weight);
        LOG(TEST) << "[" << count++ << "]: [" 
                  << source(*gep.first, g) << "] <-> [" << target(*gep.first, g) << "]: " 
                  << weight;
    }
    /// Проверка связей.
    for (GVertex &from : verts) {
        GAdjacencyIter ai, aiend; 
        boost::tie(ai, aiend) = boost::adjacent_vertices(from, g);
        for ( ; ai not_eq aiend; ++ai) { 
            LOG(TEST) << "[" << from << "] " << actor_name[from] << " -> [" << (*ai) << "] " << actor_name[*ai];
        }
    }
    /// Проверка поиска следующего узла с минимальным весом пути.
    GNameVertexMap a = dynamic_cast<TestMapper*>(tmapper.get())->getActors();
    count = 0; 
    for (GVertex &from : verts) {
        for (GVertex &to : verts) {
            if (from not_eq to) {
                std::string next = tmapper->getNextNode(actor_name[from], actor_name[to]);
                LOG(TEST) << "for {[" << from << "] " << actor_name[from] << " -> [" 
                          << to << "] " << actor_name[to] << "} next ["
                          << a[next] << "] " << next;
                //BOOST_CHECK(__test_results[count++] == to);
            }
        }
    }
}
} // test


/// Пример вывода аналитической части алгоритма.
/// 43. [25.12.2017-18:29:01.416625] [TEST] [std::__cxx11::string mapper::Mapper::getNextNode(const string&, const string&)] 0->3
/// 44. [25.12.2017-18:29:01.416635] [TEST] [std::__cxx11::string mapper::Mapper::getNextNode(const string&, const string&)] 0->3->2
/// 45. [25.12.2017-18:29:01.416642] [TEST] [std::__cxx11::string mapper::Mapper::getNextNode(const string&, const string&)] 0->(1)
/// 46. [25.12.2017-18:29:01.416652] [TEST] [void test::TestMapperLow::test_method()] for {[0] 65.133.217.202:52164 -> [1] 237.67.28.35:61332} next [1] 237.67.28.35:61332
/// 47. [25.12.2017-18:29:01.416666] [TEST] [std::__cxx11::string mapper::Mapper::getNextNode(const string&, const string&)] 0->3
/// 48. [25.12.2017-18:29:01.416673] [TEST] [std::__cxx11::string mapper::Mapper::getNextNode(const string&, const string&)] 0->3->(2)
/// 49. [25.12.2017-18:29:01.416679] [TEST] [void test::TestMapperLow::test_method()] for {[0] 65.133.217.202:52164 -> [2] 170.194.76.149:53329} next [3] 186.63.121.235:65184
/// 50. [25.12.2017-18:29:01.416692] [TEST] [std::__cxx11::string mapper::Mapper::getNextNode(const string&, const string&)] 0->(3)
/// 51. [25.12.2017-18:29:01.416697] [TEST] [void test::TestMapperLow::test_method()] for {[0] 65.133.217.202:52164 -> [3] 186.63.121.235:65184} next [3] 186.63.121.235:65184
/// 52. [25.12.2017-18:29:01.416712] [TEST] [std::__cxx11::string mapper::Mapper::getNextNode(const string&, const string&)] 0->3
/// 53. [25.12.2017-18:29:01.416720] [TEST] [std::__cxx11::string mapper::Mapper::getNextNode(const string&, const string&)] 0->3->2
/// 54. [25.12.2017-18:29:01.416726] [TEST] [std::__cxx11::string mapper::Mapper::getNextNode(const string&, const string&)] 0->1
/// 55. [25.12.2017-18:29:01.416731] [TEST] [std::__cxx11::string mapper::Mapper::getNextNode(const string&, const string&)] 0->(4)
/// 56. [25.12.2017-18:29:01.416737] [TEST] [void test::TestMapperLow::test_method()] for {[0] 65.133.217.202:52164 -> [4] 229.117.57.122:52282} next [4] 229.117.57.122:52282
/// 57. [25.12.2017-18:29:01.416751] [TEST] [std::__cxx11::string mapper::Mapper::getNextNode(const string&, const string&)] 1->(0)
/// 58. [25.12.2017-18:29:01.416757] [TEST] [void test::TestMapperLow::test_method()] for {[1] 237.67.28.35:61332 -> [0] 65.133.217.202:52164} next [0] 65.133.217.202:52164
/// 59. [25.12.2017-18:29:01.416772] [TEST] [std::__cxx11::string mapper::Mapper::getNextNode(const string&, const string&)] 1->0
/// 60. [25.12.2017-18:29:01.416778] [TEST] [std::__cxx11::string mapper::Mapper::getNextNode(const string&, const string&)] 1->0->3
/// 61. [25.12.2017-18:29:01.416785] [TEST] [std::__cxx11::string mapper::Mapper::getNextNode(const string&, const string&)] 1->0->3->(2)
/// 62. [25.12.2017-18:29:01.416790] [TEST] [void test::TestMapperLow::test_method()] for {[1] 237.67.28.35:61332 -> [2] 170.194.76.149:53329} next [0] 65.133.217.202:52164
/// 63. [25.12.2017-18:29:01.416802] [TEST] [std::__cxx11::string mapper::Mapper::getNextNode(const string&, const string&)] 1->0
/// 64. [25.12.2017-18:29:01.416808] [TEST] [std::__cxx11::string mapper::Mapper::getNextNode(const string&, const string&)] 1->0->(3)
/// 65. [25.12.2017-18:29:01.416813] [TEST] [void test::TestMapperLow::test_method()] for {[1] 237.67.28.35:61332 -> [3] 186.63.121.235:65184} next [0] 65.133.217.202:52164
/// 66. [25.12.2017-18:29:01.416827] [TEST] [std::__cxx11::string mapper::Mapper::getNextNode(const string&, const string&)] 1->0
/// 67. [25.12.2017-18:29:01.416833] [TEST] [std::__cxx11::string mapper::Mapper::getNextNode(const string&, const string&)] 1->0->3
/// 68. [25.12.2017-18:29:01.416839] [TEST] [std::__cxx11::string mapper::Mapper::getNextNode(const string&, const string&)] 1->0->3->2
/// 69. [25.12.2017-18:29:01.416855] [TEST] [std::__cxx11::string mapper::Mapper::getNextNode(const string&, const string&)] 1->0->(4)
/// 70. [25.12.2017-18:29:01.416862] [TEST] [void test::TestMapperLow::test_method()] for {[1] 237.67.28.35:61332 -> [4] 229.117.57.122:52282} next [0] 65.133.217.202:52164
/// 71. [25.12.2017-18:29:01.416876] [TEST] [std::__cxx11::string mapper::Mapper::getNextNode(const string&, const string&)] 2->3
/// 72. [25.12.2017-18:29:01.416883] [TEST] [std::__cxx11::string mapper::Mapper::getNextNode(const string&, const string&)] 2->4
/// 73. [25.12.2017-18:29:01.416889] [TEST] [std::__cxx11::string mapper::Mapper::getNextNode(const string&, const string&)] 2->3->(0)
/// 74. [25.12.2017-18:29:01.416895] [TEST] [void test::TestMapperLow::test_method()] for {[2] 170.194.76.149:53329 -> [0] 65.133.217.202:52164} next [3] 186.63.121.235:65184
/// 75. [25.12.2017-18:29:01.416907] [TEST] [std::__cxx11::string mapper::Mapper::getNextNode(const string&, const string&)] 2->3
/// 76. [25.12.2017-18:29:01.416913] [TEST] [std::__cxx11::string mapper::Mapper::getNextNode(const string&, const string&)] 2->4
/// 77. [25.12.2017-18:29:01.416919] [TEST] [std::__cxx11::string mapper::Mapper::getNextNode(const string&, const string&)] 2->3->0
/// 78. [25.12.2017-18:29:01.416925] [TEST] [std::__cxx11::string mapper::Mapper::getNextNode(const string&, const string&)] 2->3->0->(1)
/// 79. [25.12.2017-18:29:01.416931] [TEST] [void test::TestMapperLow::test_method()] for {[2] 170.194.76.149:53329 -> [1] 237.67.28.35:61332} next [3] 186.63.121.235:65184
/// 80. [25.12.2017-18:29:01.416942] [TEST] [std::__cxx11::string mapper::Mapper::getNextNode(const string&, const string&)] 2->(3)
/// 81. [25.12.2017-18:29:01.416951] [TEST] [void test::TestMapperLow::test_method()] for {[2] 170.194.76.149:53329 -> [3] 186.63.121.235:65184} next [3] 186.63.121.235:65184
/// 82. [25.12.2017-18:29:01.416963] [TEST] [std::__cxx11::string mapper::Mapper::getNextNode(const string&, const string&)] 2->3
/// 83. [25.12.2017-18:29:01.416969] [TEST] [std::__cxx11::string mapper::Mapper::getNextNode(const string&, const string&)] 2->(4)
/// 84. [25.12.2017-18:29:01.416974] [TEST] [void test::TestMapperLow::test_method()] for {[2] 170.194.76.149:53329 -> [4] 229.117.57.122:52282} next [4] 229.117.57.122:52282
/// 85. [25.12.2017-18:29:01.416987] [TEST] [std::__cxx11::string mapper::Mapper::getNextNode(const string&, const string&)] 3->2
/// 86. [25.12.2017-18:29:01.416993] [TEST] [std::__cxx11::string mapper::Mapper::getNextNode(const string&, const string&)] 3->2->4
/// 87. [25.12.2017-18:29:01.416998] [TEST] [std::__cxx11::string mapper::Mapper::getNextNode(const string&, const string&)] 3->(0)
/// 88. [25.12.2017-18:29:01.417003] [TEST] [void test::TestMapperLow::test_method()] for {[3] 186.63.121.235:65184 -> [0] 65.133.217.202:52164} next [0] 65.133.217.202:52164
/// 89. [25.12.2017-18:29:01.417045] [TEST] [std::__cxx11::string mapper::Mapper::getNextNode(const string&, const string&)] 3->2
/// 90. [25.12.2017-18:29:01.417052] [TEST] [std::__cxx11::string mapper::Mapper::getNextNode(const string&, const string&)] 3->2->4
/// 91. [25.12.2017-18:29:01.417057] [TEST] [std::__cxx11::string mapper::Mapper::getNextNode(const string&, const string&)] 3->0
/// 92. [25.12.2017-18:29:01.417062] [TEST] [std::__cxx11::string mapper::Mapper::getNextNode(const string&, const string&)] 3->0->(1)
/// 93. [25.12.2017-18:29:01.417079] [TEST] [void test::TestMapperLow::test_method()] for {[3] 186.63.121.235:65184 -> [1] 237.67.28.35:61332} next [0] 65.133.217.202:52164
/// 94. [25.12.2017-18:29:01.417106] [TEST] [std::__cxx11::string mapper::Mapper::getNextNode(const string&, const string&)] 3->(2)
/// 95. [25.12.2017-18:29:01.417113] [TEST] [void test::TestMapperLow::test_method()] for {[3] 186.63.121.235:65184 -> [2] 170.194.76.149:53329} next [2] 170.194.76.149:53329
/// 96. [25.12.2017-18:29:01.417124] [TEST] [std::__cxx11::string mapper::Mapper::getNextNode(const string&, const string&)] 3->2
/// 97. [25.12.2017-18:29:01.417132] [TEST] [std::__cxx11::string mapper::Mapper::getNextNode(const string&, const string&)] 3->2->(4)
/// 98. [25.12.2017-18:29:01.417137] [TEST] [void test::TestMapperLow::test_method()] for {[3] 186.63.121.235:65184 -> [4] 229.117.57.122:52282} next [2] 170.194.76.149:53329
/// 99. [25.12.2017-18:29:01.417149] [TEST] [std::__cxx11::string mapper::Mapper::getNextNode(const string&, const string&)] 4->2
/// 100. [25.12.2017-18:29:01.417156] [TEST] [std::__cxx11::string mapper::Mapper::getNextNode(const string&, const string&)] 4->2->3
/// 101. [25.12.2017-18:29:01.417162] [TEST] [std::__cxx11::string mapper::Mapper::getNextNode(const string&, const string&)] 4->(0)
/// 102. [25.12.2017-18:29:01.417167] [TEST] [void test::TestMapperLow::test_method()] for {[4] 229.117.57.122:52282 -> [0] 65.133.217.202:52164} next [0] 65.133.217.202:52164
/// 103. [25.12.2017-18:29:01.417183] [TEST] [std::__cxx11::string mapper::Mapper::getNextNode(const string&, const string&)] 4->2
/// 104. [25.12.2017-18:29:01.417190] [TEST] [std::__cxx11::string mapper::Mapper::getNextNode(const string&, const string&)] 4->2->3
/// 105. [25.12.2017-18:29:01.417195] [TEST] [std::__cxx11::string mapper::Mapper::getNextNode(const string&, const string&)] 4->0
/// 106. [25.12.2017-18:29:01.417202] [TEST] [std::__cxx11::string mapper::Mapper::getNextNode(const string&, const string&)] 4->0->(1)
/// 107. [25.12.2017-18:29:01.417207] [TEST] [void test::TestMapperLow::test_method()] for {[4] 229.117.57.122:52282 -> [1] 237.67.28.35:61332} next [0] 65.133.217.202:52164
/// 108. [25.12.2017-18:29:01.417219] [TEST] [std::__cxx11::string mapper::Mapper::getNextNode(const string&, const string&)] 4->(2)
/// 109. [25.12.2017-18:29:01.417224] [TEST] [void test::TestMapperLow::test_method()] for {[4] 229.117.57.122:52282 -> [2] 170.194.76.149:53329} next [2] 170.194.76.149:53329
/// 110. [25.12.2017-18:29:01.417235] [TEST] [std::__cxx11::string mapper::Mapper::getNextNode(const string&, const string&)] 4->2
/// 111. [25.12.2017-18:29:01.417242] [TEST] [std::__cxx11::string mapper::Mapper::getNextNode(const string&, const string&)] 4->2->(3)
/// 112. [25.12.2017-18:29:01.417248] [TEST] [void test::TestMapperLow::test_method()] for {[4] 229.117.57.122:52282 -> [3] 186.63.121.235:65184} next [2] 170.194.76.149:53329
