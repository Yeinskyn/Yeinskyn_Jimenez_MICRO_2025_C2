#include <stdio.h>

enum DiaDeLaSemana
{
    DOM,LUN,MAR,MIE,JUE,VIE,SAB
};

int main(void)
{
    int x;
    x = VIE;
    switch (x)
    {
    case DOM:
        printf ("Domingo\n");
        break;
    case LUN:
        printf ("Lunes\n");
        break;
    case MAR:
        printf ("Mares\n");
        break;
    case MIE:
        printf ("Miercoles\n");
        break;
    case JUE:
        printf ("Jueves\n");
        break;
    case VIE:
        printf ("Viernes\n");
        break;
    case SAB:
        printf ("Sabado\n");
        break;
    default:
        printf("Error\n");
        break;
    }
    return 0;
}
