#ifndef BOOST_STATIC_LINK
#   define BOOST_TEST_DYN_LINK
#endif // BOOST_STATIC_LINK

#define BOOST_TEST_MODULE CppExamples
#define BOOST_AUTO_TEST_MAIN

#include <unordered_set>

#include <boost/test/unit_test.hpp>
#include <boost/test/output_test_stream.hpp>

#include "Log.hpp"
#include "LinksGenerator.hpp"

namespace test {


BOOST_AUTO_TEST_CASE(TestCreateLinks) {
    LOG_TO_STDOUT;
    LinksGenerator lg(DEFAULT_ADDRESES_COUNT, MIN_WEIGHT, MAX_WEIGHT);
    Links links = lg;
    std::unordered_set<std::string> uniq_addrs;
    /// Проверить соотвествие сгенерированных весов
    for (GraphLink &gl : links) {
        uniq_addrs.insert(gl._addr_from);
        uniq_addrs.insert(gl._addr_to);
        BOOST_CHECK(MIN_WEIGHT <= gl._weigth and gl._weigth <= MAX_WEIGHT);
    }
    /// Проверить проверка количества сгенерированных адресов.
    BOOST_CHECK(uniq_addrs.size() == DEFAULT_ADDRESES_COUNT);
}
} // test
