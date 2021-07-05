#include <map>
#include <random>
#include <string>
using namespace std;

enum Cmd { SEARCH, GENERATE, EXIT };
enum Complexity { SIMPLE, BASIC, COMPLEX };
enum Status { GOOD, REPEAT, EXCEED, INVALID };
enum Type { USER, PASS, NAME_USER, NAME_PASS, USER_PASS };

class Pass {
private:
  string file;
  static map<string, Cmd> hash;
  random_device device;
  mt19937_64 engine{device()};

public:
  void run(int argc, char **argv);
  string pureSearch(const string &str, Type type);
  string lessSearch(string &name);
  void search();
  string pureGenerate(int length, Complexity complexity, Type type);
  string lessGenerate(const string &user);
  void generate();
  void add(const string &name, const string& user, const string &pass);
  Status nameCheck(const string &name);
  explicit Pass(string file);
};