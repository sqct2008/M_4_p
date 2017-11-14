#include <string>
#include <vector>
#include <SDL.h>
class Hud {
  public:
    static Hud& getInstance();
    void draw();
  private:
    Hud();
    SDL_Color color;
    std::vector<std::string> strings;
};
