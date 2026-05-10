#include <iostream>

namespace NumGen {
class Integer {
public:
  int number;
  bool conv_hist = false;
  int num_conv = 0;
  float float_cast = 0.0f;
  void displayStats() {
    std::cout << "Integer: " << number << std::endl;
    std::cout << "Conversion History (Has it been type casted recently?): "
              << conv_hist << std::endl;
    std::cout << "Number of times typecasted: " << num_conv << std::endl;
    std::cout << "Float: " << float_cast << std::endl;
  }
};
} // namespace NumGen

float toFloat(NumGen::Integer &number) {
  float toFloat = static_cast<float>(number.number);
  number.conv_hist = true;
  std::cout << "Previously type casted: " << number.num_conv << "times"
            << std::endl;
  number.num_conv++;
  number.float_cast = toFloat;
  return toFloat;
}

int toInt(NumGen::Integer &number) {
  if (number.conv_hist == 0 or number.float_cast == 0.0f) {
    std::cout << "This number is already an integer" << std::endl;
    return -99999999;
  } else {
    int toInt = static_cast<int>(number.float_cast);
    number.number = toInt;
    number.conv_hist = true;
    number.num_conv++;
    return toInt;
  }
}

int main() {
  NumGen::Integer myNum;
  std::cout << "Please enter your integer number: ";
  std::cin >> myNum.number;
  std::cout << std::endl;
  myNum.displayStats();
  std::cout << "After conversion to float:" << toFloat(myNum) << std::endl;
  myNum.displayStats();
  std::cout << "Lets try converting the number back to int " << std::endl;
  std::cout << "The number now becomes: " << toInt(myNum) << std::endl;
  myNum.displayStats();
  return 0;
}
