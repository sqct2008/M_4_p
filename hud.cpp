#include "hud.h"
#include "ioMod.h"
#include "gamedata.h"
#include "splitString.h"

Hud::Hud() {
  std::string stringList = Gamedata::getInstance().getXmlStr( "Hud/string_list" );
  std::vector<std::string> stringVec;
  SplitString::split(stringList, stringVec, ",");
  strings.reserve(stringVec.size()+2);
  strings.push_back("*********************************");
  for(auto& str : stringVec) {
    strings.push_back(Gamedata::getInstance().getXmlStr( "Hud/" + str ));
  }
  strings.push_back("*********************************");
  color.r = Gamedata::getInstance().getXmlInt( "Hud/color/red" );
  color.g = Gamedata::getInstance().getXmlInt( "Hud/color/green" );
  color.b = Gamedata::getInstance().getXmlInt( "Hud/color/blue" );
}

Hud& Hud::getInstance(){
  static Hud instance;
  return instance;
}

void Hud::draw() {
  for( int i = 0; i < static_cast<int>(strings.size()); ++i ) {
    IOmod::getInstance().writeText(strings[i], color, 250, 50 + 30 * i);
  }
}
