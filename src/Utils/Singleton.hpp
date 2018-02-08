#ifndef SINGLETON_HPP
#define SINGLETON_HPP

#include <Arduino.h>

/**
 * La classe Singleton permet de crééer une instance de <T> si elle n'existe pas, ou de la récupérer si elle existe déjà
 * @tparam T
 */

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
