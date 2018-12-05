#include <random>
#include <limits>
#include <sstream>
#include <unordered_set>
#include <string>
#include <algorithm>

#include "Log.hpp"
#include "LinksGenerator.hpp"


using namespace test;

const uint8_t MIN_ADDR_NUM = 1;
const uint8_t MAX_ADDR_NUM = 255;
const uint16_t MIN_DYNAMIC_PORT = 49152;
const uint16_t MAX_DYNAMIC_PORT = 65535;
const uint16_t MIN_LINKS_IN_ADDR = 1;
const uint16_t MAX_LINKS_IN_ADDR = 10;

typedef std::vector<std::string> Addresses;
typedef std::unordered_set<std::string> UniqAddresses;
typedef UniqAddresses::iterator UaddrIter;


struct IsBackLink {
    const std::string &_from;
    const std::string &_to;

    IsBackLink(const std::string &from_, const std::string &to_) 
        : _from(from_)
        , _to(to_)
    {}
    
    bool operator() (const GraphLink &link_) {
        return link_._addr_from == _to and link_._addr_to == _from;
    }
};
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


LinksGenerator::LinksGenerator(size_t uniq_addr_num_, size_t min_weight_, size_t max_weight_) {
    if (min_weight_ > max_weight_) {
        double tw = min_weight_;
        min_weight_ = max_weight_;
        max_weight_ = tw;
    } else if (min_weight_ == max_weight_) {
        max_weight_ += MIN_WEIGHT_DIST;
    }
    LOG(DEBUG) << "uniq_addr_num_ = " << uniq_addr_num_ << "; weight = [" << min_weight_ << ", " << max_weight_ << "]";
    /// Подготовить рандом устройство.
    std::random_device rd;
    std::mt19937 gen(rd());
    /// Подготовить рандом генераторы в заданных диапазонах.
    std::uniform_int_distribution<uint8_t> addr_dis(MIN_ADDR_NUM, MAX_ADDR_NUM);
    std::uniform_int_distribution<uint16_t> port_dis(MIN_DYNAMIC_PORT, MAX_DYNAMIC_PORT);
    std::uniform_int_distribution<size_t> weight_dis(min_weight_, max_weight_);
    size_t max_links = (uniq_addr_num_ < MAX_LINKS_IN_ADDR - 1) ? uniq_addr_num_ : MAX_LINKS_IN_ADDR;
    std::uniform_int_distribution<size_t> links_dis(MIN_LINKS_IN_ADDR, max_links);
    std::uniform_int_distribution<size_t> addr_id_dis(0, uniq_addr_num_ - 1);
    /// Сгенерировать уникальные связи.
    Addresses uniq_addrs;
    LOG(DEBUG) << "Addresses generation...";
    for (size_t i = 0; i < uniq_addr_num_; ++i) {
        std::stringstream addr_ss;
        addr_ss << std::to_string(addr_dis(gen)) << "." 
                << std::to_string(addr_dis(gen)) << "." 
                << std::to_string(addr_dis(gen)) << "."
                << std::to_string(addr_dis(gen)) << ":"
                << std::to_string(port_dis(gen));
        uniq_addrs.push_back(addr_ss.str());
        LOG(DEBUG) << addr_ss.str();
    }
    /// Сгенерировать уникальные связи м/у имеющимися в списке узлами.
    LOG(DEBUG) << "Links generation...";
    for (std::string &addr_from : uniq_addrs) {
        /// Сгенерировать связи для текущего адреса.
        size_t links_num = links_dis(gen);
        for (size_t i = 0; i < links_num; ++i) {
            size_t id;
            std::string addr_to;
            /// Исключить уже имеющиеся связи и кольцевые.
            UniqAddresses uaddrs;
            do {
                id = addr_id_dis(gen);
                addr_to = uniq_addrs[id];
                uaddrs.insert(addr_to);
            } while ((uaddrs.size() < uniq_addrs.size()) and 
                    ((addr_to == addr_from) or 
                    (std::find_if(_links.begin(), _links.end(), IsBackLink(addr_from, addr_to)) not_eq _links.end()) or 
                    (std::find_if(_links.begin(), _links.end(), IsBackLink(addr_to, addr_from)) not_eq _links.end())));
            if (uaddrs.size() < uniq_addrs.size()) {
                GraphLink gl{ addr_from, uniq_addrs[id], static_cast<double>(weight_dis(gen)) };
                _links.push_back(gl);
                LOG(DEBUG) << gl._addr_from << " -> " << gl._addr_to << ": w=" << gl._weigth;
            } else {
                LOG(WARNING) << "Links for \"" << addr_from << "\" is complette.";
            }
        }
    }
}


LinksGenerator::operator const Links& () {
    return _links;
}
