#include "CubicSpline.h"
#include "CubicSplineHelpers.inl"

#include <array>
#include <vector>

#include <xmmintrin.h>
#include <smmintrin.h>

//#define USE_SIMD_OPTIMIZATION

namespace CubicSplineTest
{
    // The Sturm method of finding roots uses subdivision. 
    const float kTolerance = 0.00001f;

    // Helper to solve a polynomial of a given degree.
    float EvaluatePolynomial(
        const float* polynomial,
        const int degree,
        const float t)
    {
        float result = 0.f;
        const float* coefficient = polynomial;
        for (int i = 0; i < degree; ++i, ++coefficient)
        {
            result += *coefficient;
            result *= t;
        }
        result += *coefficient;

        return result;
    }

    struct Polynomial5
    {
        std::array<float, 5 + 1> equation;

        // Input: polynomial with a leading coefficient of 1.0.
        float EvaluateNormedEquation(
            const float t) const;
    };

#ifndef USE_SIMD_OPTIMIZATION
    float Polynomial5::EvaluateNormedEquation(
        const float t) const
    {
        return EvaluatePolynomial(&equation[0], 5, t);
    }
#endif // !USE_SIMD_OPTIMIZATION

    constexpr int ArithmeticSum(int n) { return n * (1 + n) / 2; }

    class CubicBezierSpline
    {
    public:
        CubicBezierSpline(const WorldSpace* control_points);
        float ClosestPointToSpline(const WorldSpace& position, const QuinticSolver* solver, WorldSpace& closest) const;
        WorldSpace EvaluateAt(const float t) const;
    private:
        void Initialize();

        typedef std::array<float, 6> ClosestPointEquation;

        std::array<WorldSpace, 4> control_points_;
        std::array<WorldSpace, 4> polynomial_form_; // Coefficents derived from the control points.
        std::array<WorldSpace, 3> derivative_;
        // The closest projected point equation for a given position p, is:
        // Dot(p, derivative_) - Dot(polynomial_form_, derivative) = 0
        // precomputed_coefficients_ stores -Dot(polynomial_form_, derivative) so that only
        // Dot(p, derivative_) needs to be computed for each position.
        ClosestPointEquation precomputed_coefficients_;
        float inv_leading_coefficient_;
    };

    CubicBezierPath::CubicBezierPath(
        const WorldSpace* control_points,
        const int num_points)
    {
        int num_splines = num_points / 3;
        for (int i = 0; i < num_splines; ++i)
        {
            splines_.emplace_back(new CubicBezierSpline(&control_points[i * 3]));
        }
    }

    CubicBezierPath::~CubicBezierPath() {}

    WorldSpace CubicBezierPath::ClosestPointToPath(
        const WorldSpace& position,
        const ClosestPointSolver* solver) const
    {
        WorldSpace min_position{ 0.f };
        float min_dist_sq = std::numeric_limits<float>::max();

        // The closest point on the path, is the closest point from the set of closest points to each spline.
        WorldSpace spline_position{ 0.f };
        for (const auto& spline : splines_)
        {
            const float dist_sq = spline->ClosestPointToSpline(position, solver->Get(), spline_position);
            if (dist_sq < min_dist_sq)
            {
                min_dist_sq = dist_sq;
                min_position = spline_position;
            }
        }

        return min_position;
    }

    class QuinticSolver
    {
    public:
        QuinticSolver(float tolerance);
        int Solve(
            const Polynomial5& polynomial,
            std::array<float, 5>& out_roots,
            const float interval_min,
            const float interval_max) const;
    private:
        struct SturmInterval
        {
            SturmInterval()
            {}

            SturmInterval(float _min, float _max, int _sign_min, int _sign_max, int _id, int _roots)
                : min(_min), max(_max), sign_min(_sign_min), sign_max(_sign_max), id(_id), expected_roots(_roots)
            {}

            float min;
            float max;
            int sign_min; // Sign changes for the minimum bound.
            int sign_max; // Sign changes for the max bound.
            int id; // Id shared between this interval and its sibling.
            int expected_roots; // Total roots expected in this interval and its sibling.
        };

        struct Interval
        {
            float min;
            float max;

            Interval& operator=(const SturmInterval& sturm_interval)
            {
                min = sturm_interval.min;
                max = sturm_interval.max;
                return *this;
            }
        };

        typedef std::array<float, ArithmeticSum(5 + 1)> SturmSequence5;

        float tolerance_;
        uint32 max_divisions_;

        // Memory for the stack of intervals being searched. The goal of this is to avoid any memory allocations
        // during the solver. If the tolerance is low, this could just be put on the stack instead.
        mutable std::vector<SturmInterval> interval_storage_;

        void BuildSturmSequence(const Polynomial5& polynomial, SturmSequence5& out_sturm_polynomials) const;
        int CountSturmSignChanges(const SturmSequence5& sturm_polynomials, const float t) const;
        float SolveBisection(const Polynomial5& polynomial, const float interval_min, const float interval_max) const;
    };

    ClosestPointSolver::ClosestPointSolver() :
        solver_(new QuinticSolver(kTolerance))
    {}

    ClosestPointSolver::~ClosestPointSolver() {}

    const QuinticSolver* ClosestPointSolver::Get() const
    {
        return solver_.get();
    }

    QuinticSolver::QuinticSolver(float tolerance)
    {
        tolerance_ = tolerance;
        max_divisions_ = 1 + (uint32)log2(1.f / tolerance_);
        interval_storage_.reserve(max_divisions_ * 2);
    }

#ifndef USE_SIMD_OPTIMIZATION
    int QuinticSolver::CountSturmSignChanges(
        const SturmSequence5& sturm_polynomials,
        const float t) const
    {
        int sign_changes = 0;
        int index = ArithmeticSum(5 + 1) - 1;
        const union { float fl_val; uint32 n_val; } sign = { sturm_polynomials[index] };
        uint32 previous = sign.n_val;        
        float solution = 0.f;        
        for (int i = 1; i <= 5; ++i)
        {
            index -= i + 1;
            const float* seq = &sturm_polynomials[index];
            solution = EvaluatePolynomial(seq, i, t);            

            // Increment sign_changes if the sign bit changed.
            union { float fl_val; uint32 n_val; } sign_min = { solution };
            sign_changes += (previous ^ sign_min.n_val) >> 31;
            previous = sign_min.n_val;
        }

        return sign_changes;
}
#endif

    float QuinticSolver::SolveBisection(
        const Polynomial5& polynomial,
        const float interval_min,
        const float interval_max) const
    {
        // Only intervals of where p(min) < 0 and p(max) > 0 can contain a solution.
        // This is equivalent to the distance function from the given position to
        // the solution having a negative slope at the min and a positive slope at max.
        if (polynomial.EvaluateNormedEquation(interval_min) > 0)
        {
            return NAN;
        }
        if (polynomial.EvaluateNormedEquation(interval_max) < 0)
        {
            return NAN;
        }

        const int max_iterations = max_divisions_;
        float bisection_min = interval_min;
        float bisection_max = interval_max;
        for (int i = 0; i < max_iterations; ++i)
        {
            const float mid = (bisection_max + bisection_min) / 2.f;

            const float r = polynomial.EvaluateNormedEquation(mid);
            const union { float fl_val; uint32 n_val; } r_sign = { r };
            if (r_sign.n_val & 0x80000000u)
            {
                if (r >= -tolerance_)
                {
                    return mid;
                }
                bisection_min = mid;
            }
            else
            {
                if (r <= tolerance_)
                {
                    return mid;
                }
                bisection_max = mid;
            }
        }

        return bisection_min;
    }

    void QuinticSolver::BuildSturmSequence(
        const Polynomial5& polynomial,
        SturmSequence5& out_sturm_polynomials) const
    {
        const int degree = 5;
        std::array<double, ArithmeticSum(degree + 1)> sturm_polys;

        for (int i = 0; i <= degree; ++i)
        {
            sturm_polys[i] = polynomial.equation[i];
            sturm_polys[degree + 1 + i] = (degree - i) * sturm_polys[i]; // derivative
        }

        int index = 6;
        double* element = &sturm_polys[6 + 5]; // start of third polynomial
        for (int i = 2; i <= degree; ++i)
        {
            const int poly = degree - i;
            const double* p1 = &sturm_polys[index];
            const double* p2 = &sturm_polys[index - (poly + 3)];
            index += poly + 2;

            // long polynomial division
            const double remainder_coefficeint = (double)p2[0] / p1[0];
            const double remainder_sum = (p2[1] - remainder_coefficeint * p1[1]) / p1[0];
            for (int c = 0; c <= poly; ++c, ++element)
            {
                double coefficient = remainder_sum * p1[c + 1] - p2[c + 2];
                if (c != poly)
                {
                    coefficient += remainder_coefficeint * p1[c + 2];
                }

                *element = coefficient;
            }
        }

        for (int i = 0; i < ArithmeticSum(degree + 1); ++i)
        {
            out_sturm_polynomials[i] = (float)sturm_polys[i];
        }
    }

    int QuinticSolver::Solve(
        const Polynomial5& polynomial,
        std::array<float, 5>& out_roots,
        const float interval_min,
        const float interval_max) const
    {
        SturmSequence5 sturm_polynomials;
        BuildSturmSequence(polynomial, sturm_polynomials);

        // Set up the first interval.
        interval_storage_.clear();        
        int sign_min = CountSturmSignChanges(sturm_polynomials, interval_min);
        int sign_max = CountSturmSignChanges(sturm_polynomials, interval_max);
        const int total_roots = sign_min - sign_max;
        int id = 0;
        interval_storage_.emplace_back(interval_min, interval_max, sign_min, sign_max, id++, total_roots);
                        
        std::array<Interval, 5> root_intervals;
        int found_roots = 0;
        
        // Isolate roots
        while (!interval_storage_.empty() && total_roots != found_roots)
        {
            SturmInterval i = interval_storage_.back();
            interval_storage_.pop_back();

            int num_roots = i.sign_min - i.sign_max;

            if (num_roots <= 0)
            {
                if (!interval_storage_.empty() &&
                    interval_storage_.back().id == i.id)
                {
                    i = interval_storage_.back();
                    interval_storage_.pop_back();
                    num_roots = i.expected_roots;
                }
                else
                {
                    continue;
                }
            }

            // Prune sibling intervals based on the results of the current interval.
            if (num_roots == i.expected_roots &&
                !interval_storage_.empty() &&
                interval_storage_.back().id == i.id)
            {
                interval_storage_.pop_back();
            }
            else if (num_roots == i.expected_roots - 1 &&
                !interval_storage_.empty() &&
                interval_storage_.back().id == i.id) // This case was a biggest perf improvemnt.
            {
                root_intervals[found_roots++] = interval_storage_.back();
                interval_storage_.pop_back();
            }

            if (num_roots == 1)
            {
                root_intervals[found_roots++] = i;
            }
            else
            {
                float mid = (i.min + i.max) / 2.f;
                if (mid - i.min <= tolerance_)
                {
                    root_intervals[found_roots++] = i;
                }
                else
                {
                    // Divide the current interval and search deeper.
                    const int sign_mid = CountSturmSignChanges(sturm_polynomials, mid);
                    interval_storage_.emplace_back(i.min, mid, i.sign_min, sign_mid, id, num_roots);
                    interval_storage_.emplace_back(mid, i.max, sign_mid, i.sign_max, id, num_roots);
                    ++id;
                }
            }
        }

        int num_real_roots = 0;
        for (int i = 0; i < found_roots; ++i)
        {
            const Interval& interval = root_intervals[i];
            float root = SolveBisection(polynomial, interval.min, interval.max);
            if (!isnan(root))
            {
                out_roots[num_real_roots++] = root;
            }
        }

        return num_real_roots;
    }

    CubicBezierSpline::CubicBezierSpline(const WorldSpace* control_points)
    {
        std::copy(control_points, control_points + 4, control_points_.begin());

        Initialize();
    }

    void CubicBezierSpline::Initialize()
    {
        WorldSpace& p0 = control_points_[0];
        WorldSpace& p1 = control_points_[1];
        WorldSpace& p2 = control_points_[2];
        WorldSpace& p3 = control_points_[3];

        // Expanding out the parametric cubic Bezier curver equation.
        WorldSpace n = -1.f*p0 + 3.f*p1 + -3.f*p2 + p3;
        WorldSpace r = 3.f*p0 + -6.f*p1 + 3.f*p2;
        WorldSpace s = -3.f*p0 + 3.f*p1;
        WorldSpace& v = p0;

        polynomial_form_[0] = n;
        polynomial_form_[1] = r;
        polynomial_form_[2] = s;
        polynomial_form_[3] = v; // p0

        // The derivative which is a quadratic equation.
        WorldSpace j = 3.f*n;
        WorldSpace k = 2.f*r;
        WorldSpace& m = s;

        derivative_[0] = j;
        derivative_[1] = k;
        derivative_[2] = m;
        
        // - Dot(polynomial_form_, derivative_) divided by the leading coefficient
        inv_leading_coefficient_ = -1.f/Dot(j, n);
        precomputed_coefficients_[0] = 1.0f;
        precomputed_coefficients_[1] = -(Dot(j, r) + Dot(k, n));
        precomputed_coefficients_[2] = -(Dot(j, s) + Dot(k, r) + Dot(m, n));
        precomputed_coefficients_[3] = -(Dot(j, v) + Dot(k, s) + Dot(m, r));
        precomputed_coefficients_[4] = -(Dot(k, v) + Dot(m, s));
        precomputed_coefficients_[5] = -Dot(m, v);
        for (int i = 1; i < 6; ++i)
        {
            precomputed_coefficients_[i] *= inv_leading_coefficient_;
        }
    }

    float CubicBezierSpline::ClosestPointToSpline(
        const WorldSpace& position,
        const QuinticSolver* solver,
        WorldSpace& closest) const
    {
        Polynomial5 quintic;
        std::copy(precomputed_coefficients_.begin(), precomputed_coefficients_.end(), quintic.equation.begin());
        
        for (int i = 0; i < 3; ++i)
        {
            quintic.equation[3 + i] += Dot(position, derivative_[i]) * inv_leading_coefficient_;
        }

        std::array<float, 5> realRoots;
        const int roots = solver->Solve(quintic, realRoots, kTolerance, 1.f - kTolerance);

        // Test the first control point.
        WorldSpace min_position = control_points_[0];
        float min_dist_sq = LengthSquared(position - min_position);

        // Test the roots.
        for (int i = 0; i < roots; ++i)
        {
            const WorldSpace root_position = EvaluateAt(realRoots[i]);
            const float root_dist_sq = LengthSquared(position - root_position);
            if (root_dist_sq < min_dist_sq)
            {
                min_dist_sq = root_dist_sq;
                min_position = root_position;
            }
        }

        // Test the last control point.
        const float dist_sq = LengthSquared(position - control_points_[3]);
        if (dist_sq < min_dist_sq)
        {
            min_dist_sq = dist_sq;
            min_position = control_points_[3];
        }

        closest = min_position;
        return min_dist_sq;
    }

    WorldSpace CubicBezierSpline::EvaluateAt(
        const float t) const
    {
        // The polynomial for is faster at evaluating than the parametric.
        return t*(polynomial_form_[2] + t*(polynomial_form_[1] + t*polynomial_form_[0])) + polynomial_form_[3];
    }
}

#ifdef USE_SIMD_OPTIMIZATION
#include "CubicSplineSimd.inl"
#endif // USE_SIMD_OPTIMIZATION
