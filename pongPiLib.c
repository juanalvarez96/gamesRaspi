#include "pongPiLib.h"

//------------------------------------------------------
// FUNCIONES DE INICIALIZACION / RESET
//------------------------------------------------------

void ReseteaMatrizPongPi(tipo_pantalla_pongPi *p_pantalla) {
	int i, j = 0;

	for(i=0;i<MATRIZ_ANCHO;i++) {
		for(j=0;j<MATRIZ_ALTO;j++) {
			p_pantalla->matriz[i][j] = 0;
		}
	}
}

void ReseteaPelotaPongPi(tipo_pelota_pongPi *p_pelota) {
	// Pelota inicialmente en el centro de la pantalla
	p_pelota->x = MATRIZ_ANCHO/2;
	p_pelota->y = MATRIZ_ALTO/2 - 1;

	// Trayectoria inicial
	p_pelota->yv = 0;
	p_pelota->xv = 1;
}

void ReseteaRaquetaDer(tipo_raqueta_pongPi *p_raqueta) {
	// Raqueta inicialmente en el centro de la pantalla
	p_raqueta->x = 0;
	p_raqueta->y = 3;
	p_raqueta->ancho = RAQUETA_ANCHO;
	p_raqueta->alto = RAQUETA_ALTO;
}

void ReseteaRaquetaIzq(tipo_raqueta_pongPi *p_raqueta) {
	// Raqueta inicialmente en el centro de la pantalla
	p_raqueta->x = 9;
	p_raqueta->y = 3;
	p_raqueta->ancho = RAQUETA_ANCHO;
	p_raqueta->alto = RAQUETA_ALTO;
}

//------------------------------------------------------
// FUNCIONES DE VISUALIZACION (ACTUALIZACION DEL OBJETO PANTALLA QUE LUEGO USARA EL DISPLAY)
//------------------------------------------------------

// void PintaMensajeInicialPantallaPongPi (...): metodo encargado de aprovechar
// el display para presentar un mensaje de bienvenida al usuario
void PintaMensajeInicialPantallaPongPi (tipo_pantalla_pongPi *p_pantalla, tipo_pantalla_pongPi *p_pantalla_inicial) {
	int msj[MATRIZ_ANCHO][MATRIZ_ALTO]={
				{0,0,0,0,0,0,0},
				{0,0,0,0,0,0,0},
				{0,1,1,0,1,0,0},
				{0,1,1,0,0,1,0},
				{0,0,0,0,0,1,0},
				{0,0,0,0,0,1,0},
				{0,1,1,0,0,1,0},
				{0,1,1,0,1,0,0},
				{0,0,0,0,0,0,0},
				{0,0,0,0,0,0,0},
		};

	int i, j=0;

	for(i=0;i<MATRIZ_ANCHO;i++) {
			for(j=0;j<MATRIZ_ALTO;j++) {
				p_pantalla->matriz[i][j] = msj[i][j];
			}
	}
}

// void PintaPantallaPorTerminalPongPi (...): metodo encargado de mostrar
// el contenido o la ocupaciÃ³n de la matriz de leds en la ventana de
// terminal o consola. Este mÃ©todo sera fundamental para facilitar
// la labor de depuraciÃ³n de errores (por ejemplo, en la programaciÃ³n
// de los diferentes movimientos tanto de la raqueta como de la pelota).
void PintaPantallaPorTerminalPongPi  (tipo_pantalla_pongPi *p_pantalla) {
	int i, j=0;
	printf("PANTALLA \n");
	for(i=0;i<MATRIZ_ALTO;i++) {
			for(j=9;j>=0;j--) {
				printf("%d  ",p_pantalla->matriz[j][i]);
			}
		printf("\n");
	}
}

// void PintaRaquetaPongPi(...): funcion encargada de â€œpintarâ€� la raqueta
// en su posicion correspondiente dentro del Ã¡rea de juego
void PintaRaquetaPongPi(tipo_raqueta_pongPi *p_raqueta, tipo_pantalla_pongPi *p_pantalla) {
	int i, j = 0;

	for(i=0;i<RAQUETA_ANCHO;i++) {
		for(j=0;j<RAQUETA_ALTO;j++) {
			if (( (p_raqueta->x+i >= 0) && (p_raqueta->x+i < MATRIZ_ANCHO) ) &&
					( (p_raqueta->y+j >= 0) && (p_raqueta->y+j < MATRIZ_ALTO) ))
				p_pantalla->matriz[p_raqueta->x+i][p_raqueta->y+j] = 1;
		}
	}
}

// void PintaPelotaPongPi(...): funcion encargada de â€œpintarâ€� la pelota
// en su posicion correspondiente dentro del Ã¡rea de juego
void PintaPelotaPongPi(tipo_pelota_pongPi *p_pelota, tipo_pantalla_pongPi *p_pantalla) {
	if( (p_pelota->x >= 0) && (p_pelota->x < MATRIZ_ANCHO) ) {
		if( (p_pelota->y >= 0) && (p_pelota->y < MATRIZ_ALTO) ) {
			p_pantalla->matriz[p_pelota->x][p_pelota->y] = 1;
		}
		else {
			printf("\n\nPROBLEMAS!!!! posicion y=%d de la pelota INVALIDA!!!\n\n", p_pelota->y);
			fflush(stdout);
		}
	}
	else {
		printf("\n\nPROBLEMAS!!!! posicion x=%d de la pelota INVALIDA!!!\n\n", p_pelota->x);
		fflush(stdout);
	}
}

// void ActualizaPantallaPongPi(...): metodo cuya ejecucion estara ligada a
// cualquiera de los movimientos de la raqueta o de la pelota y que
// sera el encargado de actualizar convenientemente la estructura de datos
// en memoria que representa el Ã¡rea de juego y su correspondiente estado.
void ActualizaPantallaPongPi(tipo_pongPi* p_pongPi) {
    // Borro toda la pantalla
	ReseteaMatrizPongPi((tipo_pantalla_pongPi*)(&(p_pongPi->pantalla)));
	// Pinto todo de nuevo
	PintaRaquetaPongPi((tipo_raqueta_pongPi*)(&(p_pongPi->raquetaIzq)),(tipo_pantalla_pongPi*)(&(p_pongPi->pantalla)));
	PintaRaquetaPongPi((tipo_raqueta_pongPi*)(&(p_pongPi->raquetaDer)),(tipo_pantalla_pongPi*)(&(p_pongPi->pantalla)));
	PintaPelotaPongPi((tipo_pelota_pongPi*)(&(p_pongPi->pelota)),(tipo_pantalla_pongPi*)(&(p_pongPi->pantalla)));
}

// void Inicializapong(...): metodo encargado de la inicializaciÃ³n
// de toda variable o estructura de datos especificamente ligada al
// desarrollo del juego y su visualizacion.
void InicializaPongPi(tipo_pongPi *p_pongPi) {
	ReseteaMatrizPongPi((tipo_pantalla_pongPi*)(&(p_pongPi->pantalla)));
	ReseteaPelotaPongPi((tipo_pelota_pongPi*)(&(p_pongPi->pelota)));
	ReseteaRaquetaIzq((tipo_raqueta_pongPi*)(&(p_pongPi->raquetaIzq)));
	ReseteaRaquetaDer((tipo_raqueta_pongPi*)(&(p_pongPi->raquetaDer)));

	PintaMensajeInicialPantallaPongPi((tipo_pantalla_pongPi*)&(p_pongPi->pantalla),(tipo_pantalla_pongPi*)&(p_pongPi->pantalla));
}
