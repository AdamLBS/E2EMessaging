#include <drogon/drogon.h>

int main(int argc, char* argv[]) {
  drogon::app().loadConfigFile("config.json");
  drogon::app().run();
  return 0;
}
