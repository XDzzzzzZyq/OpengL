#pragma once

#include <vector>
#include <memory>

#include "CubicSplineHelpers.h"

namespace CubicSplineTest
{
    class CubicBezierSpline;
    class QuinticSolver;

    class ClosestPointSolver
    {
    public:
        ClosestPointSolver();
        ~ClosestPointSolver();

        const QuinticSolver* Get() const;
    private:
        std::unique_ptr<QuinticSolver> solver_;
    };

    // A Bezier path is a set of Bezier splines which are connected.
    // (The last control point of a spline, is the first control pont of the next spline.)
    class CubicBezierPath
    {
    public:
        CubicBezierPath(const WorldSpace* control_points, const int num_points);
        WorldSpace ClosestPointToPath(const WorldSpace& position, const ClosestPointSolver* solver) const;

        ~CubicBezierPath();
    private:
        std::vector<std::unique_ptr<CubicBezierSpline> > splines_;
    };
}
