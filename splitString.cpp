#include "splitString.h"

void SplitString::split(const std::string& in_string, std::vector<std::string>& vector_of_string, const std::string& spliter)
{
  std::string::size_type front_pos, back_pos;
  front_pos = 0;
  back_pos = in_string.find(spliter);

  while(back_pos != std::string::npos){
    vector_of_string.push_back(in_string.substr(front_pos, back_pos - front_pos));

    front_pos = back_pos + spliter.size();
    back_pos = in_string.find(spliter, front_pos);
  }
  if( front_pos != in_string.size())
    vector_of_string.push_back(in_string.substr(front_pos));
}
