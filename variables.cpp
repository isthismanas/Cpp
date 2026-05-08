#include <iostream>

int main() {
  int a = 10;
  std::cout << "This is the current value of the integer a: " << a << std::endl;
  std::cout << "Enter a new value for a : ";
  std::cin >> a;
  std::cout << "This is now the new value of a: " << a << std::endl;
}
