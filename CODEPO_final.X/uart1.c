#include "uart1.h"
#include <time.h>


#define ABS(x) ((x) < 0 ? - (x) : (x))

float clock_factor_3 = 3685001.4;


void uart1Config(uint32_t baudrate, uint32_t fcy, uartModeType uartMode) {
	float tmp;

	if ((baudrate > 152) && (baudrate < 10E6)) {
		//U1MODEbits.UARTEN = 0;          // d�sactive le p�riph�rique avant de le configurer
		U1MODE = uartMode;              // sp�cifie le mode de fonctionnement
		tmp = fcy/(16.0*baudrate) - 1;    // on calcule la valeur exacte � placer dans le compteur pour obtenir le baudrate d�sir�
		U1MODEbits.BRGH = 0;            // on d�cide de toujours fonctionner en high speed (cela nous limite � 152 baud minimum)
		U1BRG = tmp + 0.5;              // on assigne la valeur calcul�e pour le baudrate, ce qui la tronque � l'entier inf�rieur

		tmp = fcy/(16*(float)(U1BRG + 1));
		if (ABS(tmp-baudrate) > baudrate/50) {
			U1MODEbits.BRGH = 1;				// on d�cide de toujours fonctionner en high speed (cela nous limite � 152 baud minimum)
			tmp = fcy/(4.0*baudrate) - 1;
			U1BRG = tmp + 0.5;			// on assigne la valeur calcul�e pour le baudrate, ce qui la tronque � l'entier inf�rieur
		}
//		U1MODEbits.UARTEN = 1;				// on active l'UART
//		U1STAbits.UTXEN = 1;				// on active l'�mission (Attention, cela active aussi U2TXIF)
	}
}


inline void uart1SendChar(unsigned char data) {
	while(U1STAbits.UTXBF);
	U1TXREG = data;
}


inline unsigned char uart1GetChar(void) {
	while(U1STAbits.URXDA == 0);
	return U1RXREG;
}

inline int16_t uart1RxDataAvailable(void) {
	return U1STAbits.URXDA; //dspic a un registre pour l'uart. Etat de la communication serial. Sert � dire quand il est pret� envoyer ou recevoir
}

inline int16_t uart1TxReady(void) {
	return !U1STAbits.UTXBF;
}


inline void uart1RxIsrDisable() {
	IEC0bits.U1RXIE = 0;
}

inline void uart1TxIsrDisable() {
	IEC0bits.U1TXIE = 0;
}

inline void uart1RxIsrEnable() {
	IEC0bits.U1RXIE = 1;
}

inline void uart1TxIsrEnable() {
	IFS0bits.U1TXIF = 0;
	IEC0bits.U1TXIE = 1;
}

