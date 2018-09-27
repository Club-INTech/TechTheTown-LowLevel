#include "utils.h"

/*
 * La fonction '%' du langage C ne renvoie pas le modulo au sens mathématique du terme.
 * En réalité pour (a % b) il fait ( ABS(a) % b)*SIGNE(a)
 * Voici donc pour votre plus grand plaisir, la fonction modulo mathématiquement CORRECTE.
 *
 * De rien, c'est gratuit.
 *
 * @author: Sylvain Gaultier
 * */

int modulo(int nombre, int modulo)
{
	if(modulo <= 0)
	{
		return -1;
	}
	else if(nombre >= 0 )
	{
		return nombre % modulo;
	}
	else
	{
		return modulo - ((-nombre) % modulo);
	}
}

/* Ce bout de code permet de gerer certaines exceptions de la std */
namespace std {
	void __throw_bad_alloc()
	{
		while (true)
		{
			Serial.println("ERROR\tUnable to allocate memory");
			delay(500);
		}
	}

	void __throw_length_error(char const*e)
	{
		while (true)
		{
			Serial.print("Length Error\t");
			Serial.println(e);
			delay(500);
		}
	}

	void __throw_out_of_range(char const*e)
	{
		while (true)
		{
			Serial.print("Out of range error\t");
			Serial.println(e);
			delay(500);
		}
	}

	void __throw_out_of_range_fmt(char const*e, ...)
	{
		while (true)
		{
			Serial.print("Out of range fmt\t");
			Serial.println(e);
			delay(500);
		}
	}
}