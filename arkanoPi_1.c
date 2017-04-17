
#include "arkanoPi_1.h"

static volatile tipo_juego juego;

//------------------------------------------------------
// FUNCIONES DE ACCION
//------------------------------------------------------

// void InicializaJuego (void): funcion encargada de llevar a cabo
// la oportuna inicialización de toda variable o estructura de datos
// que resulte necesaria para el desarrollo del juego.
void InicializaJuego (void) {
	PintaLadrillos((tipo_pantalla*)(&(juego.arkanoPi.ladrillos)),(tipo_pantalla*)(&(juego.arkanoPi.pantalla)));
	PintaRaqueta((tipo_raqueta*)(&(juego.arkanoPi.raqueta)),(tipo_pantalla*)(&(juego.arkanoPi.pantalla)));
	PintaPelota((tipo_pelota*)(&(juego.arkanoPi.pelota)),(tipo_pantalla*)(&(juego.arkanoPi.pantalla)));
}

// void MueveRaquetaIzquierda (void): funcion encargada de ejecutar
// el movimiento hacia la izquierda contemplado para la raqueta.
// Debe garantizar la viabilidad del mismo mediante la comprobación
// de que la nueva posición correspondiente a la raqueta no suponga
// que ésta rebase o exceda los límites definidos para el área de juego
// (i.e. al menos uno de los leds que componen la raqueta debe permanecer
// visible durante todo el transcurso de la partida).
void MueveRaquetaIzquierda (void) {
	if( juego.arkanoPi.raqueta.x<9 ){
			juego.arkanoPi.raqueta.x+=1;
		}
	ActualizaPantalla(&(juego.arkanoPi));
}

// void MueveRaquetaDerecha (void): función similar a la anterior
// encargada del movimiento hacia la derecha.
void MueveRaquetaDerecha (void) {
	if( juego.arkanoPi.raqueta.x>-2 ){
			juego.arkanoPi.raqueta.x -=1;
		}
	ActualizaPantalla(&(juego.arkanoPi));
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
void MovimientoPelota (void) {

	switch(juego.arkanoPi.pelota.yv){
		case(-1):
				switch(juego.arkanoPi.pelota.xv) {
					case(1):// Diagonal arriba izquierda
						if( HayLadrillo(juego.arkanoPi.pelota.x+1,juego.arkanoPi.pelota.y-1) == 1 ) { // Ladrillos
							EliminaLadrillo(juego.arkanoPi.pelota.x+1,juego.arkanoPi.pelota.y-1);
							juego.arkanoPi.pelota.yv*=-1;
								}
						else if( juego.arkanoPi.pelota.x == 9 ) { // Limite izquierda
								juego.arkanoPi.pelota.xv*=-1;
						}
						else if( juego.arkanoPi.pelota.y == 0 ) { // Limite superior
								juego.arkanoPi.pelota.yv*=-1;
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
						}
						else if( juego.arkanoPi.pelota.y == 0 ) { // Limite superior
							juego.arkanoPi.pelota.yv*=-1;
						}
						else { // Ningun obstaculo
						juego.arkanoPi.pelota.y--;
						}
							break;
					case(-1):// Diagonal arriba derecha
						if( HayLadrillo(juego.arkanoPi.pelota.x-1,juego.arkanoPi.pelota.y-1)==1 ) { // Ladrillos
							EliminaLadrillo(juego.arkanoPi.pelota.x-1,juego.arkanoPi.pelota.y-1);
							juego.arkanoPi.pelota.yv*=-1;
						}
						else if( juego.arkanoPi.pelota.x == 0 ) { // Limite derecha
								juego.arkanoPi.pelota.xv*=-1;
						}
						else if( juego.arkanoPi.pelota.y == 0 ) { // Limite superior
								juego.arkanoPi.pelota.yv*=-1;
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
						}
						else if( juego.arkanoPi.pelota.y+1 == 6 ) { // Toca raqueta
							if( juego.arkanoPi.pelota.x+1 == juego.arkanoPi.raqueta.x ) {// Extremo derecho raqueta
									juego.arkanoPi.pelota.yv = -1;
									juego.arkanoPi.pelota.xv = -1;
							}
							else if( juego.arkanoPi.pelota.x+1 == juego.arkanoPi.raqueta.x+1 ) {// Centro raqueta
									juego.arkanoPi.pelota.yv = -1;
									juego.arkanoPi.pelota.xv = 0;
							}
							else if( juego.arkanoPi.pelota.x+1 == juego.arkanoPi.raqueta.x+2 ) {// Extremo izquierdo raqueta
									juego.arkanoPi.pelota.yv = -1;
									juego.arkanoPi.pelota.xv = 1;
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
							}
							else if( juego.arkanoPi.pelota.x == juego.arkanoPi.raqueta.x+1 ) {// Centro raqueta
								juego.arkanoPi.pelota.yv *= -1;
							}
							else if( juego.arkanoPi.pelota.x == juego.arkanoPi.raqueta.x+2 ) {// Extremo izquierdo raqueta
								juego.arkanoPi.pelota.yv = -1;
								juego.arkanoPi.pelota.xv = 1;
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
						if( juego.arkanoPi.pelota.x == 0 ) { // Limite izquierda
							juego.arkanoPi.pelota.xv*=-1;
						}
						else if( juego.arkanoPi.pelota.y+1 == 6 ) { // Toca raqueta
							if( juego.arkanoPi.pelota.x-1 == juego.arkanoPi.raqueta.x ) {// Extremo derecho raqueta
									juego.arkanoPi.pelota.yv = -1;
									juego.arkanoPi.pelota.xv = -1;
							}
							else if( juego.arkanoPi.pelota.x-1 == juego.arkanoPi.raqueta.x+1 ) {// Centro raqueta
									juego.arkanoPi.pelota.yv = -1;
									juego.arkanoPi.pelota.xv = 0;
							}
							else if( juego.arkanoPi.pelota.x-1 == juego.arkanoPi.raqueta.x+2 ) {// Extremo izquierdo raqueta
									juego.arkanoPi.pelota.yv = -1;
									juego.arkanoPi.pelota.xv = 1;
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
	if( juego.arkanoPi.pelota.y == 6 || CalculaLadrillosRestantes((tipo_pantalla*)(&(juego.arkanoPi.ladrillos))) == 0 ) {// Acaba partida
			juego.estado = WAIT_END;
		}
}

// void FinalJuego (void): función encargada de mostrar en la ventana de
// terminal los mensajes necesarios para informar acerca del resultado del juego.
void FinalJuego (void) {
	int result = 20-CalculaLadrillosRestantes((tipo_pantalla*)(&(juego.arkanoPi.ladrillos)));
	printf("RESULTADO:\n %d\n",result);
}

//void ReseteaJuego (void): función encargada de llevar a cabo la
// reinicialización de cuantas variables o estructuras resulten
// necesarias para dar comienzo a una nueva partida.
void ReseteaJuego (void) {
	InicializaArkanoPi(&(juego.arkanoPi));
	juego.estado = WAIT_START;
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
	// A completar por el alumno..

}

int main ()
{
	ReseteaJuego();
	 while (1) {
		 if(kbhit()) { // Funcion que detecta si se ha producido pulsacion de tecla alguna
			juego.teclaPulsada = kbread(); // Funcion que devuelve la tecla pulsada

			// Interpretacion de las pulsaciones para cada posible estado del sistema
			if( juego.estado == WAIT_START ) { // Cualquier pulsacion da comienzo al juego...
				InicializaJuego();
				juego.estado = WAIT_PUSH;
				PintaPantallaPorTerminal(&(juego.arkanoPi.pantalla));
				printf("DE START A PUSH \n");
			}
			else if( juego.estado == WAIT_END ) { // Cualquier nos devuelve al estado inicial...
				FinalJuego();
				if( juego.teclaPulsada=='i' || juego.teclaPulsada=='o' ) {
					ReseteaJuego();
					juego.estado = WAIT_START;
					PintaPantallaPorTerminal(&(juego.arkanoPi.pantalla));
					printf("DE END A START \n");
				}
				printf("NO SALGO DE END");
			}
			else { // Si estamos jugando...
				switch(juego.teclaPulsada) {
					case 'i': // Desplazamiento de la raqueta hacia la izquierda
						MueveRaquetaIzquierda();
						PintaPantallaPorTerminal(&(juego.arkanoPi.pantalla));
						printf("MUEVO IZQ \n");
						break;

					case 'o': // Desplazamiento de la raqueta hacia la derecha
						MueveRaquetaDerecha();
						PintaPantallaPorTerminal(&(juego.arkanoPi.pantalla));
						printf("MUEVO DER \n");
						break;

					case 'p': // Actualiza posicion de la pelota
						MovimientoPelota();
						PintaPantallaPorTerminal(&(juego.arkanoPi.pantalla));
						printf("MUEVO PELOTA \n");
						break;

					case 'q':
						exit(0);
						break;

					default:
						printf("INVALID KEY!!!\n");
						break;
				}
			}
		}
	}
}
