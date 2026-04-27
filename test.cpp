#include <iostream>

using namespace std;

int greet_user() {
  cout << "Hello World!" << endl;
  return 0;
}

int main() {
  int result = greet_user();
  cout << "Function returned: " << result << endl;
  return 0;
}
