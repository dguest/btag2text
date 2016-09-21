#include "cluster_rotation.hh"

#include <iostream>

std::ostream& operator<<(std::ostream& out, const Point& pt) {
  out << pt.x << ", " << pt.y << ", [" << pt.w << "]";
  return out;
}

int main(int argc, char* argv[]) {
  std::vector<Point> inputs;
  for (int nnn = 0; nnn < 10; nnn++) {
    double x = nnn*3.0;
    double y = nnn*1.0;
    double w = 1.0 + 0.001*nnn;
    inputs.push_back({ x, y, w});
    inputs.push_back({ x,-y, w});
  }
  auto principal = get_principal(inputs);
  auto out = get_points_along_principal(principal,inputs);
  for (int nnn = 0; nnn < out.size(); nnn++) {
    std::cout << "in: " << inputs.at(nnn) << ", out: " << out.at(nnn)
              << std::endl;
  }
  return 0;
}

