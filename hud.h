#include <string>
#include <vector>
#include <SDL2/SDL.h>
class Hud {
  public:
    static Hud& getInstance();
    void draw();
  private:
    Hud();
    SDL_Color color;
    std::vector<std::string> strings;
};
