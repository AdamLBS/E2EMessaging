#include <drogon/drogon.h>

int main(int argc, char* argv[]) {
  // Charge config.json (ports, DB, Redis, TLS si besoin)
  drogon::app().loadConfigFile("config.json");
  // (Option) drogon::app().enableSession(false);  // auth stateless par JWT
  drogon::app().run();
  return 0;
}
