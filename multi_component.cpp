#include <ctime>
#include <iomanip>
#include <iostream>
#include <random>
#include <string>

using namespace std;

int main() {

  cout << "================================================================="
       << endl;
  cout << "                   MULTI-COMPONENT PROGRAM                       "
       << endl;
  cout << "================================================================="
       << endl;
  cout << "         This program demonstrates using multiple headers        "
       << endl;
  cout << "         and formatting techniques in C++                        "
       << endl;

  // Component 1: Basic information display
  string name = "C++ Programming";
  string version = "C++17";
  string creator = "Bjarne Stroustrup";
  int yearCreated = 1985;

  cout << "Language: " << name << endl;
  cout << "Versrion: " << version << endl;
  cout << "Creator: " << creator << endl;
  cout << "Year created: " << yearCreated << endl << endl;

  string fact1 =
      "I have to compile about 6-7 times before I fix all the highlighted bugs";
  bool fact2 = true;
  int fact3 = 0;
  cout << "Interesting Fact 1: " << fact1 << endl;
  cout << "Interestung Fact 2: " << (fact2 ? "Yes" : "No") << endl;
  cout << "Interesting Fact 3: " << fact3 << endl;

  // Component 2: Formatted numeric output
  
  cout << "---------- COMPONENT 2: FORMATTED NUMBERS ---------" << endl;

// Create numeric variables
  double pi = 3.14159265358979323846;
  double e = 2.71828182845904523536;
  double goldenRatio = 1.61803398874989484820;

// Display with different precision
  cout << "Default display:" << endl;
  cout << "Pi = " << pi << endl;
  cout << "e = " << e << endl;
  cout << "Golden Ratio = " << goldenRatio << endl << endl;

  cout << "With 2 decimal places:" << endl;
  cout << fixed << setprecision(2);
  cout << "Pi = " << pi << endl;
  cout << "e = " << e << endl;
  cout << "Golden Ratio = " << goldenRatio << endl << endl;

  cout << "With 6 decimal places:" << endl;
  cout << fixed << setprecision(6);
  cout << "Pi = " << pi << endl;
  cout << "e = " << e << endl;
  cout << "Golden Ratio = " << goldenRatio << endl << endl;

// Reset formatting
  cout.unsetf(ios::fixed);
  cout << setprecision(6);
  // Component 3: Table display
  // Program end

  cout << "           Program execution complete                            "
       << endl;
  return 0;
}
