/**
 *  @file library/average.hpp
 *  @brief Ce fichier défini la classe Average permettant de calculer des moyennes sur les n précédentes valeurs d'une variable,
 *  		et d'actaliser efficacement cette moyenne à chaque ajout de valeur.
 *  @author Sylvain Gaultier
 *  @date 25/05/2015
 */

#ifndef AVERAGE_HPP
#define AVERAGE_HPP

/*
 * 	Limites d'utilisation : le type T doit représenter un nombre (int, float, double) et doit permettre
 * 	non seulement de contenir les valeurs de données mais aussi et surtout la somme de BUFFER_SIZE de ces
 * 	données. En gros, éviter le type int8_t avec BUFFER_SIZE grand ^.^
 */

template<typename T, unsigned int BUFFER_SIZE>
class Average
{
public:
	Average()
	{
		//buffer = (T*)malloc(sizeof(T)*BUFFER_SIZE);
		reset();
	}

	~Average()
	{
		//free(buffer);
	}

	void reset()
	{
		currentElement = 0;
		currentSum = 0;
		for(unsigned int i = 0; i < BUFFER_SIZE; i++)
		{
			buffer[i] = 0;
		}
	}

	void add(T newValue)
	{
		currentSum -= buffer[currentElement];
		buffer[currentElement] = newValue;
		currentSum += newValue;
		currentElement = (currentElement + 1) % BUFFER_SIZE;
	}

	T value()
	{
		return currentSum / (T)BUFFER_SIZE;
	}

private:
	T buffer[BUFFER_SIZE];
	unsigned int currentElement;
	T currentSum;
};

#endif // AVERAGE_HPP
