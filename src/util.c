#include <string.h>
#include "util.h"

// Retorna um representação inteira de uma string
int toInt(char *str) {

	int valor = 0;

	int i;
	for(i = 0; i < strlen(str); i++) {

		int n = (int) (str[i] - '0');

		int j;
		for(j = 0; j < strlen(str) - i - 1; j++)
			n *= 10;

		valor += n;		
	}

	return valor;
}

// Transforma o inteiro "n" em uma string, armazenada em "str"
void toString(int n, char *str) {

	int i = 0;
	while(n) {
		str[i++] = (char) (n % 10) + '0';
		n /= 10;
	}

	str[i] = '\0';

	int ult = strlen(str) - 1;
	for(i = 0; i <= ult / 2; i++) {
		char aux = str[i];
		str[i] = str[ult - i];
		str[ult - i] = aux;
	}
}