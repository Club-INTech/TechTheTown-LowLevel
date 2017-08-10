#ifndef SINGLETON_HPP
#define SINGLETON_HPP

template<class T>
class Singleton {
public:
	static T& Instance() {
		static T instance;
		return instance;
	}
protected:
	Singleton() {
	}
};

#endif
