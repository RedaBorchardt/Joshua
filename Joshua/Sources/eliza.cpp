#include <cstdlib>
 
#include <boost/algorithm/string.hpp>
#include <boost/format.hpp>
 
#include "eliza.hpp"
 
using namespace JOSHUA;
 
exchange_builder eliza::responds_to(const std::string& prompt)
{
    return exchange_builder(*this, prompt);
}
 
void eliza::add_translations()
{
    translations_.push_back(std::make_pair("am", "are"));
    translations_.push_back(std::make_pair("was", "were"));
    translations_.push_back(std::make_pair("i" , "you"));
    translations_.push_back(std::make_pair("i'd", "you would"));
    translations_.push_back(std::make_pair("you'd", "I would"));
    translations_.push_back(std::make_pair("you're", "I am"));
    translations_.push_back(std::make_pair("i've", "you have"));
    translations_.push_back(std::make_pair("i'll", "you will"));
    translations_.push_back(std::make_pair("i'm", "you are"));
    translations_.push_back(std::make_pair("my", "your"));
    translations_.push_back(std::make_pair("are", "am"));
    translations_.push_back(std::make_pair("you've", "I have"));
    translations_.push_back(std::make_pair("you'll", "I will"));
    translations_.push_back(std::make_pair("your", "my"));
    translations_.push_back(std::make_pair("yours", "mine"));
    translations_.push_back(std::make_pair("you", "me"));
    translations_.push_back(std::make_pair("me", "you"));
    translations_.push_back(std::make_pair("myself", "yourself"));
    translations_.push_back(std::make_pair("yourself", "myself"));
 
}
 
std::string eliza::translate(const std::string& input) const
{
    std::vector<std::string> words;
    boost::split(words, input, boost::is_any_of(" \t"));
    for (auto& word : words) {
        boost::algorithm::to_lower(word);
        for (auto& trans : translations_) {
            if (trans.first == word) {
                word = trans.second;
                break;
            }
        }
    }
    return boost::algorithm::join(words, " ");
}
 
std::string eliza::respond(const std::string& input) const
{
    std::string response;
    for (auto& ex : exchanges_) {
        boost::smatch m;
        if (boost::regex_search(input, m, ex.prompt_)) {
            response = ex.responses_[rand() % ex.responses_.size()];
            if (m.size() > 1 && response.find("%1%") != response.npos) {
                std::string translation = translate(std::string(m[1].first, m[1].second));
                boost::trim_right_if(translation, boost::is_punct());
                response = boost::str(boost::format(response) % translation);
            }
            break;
        }
    }
    return response;
}
