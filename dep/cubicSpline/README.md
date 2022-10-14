# CubicSplineClosestPoint
Classes that demonstrate how to use Sturm's Theroem for root finding to solve the 5th degree polynomial required to project a 3D point to the closest point on a set of cubic Bézier splines.

For a detailed description see this blog entry:
https://computingandrecording.wordpress.com/2017/03/20/closest-point-to-a-cubic-spline/

Here is an example usage:

std::vector<WorldSpace> control_points;
            
control_points.push_back(WorldSpace(74.f, 100.f, 1.3f)); // Spline 1
control_points.push_back(WorldSpace(62.f, 88.f, 0.f));
control_points.push_back(WorldSpace(136.f, 48.f, 0.f));
control_points.push_back(WorldSpace(139.f, 69.f, 8.f)); // Spline 2
			
control_points.push_back(WorldSpace(171.f, 127.f, 0.f));
control_points.push_back(WorldSpace(276.f, 159.f, 5.f));
control_points.push_back(WorldSpace(195.f, 155.f, 9.94f)); // Spline 3

control_points.push_back(WorldSpace(185.f, 155.f, 23.f));
control_points.push_back(WorldSpace(185.f, 205.f, 0.333f));
control_points.push_back(WorldSpace(233.f, 205.f, 0.f));

ScopedPointer<CubicBezierPath> bezier_path = new CubicBezierPath(&control_points[0], (int)control_points.size());
            
ScopedPointer<ClosestPointSolver> solver = new ClosestPointSolver();

for (a set of positions)
{
    WorldSpace solution = bezier_path->ClosestPointToPath(position, solver);
    // Do something.
}
