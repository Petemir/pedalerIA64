// https://dtosoftware.wordpress.com/2013/01/07/fast-sin-and-cos-functions/
#define M_PI 3.14159265358979323846264338327

 __m128 Abs(__m128 m)
{
__m128 sign = _mm_castsi128_ps(_mm_set1_epi32(0x80000000));
return _mm_andnot_ps(sign, m);
}
__m128 Sin(__m128 m_x)
{
const float B = 4.f / M_PI;
const float C = -4.f / (M_PI * M_PI);
const float P = 0.225f;
//float y = B * x + C * x * abs(x);
//y = P * (y * abs(y) - y) + y;
__m128 m_pi = _mm_set1_ps(M_PI);
__m128 m_mpi = _mm_set1_ps(-M_PI);
__m128 m_2pi = _mm_set1_ps(M_PI * 2);
__m128 m_B = _mm_set1_ps(B);
__m128 m_C = _mm_set1_ps(C);
__m128 m_P = _mm_set1_ps(P);
__m128 m1 =_mm_cmpnlt_ps(m_x, m_pi);
m1 = _mm_and_ps(m1, m_2pi);
m_x = _mm_sub_ps(m_x, m1);
m1 =_mm_cmpngt_ps(m_x, m_mpi);
m1 = _mm_and_ps(m1, m_2pi);
m_x = _mm_add_ps(m_x, m1);
__m128 m_abs = Abs(m_x);
m1 = _mm_mul_ps(m_abs, m_C);
m1 = _mm_add_ps(m1, m_B);
__m128 m_y = _mm_mul_ps(m1, m_x);
m_abs = Abs(m_y);
m1 = _mm_mul_ps(m_abs, m_y);
m1 = _mm_sub_ps(m1, m_y);
m1 = _mm_mul_ps(m1, m_P);
m_y = _mm_add_ps(m1, m_y);
return m_y;
}
