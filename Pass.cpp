#include "Pass.h"
#include <cstring>
#include <fstream>
#include <iostream>

map<string, Cmd> Pass::hash = {{"s", SEARCH},   {"S", SEARCH}, {"g", GENERATE},
                               {"G", GENERATE}, {"e", EXIT},   {"E", EXIT}};

void Pass::run(int argc, char **argv) {
  if (argc == 2) {
    string user = pureSearch(argv[1], NAME_USER);
    string pass = pureSearch(argv[1], NAME_PASS);
    if (!pass.empty())
      cout << user << " " << pass << endl;
    else
      cout << "Not found!" << endl;
  } else if (argc == 1) {
    Status status = GOOD;
    while (true) {
      if (status == GOOD)
        cout << "(S)earch, (G)enerate or (E)xit: ";
      string cmd;
      cin >> cmd;
      status = GOOD;
      switch (hash[cmd]) {
      case SEARCH:
        search();
        break;
      case GENERATE:
        generate();
        break;
      case EXIT:
        exit(0);
      default:
        cout << "Wrong input! Try again: ";
        status = INVALID;
      }
    }
  } else
    cout << "Wrong input!" << endl;
}

string Pass::pureSearch(const string &str, Type type) {
  char buf[1000] = {0};
  string name, user, pass;
  ifstream fin;
  fin.open(file, ios::in);
  while (fin.good()) {
    fin.get(buf, 1000, ' ');
    fin.ignore();
    name = buf;
    memset(buf, 0, sizeof(buf));
    fin.get(buf, 1000, ' ');
    fin.ignore();
    user = buf;
    memset(buf, 0, sizeof(buf));
    fin.get(buf, 1000, '\n');
    fin.ignore();
    pass = buf;
    memset(buf, 0, sizeof(buf));
    if (type == NAME_USER && str == name || type == USER && str == user)
      return user;
    if (type == NAME_PASS && str == name || type == USER_PASS && str == user ||
        type == PASS && str == pass)
      return pass;
  }
  fin.close();
  return "";
}

string Pass::lessSearch(string &name) {
  cout << "Input name: ";
  cin >> name;
  string user = pureSearch(name, NAME_USER);
  while (user.empty()) {
    cout << "Not found! Try again or (E)xit: ";
    cin >> name;
    if (name == "e" || name == "E")
      break;
    user = pureSearch(name, NAME_USER);
  }
  return user;
}

void Pass::search() {
  string name;
  string user = lessSearch(name);
  string pass = pureSearch(name, NAME_PASS);
  if (!user.empty())
    cout << user << " " << pass << endl;
  else
    cout << "Not found!" << endl;
}

string Pass::pureGenerate(int length, Complexity complexity, Type type) {
  string simple =
      "0123456789aAbBcCdDeEfFgGhHiIjJkKlLmMnNoOpPqQrRsStTuUvVwWxXyYzZ";
  string basic = "!@#$%^&*-_";
  string complex = "`~()=+[{]}\\|;:'\",<.>/?";
  string lib;
  switch (complexity) {
  case COMPLEX:
    lib = simple + basic + complex;
    break;
  case BASIC:
    lib = simple + basic;
    break;
  case SIMPLE:
    lib = simple;
    break;
  default:;
  }
  char pass[length + 1];
  uniform_int_distribution<int> random(0, lib.length() - 1);
  while (true) {
    for (int i = 0; i < length; i++)
      pass[i] = lib[random(engine)];
    pass[length] = 0;
    if (type == USER && pureSearch(pass, USER).empty() ||
        type == PASS && pureSearch(pass, PASS).empty())
      break;
  }
  return pass;
}

string Pass::lessGenerate(const string &user) {
  cout << "Input length: ";
  int length;
  cin >> length;
  while (length < 4 || length > 96) {
    cout << "Length should be 4~96. Try again: ";
    cin >> length;
  }
  cout << "(S)imple, (B)asic or (C)omplex: ";
  string cmd;
  cin >> cmd;
  while (cmd != "c" && cmd != "b" && cmd != "s" && cmd != "C" && cmd != "B" &&
         cmd != "S") {
    cout << "Wrong input! Try again: ";
    cin >> cmd;
  }
  string pass;
  if (cmd == "c" || cmd == "C")
    pass = pureGenerate(length, COMPLEX, PASS);
  else if (cmd == "b" || cmd == "B")
    pass = pureGenerate(length, BASIC, PASS);
  else
    pass = pureGenerate(length, SIMPLE, PASS);
  cout << "Username is " << user << " ." << endl;
  cout << "Password is " << pass << " ." << endl;
  return pass;
}

void Pass::generate() {
  string user = pureGenerate(8, SIMPLE, USER);
  string pass = lessGenerate(user);
  cout << "Input name or (E)xit: ";
  string name;
  cin >> name;
  Status status = nameCheck(name);
  while (name != "e" && name != "E" && status != GOOD) {
    switch (status) {
    case REPEAT:
      cout << "Name exists! Try again or (E)xit: ";
      break;
    case EXCEED:
      cout << "Name's length should be 1~96. Try again or (E)xit: ";
      break;
    case INVALID:
      cout << "Name shouldn't contain ' '. Try again or (E)xit: ";
    default:;
    }
    cin >> name;
    status = nameCheck(name);
  }
  if (name != "e" && name != "E")
    add(name, user, pass);
}

void Pass::add(const string &name, const string &user, const string &pass) {
  ofstream fout;
  fout.open(file, ios::app);
  fout << name << " " << user << " " << pass << "\n";
  fout.close();
}

Status Pass::nameCheck(const string &name) {
  if (!pureSearch(name, NAME_USER).empty())
    return REPEAT;
  else if (name.length() > 96)
    return EXCEED;
  else if (name.find(' ') != -1)
    return INVALID;
  else
    return GOOD;
}

Pass::Pass(string file) : file(move(file)) {}