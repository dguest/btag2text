#ifndef CLUSTER_ROTATION_HH
#define CLUSTER_ROTATION_HH

#include <vector>

struct Point
{
  double x;
  double y;
  double w;
};

Point get_principal(const std::vector<Point>& in);
std::vector<Point> get_points_along_principal(
  const Point& principal,
  const std::vector<Point>& in);


#endif
