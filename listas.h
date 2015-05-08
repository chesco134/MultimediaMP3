typedef struct nodo
{
	FILE* f;
	char* nombre;
	struct nodo* siguiente;
	struct nodo* anterior;
}Nodo;

typedef struct listaArchivos
{
	Nodo* root;
}ListaArchivos;


Nodo* crearNodo(char* nombre, FILE* f)
{
	Nodo* nodo = malloc(sizeof(Nodo));
	nodo->f = f;
	nodo->nombre = malloc(strlen(nombre));
	strcpy(nodo->nombre,nombre);
	nodo->siguiente = NULL;
	nodo->anterior = NULL;
	return nodo;
}

ListaArchivos* crearListaArchivos()
{
	ListaArchivos* nuevaLista = malloc(sizeof(ListaArchivos));
	nuevaLista->root = NULL;
	return nuevaLista;
}

void agregarNodo(ListaArchivos* lista, Nodo* nodo)
{
	if( lista->root == NULL )
	{
		lista->root = nodo;
	}
	else
	{
		Nodo* iter = lista->root;
		while( iter->siguiente != NULL )
		{
			iter = iter->siguiente;
		}
		iter->siguiente = nodo;
		if(iter != lista->root)
		{
			iter->siguiente->anterior = iter;
		}
	}
}

void eliminarUltimoNodo(ListaArchivos* lista)
{
	Nodo* iter = lista->root;
	Nodo* lastElement;
	while(iter->siguiente)
	{
		iter = iter->siguiente;
	}
	lastElement = iter;
	if ( iter->anterior != NULL )
	{
		iter->anterior->siguiente = NULL;
	}
	else
	{
		lista->root = NULL;
	}
	fclose(lastElement->f);
	free(lastElement->nombre);
	free(lastElement);
}

FILE* obtenerUltimoDescriptorArchivo(ListaArchivos* lista)
{
	Nodo* iter = lista->root;
	while(iter->siguiente)
	{
		iter = iter->siguiente;
	}
	return iter->f;
}

char* obtenerUltimoNombreArchivo(ListaArchivos* lista)
{
	Nodo* iter = lista->root;
	while(iter->siguiente)
	{
		iter = iter->siguiente;
	}
	return iter->nombre;
}
