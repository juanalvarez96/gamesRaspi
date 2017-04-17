#ifndef _ARKANOPI_H_
#define _ARKANOPI_H_

#include <time.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>
#include <sys/time.h>
#include <wiringPi.h> // A descomentar en posteriores sesiones

#include "kbhit.h" // para poder detectar teclas pulsadas sin bloqueo y leer las teclas pulsadas
#include "arkanoPiLib.h"

/*typedef enum {
	WAIT_START,
	WAIT_PUSH,
	WAIT_END} tipo_estados_juego;*/

#include "fsm.h" // crear y ejecutar la maquina de estados

#define CLK_MS 10 // PERIODO ACTUALIZACION MAQUINA ESTADOS

typedef struct {
	tipo_arkanoPi arkanoPi;
	/*tipo_estados_juego estado;*/
	char teclaPulsada;
} tipo_juego;

// FLAGS
#define FLAG_TECLA 0x01
#define FLAG_PELOTA 0x02
#define FLAG_RAQUETA_IZQUIERDA 0x04
#define FLAG_RAQUETA_DERECHA 0x08
#define FLAG_FINAL_JUEGO 0x16
#define FLAG_TIMER1 0x32

// Lock/Unlock
#define FLAGS_KEY 1
#define STD_IO_BUFFER_KEY 2

//------------------------------------------------------
// FUNCIONES DE ENTRADA === TRANSICION
//------------------------------------------------------

int CompruebaTeclaPulsada(fsm_t* this);
int CompruebaTeclaRaquetaIzquierda(fsm_t* this);
int CompruebaTeclaRaquedaDerecha(fsm_t* this);
int CompruebaTeclaPelota(fsm_t* this);
int CompruebaFinalJuego(fsm_t* this);
int CompruebaTimer1(fsm_t* this);

//------------------------------------------------------
// FUNCIONES DE ACCION === SALIDA
//------------------------------------------------------

void InicializaJuego (fsm_t* fsm);
void MueveRaquetaIzquierda (fsm_t* fsm);
void MueveRaquetaDerecha (fsm_t* fsm);
void MovimientoPelota (fsm_t* fsm);
void FinalJuego (fsm_t* fsm);
void ReseteaJuego (fsm_t* fsm);
void EnciendeLeds(fsm_t* fsm); //Ojo igual el parámetro que le has pasado es otro aunque lo dudo mucho

//------------------------------------------------------
// FUNCIONES DE INICIALIZACION
//------------------------------------------------------
int systemSetup (void);


//------------------------------------------------------
// SUBRUTINAS DE ATENCION A LAS INTERRUPCIONES
//------------------------------------------------------
PI_THREAD(thread_explora_teclado);

#endif /* ARKANOPI_H_ */
