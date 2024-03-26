/*
 * CS106L SimpleEnroll Assignment Part 1
 * Created by Fabio Ibanez
 */

#include "utils.cpp"
#include <exception>
#include <iomanip>
#include <ios>
#include <iostream>
#include <ostream>
#include <string>
#include <system_error>
#include <vector>

std::string COURSES_PATH = "./courses.csv";

bool structTest() {
  Course c{"Standard C++ Programming", "1", "2023-2024 Winter"};
  return true;
}

std::ostream &operator<<(std::ostream &os, Course c) {
  os << std::left << "title: " << c.title << std::setw(50) << " ";
  os << std::left << std::setw(10) << "number: " << c.number_of_units;
  os << std::left << std::setw(10) << "quarter: " << c.quarter;
  return os;
}

void printvec(std::vector<Course> &vec) {
  for (auto &c : vec) {
    std::cout << c << "\n";
  }
}
int main() {
  std::vector<Course> vector_of_courses;

  if (!structTest()) {
    std::cerr << "structTest Failed 😞";
  } else {
    std::cout << "structTest Passed! ✅" << '\n';
  }

  parse_csv(COURSES_PATH, vector_of_courses);

  write_courses_offered(vector_of_courses);

  write_courses_not_offered(vector_of_courses);

  FILE *pipe = popen("python3 utils/autograder.py", "r");
  if (!pipe)
    return -1;

  char buffer[128];
  while (!feof(pipe)) {
    if (fgets(buffer, 128, pipe) != NULL)
      std::cout << buffer;
  }
  pclose(pipe);

  // uncomment this line to debug
  // print_vector(vector_of_courses);
  return 0;
}
