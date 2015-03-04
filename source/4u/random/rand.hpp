#ifndef RAND_HPP
#define RAND_HPP

/* Have own random generator */
template <typename T, typename ... TArgs>
class Rand
{
public:
	virtual T get(TArgs ... args) = 0;
};

#endif // RAND_HPP
