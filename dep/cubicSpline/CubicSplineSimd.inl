#pragma once

// Simd implementations of functions declared in CubicSpline.cpp.

#include <xmmintrin.h>
#include <smmintrin.h>

namespace CubicSplineTest
{    
    float Polynomial5::EvaluateNormedEquation(
        const float t) const
    {
        const __m128 c = _mm_setr_ps(t, equation[1], equation[2], equation[3]);
        __m128 mul = _mm_set1_ps(t);
        __m128 dst = _mm_mul_ps(c, mul); // 2, 1, 1, 1
        const float t2 = _mm_cvtss_f32(dst);
        mul = _mm_setr_ps(t2, t2, t, 1.f);
        dst = _mm_dp_ps(dst, mul, 241); // 4, 3, 2, 1 
        dst = _mm_setr_ps(_mm_cvtss_f32(dst), equation[4], equation[5], 0.f);
        mul = _mm_setr_ps(t, t, 1.f, 0.f);
        dst = _mm_dp_ps(dst, mul, 113);

        const float result = _mm_cvtss_f32(dst);

        return result;
    }

    int QuinticSolver::CountSturmSignChanges(
        const SturmSequence5& sturm_polynomials,
        const float t) const
    {
        const float *p5 = &sturm_polynomials[0];
        const float *p4 = &sturm_polynomials[6];
        const float *p3 = &sturm_polynomials[6 + 5];
        const float *p2 = &sturm_polynomials[6 + 5 + 4];

        // Compute the first 4 polynomials in the Sturm Sequence in parallel.
        __m128 c = _mm_setr_ps(*p5, *p4, *p3, *p2);
        __m128 mul = _mm_set1_ps(t);
        __m128 dst = _mm_mul_ps(c, mul); // 1
        c = _mm_setr_ps(*(++p5), *(++p4), *(++p3), *(++p2));
        dst = _mm_add_ps(c, dst);
        dst = _mm_mul_ps(dst, mul); // 2
        c = _mm_setr_ps(*(++p5), *(++p4), *(++p3), *(++p2));
        dst = _mm_add_ps(c, dst);
        mul = _mm_setr_ps(t, t, t, 1.f);
        dst = _mm_mul_ps(dst, mul); // 3
        c = _mm_setr_ps(*(++p5), *(++p4), *(++p3), 0.f);
        dst = _mm_add_ps(c, dst);
        mul = _mm_setr_ps(t, t, 1.f, 1.f);
        dst = _mm_mul_ps(dst, mul); // 4
        c = _mm_setr_ps(*(++p5), *(++p4), 0.f, 0.f);
        dst = _mm_add_ps(c, dst);
        mul = _mm_setr_ps(t, 1.f, 1.f, 1.f);
        dst = _mm_mul_ps(dst, mul); // 5
        alignas(16) float sturmResult[5];
        _mm_store_ps(&sturmResult[0], dst);
        sturmResult[0] += *(++p5);
        // Compute the fifth Sturm polynomial. (The sixth is just a constant.)
        sturmResult[4] = sturm_polynomials[6 + 5 + 4 + 3] * t + sturm_polynomials[6 + 5 + 4 + 3 + 1];

        int sign_changes = 0;
        union { float fl_val; uint32 n_val; } sign = { sturm_polynomials[ArithmeticSum(5 + 1) - 1] };
        uint32 previous = sign.n_val;
        for (int i = 4; i >= 0; --i)
        {
            sign.fl_val = sturmResult[i];
            sign_changes += (previous ^ sign.n_val) >> 31;
            previous = sign.n_val;
        }

        return sign_changes;
    }
}
