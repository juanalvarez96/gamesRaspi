#ifndef _PONGPI_H_
#define _PONGPI_H_

#include <time.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>
#include <sys/time.h>
#include <wiringPi.h> // A descomentar en posteriores sesiones

#include "fsm.h" // crear y ejecutar la maquina de estados
#include "tmr.h" // crear y ejecutar timer

#include "arkanoPiLib.h"
#include "pongPiLib.h"

#define CLK_MS 10 // PERIODO ACTUALIZACION MAQUINA ESTADOS

typedef struct {
	tipo_arkanoPi arkanoPi;
	tipo_pongPi pongPi;
	char teclaPulsada;
} tipo_juego;

// FLAGS
#define FLAG_PELOTA_ARKANOPI 0x01
#define FLAG_PELOTA_PONGPI 0x02
#define FLAG_RAQUETA_IZQUIERDA 0x04
#define FLAG_RAQUETA_DERECHA 0x08
#define FLAG_RAQUETA_IZQUIERDA_1 0x10 // JUGADOR 1 = DER
#define FLAG_RAQUETA_DERECHA_1 0x20
#define FLAG_RAQUETA_IZQUIERDA_2 0x40 // JUGADOR 2 = IZQ
#define FLAG_RAQUETA_DERECHA_2 0x80
#define FLAG_FINAL_JUEGO 0x100
#define FLAG_PAUSA_JUEGO 0x200

// Lock/Unlock
#define FLAGS_KEY 1
#define STD_IO_BUFFER_KEY 2

//------------------------------------------------------
// FUNCIONES DE ENTRADA === TRANSICION
//------------------------------------------------------

int CompruebaTeclaPulsada(fsm_t* this);
int CompruebaTeclaRaquetaIzquierda(fsm_t* this);
int CompruebaTeclaRaquetaDerecha(fsm_t* this);
int CompruebaTeclaRaquetaIzquierda1(fsm_t* this);
int CompruebaTeclaRaquetaDerecha1(fsm_t* this);
int CompruebaTeclaRaquetaIzquierda2(fsm_t* this);
int CompruebaTeclaRaquetaDerecha2(fsm_t* this);
int CompruebaTeclaPelotaArkanoPi(fsm_t* this);
int CompruebaTeclaPelotaPongPi(fsm_t* this);
int CompruebaFinalJuego(fsm_t* this);
int CompruebaPausa(fsm_t* this);

//------------------------------------------------------
// FUNCIONES DE ACCION === SALIDA
//------------------------------------------------------

void InicializaJuego (void);
void MueveRaquetaIzquierda (void);
void MueveRaquetaDerecha (void);
void MueveRaquetaIzquierda1 (void);
void MueveRaquetaDerecha1 (void);
void MueveRaquetaIzquierda2 (void);
void MueveRaquetaDerecha2 (void);
void MovimientoPelotaArkanoPi (void);
void MovimientoPelotaPongPi (void);
void FinalJuego (void);
void ReseteaJuego (void);
void PausaJuego (void);

//------------------------------------------------------
// FUNCIONES DE INICIALIZACION
//------------------------------------------------------
int systemSetup (void);
void fsm_setup(fsm_t* fsm);

//------------------------------------------------------
// SUBRUTINAS DE ATENCION A LAS INTERRUPCIONES
//------------------------------------------------------
PI_THREAD(thread_pelota);

#endif /* PONGPI_H_ */
