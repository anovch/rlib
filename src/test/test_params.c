/*
 * test_params.c
 *
 *  Created on: 19 gru 2018
 *      Author: lukasz
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <utils/params.h>
#include "platform/storage.h"
#include "platform/mem.h"

const char* dificult_string = "new value \nwith new line and spetial char @#$%^";
const char* all_dificult_string = "key1=new value \nwith new line and spetial char @#$%^|key2=val2|";

void test_params() {
	Params* par = params_init();

	unsigned char pass = 1;
	// dodajemy dwa parametry
	params_set(par,"key1","val1");
	params_set(par,"key2","val2");

	if (strcmp(params_get(par, "key1"),"val1")!=0) {
		pass = 0;
	}
	if (strcmp(params_get(par, "key2"),"val2")!=0) {
		pass = 0;
	}

	// zmianiamy parametr
	params_set(par,"key1",dificult_string);
	if (strcmp(params_get(par, "key1"),dificult_string)!=0) {
		pass = 0;
	}

	// konwersja wszystkich parametrow do stringu
	const char* tmp = params_get_all(par);
	if (strcmp(tmp,all_dificult_string)!=0) {
		pass = 0;
	}
	port_free(tmp);

	// dodajemy jescze parametr
	params_set(par,"key3","val3");

	tmp = params_get_all(par);
	// zapisujemy do pliku
	store(0,tmp);
	port_free(tmp);

	tmp = load(0);
	// odczytujemy pliku
	params_set_all(par,tmp);
	port_free(tmp);

	// sprawdzamy
	if (strcmp(params_get(par, "key1"),dificult_string)!=0) {
		pass = 0;
	}
	if (strcmp(params_get(par, "key2"),"val2")!=0) {
		pass = 0;
	}
	if (strcmp(params_get(par, "key3"),"val3")!=0) {
		pass = 0;
	}







	if (pass) {
		printf("Parameters test PASS\n");
	}
	else {
		printf("Parameters test ERROR\n");
	}




	params_free(par);
}
