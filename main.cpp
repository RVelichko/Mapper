/** Copyright &copy; 2017, rostislav.vel@gmail.com.
 * \brief  Приложение демонстрирующее работу алгоритма поиска кратчайшего пути.
 * \author Величко Ростислав
 * \date   21.12.2017
 */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include <random>
#include <memory>
#include <iostream>

#include "Log.hpp"
#include "LinksGenerator.hpp"
#include "Mapper.hpp"


struct GlobalArgs {
    size_t _min_weight;       /// параметр -w
    size_t _max_weight;       /// параметр -W
    size_t _addreses_num;     /// параметр -n
    size_t _view_results_num; /// параметр -N
} __global_args;


static const char *__opt_string = "w:W:n:t:h?";

static const int MIN_VIEWING_RESULTS_NUM = 5;
static const int DEFAULT_VIEWING_RESULTS_NUM = 15;
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


void HelpMessage() {
    std::cout << "  Use:\n\t#mapper -w " << MIN_WEIGHT << " -W " << MAX_WEIGHT << " -n " << DEFAULT_ADDRESES_COUNT << "\n"
              << "  Args:\n"
              << "\t[-w]\t Min weight. Default: " << MIN_WEIGHT << "\n"
              << "\t[-W]\t Max weight. Default: " << MAX_WEIGHT << "\n"
              << "\t[-n]\t Num of adresses. Default: " << DEFAULT_ADDRESES_COUNT << " and min: " << MIN_ADDRESES_COUNT << "\n"
              << "\t[-N]\t Num of viewing results . Default: " << DEFAULT_VIEWING_RESULTS_NUM << "\n"
              << "__________________________________________________________________\n\n";
    exit(EXIT_FAILURE);
}
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


typedef test::LinksGenerator LinksGenerator;
typedef test::GraphLink GraphLink;
typedef test::Links Links;
typedef mapper::Mapper Mapper;


int main(int argc_, char **argv_) {
    LOG_TO_STDOUT;
    int opt = 0;

    /// Инициализация globalArgs до начала работы с ней.
    __global_args._min_weight = MIN_WEIGHT;  
    __global_args._max_weight = MAX_WEIGHT;  
    __global_args._addreses_num = DEFAULT_ADDRESES_COUNT;
    __global_args._view_results_num = DEFAULT_VIEWING_RESULTS_NUM;

    /// Обработка входных опций.
    opt = getopt(argc_, argv_, __opt_string);
    while(opt != -1) {
        size_t value = 0;
        switch(opt) {
            case 'w':
                value = strtol(optarg, (char**)nullptr, 10);
                if (not value or (value < MIN_WEIGHT) or ((MAX_WEIGHT - MIN_WEIGHT_DIST) < value)) {
                    HelpMessage();
                }
                __global_args._min_weight = value;
                break;
            case 'W':
                value = strtol(optarg, (char**)nullptr, 10);
                if (not value or (value < MIN_WEIGHT + MIN_WEIGHT_DIST) or (MAX_WEIGHT < value)) {
                    HelpMessage();
                }
                __global_args._max_weight = value;
                break;
            case 'n':
                value = strtol(optarg, (char**)nullptr, 10);
                if (not value or (value < MIN_ADDRESES_COUNT)) {
                    HelpMessage();
                }
                __global_args._addreses_num = value;
                break;
            case 'N':
                value = strtol(optarg, (char**)nullptr, 10);
                if (not value or (value < MIN_VIEWING_RESULTS_NUM)) {
                    HelpMessage();
                }
                __global_args._view_results_num = value;
                break;

            case 'h':
            case '?':
                HelpMessage();
                break;

            default: break;
        }
        opt = getopt(argc_, argv_, __opt_string);
    }
    /// Сгенерировать связи.
    LinksGenerator lg(__global_args._addreses_num, __global_args._min_weight, __global_args._max_weight);
    Links links = lg;
    /// Выполнить поиск маршрутов.
    Mapper mapper;
    for (GraphLink &l : links) {
        mapper.insert(l._addr_from, l._addr_to, l._weigth);
    }
    /// Подготовить рандом устройство.
    std::random_device rd;
    std::mt19937 gen(rd());
    /// Подготовить рандом генераторы в заданных диапазонах.
    std::uniform_int_distribution<size_t> from_dis(0, links.size());
    std::uniform_int_distribution<size_t> to_dis(0, links.size());
    /// Получить произвольно выбранные маршруты.
    for (size_t count = 0; count < __global_args._view_results_num; ++count) {
        size_t from_id = from_dis(gen);
        size_t to_id = to_dis(gen);
        if (links.size() <= from_id + __global_args._view_results_num + 1) {
            from_id = links.size() - __global_args._view_results_num - 2;
        }
        if (links.size() <= to_id + __global_args._view_results_num + 1) {
            to_id = links.size() - __global_args._view_results_num - 2;
        }
        if (to_id == from_id) {
            ++to_id;
        }
        std::string from = links[from_id]._addr_from;
        std::string to = links[to_id]._addr_from;
        std::string next = mapper.getNextNode(from, to);
        LOG(DEBUG) << "[" << from << " -> " << to << "]: " << next;
    }
    
    return EXIT_SUCCESS;
}
