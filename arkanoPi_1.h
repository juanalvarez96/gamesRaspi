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



#include "fsm.h" // crear y ejecutar la maquina de estados
#include "tmr.h" // crear y ejecutar timer

#define CLK_MS 10 // PERIODO ACTUALIZACION MAQUINA ESTADOS

typedef struct {
	tipo_arkanoPi arkanoPi;
	/*tipo_estados_juego estado;*/
	char teclaPulsada;
} tipo_juego;

// FLAGS
#define FLAG_PELOTA 0x01
#define FLAG_RAQUETA_IZQUIERDA 0x02
#define FLAG_RAQUETA_DERECHA 0x04
#define FLAG_FINAL_JUEGO 0x8

// Lock/Unlock
#define FLAGS_KEY 1
#define STD_IO_BUFFER_KEY 2

//------------------------------------------------------
// FUNCIONES DE ENTRADA === TRANSICION
//------------------------------------------------------

int CompruebaTeclaPulsada(fsm_t* this);
int CompruebaTeclaRaquetaIzquierda(fsm_t* this);
int CompruebaTeclaRaquetaDerecha(fsm_t* this);
int CompruebaTeclaPelota(fsm_t* this);
int CompruebaFinalJuego(fsm_t* this);

//------------------------------------------------------
// FUNCIONES DE ACCION === SALIDA
//------------------------------------------------------

void InicializaJuego (void);
void MueveRaquetaIzquierda (void);
void MueveRaquetaDerecha (void);
void MovimientoPelota (void);
void FinalJuego (void);
void ReseteaJuego (void);

//------------------------------------------------------
// FUNCIONES DE INICIALIZACION
//------------------------------------------------------
int systemSetup (void);
void fsm_setup(fsm_t* arkano_fsm);

//------------------------------------------------------
// SUBRUTINAS DE ATENCION A LAS INTERRUPCIONES
//------------------------------------------------------
PI_THREAD(thread_pelota);

#endif /* ARKANOPI_H_ */
