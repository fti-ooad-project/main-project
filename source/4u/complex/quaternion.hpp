#ifndef QUATERNION_HPP
#define QUATERNION_HPP

#include"complex.hpp"

template <typename T>
class complex<complex<T>>
{
private:
	complex<T> data[2];

public:
	complex()
	{

	}
	template <typename S>
	complex(const S &a)
	{
		data[0] = static_cast<complex<T>>(a);
		data[1] = static_cast<complex<T>>(0);
	}
	template <typename S>
	complex(const S &a, const S &b)
	{
		data[0] = static_cast<complex<T>>(a);
		data[1] = static_cast<complex<T>>(b);
	}
	complex(const T &a, const T &b, const T &c, const T &d)
	{
		data[0] = complex<T>(a,b);
		data[1] = complex<T>(c,d);
	}
	template <typename S>
	complex(const complex<S> &c)
	{
		data[0] = static_cast<complex<T>>(c.data[0]);
		data[1] = static_cast<complex<T>>(c.data[1]);
	}

	inline T &r()
	{
		return data[0].re();
	}
	inline T &i()
	{
		return data[0].im();
	}
	inline T &j()
	{
		return data[1].re();
	}
	inline T &k()
	{
		return data[1].im();
	}

	inline complex<T> &re()
	{
		return data[0];
	}
	inline complex<T> &im()
	{
		return data[1];
	}
	inline complex<T> re() const
	{
		return data[0];
	}
	inline complex<T> im() const
	{
		return data[1];
	}
};

/*
template <typename T>
typedef complex<complex<T>> quaternion<T>;
*/

#endif // QUATERNION_HPP
