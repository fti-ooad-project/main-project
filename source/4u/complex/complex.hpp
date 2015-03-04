#ifndef COMPLEX_HPP
#define COMPLEX_HPP

template <typename T>
class complex
{
private:
	T data[2];

public:
	complex()
	{

	}
	template <typename S>
	complex(const S &a)
	{
		data[0] = static_cast<T>(a);
		data[1] = static_cast<T>(0);
	}
	template <typename S>
	complex(const S &a, const S &b)
	{
		data[0] = static_cast<T>(a);
		data[1] = static_cast<T>(b);
	}
	template <typename S>
	complex(const complex<S> &c)
	{
		data[0] = static_cast<T>(c.data[0]);
		data[1] = static_cast<T>(c.data[1]);
	}

	inline T &re()
	{
		return data[0];
	}
	inline T &im()
	{
		return data[1];
	}
	inline T re() const
	{
		return data[0];
	}
	inline T im() const
	{
		return data[1];
	}
};

template <typename T>
inline constexpr complex<T> operator + (const complex<T> &c)
{
	return c;
}
template <typename T>
inline constexpr complex<T> operator - (const complex<T> &c)
{
	return complex<T>(-c.re(),-c.im());
}
template <typename T>
inline constexpr complex<T> operator + (const complex<T> &a, const complex<T> &b)
{
	return complex<T>(a.re() + b.re(), a.im() + b.im());
}
template <typename T>
inline constexpr complex<T> operator - (const complex<T> &a, const complex<T> &b)
{
	return a+(-b);
}
template <typename T, typename S>
inline constexpr complex<T> operator * (const S &a, const complex<T> &b)
{
	return complex<T>(static_cast<T>(a)*b.re(),static_cast<T>(a)*b.im());
}
template <typename T, typename S>
inline constexpr complex<T> operator * (const complex<T> &a, const S &b)
{
	return b*a;
}
template <typename T, typename S>
inline constexpr complex<T> operator / (const complex<T> &a, const S &b)
{
	return a*(static_cast<T>(1)/static_cast<T>(b));
}

/* Conjugation */
/* If conjugation operation isn't defined for type */
template <typename T>
inline constexpr T conj(const T &a)
{
	return a;
}
template <typename T>
inline constexpr complex<T> conj(const complex<T> &c)
{
	return complex<T>(conj(c.re()),-c.im());
}

/* Normalizing operation */
/* abs^2() is using instead of abs() because sqrt() is expensive operation */
template <typename T>
inline constexpr T abs2(const T &c)
{
	return c*conj(c);
}

/* Multiplication */
/* Not symmetric. Is not commutative if a != conj(a) */
template <typename T>
inline constexpr complex<T> operator * (const complex<T> &a, const complex<T> &b)
{
	return complex<T>(a.re()*b.re() - conj(b.im())*a.im(), b.im()*a.re() + a.im()*conj(b.re()));
}

/* Division */
template <typename T>
inline constexpr complex<T> operator / (const complex<T> &a, const complex<T> &b)
{
	a*conj(b)/abs2(b);
}

#endif // COMPLEX_HPP
