#ifndef OP_H
#define OP_H

template <typename T>
inline constexpr T _abs(const T &a)
{
	return (2*(a > 0) - 1)*a;
}

/* min max without conditional */
template <typename T>
inline constexpr T _min(const T &a, const T &b)
{
	return a + (b - a)*(a > b);
}
template <typename T>
inline constexpr T _max(const T &a, const T &b)
{
	return a + (b - a)*(a < b);
}

/* signum operation */
template <typename T>
inline constexpr int sgn(T arg)
{
	return static_cast<int>(static_cast<T>(0) < arg) - static_cast<int>(arg < static_cast<T>(0));
}
#define _sgn sgn

/*
 * Redefinitions of standard C/C++ operations
 * which aren't correct for negative values.
 */
template <typename T>
inline T _mod(T num, T den)
{
	const bool neg = num < 0;
	return ((num + neg) % den) + neg*(den - 1);
	// return (num<0)?(((num+1)%den)+den-1):(num%den);
}
template <typename T>
inline T _div(T num, T den)
{
	if(den<0){num=-num;den=-den;}
	const bool neg = num < 0;
	return ((num + neg)/den) - neg;
	// if(den<0){num=-num;den=-den;}
	// return (num<0)?(((num+1)/den)-1):(num/den);
}
template <typename T>
inline int divmod(T &num, const T &den)
{
	T ret = _div(num,den);
	num = _mod(num,den);
    return ret;
}
#define _divmod divmod

/* clamping operation */
template <typename T>
inline float clamp(T x)
{

	return x < static_cast<T>(0) ? static_cast<T>(0) : (x > static_cast<T>(1) ? static_cast<T>(1) : x);

}
#define _clamp clamp

template <typename T>
inline T _pow(T arg, int exp)
{
	if(exp > 0)
	{
		return _pow(arg,exp-1)*arg;
	}
	else if(exp == 0)
	{
		return static_cast<T>(1);
	}
	else
	{
		return _pow(static_cast<T>(1)/arg,exp);
	}
}

/* x^^2 operator */
template <typename T>
inline constexpr T sqr(T arg)
{
	return arg*arg;
}
#define _sqr sqr

#endif // OP_H
