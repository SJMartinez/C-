#include <iostream>
#include <fstream>
#include <ostream>
#include <string>
#include <iomanip>
#include <stdio.h>
#include <typeinfo>
//hice pruebas y el programa funciona bien con guardado y lectura de datos, carga por consola, mostrar los datos y con el d'hont.

/*Cosas que me parecen se pueden mejorar con más tiempo:
-CompararL y CompararV estan fuera de la TDALista cuando por concepto de encapsulación deberían estar adentro. El tema es que tampoco
podría estar adentro ya que no es la idea que el usuario acceda a esas funciones internas y tampoco puedo ponerlas en private ya
que escapa al tema de la cursada por el momento. Quizás esas funciones se pueden omitir usando de manera más eficiente los punteros
auxiliares adentro de las funciones AgregarV y AgregarL.

-Descubrí que existen problemas con los templates y pasar arrays de chars como argumento, por lo que para usar la pila template con
cadenas de caracteres y floats, tuve que hacer conversiones de char a int en el medio. tampoco pude convertir fácilmente los arrays
de chars en float, por lo que los resultados de cocientes del dhont salen redondeados y cuantos menos votos en total esto
influye peor en el resultado general. usar dos tipos de pilas diferente int y char

-mostrar los resultados dos veces seguidas provoca error ya que las pilas se vacian en la primera vez, a menos que
antes de la segunda vez se cargue la lista desde un archivo. se puede arreglar facil igual, ya lo hice en otras funciones
de este programa. quizas usar Colas en vez de Pilas hacía más fácil todo
*/
using namespace std;
//structs
typedef char str[20];
typedef char str100[200];




//pila en modo template
template <typename TS>
struct spNodoPila {
	TS info;
	spNodoPila<TS>* next;
};


template <typename TS>
struct ADTPila {
	//punteros auxiliares
	spNodoPila <TS>* headp = NULL;
	spNodoPila <TS>* posp = NULL;




	//metodos push y pop
	void push(TS dato) {
		spNodoPila<TS>* nodo = new spNodoPila<TS>();

		strcpy(nodo->info, dato);

		nodo->next = NULL;
		if (headp == NULL) {
			headp = nodo;
		}
		else {
			nodo->next = headp;
			headp = nodo;
		}
	}
	void pop(TS& dato) {
		spNodoPila<TS>* nodo = headp;
		if (headp->next != NULL) {

			strcpy(dato, nodo->info);

			posp = headp->next;
			delete(headp);
			headp = posp;
			posp = NULL;
		}
		else {
			strcpy(dato, nodo->info);
			delete(headp);
			headp = NULL;
		}
	}

	//mostrar pila, copiar y borrar
	void mostrarPila() {
		ADTPila<TS> aux;
		TS dato;

		while (headp != NULL) {
			pop(dato);
			cout << dato << endl;
			aux.push(dato);
		}
		while (aux.headp != NULL) {
			aux.pop(dato);
			push(dato);
		}

	}
	void copiarPila(ADTPila<TS>& destino) {
		ADTPila<TS> aux;
		ADTPila<TS> aux2;
		TS dato;

		while (headp != NULL) {
			pop(dato);
			aux2.push(dato);
			aux.push(dato);
		}
		while (aux.headp != NULL) {
			aux.pop(dato);
			destino.push(dato);
			push(dato);
		}

	}

	void borrarPila() {
		TS dato;
		while (headp != NULL) {
			pop(dato);
		}
	}
};
//structs y nodos del TAD Lista
struct  sInfoVotos {
	int voto;
	str sexo;
	int edad;

};

struct spNodoVotos {
	sInfoVotos info;
	spNodoVotos* next;

};

struct sInfoLista {
	str nomLista;
	int cantvotos = 0;
	int numLista;
	int bancas = 0;
	str100 final = "";
	int cantcand = 0;
};

struct spNodoLista {
	sInfoLista info;
	//pila que uso para guardar candidatos
	ADTPila <str> cand;
	//pila para guardar cocientes del dhont de cada lista
	ADTPila<str> cocientes;
	//punteros prev (previo) next, (siguiente), vlink (head de la sublista votos), posv(puntero auxiliar)
	spNodoLista* prev;
	spNodoLista* next;
	spNodoVotos* vlink;
	spNodoVotos* posv;
	//metodo para borrar todos los votos del nodo
	void borrarVotos() {
		posv = NULL;
		while (vlink != NULL) {

			posv = vlink->next;
			delete(vlink);
			vlink = posv;
		}
		posv = NULL;
	}
};

//dos funciones que usa la lista para comparar variables al ingresarlas
void compararL(spNodoLista*& first, spNodoLista*& sec, spNodoLista*& reartemp) {


	//si hay first pero no tiene next, sec pasa a ser su next y el rear
	if (first->next == NULL) {
		first->next = sec;
		sec->prev = first;
		sec->next = NULL;
		reartemp = sec;
	}
	//si hay first y el sec es menor que su next, el nodo se ubica entre los dos
	else if (first->next->info.numLista > sec->info.numLista) {
		sec->prev = first;
		sec->next = first->next;
		first->next->prev = sec;
		first->next = sec;
	}

	//ninguno de esos casos, comienza la recursión
	else {
		compararL(first->next, sec, reartemp);
	}

}

void compararV(spNodoVotos*& first, spNodoVotos*& sec) {


	//si hay first pero no tiene next, sec pasa a ser su next
	if (first->next == NULL) {
		first->next = sec;
		sec->next = NULL;


	}
	//si hay first y el sec es menor que su next, el nodo se ubica entre los dos
	else if (first->next->info.edad > sec->info.edad) {
		sec->next = first->next;
		first->next = sec;
	}

	//ninguno de esos casos, comienza la recursión
	else {
		compararV(first->next, sec);
	}

}



//struct TDA de la lista
struct ADTLista {
	//head es cabeza, rear retaguardia(atras), pos puntero auxiliar
	spNodoLista* head = NULL;
	spNodoLista* rear = NULL;
	spNodoLista* pos = NULL;

	int salida = 0;
	//total votos validos
	int totalv = 0;
	//total votos blanco
	int totalb = 0;
	//total votos invalidos
	int totali = 0;

	//metodos internos
	void borrarLista() {
		pos = NULL;
		while (head != NULL) {
			pos = head->next;
			head->borrarVotos();
			head->cand.borrarPila();
			delete(head);
			head = pos;
		}
		rear = NULL;
		pos = NULL;
		salida = 0;
		totalv = 0;
		totalb = 0;
		totali = 0;
	}

	//método para mostrar el dhont. es un encare diferente al tp anterior. en la funcion "cocientes" a cada lista se le asigna
	//una pila con 5 cocientes para calcular el dhont. luego en esta funcion se compara cada punta de pila y se extrae la mayor, en un
	// de 5 veces, que es la cantidad de bancas en juego. cada cociente ganador extraido a su vez saca un candidato de la pila
	//de candidatos ganadores y los guarda en un string acumulativo, que se va agrandando con cada candidato
	//que gana bancas para la lista. al final de todos los calculos muestro los strings finales de cada lista ganadora, obteniendo
	//asi la tabla
	void dhont() {
		spNodoLista* aux;

		int x = 0;
		
			if (head != rear) {
				aux = head->next;
			}
			else {
				aux = head;
			}
			pos = head;
			spNodoLista* mayor = NULL;

			float one = 0;
			float two = 0;
			int exit = 0;
			
			str peek1;
			str peek2;
			for (int x = 0; x < 5; x++) {
				
				while (exit == 0) {
					//miro las cabezas de pila y las asigno a variables auxiliares
					strcpy(peek1, pos->cocientes.headp->info);

					strcpy(peek2, aux->cocientes.headp->info);
					
					sscanf(peek1, "%d", &one);
					sscanf(peek2, "%d", &two);
				//las comparo
					if (one <= two) {
						mayor = aux;
						break;
						
					}
					else {
						mayor = pos;
						break;
						
					}
					if (aux == rear) {
						break;
						
					}
					aux = aux->next;
				}
				
				//asigno los strings. tuve que aplicar varios metodos propios de formateo para que quede bien
				mayor->info.bancas++;
				string cadena(mayor->info.final);
				string cadenaf; 
				str inf1 = "";
				
				str mayors;
				
				if (mayor->info.final == "") {
					
					mayor->cand.pop(mayors);
					string cadena(mayors);
					cadena.erase(remove_if(cadena.begin(), cadena.end(), isspace), cadena.end());
					cadena.insert(cadena.end(), 7 - cadena.size(), ' ');
					
					cadenaf = cadena;
					mayor->cocientes.pop(inf1);
				
					
				}
				else {
					
					mayor->cand.pop(mayors);					
					mayor->cocientes.pop(inf1);
					string cadena2(mayors);
					cadena2.erase(remove_if(cadena2.begin(), cadena2.end(), isspace), cadena2.end());
					cadena2.insert(cadena2.end(), 15 - cadena2.size(), ' ');
				
					cadenaf = cadena + cadena2;
				
					
				}
				strcpy(mayor->info.final, cadenaf.c_str());
				aux = head->next;
			}
			
		//muestro la lista
		pos = head;
		cout << "        " << "Banca 1" << "        " << "Banca 2" << "        "
			<< "Banca 3" << "        " << "Banca 4" << "        " << "Banca 5" << "        " << endl;
		while (pos) {
			cout << "Lista: " << pos->info.numLista <<""<< pos->info.final << endl;
			if (pos == rear) {
				break;
			}
			pos = pos->next;
		}
		pos = NULL;
	}
	//funcion de cocientes antes mencionada
	void fCocientes() {
		pos = head;
		int exit = 0;

		while (exit == 0) {
			//si no pasa de 3% de votos totales ni se le asigna cocientes a la lista
			if ((float(pos->info.cantvotos) * 100 / float(totalv)) >= 3) {
				
				float cociente = 0;
				int y = 5;
				while (y >= 1) {
					
					cociente = float(pos->info.cantvotos / y);
					string cocientestring = to_string(cociente);
					str cocientec;
					strcpy(cocientec, cocientestring.c_str());
					pos->cocientes.push(cocientec);
					y--;
				}
				
			}
			else {
			
				int y = 5;
				while (y >= 1) {
					str cero = "0";
					pos->cocientes.push(cero);
					y--;
				}
			}
			if (pos == rear) {
				break;
				
			}
			pos = pos->next;
			
		}
		
		pos = NULL;
	}

	//metodo de agregar lista
	void agregarLista(sInfoLista info) {
		spNodoLista* nodo = new(spNodoLista);
		nodo->info = info;
		nodo->next = NULL;
		nodo->prev = NULL;
		pos = NULL;
		nodo->vlink = NULL;
		//si no hay head, nodo pasa a ser head y rear
		if (head == NULL) {
			head = nodo;
			rear = nodo;

		}


		//si hay head y es mayor que el nuevo nodo, nodo pasa a ser head
		else if ((head->info.numLista) > (nodo->info.numLista)) {
			nodo->next = head;
			head->prev = nodo;
			//si head->next era null, head pasa también a ser rear
			if (head->next == NULL) {
				rear = head;

			}
			head = nodo;


		}

		//si hay head, es menor que el nodo pero no tiene next, nodo pasa a ser su next y el rear
		else if (head->next == NULL) {
			head->next = nodo;
			nodo->prev = head;
			rear = nodo;

		}
		//si hay head y el nodo es menor que su next, el nodo se ubica entre los dos
		else if (head->next->info.numLista > nodo->info.numLista) {
			nodo->prev = head;
			nodo->next = head->next;
			head->next->prev = nodo;
			head->next = nodo;
		}


		//ninguno de los casos anteriores, se usa esta funcion recursiva
		else {
			compararL(head->next, nodo, rear);
		}
	}
	//metodo para mostrar la lista entera
	void mostrar() {

		if (pos == NULL) {
			pos = head;

		}
		else if (pos != NULL) {
			pos = pos->next;

		}
		cout << "Nombre lista: " << pos->info.nomLista << " - Número: " << pos->info.numLista << " - Candidatos: " << endl;

		pos->cand.mostrarPila();

		if (pos->vlink != NULL) {
			spNodoVotos* posv = pos->vlink;
			cout << "Votos:" << endl;

			for (int x = 0; x < pos->info.cantvotos; x++)
			{
				cout << "Edad: " << posv->info.edad << " - Sexo: " <<posv->info.sexo << endl;
				posv = posv->next;
			}
			cout << "" << endl;

		}

		if (pos != rear) {
			mostrar();

		}
		if (salida == 0) {
			cout << left << setw(9) << "Total votos validos: " << totalv << endl;
			cout << left << setw(9) << "Total votos en blanco: " << totalb << endl;
			cout << left << setw(9) << "Total votos nulos: " << totali << endl;
			pos = NULL;

			salida++;
		}

	}
	//agregar candidatos a la pila de candidatos
	void agregarCand(int num) {
		str f = "";
		buscarLista(num);

		printf("\nIngresar candidatos, 0= termina ingreso candidatos\n\n");
		cin >> f;
		for (int i = 0; i < 25; i++) {
			if (strcmp(f, "0") == 0) break;
			str f1;
			strcpy(f1, f);
			pos->cand.push(f1);
			pos->info.cantcand++;
			cin.ignore();
			printf("\nIngresar candidatos, 0= termina ingreso candidatos\n\n");
			cin >> f;
		}
		pos = NULL;
	}
	//agregar votos (sublista)
	void agregarVotos(sInfoVotos votos) {

		spNodoVotos* nodo = new(spNodoVotos);
		nodo->info = votos;
		nodo->next = NULL;

		pos = NULL;
		buscarLista(nodo->info.voto);

		//si vlink es NULL (sublista vacia) el nodo pasa a ser el primero
		if (pos->vlink == NULL) {
			pos->vlink = nodo;
			pos->info.cantvotos++;
		}
		//si hay vlink y es mayor que el nuevo nodo, nodo pasa a ser vlink
		else if ((pos->vlink->info.edad) > (nodo->info.edad)) {
			nodo->next = pos->vlink;
			pos->vlink = nodo;
			pos->info.cantvotos++;

		}

		//si hay vlink, no tiene next y el nodo es menor, nodo pasa a ser su next
		else if (pos->vlink->next == NULL) {
			pos->vlink->next = nodo;
			pos->info.cantvotos++;

		}
		//si hay vlink y el nodo es menor que su next, el nodo se ubica entre los dos
		else if (pos->vlink->next->info.edad > nodo->info.edad) {
			nodo->next = pos->vlink->next;
			pos->vlink->next = nodo;
			pos->info.cantvotos++;
		}


		//ninguno de los casos anteriores, se usa esta funcion recursiva
		else {
			compararV(pos->vlink->next, nodo);
			pos->info.cantvotos++;
		}
		pos = NULL;
	}
	//funcion interna para identificar posicion de listas
	void buscarLista(int num) {

		pos = head;
		int x = 0;
		while (x != 1) {

			if (pos->info.numLista == num) {
				break;
			}
			pos = pos->next;

		}


	}

};
//prototipos
void inicializar();
ADTLista nuevaLista();
sInfoLista getDatoLista();
void nuevaSublista(ADTLista& lista);
int getDatoVoto(sInfoVotos& votos);
void guardarL(ADTLista& lista, FILE*& file);
void guardarPila(spNodoLista*& listan, FILE*& file);
void guardarSub(spNodoLista* listan, FILE*& file);
void leer(ADTLista& lista, FILE*& file);
int main()
{
	setlocale(LC_CTYPE, "Spanish");


	inicializar();
	return 0;

}


//aqui el usuario elije qué hacer. puede ingresar listas y votos cuantas veces quiera, de manera ordenada siempre.
void inicializar() {
	ADTLista lista;
	FILE* archivo;
	int res = 0;
	while (res != 12) {
		cout << "Inserte 1 para cargar lista desde archivo. Inserte 2 ingresar nueva lista" << endl;
		cout << "Inserte 3 para ingresar nuevos votos. Inserte 4 para mostrar lista actual." << endl;
		cout << "5 para mostrar resultados. Inserte 6 para guardar. 0 termina el programa. " << endl;
		cin >> res;
		
		switch (res) {
		case 0:
			res = 12;
				break;
		case 1:
			leer(lista, archivo);
			cout << "Lista cargada en memoria!" << endl;
			break;
		case 2:
			lista = nuevaLista();
			break;
		case 3:
			nuevaSublista(lista);
			break;
		case 4:
			lista.mostrar();
			lista.salida = 0;
			break;
		case 5:
			lista.fCocientes();			
			lista.dhont();
			break;
		case 6:
			cout << "Lista guardada en archivo!" << endl;
			guardarL(lista, archivo);
			break;
		}
	}
}




//inicializa la lista
ADTLista nuevaLista() {
	ADTLista lista;
	sInfoLista info;
	int exit = 0;
	while (exit != 1) {
		//pide los datos de la lista
		info = getDatoLista();
		//agrega los datos a la lista
		lista.agregarLista(info);
		//pide candidatos para la lista
		lista.agregarCand(info.numLista);
		cout << "Lista ingresada. Pulse 1 para terminar ingreso de listas, otro número si desea seguir ingresando" << endl;
		cin >> exit;
	}
	return lista;
}




sInfoLista getDatoLista() {
	sInfoLista lista;
	printf("Ingrese nombre lista\n\n");

	cin >> lista.nomLista;
	cin.ignore();
	printf("\nIngresar Num de lista\n\n");
	cin >> lista.numLista;
	cin.ignore();

	return lista;

}
//funcion para agregar los votos a las listas. esta armada para diferenciar blancos e invalidos.
void nuevaSublista(ADTLista& lista) {

	sInfoVotos info;
	int exit = 1;
	int flag = 0;
	while (exit != 0) {
		flag = getDatoVoto(info);
		if (flag == 0) {
			exit = 0;
			break;
		}
		else if (flag == 98) {
			cout << "Voto en blanco registrado" << endl;
			lista.totalb++;
			lista.totalv++;
		}
		else if (flag == 99) {
			cout << "Voto invalido registrado" << endl;
			lista.totali++;
		}
		else {
			lista.agregarVotos(info);
			lista.totalv++;
			cout << "Votos ingresados." << endl;
		}

	}

}

int getDatoVoto(sInfoVotos& votos) {
	printf("Ingrese lista a votar. 98 para voto en blanco y 99 para voto invalido. 0 termina el ingreso \n\n");
	int flag = 0;
	cin >> flag;
	if (flag != 99 && flag != 98 && flag != 0) {
		votos.voto = flag;
		cin.ignore();

		printf("\nIngresar el sexo del votante \n\n");
		cin >> votos.sexo;
		cin.ignore();
		printf("\nIngresar edad del votante\n\n");
		cin >> votos.edad;
		cin.ignore();
	}

	return flag;

}
//guardo la lista ordenadamente.
void guardarL(ADTLista& lista, FILE*& file) {
	sInfoLista infolist;
	file = fopen("Lista.dat", "wb");
	int x = 0;
	int vv = 0;
	//primero guardo las variables auxiliares de votos totales, invalidos y en blanco
	lista.pos = lista.head;
	int totalva = lista.totalv;
	int totalinv = lista.totali;
	int totalb = lista.totalb;
	fwrite(&totalva, sizeof(totalva), 1, file);
	fwrite(&totalinv, sizeof(totalinv), 1, file);
	fwrite(&totalb, sizeof(totalb), 1, file);
	//hasta que me quedo sin listas a guardar, el whle progresa
	while (lista.pos != NULL) {
		//guardo la info de la lista
		infolist = lista.pos->info;
		fwrite(&infolist, sizeof(infolist), 1, file);
		//guardo pilas de candidatos
		guardarPila(lista.pos, file);
		//guardo votos
		guardarSub(lista.pos, file);

		if (lista.pos == lista.rear) {
			break;
		}
		lista.pos = lista.pos->next;
	}
	fclose(file);
	lista.pos = NULL;
}

//para guardar sublistas
void guardarSub(spNodoLista* listan, FILE*& file) {
	sInfoVotos votos;
	spNodoVotos* nodo = listan->vlink;
	for (int x = 0; x < listan->info.cantvotos; x++) {

		votos = nodo->info;
		fwrite(&votos, sizeof(votos), 1, file);
		nodo = nodo->next;
	}
}
//y guardar pilas
void guardarPila(spNodoLista*& listan, FILE*& file) {
	str dato;
	ADTPila<str> pila;
	listan->cand.copiarPila(pila);
	for (int x = 0; x < listan->info.cantcand; x++) {
		pila.pop(dato);
		fwrite(&dato, sizeof(dato), 1, file);
	}
	pila.borrarPila();
}

//para leer los datos guardados es el proceso inverso. me valgo del orden en que guarde las cosas(3 variables de control de votos,
//lista->pilas->votos. puedo saber cuantas pilas y votos "leer" del archivo gracias a variables almacenadas en las listas, cantv y cantc.
void leer(ADTLista& lista, FILE*& file) {
	file = fopen("Lista.dat", "rb");
	str datopila;
	sInfoLista inflist;
	sInfoVotos votos;
	lista.borrarLista();
	int ent = 0;
	int x = 0;
	int cantv = 0;
	int cantc = 0;
	spNodoLista* aux = NULL;
	int totalva;
	int totalinv;
	int totalb;
	fread(&totalva, sizeof(totalva), 1, file);
	fread(&totalinv, sizeof(totalinv), 1, file);
	fread(&totalb, sizeof(totalb), 1, file);
	lista.totalv = totalva;
	lista.totali = totalinv;
	lista.totalb = totalb;
	while (1) {
		if (fread(&inflist, sizeof(inflist), 1, file) == NULL) {
			lista.pos = NULL;
			fclose(file);
			break;
		}
		cantv = inflist.cantvotos;
		inflist.cantvotos = 0;
		cantc = inflist.cantcand;
		inflist.cantcand = 0;

		lista.agregarLista(inflist);

		if (lista.head == lista.rear) {
			aux = lista.head;


		}
		else
		{

			aux = aux->next;
		}
		for (int x = 0; x < cantc; x++) {
			fread(&datopila, sizeof(datopila), 1, file);
			aux->cand.push(datopila);
		}

		for (int x = 0; x < cantv; x++) {
			fread(&votos, sizeof(votos), 1, file);
			lista.agregarVotos(votos);

		}

	}

}