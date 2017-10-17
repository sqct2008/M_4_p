#include <string>
#include <vector>

class SplitString{
  public:
    SplitString() = delete;
    SplitString(SplitString&) = delete;
    static void split(const std::string&, std::vector<std::string>&, const std::string&);
};
