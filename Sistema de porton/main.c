#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <time.h>
#include <unistd.h>

// Definición de constantes para los estados del sistema
#define MICRO ESP32
#define ESTADO_INICIAL 0
#define ESTADO_CERRANDO 1
#define ESTADO_ABRIENDO 2
#define ESTADO_CERRADO 3
#define ESTADO_ABIERTO 4
#define ESTADO_ERR 5
#define ESTADO_STOP 6

// Definiciones de códigos de error
#define ERR_OK 0
#define ERR_OT 1
#define ERR_LSW 2

//funciones de cada estado
int Func_ESTADO_INICIAL(void);
int Func_ESTADO_CERRANDO(void);
int Func_ESTADO_ABRIENDO(void);
int Func_ESTADO_CERRADO(void);
int Func_ESTADO_ABIERTO(void);
int Func_ESTADO_ERR(void);
int Func_ESTADO_STOP(void);

//variables globales de estado
int EstadoSiguiente = ESTADO_INICIAL;
int EstadoActual = ESTADO_INICIAL;
int EstadoAnterior = ESTADO_INICIAL;

// estado de lámpara intermitente
bool lampState = false;

// Estructura de entradas y salidas del sistema
struct IO
{
    unsigned int LSC:1; //entrada limitswitch de puerta cerrada
    unsigned int LSA:1; //entrada limitswitch de puerta abierta
    unsigned int BA:1;  //Boton abrir
    unsigned int BC:1;  //Boton cerrar
    unsigned int SE:1;  //Entrada de Stop Emergency
    unsigned int MA:1;  //Salida motor direccion abrir
    unsigned int MC:1;  //Salida motor direccion cerrar
    unsigned int lampara:1; //estado de la lámpara (1 = encendida, 0 = apagada)
    unsigned int buzzer:1; // estado del buzzer (1 = encendido, 0 = apagado)
    unsigned int PP:1;   // Botón Push-Push para abrir o cerrar dependiendo del estado
    unsigned int MQTT_CMD:2; // Entrada desde red WiFi/MQTT con 2 bits, lo que sería igual
} io;

// Estructura para variables de estado del sistema
struct STATUS
{
    unsigned int cntTimerCA;   // Contador para cierre automático (TCA)
    unsigned int cntRunTimer;  // Contador para tiempo máximo de movimiento
    int ERR_COD;               // Código de error actual
};

// Estructura de configuración del sistema
struct CONFIG
{
    unsigned int RunTimer;     // Tiempo máximo permitido para mover el portón
    unsigned int TimerCA;      // Tiempo para cierre automático
} config;

// Inicia status
struct STATUS status = {0, 0, ERR_OK};
// configura el tiempo del RunTimer (el primero) y el TimerCA (el segundo)
struct CONFIG config = {180, 100};

int main()
{

// Bucle infinito que ejecuta continuamente la máquina de estados
    for(;;)
    {
        if(EstadoSiguiente == ESTADO_INICIAL)
        {
            EstadoSiguiente = Func_ESTADO_INICIAL();
        }

        if(EstadoSiguiente == ESTADO_ABIERTO)
        {
            EstadoSiguiente = Func_ESTADO_ABIERTO();
        }

        if(EstadoSiguiente == ESTADO_ABRIENDO)
        {
            EstadoSiguiente = Func_ESTADO_ABRIENDO();
        }

        if(EstadoSiguiente == ESTADO_CERRADO)
        {
            EstadoSiguiente = Func_ESTADO_CERRADO();
        }

        if(EstadoSiguiente == ESTADO_CERRANDO)
        {
            EstadoSiguiente = Func_ESTADO_CERRANDO();
        }

        if(EstadoSiguiente == ESTADO_ERR)
        {
            EstadoSiguiente = Func_ESTADO_ERR();
        }

        if(EstadoSiguiente == ESTADO_STOP)
        {
            EstadoSiguiente = Func_ESTADO_STOP();
        }

    }
    return 0;
}
int Func_ESTADO_INICIAL(void)
{
    //funciones de estado estaticas (una sola vez)
    status.cntRunTimer = 0;//reinicio del timer

    io.LSA = true;
    io.LSC = true;

    EstadoAnterior = EstadoActual;
    EstadoActual = ESTADO_INICIAL;
    printf("Estado INICIAL se dirige al ");


    //verifica si existe un error
    if(io.LSC == true && io.LSA == true)
    {
        status.ERR_COD = ERR_LSW;
        return ESTADO_ERR;
    }
    //puerta cerrada
    if(io.LSC == true && io.LSA == false)
    {
        return ESTADO_CERRADO;
    }
    //puerta abierta
    if(io.LSC == false && io.LSA == true)
    {
        return ESTADO_CERRANDO;
    }

    //puerta en estado desconocido
    if(io.LSC == false && io.LSA == false)
    {
        return ESTADO_STOP;
    }
}
int Func_ESTADO_CERRANDO(void)
{
    EstadoAnterior = EstadoActual;
    EstadoActual = ESTADO_CERRANDO;

    //funciones de estado estaticas (una sola vez)
    status.cntRunTimer = 0;//reinicio del timer
    io.MA = false;
    io.MC = true;
    io.BA = false;
    io.BC = false;
    //ciclo de estado
    for (;;)
    {

        // Intermitencia de la lámpara y buzzer
        lampState = !lampState;
        io.lampara = lampState; // actualizar el estado real de la lámpara
        io.buzzer = lampState; // buzzer sigue el mismo patrón

        if (io.lampara)
            printf("LÁMPARA: ENCENDIDO\n");
        else
            printf("LÁMPARA: APAGADO\n");

        if (io.buzzer)
            printf("BUZZER: SONANDO\n\n");
        else
            printf("BUZZER: SILENCIO\n\n");

        usleep(250000); // 0.25 segundos = 250,000 microsegundos

        if (io.LSC == true)
        {
            return ESTADO_CERRADO;
        }

        if (io.BA == true || io.BC == true)
        {
            return ESTADO_STOP;
        }
        //verifica error de run timer
        if (status.cntRunTimer > config.RunTimer)
        {
            status.ERR_COD = ERR_OT;
            return ESTADO_ERR;
        }
    }
}


int Func_ESTADO_ABRIENDO(void)
{
    EstadoAnterior = EstadoActual;
    EstadoActual = ESTADO_ABRIENDO;
    printf("==> Estado actual: ABRIENDO\n");

    //funciones de estado estaticas (una sola vez)
    status.cntRunTimer = 0;//reinicio del timer
    io.MA = true;
    io.MC = false;
    io.BA = false;
    io.BC = false;
    //ciclo de estado
    for(;;)
    {
        // Intermitencia de la lámpara y buzzer
        lampState = !lampState;
        io.lampara = lampState; // actualizar el estado real de la lámpara
        io.buzzer = lampState; // buzzer sigue el mismo patrón

        if (io.lampara)
            printf("LÁMPARA: ENCENDIDO\n");
        else
            printf("LÁMPARA: APAGADO\n");

        if (io.buzzer)
            printf("BUZZER: SONANDO\n\n");
        else
            printf("BUZZER: SILENCIO\n\n");

        usleep(500000); // 0.5 segundos = 500,000 microsegundos

        if(io.LSA == true)
        {
            return ESTADO_ABIERTO;
        }

        if(io.BA == true || io.BC == true)
        {
            return ESTADO_STOP;
        }

        //verifica error de run timer
        if(status.cntRunTimer > config.RunTimer)
        {
            status.ERR_COD = ERR_OT;
            return ESTADO_ERR;
        }
    }
}
int Func_ESTADO_CERRADO(void)
{
    EstadoAnterior = EstadoActual;
    EstadoActual = ESTADO_CERRADO;
    printf("==> Estado actual: CERRADO\n");

    //funciones de estado estaticas (una sola vez)
    io.MA = false;
    io.MC = false;
    io.BA = false;
    //ciclo de estado
    for(;;)
    {
        if(io.BA == true || io.PP == true)
        {
            return ESTADO_ABRIENDO;
        }
        //boton PP abre
    }
}
int Func_ESTADO_ABIERTO(void)
{
    EstadoAnterior = EstadoActual;
    EstadoActual = ESTADO_ABIERTO;
    printf("==> Estado actual: ABIERTO\n");

    //funciones de estado estaticas (una sola vez)
    io.MA = false;
    io.MC = false;
    io.BC = false;
    io.lampara = true; // mantener encendida
    status.cntTimerCA = 0; // reinicia el contador TCA al entrar


    //ciclo de estado
    for (;;)
    {
        printf("LÁMPARA: ENCENDIDO (puerta completamente abierta)\n");
        sleep(1); // espera 1 segundo
        status.cntTimerCA++; // incrementa el contador TCA

        if (status.cntTimerCA > 180 || io.BC == true ||io.PP == true) // si TCA > 3 min o si se presiona boton cerrar
        {
            return ESTADO_CERRANDO;
        }
        if (io.MA == false && io.MC == false && io.BC == false)
        {
            return ESTADO_ABIERTO;
        }
    }

}
int Func_ESTADO_ERR(void)
{
    EstadoAnterior = EstadoActual;
    EstadoActual = ESTADO_ERR;
    printf("==> Estado actual: ERROR\n\n");

    //funciones de estado estaticas (una sola vez)
    //Detener todo
    io.MA = false;
    io.MC = false;
    io.BA = false;
    io.BC = false;
    io.lampara = false;
    io.buzzer = false;

    // Variable para evitar imprimir múltiples veces
    bool mensajeMostrado = false;

    for (;;)
    {
        if (io.LSC == true && io.LSA == true)
        {
            status.ERR_COD = ERR_LSW;

            if (!mensajeMostrado)
            {
                printf("ERROR: Ambos limit switches activos (LSC y LSA).\n");
                mensajeMostrado = true;
            }
        }

        if (status.ERR_COD == ERR_LSW)
        {
            if ((io.LSC == false && io.LSA == true) || (io.LSC == true && io.LSA == false))
            {
                printf("Error corregido. Volviendo al estado inicial.\n");
                status.ERR_COD = ERR_OK;
                return ESTADO_INICIAL;
            }
        }

        if (status.ERR_COD == ERR_OT)
        {
            if (!mensajeMostrado)
            {
                printf("ERROR: han pasado más de 3 minutos con el portón abierto.\n");
                printf("Volviendo a cerrar el portón por seguridad.\n");
                mensajeMostrado = true;
            }
            return ESTADO_CERRANDO;
        }

        sleep(1);
    }
}

int Func_ESTADO_STOP(void)
{
    EstadoAnterior = EstadoActual;
    EstadoActual = ESTADO_STOP;
    printf("==> Estado: STOP / Emergencia\n");

    //funciones de estado estaticas (una sola vez)
    io.MA = false;
    io.MC = false;
    io.BA = false;
    io.BC = false;

    //ciclo de estado
    for (;;)
    {
        // Si se presiona BA y la puerta no está completamente abierta
        if (io.BA == true && io.LSA == false)
        {
            return ESTADO_ABRIENDO;
        }

// Si se presiona BC y la puerta no está completamente cerrada
        if (io.BC == true && io.LSC == false)
        {
            return ESTADO_CERRANDO;
        }

// Si se presiona PP:
// - Si la puerta está abierta → cerrar
// - Si la puerta está cerrada → abrir
// - Si está en lugar desconocido → cerrar por seguridad
        if (io.PP == true)
        {
            if (io.LSA == true)
                return ESTADO_CERRANDO;
            else if (io.LSC == true)
                return ESTADO_ABRIENDO;
            else
                return ESTADO_CERRANDO;
        }

        if (io.LSC == true && io.LSA == true)
        {
            status.ERR_COD = ERR_OT;
            return ESTADO_ERR;
        }
        if (io.BA == true && io.BC == true)
        {
            return ESTADO_STOP;
        }


    }
}


//Se ejecuta cada 100 ms con el timer del micro. Su funcionamiento depende del micro
void TimerCallback (void)
{
    status.cntRunTimer++;
    status.cntTimerCA++;

    io.LSA = gpio_getval(pin1);
}
