#include "arkanoPi_1.h" // Crear y usar la maquina de estados
#include <wiringPi.h>
#include <stdlib.h>


// Definicion estados
enum fsm_state {
	WAIT_START, WAIT_PUSH, WAIT_END
};

enum fsm_state2 {
	WAIT_COL1, WAIT_COL2, WAIT_COL3, WAIT_COL4, WAIT_COL5, WAIT_COL6, WAIT_COL7, WAIT_COL8, WAIT_COL9, WAIT_COL10
};

volatile int flags = 0;
volatile int flags_timer = 0;
int contador_col = 0;
tmr_t* myTimer;


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
	result = (flags & FLAG_TECLA);
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

int CompruebaTeclaRaquedaDerecha(fsm_t* this) {
	int result;

	piLock(FLAGS_KEY);
	result = (flags & FLAG_RAQUETA_DERECHA);
	piUnlock(FLAGS_KEY);

	return result;
}

int CompruebaTeclaPelota(fsm_t* this) {
	int result;

	piLock(FLAGS_KEY);
	result = (flags & FLAG_PELOTA);
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

int CompruebaTimer1(fsm_t* this) {
	int result;

	result = (flags_timer & FLAG_TIMER1);//Comprobar (flags & FLAG_TIMER1);

	return result;
}

//------------------------------------------------------
// FUNCIONES DEL TIMER
//------------------------------------------------------

//Acci�n a realizar cuando se arranca el timer

static void timer_isr (union sigval arg) {
	//piLock(FLAGS_KEY);
	//flags |= FLAG_TIMER1;
	//piUnlock(FLAGS_KEY);
	flags_timer |= FLAG_TIMER1;
	tmr_startms(myTimer,1);
	//OJO:CADA VEZ QUE SALTE EL TEMPORIZADOR SE LLAMAR� A ESTA FUNCI�N.
}

//------------------------------------------------------
// FUNCIONES DE ACCION
//------------------------------------------------------

// void InicializaJuego (void): funcion encargada de llevar a cabo
// la oportuna inicialización de toda variable o estructura de datos
// que resulte necesaria para el desarrollo del juego.
void InicializaJuego (fsm_t* fsm) {
	piLock (FLAGS_KEY);
	flags &= ~FLAG_TECLA;
	flags &= ~FLAG_PELOTA;
	flags &= ~FLAG_RAQUETA_IZQUIERDA;
	flags &= ~FLAG_RAQUETA_DERECHA;
	flags &= ~FLAG_FINAL_JUEGO;
	piUnlock (FLAGS_KEY);
	flags_timer &= ~FLAG_TIMER1;
	PintaLadrillos((tipo_pantalla*)(&(juego.arkanoPi.ladrillos)),(tipo_pantalla*)(&(juego.arkanoPi.pantalla)));
	PintaRaqueta((tipo_raqueta*)(&(juego.arkanoPi.raqueta)),(tipo_pantalla*)(&(juego.arkanoPi.pantalla)));
	PintaPelota((tipo_pelota*)(&(juego.arkanoPi.pelota)),(tipo_pantalla*)(&(juego.arkanoPi.pantalla)));
	//PintaPantallaPorTerminal(&(juego.arkanoPi.pantalla));
}

// void MueveRaquetaIzquierda (void): funcion encargada de ejecutar
// el movimiento hacia la izquierda contemplado para la raqueta.
// Debe garantizar la viabilidad del mismo mediante la comprobación
// de que la nueva posición correspondiente a la raqueta no suponga
// que ésta rebase o exceda los límites definidos para el área de juego
// (i.e. al menos uno de los leds que componen la raqueta debe permanecer
// visible durante todo el transcurso de la partida).
void MueveRaquetaIzquierda (fsm_t* fsm) {
	piLock(FLAGS_KEY);
	flags &= ~FLAG_RAQUETA_IZQUIERDA;
	piUnlock(FLAGS_KEY);
	if( juego.arkanoPi.raqueta.x<9 ){
		juego.arkanoPi.raqueta.x+=1;
	}
	ActualizaPantalla(&(juego.arkanoPi));
	//PintaPantallaPorTerminal(&(juego.arkanoPi.pantalla));
}

// void MueveRaquetaDerecha (void): función similar a la anterior
// encargada del movimiento hacia la derecha.
void MueveRaquetaDerecha (fsm_t* fsm) {
	piLock(FLAGS_KEY);
	flags &= ~FLAG_RAQUETA_DERECHA;
	piUnlock(FLAGS_KEY);
	if( juego.arkanoPi.raqueta.x>-2 ){
		juego.arkanoPi.raqueta.x -=1;
	}
	ActualizaPantalla(&(juego.arkanoPi));
	//PintaPantallaPorTerminal(&(juego.arkanoPi.pantalla));
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
void MovimientoPelota (fsm_t* fsm) {
	piLock(FLAGS_KEY);
	flags &= ~FLAG_PELOTA;
	piUnlock(FLAGS_KEY);

	switch(juego.arkanoPi.pelota.yv){
	case(-1):
																								switch(juego.arkanoPi.pelota.xv) {
																								case(1):// Diagonal arriba izquierda
																										if( HayLadrillo(juego.arkanoPi.pelota.x+1,juego.arkanoPi.pelota.y-1) == 1 ) { // Ladrillos
																											EliminaLadrillo(juego.arkanoPi.pelota.x+1,juego.arkanoPi.pelota.y-1);
																											juego.arkanoPi.pelota.yv*=-1;
																											piLock(FLAGS_KEY);
																											flags |= FLAG_PELOTA;
																											piUnlock(FLAGS_KEY);

																										}
																										else if( juego.arkanoPi.pelota.x == 9 ) { // Limite izquierda
																											juego.arkanoPi.pelota.xv*=-1;
																											piLock(FLAGS_KEY);
																											flags |= FLAG_PELOTA;
																											piUnlock(FLAGS_KEY);
																										}
																										else if( juego.arkanoPi.pelota.y == 0 ) { // Limite superior
																											juego.arkanoPi.pelota.yv*=-1;
																											piLock(FLAGS_KEY);
																											flags |= FLAG_PELOTA;
																											piUnlock(FLAGS_KEY);
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
																											piLock(FLAGS_KEY);
																											flags |= FLAG_PELOTA;
																											piUnlock(FLAGS_KEY);
																										}
																										else if( juego.arkanoPi.pelota.y == 0 ) { // Limite superior
																											juego.arkanoPi.pelota.yv*=-1;
																											piLock(FLAGS_KEY);
																											flags |= FLAG_PELOTA;
																											piUnlock(FLAGS_KEY);
																										}
																										else { // Ningun obstaculo
																											juego.arkanoPi.pelota.y--;
																										}
																								break;
																								case(-1):// Diagonal arriba derecha
																										if( HayLadrillo(juego.arkanoPi.pelota.x-1,juego.arkanoPi.pelota.y-1)==1 ) { // Ladrillos
																											EliminaLadrillo(juego.arkanoPi.pelota.x-1,juego.arkanoPi.pelota.y-1);
																											juego.arkanoPi.pelota.yv*=-1;
																											piLock(FLAGS_KEY);
																											flags |= FLAG_PELOTA;
																											piUnlock(FLAGS_KEY);
																										}
																										else if( juego.arkanoPi.pelota.x == 0 ) { // Limite derecha
																											juego.arkanoPi.pelota.xv*=-1;
																											piLock(FLAGS_KEY);
																											flags |= FLAG_PELOTA;
																											piUnlock(FLAGS_KEY);
																										}
																										else if( juego.arkanoPi.pelota.y == 0 ) { // Limite superior
																											juego.arkanoPi.pelota.yv*=-1;
																											piLock(FLAGS_KEY);
																											flags |= FLAG_PELOTA;
																											piUnlock(FLAGS_KEY);
																										}
																										else { // Ningun obstaculo
																											juego.arkanoPi.pelota.x--;
																											juego.arkanoPi.pelota.y--;
																										}
																								break;
																								}
	break;
																								case(1):
																								switch(juego.arkanoPi.pelota.xv) {
																								case(1):// Diagonal abajo izquierda
																										if( juego.arkanoPi.pelota.x == 9 ) { // Limite izquierda
																											juego.arkanoPi.pelota.xv*=-1;
																											piLock(FLAGS_KEY);
																											flags |= FLAG_PELOTA;
																											piUnlock(FLAGS_KEY);
																										}
																										else if( HayLadrillo(juego.arkanoPi.pelota.x+1,juego.arkanoPi.pelota.y+1)==1 ) { // Ladrillos
																											EliminaLadrillo(juego.arkanoPi.pelota.x+1,juego.arkanoPi.pelota.y+1);
																											juego.arkanoPi.pelota.yv*=-1;
																											piLock(FLAGS_KEY);
																											flags |= FLAG_PELOTA;
																											piUnlock(FLAGS_KEY);
																										}
																										else if( juego.arkanoPi.pelota.y+1 == 6 ) { // Toca raqueta
																											if( juego.arkanoPi.pelota.x+1 == juego.arkanoPi.raqueta.x ) {// Extremo derecho raqueta
																												juego.arkanoPi.pelota.yv = -1;
																												juego.arkanoPi.pelota.xv = -1;
																												piLock(FLAGS_KEY);
																												flags |= FLAG_PELOTA;
																												piUnlock(FLAGS_KEY);
																											}
																											else if( juego.arkanoPi.pelota.x+1 == juego.arkanoPi.raqueta.x+1 ) {// Centro raqueta
																												juego.arkanoPi.pelota.yv = -1;
																												juego.arkanoPi.pelota.xv = 0;
																												piLock(FLAGS_KEY);
																												flags |= FLAG_PELOTA;
																												piUnlock(FLAGS_KEY);
																											}
																											else if( juego.arkanoPi.pelota.x+1 == juego.arkanoPi.raqueta.x+2 ) {// Extremo izquierdo raqueta
																												juego.arkanoPi.pelota.yv = -1;
																												juego.arkanoPi.pelota.xv = 1;
																												piLock(FLAGS_KEY);
																												flags |= FLAG_PELOTA;
																												piUnlock(FLAGS_KEY);
																											}
																											else { //  Va a pasar limite inferior
																												juego.arkanoPi.pelota.y++;
																												juego.arkanoPi.pelota.x++;
																											}
																										}
																										else { // Ningun obstaculo
																											juego.arkanoPi.pelota.y++;
																											juego.arkanoPi.pelota.x++;
																										}
																								break;
																								case(0):// Vertical abajo
																										if( juego.arkanoPi.pelota.y+1 == 6 ) {// Toca raqueta
																											if( juego.arkanoPi.pelota.x == juego.arkanoPi.raqueta.x ) {// Extremo derecho raqueta
																												juego.arkanoPi.pelota.yv = -1;
																												juego.arkanoPi.pelota.xv = -1;
																												piLock(FLAGS_KEY);
																												flags |= FLAG_PELOTA;
																												piUnlock(FLAGS_KEY);
																											}
																											else if( juego.arkanoPi.pelota.x == juego.arkanoPi.raqueta.x+1 ) {// Centro raqueta
																												juego.arkanoPi.pelota.yv *= -1;
																												piLock(FLAGS_KEY);
																												flags |= FLAG_PELOTA;
																												piUnlock(FLAGS_KEY);
																											}
																											else if( juego.arkanoPi.pelota.x == juego.arkanoPi.raqueta.x+2 ) {// Extremo izquierdo raqueta
																												juego.arkanoPi.pelota.yv = -1;
																												juego.arkanoPi.pelota.xv = 1;
																												piLock(FLAGS_KEY);
																												flags |= FLAG_PELOTA;
																												piUnlock(FLAGS_KEY);
																											}
																											else { //  Va a pasar limite inferior
																												juego.arkanoPi.pelota.y++;
																											}
																										}
																										else { // Ningun obstaculo
																											juego.arkanoPi.pelota.y++;
																										}
																								break;
																								case(-1):// Diagonal abajo derecha
																										if( juego.arkanoPi.pelota.x == 0 ) { // Limite derecha
																											juego.arkanoPi.pelota.xv*=-1;
																											piLock(FLAGS_KEY);
																											flags |= FLAG_PELOTA;
																											piUnlock(FLAGS_KEY);
																										}
																										else if( juego.arkanoPi.pelota.y+1 == 6 ) { // Toca raqueta
																											if( juego.arkanoPi.pelota.x-1 == juego.arkanoPi.raqueta.x ) {// Extremo derecho raqueta
																												juego.arkanoPi.pelota.yv = -1;
																												juego.arkanoPi.pelota.xv = -1;
																												piLock(FLAGS_KEY);
																												flags |= FLAG_PELOTA;
																												piUnlock(FLAGS_KEY);
																											}
																											else if( juego.arkanoPi.pelota.x-1 == juego.arkanoPi.raqueta.x+1 ) {// Centro raqueta
																												juego.arkanoPi.pelota.yv = -1;
																												juego.arkanoPi.pelota.xv = 0;
																												piLock(FLAGS_KEY);
																												flags |= FLAG_PELOTA;
																												piUnlock(FLAGS_KEY);
																											}
																											else if( juego.arkanoPi.pelota.x-1 == juego.arkanoPi.raqueta.x+2 ) {// Extremo izquierdo raqueta
																												juego.arkanoPi.pelota.yv = -1;
																												juego.arkanoPi.pelota.xv = 1;
																												piLock(FLAGS_KEY);
																												flags |= FLAG_PELOTA;
																												piUnlock(FLAGS_KEY);
																											}
																											else { //  Va a pasar limite inferior
																												juego.arkanoPi.pelota.y++;
																												juego.arkanoPi.pelota.x--;
																											}
																										}
																										else { // Ningun obstaculo
																											juego.arkanoPi.pelota.y++;
																											juego.arkanoPi.pelota.x--;
																										}
																								break;
																								}
																								break;
	}
	ActualizaPantalla(&(juego.arkanoPi));
	if(CalculaLadrillosRestantes((tipo_pantalla*)(&(juego.arkanoPi.ladrillos))) == 0 || juego.arkanoPi.pelota.y == 6) {
		piLock(FLAGS_KEY);
		flags |= FLAG_FINAL_JUEGO;
		piUnlock(FLAGS_KEY);
	}
	//PintaPantallaPorTerminal(&(juego.arkanoPi.pantalla));

}

// void FinalJuego (void): función encargada de mostrar en la ventana de
// terminal los mensajes necesarios para informar acerca del resultado del juego.
void FinalJuego (fsm_t* fsm) {
	piLock(FLAGS_KEY);
	flags &= ~FLAG_FINAL_JUEGO;
	piUnlock(FLAGS_KEY);

	int result = 20-CalculaLadrillosRestantes((tipo_pantalla*)(&(juego.arkanoPi.ladrillos)));
	printf("RESULTADO:\n %d\n",result);

}

//void ReseteaJuego (void): función encargada de llevar a cabo la
// reinicialización de cuantas variables o estructuras resulten
// necesarias para dar comienzo a una nueva partida.
void ReseteaJuego (fsm_t* fsm) {
	piLock(FLAGS_KEY);
	flags &= ~FLAG_TECLA;
	piUnlock(FLAGS_KEY);

	InicializaArkanoPi(&(juego.arkanoPi));
	//PintaPantallaPorTerminal(&(juego.arkanoPi.pantalla));

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

void EnciendeLeds(fsm_t* fsm){
	//piLock(FLAGS_KEY);
	//flags &= ~FLAG_TIMER1;
	//piUnlock(FLAGS_KEY);
	flags_timer &= ~FLAG_TIMER1;
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

	int x = 0;
	int y = 0;
	piLock(STD_IO_BUFFER_KEY);

	// Lanzamos thread para explorar teclado
	x = piThreadCreate(thread_explora_teclado);
	y = piThreadCreate(thread_timer1);

	if(x!=0) {
		printf("it didn't start! (x) \n");
		piUnlock(STD_IO_BUFFER_KEY);
		return -1;
	}

	if(y!=0) {
		printf("it didn't start! (y) \n");
		piUnlock(STD_IO_BUFFER_KEY);
		return -1;
	}

	piUnlock(STD_IO_BUFFER_KEY);
	return 1;
}

void fsm_setup(fsm_t* arkano_fsm){
	piLock(FLAGS_KEY);
	flags=0;
	piUnlock(FLAGS_KEY);

	ReseteaJuego(arkano_fsm);
} 

void fsm_setup_leds(fsm_t* arkano_fsm) {
	//piLock(FLAGS_KEY);
	flags_timer=0;
	//piUnlock(FLAGS_KEY);
}

//------------------------------------------------------
// PI THREADs
//------------------------------------------------------
PI_THREAD(thread_explora_teclado) {
	int teclaPulsada;
	while (1) {
		delay(10); // wiringPi pauses program

		piLock(STD_IO_BUFFER_KEY);

		if(kbhit()) { // Funcion que detecta si se ha producido pulsacion de tecla alguna
			teclaPulsada = kbread(); // Funcion que devuelve la tecla pulsada

			switch(teclaPulsada) {
			case 'i': // Desplazamiento de la raqueta hacia la izquierda
				piLock(FLAGS_KEY);
				flags |= FLAG_RAQUETA_IZQUIERDA;
				piUnlock(FLAGS_KEY);
				break;

			case 'o': // Desplazamiento de la raqueta hacia la derecha
				piLock(FLAGS_KEY);
				flags |= FLAG_RAQUETA_DERECHA;
				piUnlock(FLAGS_KEY);
				break;

			case 'p': // Actualiza posicion de la pelota
				piLock(FLAGS_KEY);
				flags |= FLAG_PELOTA;
				piUnlock(FLAGS_KEY);
				break;

			case 'q':
				exit(0);
				break;

			default:
				piLock(FLAGS_KEY);
				flags |= FLAG_TECLA;
				piUnlock(FLAGS_KEY);
				break;
			}
		}

		piUnlock(STD_IO_BUFFER_KEY);
	}
}

PI_THREAD(thread_timer1) {
	fsm_trans_t state_tabla2[] = {
			{WAIT_COL1,CompruebaTimer1,WAIT_COL2,EnciendeLeds},
			{WAIT_COL2,CompruebaTimer1,WAIT_COL3,EnciendeLeds},
			{WAIT_COL3,CompruebaTimer1,WAIT_COL4,EnciendeLeds},
			{WAIT_COL4,CompruebaTimer1,WAIT_COL5,EnciendeLeds},
			{WAIT_COL5,CompruebaTimer1,WAIT_COL6,EnciendeLeds},
			{WAIT_COL6,CompruebaTimer1,WAIT_COL7,EnciendeLeds},
			{WAIT_COL7,CompruebaTimer1,WAIT_COL8,EnciendeLeds},
			{WAIT_COL8,CompruebaTimer1,WAIT_COL9,EnciendeLeds},
			{WAIT_COL9,CompruebaTimer1,WAIT_COL10,EnciendeLeds},
			{WAIT_COL10,CompruebaTimer1,WAIT_COL1,EnciendeLeds},
			{-1,NULL,-1,NULL},
	};
	fsm_t* leds_fsm = fsm_new (WAIT_COL1, state_tabla2, NULL);
	fsm_setup_leds(leds_fsm);
	myTimer = tmr_new(timer_isr);
	tmr_startms(myTimer, 1);
	while(1){
		fsm_fire(leds_fsm);
	}
	fsm_destroy(leds_fsm);
}

int main () {
	unsigned int next;

	// Maquina estados principal: transiciones
	// EstadoOrigen, FuncionEntrada,EstadoDestino,FuncionSalida}
	fsm_trans_t state_tabla[] = {
			{WAIT_START,CompruebaTeclaPulsada,WAIT_PUSH,InicializaJuego},
			{WAIT_PUSH,CompruebaTeclaPelota,WAIT_PUSH,MovimientoPelota},
			{WAIT_PUSH,CompruebaTeclaRaquedaDerecha,WAIT_PUSH,MueveRaquetaDerecha},
			{WAIT_PUSH,CompruebaTeclaRaquetaIzquierda,WAIT_PUSH,MueveRaquetaIzquierda},
			{WAIT_PUSH,CompruebaFinalJuego,WAIT_END,FinalJuego},
			{WAIT_END,CompruebaTeclaPulsada,WAIT_START,ReseteaJuego},
			{-1,NULL,-1,NULL},
	};

	fsm_t* arkano_fsm = fsm_new (WAIT_END, state_tabla, NULL);
	//Configuraci�n e inicalizaci�n del sistema
	systemSetup();
	fsm_setup(arkano_fsm);
	next=millis();
	while(1){
		fsm_fire(arkano_fsm);
		next+=CLK_MS;
		delay_until(next);
	}
	fsm_destroy(arkano_fsm);
}
