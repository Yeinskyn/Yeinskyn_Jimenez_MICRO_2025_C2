#include <stdio.h>
#include <stdlib.h>

void crearUnArregloEntero(int *puntAInt, int dim);
int * crearUnArregloEntero2(int *puntAInt, int dim);
int crearUnArregloEntero3(int **puntAInt, int dim);

int main()
{
    int arreglo [10];
    int *punt;
    int val = crearUnArregloEntero3(&punt, 10);
    mostrar (punt, val);
    return 0;
}

int crearUnArregloEntero3(int **puntAInt, int dim)
{
    *puntAInt= (int) malloc (dim *sizeof(int));
    int i;
    for(i=0; i<dim; i++)
    {
        (*puntAInt)[i] = i;
    }
    printf("\n");
    return i;

}
void crearUnArregloEntero(int * puntAInt, int dim)
{
    puntAInt= (int) malloc (dim *sizeof(int));
    int i;
    for(i=0; i<dim; i++)
    {
        puntAInt[i] = i;
    }
    printf("\n");
    mostrar(puntAInt, 10);

}

int * crearUnArregloEntero2(int * puntAInt, int dim)
{
    puntAInt= (int) malloc (dim *sizeof(int));
    int i;
    for(i=0; i<dim; i++)
    {
        puntAInt[i] = i;
    }
    printf("\n");
    return puntAInt;

}

void mostrar (int a[], int val)
{
    int i;
    for (i=0; i<val; i++)
    {
        printf("- | %d | -", a[i]);
    }
}

//Si se quisiera trabajar con void, sería el siguiente código

/*#include <stdio.h>
#include <stdlib.h>

void crearUnArregloEntero( int *puntAInt, int dim);
int * crearUnArregloEntero2(int *puntAInt, int dim);
void crearUnArregloEntero3(int **puntAInt, int dim);

int main()
{
    int arreglo [10];
    int *punt;
    crearUnArregloEntero3(&punt, 10);
    mostrar (punt, 10);
    return 0;
}

void crearUnArregloEntero3(int **puntAInt, int dim)
{
    *puntAInt= (int) malloc (dim *sizeof(int));
    int i;
    for(i=0; i<dim; i++)
    {
        (*puntAInt)[i] = i;
    }
    printf("\n");


}
void crearUnArregloEntero(int * puntAInt, int dim)
{
    puntAInt= (int) malloc (dim *sizeof(int));
    int i;
    for(i=0; i<dim; i++)
    {
        puntAInt[i] = i;
    }
    printf("\n");
    mostrar(puntAInt, 10);

}

int * crearUnArregloEntero2(int * puntAInt, int dim)
{
    puntAInt= (int) malloc (dim *sizeof(int));
    int i;
    for(i=0; i<dim; i++)
    {
        puntAInt[i] = i;
    }
    printf("\n");
    return puntAInt;

}

void mostrar (int a[], int val)
{
    int i;
    for (i=0; i<val; i++)
    {
        printf("| %d |", a[i]);
    }
} */

