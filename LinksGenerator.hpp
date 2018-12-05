/** Copyright &copy; 2017, rostislav.vel@gmail.com.
 * \brief  Класс генерирующий адреса .
 * \author Величко Ростислав
 * \date   21.12.2017
 */

#pragma once

#include <vector>
#include <string>
 
const size_t MIN_WEIGHT = 10;
const size_t MAX_WEIGHT = 10000;
const size_t MIN_WEIGHT_DIST = static_cast<size_t>((MAX_WEIGHT - MIN_WEIGHT) / 50);
const size_t MIN_ADDRESES_COUNT = 5;
const size_t DEFAULT_ADDRESES_COUNT = 100;


namespace test {

struct GraphLink {
    std::string _addr_from;
    std::string _addr_to;
    double _weigth;
};


typedef std::vector<GraphLink> Links; 


class LinksGenerator { 
    Links _links; ///< Массив сгенерированных связей.
    
    /**
     * \brief Метод генерирует произволный адрес и порт.
     * \return Сгенерированый адрес в строковом представлении.
     */
    std::string generateAddress();
    
public:
    /**
     * \brief Конструктор выполняет генерацию произвольных адресов и связей между ними с весами.
     * \param uniq_addr_num_ Общее количество уникальных узлов.
     * \param min_weight_ Минимальное значение веса.
     * \param max_weight_ Максимальное значение веса.
     */
    explicit LinksGenerator(size_t uniq_addr_num_, size_t min_weight_, size_t max_weight_);
    
    operator const Links& (); 
};
} /// test
