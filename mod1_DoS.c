/*-------------------------------------------
Autor: Francisco Javier Santos
Fecha: 10-02-2017
Descripción: Fichero que contiene la 
implementación del primer módulo centrado 
en los ataques DoS
-------------------------------------------*/

#include "func_aux_mod1.h"

/*----------------------------------------
	Función que implementa el primer 
	módulo

		Recibe: Nada

		Devuelve: Nada
----------------------------------------*/

void *mod1 (){

	
	//Estructura que almacenará los datos relativos al fichero
	struct CONTENIDO_FICHERO contenido_del_fichero;
	//Estructura que almacenará los datos relativos a las coincidencias
	struct ESTRUCTURA_REGLA contenido_fichero_alerta;
	//Variable donde almacenamos el fichero que queremos leer
	char *nombre_fichero = (char *)malloc(sizeof(char)*NUM_CARACTERES_PALABRA);
	//Variable auxiliar para almacenar el nombre del fichero que se crea
	char *nuevo_fichero = (char *)malloc(sizeof(char)*NUM_CARACTERES_PALABRA);
	//Variable auxiliar para conocer el número de líneas que leimos anteriormente
	int num_lineas_anterior;
	//Variable para conocer si hemos creado alguna regla
	bool regla_creada;
	

	//Inicializamos las variables
	strcpy(nombre_fichero, FICHERO_ALERT_SNORT);
	num_lineas_anterior = INICIO;
	regla_creada=false;
	
	
	while(true){	
			
		//Leemos el fichero
		contenido_del_fichero = lee_fichero(nombre_fichero);


		//Comprobamos si el número de líneas leídas está cerca del máximo (NUM_FRASES)
		if ( contenido_del_fichero.num_frases_fichero < LIMITE_LINEAS_LEIDAS){
			
			//Comprobamos que hemos leído una línea nueva
			if (contenido_del_fichero.num_frases_fichero > num_lineas_anterior){
				
				//Buscamos alguna alerta de DoS
				contenido_fichero_alerta = comprueba_Coincidencia_Fichero_Leido(contenido_del_fichero, "DoS", num_lineas_anterior);
					
				//Si hemos detectado alguna alerta nueva
				if (contenido_fichero_alerta.numero_lineas > INICIO){	
						
					//Creamos las reglas con las coincidencias que hayamos encontrado
					regla_creada = crea_y_escribe_regla(FICHERO_REGLAS_SNORT, contenido_fichero_alerta, "DOS");
						
					//Comprobamos si hemos escrito alguna regla	
					if (regla_creada==true){

						//Cambiamos el valor de la bandera
						regla_creada = false;

						//Recargamos Snort para hacer efectivo los cambios
						recarga_Snort();
					}
				}
			}
			
		}

		//En el caso en el que estemos cerca del límite
		else {
				
			//Llamamos a la función deseada para que nos vacie el fichero
			nuevo_fichero = vacia_fichero(nombre_fichero);
			/*--------------------------------------------------------
				Para asegurarnos que no perdemos información
				en ningún momento, volvemos a leer el fichero
				al que le hemos cambiado el nombre siempre y
				cuando no supere el número máximo (NUM_FRASES)
					
				Esta comprobación la hacemos por si el fichero 
				se encontraba en el rango entre LIMITE y NUM_FRASES
				y hay información que se pueda perder

			--------------------------------------------------------*/
			contenido_del_fichero = lee_fichero(nuevo_fichero);



			//Comprobamos que ese fichero no ha superado el límite
			if (contenido_del_fichero.num_frases_fichero < NUM_FRASES){
					
				//Buscamos alguna alerta de DoS
				contenido_fichero_alerta = comprueba_Coincidencia_Fichero_Leido(contenido_del_fichero, "DoS", num_lineas_anterior);
				
				//Si hemos detectano alguna alerta nueva
				if (contenido_fichero_alerta.numero_lineas > INICIO){
						
					//Creamos las reglas con las que hayamos encontrado
					regla_creada = crea_y_escribe_regla(FICHERO_REGLAS_SNORT, contenido_fichero_alerta, "DOS");
					
					//Comprobamos si hemos creado la regla
					if (regla_creada == true){
						
						//Cambiamos el valor de la bandera
						regla_creada = false;

						//Recargamos Snort
						recarga_Snort();
					}
				}
				
			}
			
			//Inicializamos el número de líneas leídas para la próxima iteración 
			contenido_del_fichero.num_frases_fichero = INICIO;
		}

		//Almacenamos el número de líneas leídas anteriormente
		num_lineas_anterior = contenido_del_fichero.num_frases_fichero;
		
		//Esperamos un tiempo para volver a leer el fichero
		sleep(INTERVALO_LECTURA);
	}
		
	//Liberamos la memoria
	free(nombre_fichero);
	free(nuevo_fichero);

	pthread_exit(NULL);
}

