#include <iostream>

using namespace std;

int greet_user(int i) {
  cout << "Hello World!" << endl;
  return i + 10;
}

int main() {
  int result = greet_user(57);
  cout << "Function returned: " << result << endl;
  return 0;
}
