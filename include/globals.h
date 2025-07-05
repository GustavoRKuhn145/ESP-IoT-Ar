#ifndef GLOBALS_H
#define GLOBALS_H

extern unsigned long lastIrReceiveTime;
extern bool isDataReceived;
extern unsigned long currentMillis;

extern int mode; // 0 = modo normal: monitora o consumo; 1 = modo de envio: envia o comando de desligar; 2 = modo de cadastro: ativa o protocolo de cadastro
extern unsigned long waitStart;

#endif