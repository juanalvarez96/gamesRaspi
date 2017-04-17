#include <wiringPi.h>
#include <stdlib.h>

#include "juegosPi.h"

//------------------------------------------------------
// FALTA POR HACER
//------------------------------------------------------
// Repasar conflictos .h
// Movimiento Pelota Pong
// Repasar flags
// Repasar Estados
// Mejorar LEDs
// Pantalla inicial
// Cambiar de juego


// Definicion estados
enum fsm_state {
	WAIT_START, WAIT_PUSH, WAIT_END
};

volatile int flags = 0;
int sel = 0;
int pausa = 0;
int contador_col = 0;
tmr_t* myTimerPelota;

static volatile tipo_juego juego;

// espera hasta activacion reloj
void delay_until(unsigned int next) {
	unsigned int now = millis();

	if(next > now) {
		delay(next - now);
	}
}

//------------------------------------------------------
// FUNCIONES DE LA MAQUINA DE ESTADOS
//------------------------------------------------------

int CompruebaTeclaPulsada(fsm_t* this) {
	int result;

	piLock(FLAGS_KEY);
	result = (flags & FLAG_RAQUETA_IZQUIERDA) || (flags & FLAG_RAQUETA_DERECHA) || (flags & FLAG_RAQUETA_IZQUIERDA_1)
			|| (flags & FLAG_RAQUETA_DERECHA_1) ||  (flags & FLAG_RAQUETA_IZQUIERDA_2) || (flags & FLAG_RAQUETA_DERECHA_2);
	piUnlock(FLAGS_KEY);

	return result;
}

int CompruebaTeclaRaquetaIzquierda(fsm_t* this) {
	int result;

	piLock(FLAGS_KEY);
	result = (flags & FLAG_RAQUETA_IZQUIERDA);
	piUnlock(FLAGS_KEY);

	return result;
}

int CompruebaTeclaRaquetaDerecha(fsm_t* this) {
	int result;

	piLock(FLAGS_KEY);
	result = (flags & FLAG_RAQUETA_DERECHA);
	piUnlock(FLAGS_KEY);

	return result;
}

int CompruebaTeclaRaquetaIzquierda1(fsm_t* this) {
	int result;

	piLock(FLAGS_KEY);
	result = (flags & FLAG_RAQUETA_IZQUIERDA_1);
	piUnlock(FLAGS_KEY);

	return result;
}

int CompruebaTeclaRaquetaDerecha1(fsm_t* this) {
	int result;

	piLock(FLAGS_KEY);
	result = (flags & FLAG_RAQUETA_DERECHA_1);
	piUnlock(FLAGS_KEY);

	return result;
}

int CompruebaTeclaRaquetaIzquierda2(fsm_t* this) {
	int result;

	piLock(FLAGS_KEY);
	result = (flags & FLAG_RAQUETA_IZQUIERDA_2);
	piUnlock(FLAGS_KEY);

	return result;
}

int CompruebaTeclaRaquetaDerecha2(fsm_t* this) {
	int result;

	piLock(FLAGS_KEY);
	result = (flags & FLAG_RAQUETA_DERECHA_2);
	piUnlock(FLAGS_KEY);

	return result;
}

int CompruebaTeclaPelotaArkano(fsm_t* this) {
	int result;

	piLock(FLAGS_KEY);
	result = (flags & FLAG_PELOTA_ARKANOPI);
	piUnlock(FLAGS_KEY);

	return result;
}

int CompruebaTeclaPelotaPongPi(fsm_t* this) {
	int result;

	piLock(FLAGS_KEY);
	result = (flags & FLAG_PELOTA_PONGPI);
	piUnlock(FLAGS_KEY);

	return result;
}

int CompruebaFinalJuego(fsm_t* this) {
	int result;

	piLock(FLAGS_KEY);
	result = (flags & FLAG_FINAL_JUEGO);
	piUnlock(FLAGS_KEY);

	return result;
}

int CompruebaPausa(fsm_t* this) {
	int result;

	piLock(FLAGS_KEY);
	result = (flags & FLAG_PAUSA_JUEGO);
	piUnlock(FLAGS_KEY);

	return result;
}

//------------------------------------------------------
// FUNCIONES DEL TIMER
//------------------------------------------------------

//Acci�n a realizar cuando se arranca el timer
static void timer_isr_pelota (union sigval arg) {
	piLock(FLAGS_KEY);
	if ( sel == 1) {
	flags |= FLAG_PELOTA_ARKANOPI;
	}
	else {
	flags |= FLAG_PELOTA_PONGPI;
	}
	piUnlock(FLAGS_KEY);
	tmr_startms(myTimerPelota,500);
}

//------------------------------------------------------
// FUNCIONES DE LOS PULSADORES
//------------------------------------------------------
static void boton_isr_left(void) {
	piLock(FLAGS_KEY);
	flags |= FLAG_RAQUETA_IZQUIERDA;
	piUnlock(FLAGS_KEY);
}

static void boton_isr_right(void) {
	piLock(FLAGS_KEY);
	flags |= FLAG_RAQUETA_DERECHA;
	piUnlock(FLAGS_KEY);
}

static void boton_isr_left_1(void) {
	piLock(FLAGS_KEY);
	flags |= FLAG_RAQUETA_IZQUIERDA_1;
	piUnlock(FLAGS_KEY);
}

static void boton_isr_right_1(void) {
	piLock(FLAGS_KEY);
	flags |= FLAG_RAQUETA_DERECHA_1;
	piUnlock(FLAGS_KEY);
}

static void boton_isr_left_2(void) {
	piLock(FLAGS_KEY);
	flags |= FLAG_RAQUETA_IZQUIERDA_2;
	piUnlock(FLAGS_KEY);
}

static void boton_isr_right_2(void) {
	piLock(FLAGS_KEY);
	flags |= FLAG_RAQUETA_DERECHA_2;
	piUnlock(FLAGS_KEY);
}

static void boton_isr_pausa(void) {	
	pausa++;
	piLock(FLAGS_KEY);
	flags |= FLAG_PAUSA_JUEGO;
	piUnlock(FLAGS_KEY);
}

static void boton_isr_quit(void){
	exit(0);
}
//------------------------------------------------------
// FUNCIONES DE ACCION
//------------------------------------------------------

void PausaJuego (void) {
	piLock (FLAGS_KEY);
	flags &= ~FLAG_PAUSA_JUEGO;
	while( pausa == 1 ) {
		sleep(1);
	}
	pausa = 0;
	piUnlock (FLAGS_KEY);
}

// void InicializaJuego (void): funcion encargada de llevar a cabo
// la oportuna inicialización de toda variable o estructura de datos
// que resulte necesaria para el desarrollo del juego.
void InicializaJuego (void) {
	piLock (FLAGS_KEY);
	flags &= ~FLAG_FINAL_JUEGO;
	flags &= ~FLAG_PELOTA_ARKANOPI;
	flags &= ~FLAG_PELOTA_PONGPI;
	flags &= ~FLAG_RAQUETA_IZQUIERDA;
	flags &= ~FLAG_RAQUETA_DERECHA;
	flags &= ~FLAG_RAQUETA_IZQUIERDA_1;
	flags &= ~FLAG_RAQUETA_DERECHA_1;
	flags &= ~FLAG_RAQUETA_IZQUIERDA_2;
	flags &= ~FLAG_RAQUETA_DERECHA_2;
	flags &= ~FLAG_FINAL_JUEGO;
	piUnlock (FLAGS_KEY);
	if ( sel==1 ) {
		PintaLadrillos((tipo_pantalla_arkanoPi*)(&(juego.arkanoPi.ladrillos)),(tipo_pantalla_arkanoPi*)(&(juego.arkanoPi.pantalla)));
		PintaRaqueta((tipo_raqueta_arkanoPi*)(&(juego.arkanoPi.raqueta)),(tipo_pantalla_arkanoPi*)(&(juego.arkanoPi.pantalla)));
		PintaPelota((tipo_pelota_arkanoPi*)(&(juego.arkanoPi.pelota)),(tipo_pantalla_arkanoPi*)(&(juego.arkanoPi.pantalla)));
	}
	else {
		PintaRaqueta((tipo_raqueta_pongPi*)(&(juego.pongPi.raquetaIzq)),(tipo_pantalla_pongPi*)(&(juego.pongPi.pantalla)));
		PintaRaqueta((tipo_raqueta_pongPi*)(&(juego.pongPi.raquetaDer)),(tipo_pantalla_pongPi*)(&(juego.pongPi.pantalla)));
		PintaPelota((tipo_pelota_pongPi*)(&(juego.pongPi.pelota)),(tipo_pantalla_pongPi*)(&(juego.pongPi.pantalla)));
	}
}

// void MueveRaquetaIzquierda (void): funcion encargada de ejecutar
// el movimiento hacia la izquierda contemplado para la raqueta.
// Debe garantizar la viabilidad del mismo mediante la comprobación
// de que la nueva posición correspondiente a la raqueta no suponga
// que ésta rebase o exceda los límites definidos para el área de juego
// (i.e. al menos uno de los leds que componen la raqueta debe permanecer
// visible durante todo el transcurso de la partida).
void MueveRaquetaIzquierda (void) {
	piLock(FLAGS_KEY);
	flags &= ~FLAG_RAQUETA_IZQUIERDA;
	piUnlock(FLAGS_KEY);
	if( juego.arkanoPi.raqueta.x<9 ){
		juego.arkanoPi.raqueta.x+=1;
	}
	ActualizaPantalla(&(juego.arkanoPi));
}

// void MueveRaquetaDerecha (void): función similar a la anterior
// encargada del movimiento hacia la derecha.
void MueveRaquetaDerecha (void) {
	piLock(FLAGS_KEY);
	flags &= ~FLAG_RAQUETA_DERECHA;
	piUnlock(FLAGS_KEY);
	if( juego.arkanoPi.raqueta.x>-2 ){
		juego.arkanoPi.raqueta.x -=1;
	}
	ActualizaPantalla(&(juego.arkanoPi));
}

// void MueveRaquetaIzquierda (void): funcion encargada de ejecutar
// el movimiento hacia la izquierda contemplado para la raqueta.
// Debe garantizar la viabilidad del mismo mediante la comprobación
// de que la nueva posición correspondiente a la raqueta no suponga
// que ésta rebase o exceda los límites definidos para el área de juego
// (i.e. al menos uno de los leds que componen la raqueta debe permanecer
// visible durante todo el transcurso de la partida).
void MueveRaquetaIzquierda1 (void) {
	piLock(FLAGS_KEY);
	flags &= ~FLAG_RAQUETA_IZQUIERDA_1;
	piUnlock(FLAGS_KEY);
	if( juego.pongPi.raquetaIzq.y<8 ){
		juego.pongPi.raquetaIzq.y+=1;
	}
	ActualizaPantalla(&(juego.pongPi));
}

// void MueveRaquetaDerecha (void): función similar a la anterior
// encargada del movimiento hacia la derecha.
void MueveRaquetaDerecha1 (void) {
	piLock(FLAGS_KEY);
	flags &= ~FLAG_RAQUETA_DERECHA_1;
	piUnlock(FLAGS_KEY);
	if( juego.pongPi.raquetaIzq.y>-2 ){
		juego.pongPi.raquetaIzq.y-=1;
	}
	ActualizaPantalla(&(juego.pongPi));
}

// void MueveRaquetaIzquierda (void): funcion encargada de ejecutar
// el movimiento hacia la izquierda contemplado para la raqueta.
// Debe garantizar la viabilidad del mismo mediante la comprobación
// de que la nueva posición correspondiente a la raqueta no suponga
// que ésta rebase o exceda los límites definidos para el área de juego
// (i.e. al menos uno de los leds que componen la raqueta debe permanecer
// visible durante todo el transcurso de la partida).
void MueveRaquetaIzquierda2 (void) {
	piLock(FLAGS_KEY);
	flags &= ~FLAG_RAQUETA_IZQUIERDA_2;
	piUnlock(FLAGS_KEY);
	if( juego.pongPi.raquetaDer.y>-2 ){
		juego.pongPi.raquetaDer.y-=1;
	}
	ActualizaPantalla(&(juego.pongPi));
}

// void MueveRaquetaDerecha (void): función similar a la anterior
// encargada del movimiento hacia la derecha.
void MueveRaquetaDerecha2 (void) {
	piLock(FLAGS_KEY);
	flags &= ~FLAG_RAQUETA_DERECHA_2;
	piUnlock(FLAGS_KEY);
	if( juego.pongPi.raquetaDer.y<8 ){
		juego.pongPi.raquetaDer.y+=1;
	}
	ActualizaPantalla(&(juego.pongPi));
}

int HayLadrillo(int x,int y) {
	if( juego.arkanoPi.ladrillos.matriz[x][y]==1 ) {
		return 1;
	}
	return 0;
}

void EliminaLadrillo(int x,int y) {
	juego.arkanoPi.ladrillos.matriz[x][y] = 0;
}

// void MovimientoPelota (void): función encargada de actualizar la
// posición de la pelota conforme a la trayectoria definida para ésta.
// Para ello deberá identificar los posibles rebotes de la pelota para,
// en ese caso, modificar su correspondiente trayectoria (los rebotes
// detectados contra alguno de los ladrillos implicarán adicionalmente
// la eliminación del ladrillo). Del mismo modo, deberá también
// identificar las situaciones en las que se dé por finalizada la partida:
// bien porque el jugador no consiga devolver la pelota, y por tanto ésta
// rebase el límite inferior del área de juego, bien porque se agoten
// los ladrillos visibles en el área de juego.
void MovimientoPelotaArkanoPi (void) {
	piLock(FLAGS_KEY);
	flags &= ~FLAG_PELOTA_ARKANOPI;
	piUnlock(FLAGS_KEY);

	if(juego.arkanoPi.pelota.yv == -1){
		switch(juego.arkanoPi.pelota.xv) {
		case(1):// Diagonal arriba izquierda
		if( HayLadrillo(juego.arkanoPi.pelota.x+1,juego.arkanoPi.pelota.y-1) == 1 ) { // Ladrillos
			EliminaLadrillo(juego.arkanoPi.pelota.x+1,juego.arkanoPi.pelota.y-1);
			juego.arkanoPi.pelota.yv*=-1;
			MovimientoPelotaArkanoPi();

		}
		else if( juego.arkanoPi.pelota.x == 9 ) { // Limite izquierda
			juego.arkanoPi.pelota.xv*=-1;
			MovimientoPelotaArkanoPi();
		}
		else if( juego.arkanoPi.pelota.y == 0 ) { // Limite superior
			juego.arkanoPi.pelota.yv*=-1;
			MovimientoPelotaArkanoPi();
		}
		else { // Ningun obstaculo
			juego.arkanoPi.pelota.x++;
			juego.arkanoPi.pelota.y--;
		}
		break;
		case(0):// Vertical arriba
		if( HayLadrillo(juego.arkanoPi.pelota.x,juego.arkanoPi.pelota.y-1)==1 ) { // Ladrillos
			EliminaLadrillo(juego.arkanoPi.pelota.x,juego.arkanoPi.pelota.y-1);
			juego.arkanoPi.pelota.yv*=-1;
			MovimientoPelotaArkanoPi();
		}
		else if( juego.arkanoPi.pelota.y == 0 ) { // Limite superior
			juego.arkanoPi.pelota.yv*=-1;
			MovimientoPelotaArkanoPi();
		}
		else { // Ningun obstaculo
			juego.arkanoPi.pelota.y--;
		}
		break;
		case(-1):// Diagonal arriba derecha
		if( HayLadrillo(juego.arkanoPi.pelota.x-1,juego.arkanoPi.pelota.y-1)==1 ) { // Ladrillos
			EliminaLadrillo(juego.arkanoPi.pelota.x-1,juego.arkanoPi.pelota.y-1);
			juego.arkanoPi.pelota.yv*=-1;
			MovimientoPelotaArkanoPi();
		}
		else if( juego.arkanoPi.pelota.x == 0 ) { // Limite derecha
			juego.arkanoPi.pelota.xv*=-1;
			MovimientoPelotaArkanoPi();
		}
		else if( juego.arkanoPi.pelota.y == 0 ) { // Limite superior
			juego.arkanoPi.pelota.yv*=-1;
			MovimientoPelotaArkanoPi();
		}
		else { // Ningun obstaculo
			juego.arkanoPi.pelota.x--;
			juego.arkanoPi.pelota.y--;
		}
		break;
		}
	}

	else {
		switch(juego.arkanoPi.pelota.xv) {
		case(1):// Diagonal abajo izquierda
		if( juego.arkanoPi.pelota.x == 9 ) { // Limite izquierda
			juego.arkanoPi.pelota.xv*=-1;
			MovimientoPelotaArkanoPi();
		}
		else if( juego.arkanoPi.pelota.y == 6) { //  Va a pasar limite inferior
			piLock(FLAGS_KEY);
			flags |= FLAG_FINAL_JUEGO;
			piUnlock(FLAGS_KEY);
			break;
		}
		else if( HayLadrillo(juego.arkanoPi.pelota.x+1,juego.arkanoPi.pelota.y+1)==1 ) { // Ladrillos
			EliminaLadrillo(juego.arkanoPi.pelota.x+1,juego.arkanoPi.pelota.y+1);
			juego.arkanoPi.pelota.yv*=-1;
			MovimientoPelotaArkanoPi();
		}
		else if( juego.arkanoPi.pelota.y+1 == 6 ) { // Toca raqueta
			if( juego.arkanoPi.pelota.x+1 == juego.arkanoPi.raqueta.x ) {// Extremo derecho raqueta
				juego.arkanoPi.pelota.yv = -1;
				juego.arkanoPi.pelota.xv = -1;
				MovimientoPelotaArkanoPi();
			}
			else if( juego.arkanoPi.pelota.x+1 == juego.arkanoPi.raqueta.x+1 ) {// Centro raqueta
				juego.arkanoPi.pelota.yv = -1;
				juego.arkanoPi.pelota.xv = 0;
				MovimientoPelotaArkanoPi();
			}
			else if( juego.arkanoPi.pelota.x+1 == juego.arkanoPi.raqueta.x+2 ) {// Extremo izquierdo raqueta
				juego.arkanoPi.pelota.yv = -1;
				juego.arkanoPi.pelota.xv = 1;
				MovimientoPelotaArkanoPi();
			}
			else {
				juego.arkanoPi.pelota.y++;
				juego.arkanoPi.pelota.x++;
				MovimientoPelotaArkanoPi();
			}
		}
		else { // Ningun obstaculo
			juego.arkanoPi.pelota.y++;
			juego.arkanoPi.pelota.x++;
		}
		break;
		case(0):// Vertical abajo
		if( juego.arkanoPi.pelota.y == 6) { //  Va a pasar limite inferior
				piLock(FLAGS_KEY);
				flags |= FLAG_FINAL_JUEGO;
				piUnlock(FLAGS_KEY);
				break;
		}
		else if( juego.arkanoPi.pelota.y+1 == 6 ) {// Toca raqueta
			if( juego.arkanoPi.pelota.x == juego.arkanoPi.raqueta.x ) {// Extremo derecho raqueta
				juego.arkanoPi.pelota.yv = -1;
				juego.arkanoPi.pelota.xv = -1;
				MovimientoPelotaArkanoPi();
			}
			else if( juego.arkanoPi.pelota.x == juego.arkanoPi.raqueta.x+1 ) {// Centro raqueta
				juego.arkanoPi.pelota.yv *= -1;
				MovimientoPelotaArkanoPi();
			}
			else if( juego.arkanoPi.pelota.x == juego.arkanoPi.raqueta.x+2 ) {// Extremo izquierdo raqueta
				juego.arkanoPi.pelota.yv = -1;
				juego.arkanoPi.pelota.xv = 1;
				MovimientoPelotaArkanoPi();
			}
			else {
				juego.arkanoPi.pelota.y++;
				MovimientoPelotaArkanoPi();
			}
		}
		else if( HayLadrillo(juego.arkanoPi.pelota.x,juego.arkanoPi.pelota.y+1)==1 ) { // Ladrillos
			EliminaLadrillo(juego.arkanoPi.pelota.x,juego.arkanoPi.pelota.y+1);
			juego.arkanoPi.pelota.yv*=-1;
			MovimientoPelotaArkanoPi();
		}
		else { // Ningun obstaculo
			juego.arkanoPi.pelota.y++;
		}
		break;
		case(-1):// Diagonal abajo derecha
		if( juego.arkanoPi.pelota.x == 0 ) { // Limite derecha
			juego.arkanoPi.pelota.xv*=-1;
			MovimientoPelotaArkanoPi();
		}
		else if( juego.arkanoPi.pelota.y == 6) { //  Va a pasar limite inferior
			piLock(FLAGS_KEY);
			flags |= FLAG_FINAL_JUEGO;
			piUnlock(FLAGS_KEY);
			break;
		}
		else if( juego.arkanoPi.pelota.y+1 == 6 ) { // Toca raqueta
			if( juego.arkanoPi.pelota.x-1 == juego.arkanoPi.raqueta.x ) {// Extremo derecho raqueta
				juego.arkanoPi.pelota.yv = -1;
				juego.arkanoPi.pelota.xv = -1;
				MovimientoPelotaArkanoPi();
			}
			else if( juego.arkanoPi.pelota.x-1 == juego.arkanoPi.raqueta.x+1 ) {// Centro raqueta
				juego.arkanoPi.pelota.yv = -1;
				juego.arkanoPi.pelota.xv = 0;
				MovimientoPelotaArkanoPi();
			}
			else if( juego.arkanoPi.pelota.x-1 == juego.arkanoPi.raqueta.x+2 ) {// Extremo izquierdo raqueta
				juego.arkanoPi.pelota.yv = -1;
				juego.arkanoPi.pelota.xv = 1;
				MovimientoPelotaArkanoPi();
			}
			else {
				juego.arkanoPi.pelota.y++;
				juego.arkanoPi.pelota.x--;
				MovimientoPelotaArkanoPi();
			}
		}
		else if( HayLadrillo(juego.arkanoPi.pelota.x-1,juego.arkanoPi.pelota.y+1)==1 ) { // Ladrillos
			EliminaLadrillo(juego.arkanoPi.pelota.x-1,juego.arkanoPi.pelota.y+1);
			juego.arkanoPi.pelota.yv*=-1;
			MovimientoPelotaArkanoPi();
		}
		else { // Ningun obstaculo
			juego.arkanoPi.pelota.y++;
			juego.arkanoPi.pelota.x--;
		}
		break;
		}
	}
	ActualizaPantalla(&(juego.arkanoPi));
	if(CalculaLadrillosRestantes((tipo_pantalla_arkanoPi*)(&(juego.arkanoPi.ladrillos))) == 0) {
		piLock(FLAGS_KEY);
		flags |= FLAG_FINAL_JUEGO;
		piUnlock(FLAGS_KEY);
	}
}

// void MovimientoPelota (void): función encargada de actualizar la
// posición de la pelota conforme a la trayectoria definida para ésta.
// Para ello deberá identificar los posibles rebotes de la pelota para,
// en ese caso, modificar su correspondiente trayectoria (los rebotes
// detectados contra alguno de los ladrillos implicarán adicionalmente
// la eliminación del ladrillo). Del mismo modo, deberá también
// identificar las situaciones en las que se dé por finalizada la partida:
// bien porque el jugador no consiga devolver la pelota, y por tanto ésta
// rebase el límite inferior del área de juego, bien porque se agoten
// los ladrillos visibles en el área de juego.
void MovimientoPelotaPongPi (void) {
	piLock(FLAGS_KEY);
	flags &= ~FLAG_PELOTA_PONGPI;
	piUnlock(FLAGS_KEY);

	if(juego.arkanoPi.pelota.yv == -1){
		switch(juego.arkanoPi.pelota.xv) {
		case(1):// Diagonal arriba izquierda
		else if( juego.arkanoPi.pelota.x == 9 ) { // Limite izquierda
			juego.arkanoPi.pelota.xv*=-1;
			MovimientoPelota();
		}
		else if( juego.arkanoPi.pelota.y == 0 ) { // Limite superior
			juego.arkanoPi.pelota.yv*=-1;
			MovimientoPelota();
		}
		else { // Ningun obstaculo
			juego.arkanoPi.pelota.x++;
			juego.arkanoPi.pelota.y--;
		}
		break;
		case(0):// Vertical arriba}
		else if( juego.arkanoPi.pelota.y == 0 ) { // Limite superior
			juego.arkanoPi.pelota.yv*=-1;
			MovimientoPelota();
		}
		else { // Ningun obstaculo
			juego.arkanoPi.pelota.y--;
		}
		break;
		case(-1):// Diagonal arriba derecha
		else if( juego.arkanoPi.pelota.x == 0 ) { // Limite derecha
			juego.arkanoPi.pelota.xv*=-1;
			MovimientoPelota();
		}
		else if( juego.arkanoPi.pelota.y == 0 ) { // Limite superior
			juego.arkanoPi.pelota.yv*=-1;
			MovimientoPelota();
		}
		else { // Ningun obstaculo
			juego.arkanoPi.pelota.x--;
			juego.arkanoPi.pelota.y--;
		}
		break;
		}
	}

	else {
		switch(juego.arkanoPi.pelota.xv) {
		case(1):// Diagonal abajo izquierda
		if( juego.arkanoPi.pelota.x == 9 ) { // Limite izquierda
			juego.arkanoPi.pelota.xv*=-1;
			MovimientoPelota();
		}
		else if( juego.arkanoPi.pelota.y == 6) { //  Va a pasar limite inferior
			piLock(FLAGS_KEY);
			flags |= FLAG_FINAL_JUEGO;
			piUnlock(FLAGS_KEY);
			break;
		}
		else if( HayLadrillo(juego.arkanoPi.pelota.x+1,juego.arkanoPi.pelota.y+1)==1 ) { // Ladrillos
			EliminaLadrillo(juego.arkanoPi.pelota.x+1,juego.arkanoPi.pelota.y+1);
			juego.arkanoPi.pelota.yv*=-1;
			MovimientoPelota();
		}
		else if( juego.arkanoPi.pelota.y+1 == 6 ) { // Toca raqueta
			if( juego.arkanoPi.pelota.x+1 == juego.arkanoPi.raqueta.x ) {// Extremo derecho raqueta
				juego.arkanoPi.pelota.yv = -1;
				juego.arkanoPi.pelota.xv = -1;
				MovimientoPelota();
			}
			else if( juego.arkanoPi.pelota.x+1 == juego.arkanoPi.raqueta.x+1 ) {// Centro raqueta
				juego.arkanoPi.pelota.yv = -1;
				juego.arkanoPi.pelota.xv = 0;
				MovimientoPelota();
			}
			else if( juego.arkanoPi.pelota.x+1 == juego.arkanoPi.raqueta.x+2 ) {// Extremo izquierdo raqueta
				juego.arkanoPi.pelota.yv = -1;
				juego.arkanoPi.pelota.xv = 1;
				MovimientoPelota();
			}
			else {
				juego.arkanoPi.pelota.y++;
				juego.arkanoPi.pelota.x++;
				MovimientoPelota();
			}
		}
		else { // Ningun obstaculo
			juego.arkanoPi.pelota.y++;
			juego.arkanoPi.pelota.x++;
		}
		break;
		case(0):// Vertical abajo
		if( juego.arkanoPi.pelota.y == 6) { //  Va a pasar limite inferior
				piLock(FLAGS_KEY);
				flags |= FLAG_FINAL_JUEGO;
				piUnlock(FLAGS_KEY);
				break;
		}
		else if( juego.arkanoPi.pelota.y+1 == 6 ) {// Toca raqueta
			if( juego.arkanoPi.pelota.x == juego.arkanoPi.raqueta.x ) {// Extremo derecho raqueta
				juego.arkanoPi.pelota.yv = -1;
				juego.arkanoPi.pelota.xv = -1;
				MovimientoPelota();
			}
			else if( juego.arkanoPi.pelota.x == juego.arkanoPi.raqueta.x+1 ) {// Centro raqueta
				juego.arkanoPi.pelota.yv *= -1;
				MovimientoPelota();
			}
			else if( juego.arkanoPi.pelota.x == juego.arkanoPi.raqueta.x+2 ) {// Extremo izquierdo raqueta
				juego.arkanoPi.pelota.yv = -1;
				juego.arkanoPi.pelota.xv = 1;
				MovimientoPelota();
			}
			else {
				juego.arkanoPi.pelota.y++;
				MovimientoPelota();
			}
		}
		else if( HayLadrillo(juego.arkanoPi.pelota.x,juego.arkanoPi.pelota.y+1)==1 ) { // Ladrillos
			EliminaLadrillo(juego.arkanoPi.pelota.x,juego.arkanoPi.pelota.y+1);
			juego.arkanoPi.pelota.yv*=-1;
			MovimientoPelota();
		}
		else { // Ningun obstaculo
			juego.arkanoPi.pelota.y++;
		}
		break;
		case(-1):// Diagonal abajo derecha
		if( juego.arkanoPi.pelota.x == 0 ) { // Limite derecha
			juego.arkanoPi.pelota.xv*=-1;
			MovimientoPelota();
		}
		else if( juego.arkanoPi.pelota.y == 6) { //  Va a pasar limite inferior
			piLock(FLAGS_KEY);
			flags |= FLAG_FINAL_JUEGO;
			piUnlock(FLAGS_KEY);
			break;
		}
		else if( juego.arkanoPi.pelota.y+1 == 6 ) { // Toca raqueta
			if( juego.arkanoPi.pelota.x-1 == juego.arkanoPi.raqueta.x ) {// Extremo derecho raqueta
				juego.arkanoPi.pelota.yv = -1;
				juego.arkanoPi.pelota.xv = -1;
				MovimientoPelota();
			}
			else if( juego.arkanoPi.pelota.x-1 == juego.arkanoPi.raqueta.x+1 ) {// Centro raqueta
				juego.arkanoPi.pelota.yv = -1;
				juego.arkanoPi.pelota.xv = 0;
				MovimientoPelota();
			}
			else if( juego.arkanoPi.pelota.x-1 == juego.arkanoPi.raqueta.x+2 ) {// Extremo izquierdo raqueta
				juego.arkanoPi.pelota.yv = -1;
				juego.arkanoPi.pelota.xv = 1;
				MovimientoPelota();
			}
			else {
				juego.arkanoPi.pelota.y++;
				juego.arkanoPi.pelota.x--;
				MovimientoPelota();
			}
		}
		else { // Ningun obstaculo
			juego.arkanoPi.pelota.y++;
			juego.arkanoPi.pelota.x--;
		}
		break;
		}
	}
	ActualizaPantalla(&(juego.arkanoPi));
}

// void FinalJuego (void): función encargada de mostrar en la ventana de
// terminal los mensajes necesarios para informar acerca del resultado del juego.
void FinalJuego (void) {
	piLock(FLAGS_KEY);
	flags &= ~FLAG_FINAL_JUEGO;
	piUnlock(FLAGS_KEY);
	int result = 20-CalculaLadrillosRestantes((tipo_pantalla_arkanoPi*)(&(juego.arkanoPi.ladrillos)));
	printf("RESULTADO:\n %d\n",result);
}

//void ReseteaJuego (void): función encargada de llevar a cabo la
// reinicialización de cuantas variables o estructuras resulten
// necesarias para dar comienzo a una nueva partida.
void ReseteaJuego (void) {
	piLock(FLAGS_KEY);
	flags &= ~FLAG_RAQUETA_IZQUIERDA;
	flags &= ~FLAG_RAQUETA_DERECHA;
	piUnlock(FLAGS_KEY);
	if ( sel == 1 ) {
		InicializaArkanoPi(&(juego.arkanoPi));
	}
	else {
		InicializaPongPi(&(juego.pongPi));
	}
}

void IluminaFilas(){
	int j;
	for (j=0;j<7;j++){
		switch (j){
		case 0:
			if(juego.arkanoPi.pantalla.matriz[9-contador_col][j]==1){
				digitalWrite(0,LOW);


			}
			break;
		case 1:
			if(juego.arkanoPi.pantalla.matriz[9-contador_col][j]==1){
				digitalWrite(1,LOW);

			}
			break;
		case 2:
			if(juego.arkanoPi.pantalla.matriz[9-contador_col][j]==1){
				digitalWrite(2,LOW);

			}
			break;
		case 3:
			if(juego.arkanoPi.pantalla.matriz[9-contador_col][j]==1){
				digitalWrite(3,LOW);

			}
			break;
		case 4:
			if(juego.arkanoPi.pantalla.matriz[9-contador_col][j]==1){
				digitalWrite(4,LOW);

			}
			break;
		case 5:
			if(juego.arkanoPi.pantalla.matriz[9-contador_col][j]==1){
				digitalWrite(7,LOW);

			}
			break;
		case 6:
			if(juego.arkanoPi.pantalla.matriz[9-contador_col][j]==1){
				digitalWrite(23,LOW);

			}
			break;
		default:

			printf("ERROR\n");
		}
	}
}

void EnciendeLeds(){
	//Ponemos todo a cero
	digitalWrite(0,HIGH); // FILAS
	digitalWrite(1,HIGH);
	digitalWrite(2,HIGH);
	digitalWrite(3,HIGH);
	digitalWrite(4,HIGH);
	digitalWrite(7,HIGH);
	digitalWrite(23,HIGH);
	digitalWrite(14,LOW); // COLUMNAS
	digitalWrite(17,LOW);
	digitalWrite(18,LOW);
	digitalWrite(22,LOW);
	switch (contador_col){
	case (0):
	digitalWrite(14,LOW);
	digitalWrite(17,LOW);
	digitalWrite(18,LOW);
	digitalWrite(22,LOW);
	break;
	case(1):
																	digitalWrite(14,HIGH);
	digitalWrite(17,LOW);
	digitalWrite(18,LOW);
	digitalWrite(22,LOW);
	break;
	case(2):
																	digitalWrite(14,LOW);
	digitalWrite(17,HIGH);
	digitalWrite(18,LOW);
	digitalWrite(22,LOW);
	break;
	case (3):
																	digitalWrite(14,HIGH);
	digitalWrite(17,HIGH);
	digitalWrite(18,LOW);
	digitalWrite(22,LOW);
	break;
	case (4):
																	digitalWrite(14,LOW);
	digitalWrite(17,LOW);
	digitalWrite(18,HIGH);
	digitalWrite(22,LOW);
	break;
	case (5):
																	digitalWrite(14,HIGH);
	digitalWrite(17,LOW);
	digitalWrite(18,HIGH);
	digitalWrite(22,LOW);
	break;
	case (6):
																	digitalWrite(14,LOW);
	digitalWrite(17,HIGH);
	digitalWrite(18,HIGH);
	digitalWrite(22,LOW);
	break;
	case (7):
																	digitalWrite(14,HIGH);
	digitalWrite(17,HIGH);
	digitalWrite(18,HIGH);
	digitalWrite(22,LOW);
	break;
	case (8):
																	digitalWrite(14,LOW);
	digitalWrite(17,LOW);
	digitalWrite(18,LOW);
	digitalWrite(22,HIGH);
	break;
	case (9):
																	digitalWrite(14,HIGH);
	digitalWrite(17,LOW);
	digitalWrite(18,LOW);
	digitalWrite(22,HIGH);
	break;
	default:
		printf("ERROR\n");
		break;
	}
	IluminaFilas();

	if(contador_col<9){
		contador_col++;
	}
	else{
		contador_col=0;
	}
}

//------------------------------------------------------
// FUNCIONES DE INICIALIZACION
//------------------------------------------------------

// int systemSetup (void): procedimiento de configuracion del sistema.
// Realizará, entra otras, todas las operaciones necesarias para:
// configurar el uso de posibles librerías (e.g. Wiring Pi),
// configurar las interrupciones externas asociadas a los pines GPIO,
// configurar las interrupciones periódicas y sus correspondientes temporizadores,
// crear, si fuese necesario, los threads adicionales que pueda requerir el sistema
int systemSetup (void) {
	// Setting up the wiringPi library
	if(wiringPiSetupGpio() < 0) {
		printf("Unable to setup wiringPi\n");
		piUnlock(STD_IO_BUFFER_KEY);
		return -1;
	}
	pinMode(0,OUTPUT); // FILAS
	pinMode(1,OUTPUT);
	pinMode(2,OUTPUT);
	pinMode(3,OUTPUT);
	pinMode(4,OUTPUT);
	pinMode(7,OUTPUT);
	pinMode(23,OUTPUT);

	pinMode(14,OUTPUT); // COLUMNAS
	pinMode(17,OUTPUT);
	pinMode(18,OUTPUT);
	pinMode(22,OUTPUT);

	pinMode(5,INPUT); //PULSADORES
	pinMode(6,INPUT);
	pinMode(12,INPUT);
	pinMode(13,INPUT);
	pinMode(16,INPUT);
	pinMode(19,INPUT);
	pinMode(20,INPUT);
	pinMode(21,INPUT);
	pullUpDnControl(5, PUD_DOWN);
	pullUpDnControl(6, PUD_DOWN);
	pullUpDnControl(12, PUD_DOWN);
	pullUpDnControl(13, PUD_DOWN);
	pullUpDnControl(16, PUD_DOWN);
	pullUpDnControl(19, PUD_DOWN);
	pullUpDnControl(20, PUD_DOWN);
	pullUpDnControl(21, PUD_DOWN);
	wiringPiISR(5,INT_EDGE_FALLING,boton_isr_right);
	wiringPiISR(6,INT_EDGE_FALLING,boton_isr_left);
	wiringPiISR(12, INT_EDGE_FALLING,boton_isr_right_1);
	wiringPiISR(13, INT_EDGE_FALLING,boton_isr_left_1);
	wiringPiISR(16, INT_EDGE_FALLING,boton_isr_right_2);
	wiringPiISR(19, INT_EDGE_FALLING,boton_isr_left_2);
	wiringPiISR(20, INT_EDGE_FALLING,boton_isr_pausa);
	wiringPiISR(21, INT_EDGE_FALLING,boton_isr_quit);

	myTimerPelota = tmr_new(timer_isr_pelota);
	tmr_startms(myTimerPelota, 500);

	int y = 0;
	piLock(STD_IO_BUFFER_KEY);
	y = piThreadCreate(thread_leds);

	if(y!=0) {
		printf("it didn't start! (y) \n");
		piUnlock(STD_IO_BUFFER_KEY);
		return -1;
	}
	piUnlock(STD_IO_BUFFER_KEY);
	return 1;
}

void fsm_setup(fsm_t* fsm){
	piLock(FLAGS_KEY);
	flags=0;
	piUnlock(FLAGS_KEY);

	ReseteaJuego();
} 

//------------------------------------------------------
// PI THREADs
//------------------------------------------------------

PI_THREAD(thread_leds) {
	while(1){
		EnciendeLeds();
		delay(1);
		}
}

int main () {
	unsigned int next;
	systemSetup();
	while( sel == 0 ) {
		sleep(1);
	}
	// Maquina estados principal: transiciones
	// EstadoOrigen, FuncionEntrada,EstadoDestino,FuncionSalida;

	fsm_trans_t state_tabla[] = {
			{WAIT_START,CompruebaTeclaPulsada,WAIT_PUSH,InicializaJuego},
			{WAIT_PUSH,CompruebaFinalJuego,WAIT_END,FinalJuego},
			{WAIT_PUSH,CompruebaTeclaPelotaArkano,WAIT_PUSH,MovimientoPelotaArkanoPi},
			{WAIT_PUSH,CompruebaTeclaPelotaPongPi,WAIT_PUSH,MovimientoPelotaPongPi},
			{WAIT_PUSH,CompruebaTeclaRaquetaDerecha,WAIT_PUSH,MueveRaquetaDerecha},
			{WAIT_PUSH,CompruebaTeclaRaquetaIzquierda,WAIT_PUSH,MueveRaquetaIzquierda},
			{WAIT_PUSH,CompruebaTeclaRaquetaDerecha1,WAIT_PUSH,MueveRaquetaDerecha1},
			{WAIT_PUSH,CompruebaTeclaRaquetaIzquierda1,WAIT_PUSH,MueveRaquetaIzquierda1},
			{WAIT_PUSH,CompruebaTeclaRaquetaDerecha2,WAIT_PUSH,MueveRaquetaDerecha2},
			{WAIT_PUSH,CompruebaTeclaRaquetaIzquierda2,WAIT_PUSH,MueveRaquetaIzquierda2},
			{WAIT_PUSH,CompruebaPausa,WAIT_PUSH,PausaJuego},
			{WAIT_END,CompruebaTeclaPulsada,WAIT_START,ReseteaJuego},
			{-1,NULL,-1,NULL},
	};

	//Configuraci�n e inicalizaci�n del sistema
	fsm_t* fsm = fsm_new (WAIT_END, state_tabla, NULL);
	fsm_setup(fsm);
	next=millis();
	while(1){
		fsm_fire(fsm);
		next+=CLK_MS;
		delay_until(next);
	}
	fsm_destroy(fsm);
}
