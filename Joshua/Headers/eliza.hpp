#ifndef ELIZA_HPP
#define ELIZA_HPP
 
#include <string>
#include <vector>
#include <boost/regex.hpp>
 
namespace JOSHUA
{
 
struct exchange
{
    boost::regex prompt_;
    std::vector<std::string> responses_;
    explicit exchange(const std::string& prompt)
      : prompt_(prompt, boost::regex::icase)
    {
    }
};
 
class exchange_builder;
 
class eliza
{
    std::string name_;
    std::vector<exchange> exchanges_;
    std::vector<std::pair<std::string, std::string> > translations_;
public:
    eliza(const std::string& name = "Joshua")
      : name_(name)
    {
        add_translations();
    }
    const std::string& name() const
    {
        return name_;
    }
    exchange_builder responds_to(const std::string& prompt);
    void add_exchange(const exchange& ex)
    {
        exchanges_.push_back(ex);
    }
    std::string respond(const std::string& input) const;
private:
    void add_translations();
    std::string translate(const std::string& input) const;
};
 
class exchange_builder
{
    friend eliza;
    eliza& eliza_;
    exchange exchange_;
 
    exchange_builder(eliza& el, const std::string& prompt)
      : eliza_(el), exchange_(prompt)
    {
    }
 
public:
    ~exchange_builder()
    {
        eliza_.add_exchange(exchange_);
    }
 
    exchange_builder& with(const std::string& response)
    {
        exchange_.responses_.push_back(response);
        return *this;
    }
};
 
}; // namespace MB
 
#endif // ELIZA_HPP
