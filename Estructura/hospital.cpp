#include <iostream>     // Librería para operaciones de entrada y salida (cin, cout)
#include <string>       // Librería para usar la clase string
#include <cstdlib>      // Librería para usar system("cls") y system("pause")

using namespace std;    // Permite usar nombres estándar sin anteponer "std::"

// ===================================================
// PALETA DE COLORES ANSI (VARIADA PARA HOSPITAL)
// ===================================================
#define RESET       "\033[0m"
#define ROJO_ALERTA "\033[1;91m"   // Rojo brillante para errores y pacientes críticos
#define ESMERALDA   "\033[1;92m"   // Verde brillante para confirmaciones y nivel leve
#define AMBAR       "\033[1;93m"   // Amarillo brillante para advertencias y nivel urgente
#define AZUL_MARINO "\033[1;34m"   // Azul institucional para marcos principales
#define VIOLETA     "\033[1;95m"   // Púrpura brillante para el módulo de Farmacia (Pila)
#define TURQUESA    "\033[1;96m"   // Cian brillante para encabezados médicos y Lista
#define BLANCO_BR   "\033[1;97m"   // Blanco brillante para resaltar datos clave

// ===================================================
// DEFINICIÓN DE ESTRUCTURAS DINÁMICAS
// ===================================================

// 1. Estructura para representar un paciente (Lista Enlazada)
struct Paciente {
    int id;                 // Identificador único del paciente (Historia Clínica)
    string nombre;          // Nombre completo del paciente
    int prioridad;          // Nivel de gravedad (1 = Crítico, 2 = Urgente, 3 = Leve)
    Paciente* siguiente;    // Puntero al siguiente paciente (lista enlazada)
};

// Puntero global al inicio de la lista de pacientes
Paciente* cabezaPacientes = NULL;

// 2. Estructura para nodo de la cola de triaje (Cola de Prioridad)
struct NodoTriaje {
    Paciente* paciente;     // Puntero al paciente contenido en la lista maestra
    NodoTriaje* siguiente;  // Puntero al siguiente nodo en la cola
};

// Puntero global al inicio de la cola de triaje
NodoTriaje* cabezaTriaje = NULL;

// 3. Estructura para representar insumos/medicamentos asignados (Pila LIFO)
struct BloqueInsumo {
    Paciente* paciente;         // Apuntador al paciente dueño del insumo
    int cantidad;               // Cantidad de dosis o unidades asignadas
    BloqueInsumo* siguiente;    // Apuntador al siguiente bloque en la pila
};

// Puntero al tope de la pila de insumos
BloqueInsumo* topeInsumos = NULL;

//---------------- FUNCIONES AUXILIARES Y GRÁFICOS ----------------//

// Busca un paciente por su ID en la lista enlazada
Paciente* buscarPacientePorID(int id) {
    Paciente* actual = cabezaPacientes; // Comienza desde la cabeza de la lista
    while (actual != NULL) {
        if (actual->id == id)           // Si se encuentra el ID
            return actual;              // Retorna el puntero al paciente
        actual = actual->siguiente;     // Avanza al siguiente nodo
    }
    return NULL; // No se encontró el paciente
}

// Verifica si un paciente con el ID ya está en la cola de triaje
bool estaEnTriaje(int id) {
    NodoTriaje* actual = cabezaTriaje;  // Comienza desde la cabeza de la cola
    while (actual != NULL) {
        if (actual->paciente->id == id) // Si encuentra el ID en algún nodo
            return true;                // Ya está en la cola
        actual = actual->siguiente;     // Avanza al siguiente nodo
    }
    return false; // No se encontró el paciente en la cola
}

// Verifica si un paciente tiene insumos asignados en la pila
bool tieneInsumosEnPila(int id) {
    BloqueInsumo* actual = topeInsumos;
    while (actual != NULL) {
        if (actual->paciente->id == id)
            return true;
        actual = actual->siguiente;
    }
    return false;
}

// Dibuja los nodos de la Lista Enlazada en consola
void graficarListaPacientes() {
    cout << "\n" << BLANCO_BR << "  [MEMORIA DINAMICA - LISTA DE PACIENTES]" << RESET << "\n  ";
    cout << AMBAR << "CABEZA -> " << RESET;
    Paciente* actual = cabezaPacientes;
    while (actual != NULL) {
        cout << TURQUESA << "[" << actual->id << " | " << actual->nombre << " | P" << actual->prioridad << "]" << RESET
             << BLANCO_BR << " --> " << RESET;
        actual = actual->siguiente;
    }
    cout << ROJO_ALERTA << "NULL" << RESET << "\n\n";
}

// Dibuja los nodos de la Cola de Prioridad en consola
void graficarColaTriaje() {
    cout << "\n" << BLANCO_BR << "  [MEMORIA DINAMICA - COLA DE TRIAJE]" << RESET << "\n  ";
    cout << AMBAR << "FRENTE -> " << RESET;
    NodoTriaje* actual = cabezaTriaje;
    while (actual != NULL) {
        string colorPrio = (actual->paciente->prioridad == 1) ? ROJO_ALERTA : 
                           (actual->paciente->prioridad == 2 ? AMBAR : ESMERALDA);
        cout << colorPrio << "[" << actual->paciente->id << " | " << actual->paciente->nombre 
             << " (P" << actual->paciente->prioridad << ")]" << RESET
             << BLANCO_BR << " --> " << RESET;
        actual = actual->siguiente;
    }
    cout << ROJO_ALERTA << "NULL" << RESET << "\n\n";
}

// Dibuja los bloques de la Pila LIFO verticalmente
void graficarPilaInsumos() {
    cout << "\n" << BLANCO_BR << "  [MEMORIA DINAMICA - PILA DE INSUMOS (LIFO)]" << RESET << "\n";
    BloqueInsumo* actual = topeInsumos;
    bool esTope = true;
    while (actual != NULL) {
        cout << "      " << VIOLETA << "+--------------------------------+" << RESET;
        if (esTope) {
            cout << AMBAR << " <-- TOPE (Ultimo asignado)" << RESET;
            esTope = false;
        }
        cout << "\n      " << VIOLETA << "| " << BLANCO_BR << "ID:" << actual->paciente->id
             << " | " << actual->paciente->nombre << " (" << actual->cantidad << " u.)" << VIOLETA << " |" << RESET << "\n";
        cout << "      " << VIOLETA << "+--------------------------------+" << RESET << "\n";
        cout << "                     " << BLANCO_BR << "|" << RESET << "\n";
        cout << "                     " << BLANCO_BR << "v" << RESET << "\n";
        actual = actual->siguiente;
    }
    cout << "                    " << ROJO_ALERTA << "NULL" << RESET << "\n\n";
}

//---------------- 1. GESTOR DE PACIENTES ----------------//

// Inserta un nuevo paciente en la lista enlazada
void insertarPaciente() {
    cout << TURQUESA << "\n--- Registrar nuevo paciente ---\n" << RESET;

    int id;
    do {
        cout << "Ingrese ID del paciente (entero positivo): ";
        if (!(cin >> id) || id <= 0) {
            cout << ROJO_ALERTA << "Error: El ID debe ser un numero entero positivo.\n" << RESET;
            id = -1;
        } else if (buscarPacientePorID(id) != NULL) {
            cout << ROJO_ALERTA << "Error: Ya existe un paciente con ese ID.\n" << RESET;
            id = -1;                          // Repetir entrada si ya existe
        }
        cin.clear();                          // Limpia el estado de cin
        cin.ignore(10000, '\n');              // Limpia el buffer del salto de línea
    } while (id <= 0);                        // Repite hasta que el ID sea válido

    string nombre;
    do {
        cout << "Ingrese nombre del paciente: ";
        getline(cin, nombre);                 // Lee el nombre completo con espacios
        if (nombre.empty()) {
            cout << ROJO_ALERTA << "Error: El nombre no puede estar vacio.\n" << RESET;
        }
    } while (nombre.empty());                 // Repite hasta que el nombre no esté vacío

    int prioridad;
    do {
        cout << "Ingrese nivel de gravedad (1 = Critico, 2 = Urgente, 3 = Leve): ";
        if (!(cin >> prioridad) || prioridad <= 0) {
            cout << ROJO_ALERTA << "Error: La prioridad debe ser un numero entero positivo.\n" << RESET;
            prioridad = -1;
        }
        cin.clear();
        cin.ignore(10000, '\n');
    } while (prioridad <= 0);                 // Repite hasta que la prioridad sea válida

    Paciente* nuevo = new Paciente;           // Crea un nuevo paciente dinámicamente
    nuevo->id = id;                           // Asigna los valores
    nuevo->nombre = nombre;
    nuevo->prioridad = prioridad;
    nuevo->siguiente = NULL;

    if (cabezaPacientes == NULL) {            // Si la lista está vacía
        cabezaPacientes = nuevo;              // El nuevo paciente es el primero
    } else {
        Paciente* actual = cabezaPacientes;
        while (actual->siguiente != NULL) actual = actual->siguiente;
        actual->siguiente = nuevo;            // Agrega el paciente al final de la lista
    }

    cout << ESMERALDA << "Paciente registrado correctamente.\n" << RESET;
}

// Elimina un paciente de la lista enlazada por ID
void eliminarPaciente() {
    cout << TURQUESA << "\n--- Dar de alta / Eliminar paciente ---\n" << RESET;
    int id;
    do {
        cout << "Ingrese ID del paciente a eliminar: ";
        if (!(cin >> id) || id <= 0) {
            cout << ROJO_ALERTA << "Error: Debe ingresar un numero entero positivo.\n" << RESET;
            id = -1;
        }
        cin.clear();
        cin.ignore(10000, '\n');
    } while (id <= 0);                        // Repite hasta que el ID sea válido

    if (cabezaPacientes == NULL) {            // Lista vacía
        cout << AMBAR << "No hay pacientes para eliminar.\n" << RESET;
        return;
    }

    // Evita borrar un paciente que sigue en triaje o con insumos activos
    if (estaEnTriaje(id)) {
        cout << ROJO_ALERTA << "Error: El paciente sigue en la cola de triaje. Atiendalo primero.\n" << RESET;
        return;
    }
    if (tieneInsumosEnPila(id)) {
        cout << ROJO_ALERTA << "Error: El paciente tiene insumos asignados. Liberelos primero.\n" << RESET;
        return;
    }

    if (cabezaPacientes->id == id) {          // El paciente a eliminar es el primero
        Paciente* temp = cabezaPacientes;
        cabezaPacientes = cabezaPacientes->siguiente;
        delete temp;                          // Libera la memoria
        cout << ESMERALDA << "Paciente eliminado del registro.\n" << RESET;
        return;
    }

    Paciente* actual = cabezaPacientes;
    while (actual->siguiente != NULL && actual->siguiente->id != id)
        actual = actual->siguiente;

    if (actual->siguiente != NULL) {          // Se encontró el paciente
        Paciente* temp = actual->siguiente;
        actual->siguiente = temp->siguiente;  // Lo elimina de la lista
        delete temp;                          // Libera la memoria
        cout << ESMERALDA << "Paciente eliminado del registro.\n" << RESET;
    } else {
        cout << ROJO_ALERTA << "Paciente no encontrado.\n" << RESET;
    }
}

// Muestra todos los pacientes registrados
void mostrarPacientes() {
    cout << TURQUESA << "\n--- Lista general de pacientes ---\n" << RESET;
    
    // Verifica si no hay pacientes en la lista general
    if (cabezaPacientes == NULL) {
        cout << AMBAR << "No hay pacientes registrados.\n" << RESET;
        return;
    }

    graficarListaPacientes();

    // Recorre la lista de pacientes e imprime sus datos
    Paciente* actual = cabezaPacientes;
    while (actual != NULL) {
        cout << TURQUESA << "  * " << RESET
             << "ID: " << BLANCO_BR << actual->id << RESET
             << ", Nombre: " << actual->nombre
             << ", Gravedad: " << actual->prioridad << "\n";
        actual = actual->siguiente; // Avanza al siguiente nodo
    }
}

//---------------- 2. SALA DE TRIAJE (COLA DE PRIORIDAD) ----------------//

// Inserta un paciente en la cola ordenado por prioridad (menor número = mayor gravedad)
void encolarPacienteEnTriaje() {
    cout << ESMERALDA << "\n--- Encolar paciente en sala de triaje ---\n" << RESET;

    int id;
    do {
        cout << "Ingrese ID del paciente a encolar: ";
        if (!(cin >> id) || id <= 0) {
            cout << ROJO_ALERTA << "Error: Debe ingresar un numero entero positivo.\n" << RESET;
            id = -1;
        }
        cin.clear();
        cin.ignore(10000, '\n');
    } while (id <= 0);

    Paciente* p = buscarPacientePorID(id);    // Busca el paciente en la lista general
    
    // Si no se encuentra, muestra error
    if (p == NULL) {
        cout << ROJO_ALERTA << "Error: No existe un paciente con ese ID en la lista general.\n" << RESET;
        return;
    }
    
    // Verifica si ya está en la cola
    if (estaEnTriaje(id)) {
        cout << AMBAR << "Error: El paciente ya esta en la cola de triaje.\n" << RESET;
        return;
    }
    
    // Crea un nuevo nodo de la cola
    NodoTriaje* nuevo = new NodoTriaje;       // Crea un nuevo nodo
    nuevo->paciente = p;                      // Apunta al paciente encontrado
    nuevo->siguiente = NULL;            

    // Inserta el nodo ordenado por prioridad (menor valor = más prioritario)
    if (cabezaTriaje == NULL || cabezaTriaje->paciente->prioridad > p->prioridad) {
        // Si la cola está vacía o el nuevo paciente tiene mayor urgencia
        nuevo->siguiente = cabezaTriaje;
        cabezaTriaje = nuevo;                 // Se convierte en el nuevo primero
    } else {
        NodoTriaje* actual = cabezaTriaje;
        while (actual->siguiente != NULL && actual->siguiente->paciente->prioridad <= p->prioridad) {
            actual = actual->siguiente;       // Avanza mientras encuentre igual o menor prioridad
        }
        // Inserta el nuevo nodo en la posición adecuada
        nuevo->siguiente = actual->siguiente;
        actual->siguiente = nuevo;
    }

    cout << ESMERALDA << "Paciente encolado en triaje correctamente.\n" << RESET;
}

// Desencola y atiende al primer paciente (mayor gravedad)
void desencolarYAtenderPaciente() {
    cout << ESMERALDA << "\n--- Desencolar y atender paciente en consultorio ---\n" << RESET;
    
    // Verifica si la cola está vacía
    if (cabezaTriaje == NULL) {
        cout << AMBAR << "La cola de triaje esta vacia.\n" << RESET;
        return;
    }

    NodoTriaje* temp = cabezaTriaje;          // Toma el primer nodo (mayor prioridad)
    cabezaTriaje = cabezaTriaje->siguiente;   // Avanza el inicio de la cola
    
    // Muestra información del paciente atendido
    cout << ESMERALDA << "Atendiendo paciente:\n" << RESET;
    cout << "ID: " << BLANCO_BR << temp->paciente->id << RESET
         << ", Nombre: " << temp->paciente->nombre
         << ", Gravedad: " << temp->paciente->prioridad << "\n";

    delete temp;    // Libera el nodo atendido
}

// Muestra la cola de triaje
void mostrarColaTriaje() {
    cout << ESMERALDA << "\n--- Cola de triaje (ordenada por gravedad) ---\n" << RESET;
    
    // Si está vacía, lo indica
    if (cabezaTriaje == NULL) {
        cout << AMBAR << "La cola de triaje esta vacia.\n" << RESET;
        return;
    }

    graficarColaTriaje();
    
    // Recorre la cola e imprime cada paciente
    NodoTriaje* actual = cabezaTriaje;
    while (actual != NULL) {
        cout << ESMERALDA << "  * " << RESET
             << "ID: " << BLANCO_BR << actual->paciente->id << RESET
             << ", Nombre: " << actual->paciente->nombre
             << ", Gravedad: " << actual->paciente->prioridad << "\n";
        actual = actual->siguiente;
    }
}

//---------------- 3. GESTOR DE INSUMOS / MEDICAMENTOS (PILA) ----------------//

// Asigna insumos o dosis médicas a un paciente (push a pila)
void asignarInsumos() {
    cout << VIOLETA << "\n--- Asignar insumos / medicamentos a paciente ---\n" << RESET;

    int id;
    do {
        cout << "Ingrese ID del paciente: ";
        if (!(cin >> id) || id <= 0) {
            cout << ROJO_ALERTA << "Error: Debe ingresar un numero entero positivo.\n" << RESET;
            id = -1;
        }
        cin.clear();
        cin.ignore(10000, '\n');
    } while (id <= 0);

    // Buscar el paciente por su ID en la lista de pacientes
    Paciente* p = buscarPacientePorID(id);
    if (p == NULL) {
        cout << ROJO_ALERTA << "Error: No existe un paciente con ID " << id << " en la lista.\n" << RESET;
        return;
    }

    int cantidad;
    do {
        cout << "Ingrese cantidad de dosis / insumos a asignar: ";
        if (!(cin >> cantidad) || cantidad <= 0) {
            cout << ROJO_ALERTA << "Error: Debe ingresar un numero entero positivo.\n" << RESET;
            cantidad = -1;
        }
        cin.clear();
        cin.ignore(10000, '\n');
    } while (cantidad <= 0);

    // Crear un nuevo bloque de insumos
    BloqueInsumo* nuevo = new BloqueInsumo;
    nuevo->paciente = p;                  // Asociar el bloque con el paciente
    nuevo->cantidad = cantidad;           // Establecer la cantidad asignada
    nuevo->siguiente = topeInsumos;       // Insertar al inicio de la pila
    topeInsumos = nuevo;                  // El nuevo bloque es ahora el tope de la pila

    cout << ESMERALDA << "Insumos asignados al paciente " << id << " (cantidad: " << cantidad << " u.)\n" << RESET;
}

// Libera el bloque superior de la pila de insumos (último asignado)
void liberarInsumos() {
    cout << VIOLETA << "\n--- Liberar / retirar ultimo insumo asignado ---\n" << RESET;

    if (topeInsumos == NULL) {
        cout << AMBAR << "No hay insumos asignados para liberar.\n" << RESET;
        return;
    }

    BloqueInsumo* temp = topeInsumos;     // Guardar el bloque superior de la pila
    topeInsumos = topeInsumos->siguiente; // Actualizar el tope de la pila

    cout << AMBAR << "Insumos liberados del paciente con ID " << BLANCO_BR << temp->paciente->id << RESET
         << AMBAR << " (cantidad: " << temp->cantidad << " u.)\n" << RESET;
    
    delete temp;  // Eliminar el bloque de insumos
}

// Muestra el estado actual de los insumos asignados
void estadoInsumos() {
    cout << VIOLETA << "\n--- Estado actual de insumos asignados ---\n" << RESET;

    if (topeInsumos == NULL) {
        cout << AMBAR << "No hay insumos asignados actualmente.\n" << RESET;
        return;
    }

    graficarPilaInsumos();

    int total = 0;
    BloqueInsumo* actual = topeInsumos;
    while (actual != NULL) {
        cout << VIOLETA << "  * " << RESET
             << "Paciente " << BLANCO_BR << actual->paciente->id << RESET
             << " - Nombre: " << actual->paciente->nombre
             << " - Cantidad: " << actual->cantidad << " u.\n";
        total += actual->cantidad;
        actual = actual->siguiente;
    }
    cout << ESMERALDA << "\nTotal de dosis/insumos en uso: " << total << " unidades\n" << RESET;
}

//---------------- MENÚ PRINCIPAL ----------------//

void menu() {
    int opcionPrincipal;

    do {
        do {
            cout << AZUL_MARINO << "\n=============================================\n" << RESET;
            cout << TURQUESA    << "    SISTEMA DE GESTION HOSPITALARIA (C++)    \n" << RESET;
            cout << AZUL_MARINO << "=============================================\n" << RESET;
            cout << BLANCO_BR << "1. " << RESET << "Gestor de Pacientes " << TURQUESA << "(Lista Enlazada)\n" << RESET;
            cout << BLANCO_BR << "2. " << RESET << "Sala de Triaje " << ESMERALDA << "(Cola de Prioridad)\n" << RESET;
            cout << BLANCO_BR << "3. " << RESET << "Gestor de Insumos " << VIOLETA << "(Pila LIFO)\n" << RESET;
            cout << BLANCO_BR << "4. " << ROJO_ALERTA << "Salir\n" << RESET;
            cout << AZUL_MARINO << "---------------------------------------------\n" << RESET;
            cout << "Seleccione una opcion (1-4): ";

            if (!(cin >> opcionPrincipal) || opcionPrincipal < 1 || opcionPrincipal > 4) {
                cout << ROJO_ALERTA << "Opcion invalida. Por favor ingrese un numero entre 1 y 4.\n" << RESET;
                opcionPrincipal = -1;
            }
            cin.clear();
            cin.ignore(10000, '\n');
        } while (opcionPrincipal < 1 || opcionPrincipal > 4);

        switch (opcionPrincipal) {
            case 1: {
                int opcionPaciente;
                do {
                    cout << TURQUESA << "\n--- Gestor de Pacientes ---\n" << RESET;
                    cout << "1. Registrar nuevo paciente\n";
                    cout << "2. Eliminar paciente\n";
                    cout << "3. Mostrar todos los pacientes\n";
                    cout << "4. Volver al menu principal\n";
                    cout << "Seleccione una opcion (1-4): ";
                    if (!(cin >> opcionPaciente)) opcionPaciente = -1;
                    cin.clear();
                    cin.ignore(10000, '\n');

                    switch (opcionPaciente) {
                        case 1: insertarPaciente(); break;
                        case 2: eliminarPaciente(); break;
                        case 3: mostrarPacientes(); break;
                        case 4: system("cls"); continue;
                        default: cout << ROJO_ALERTA << "Opcion invalida.\n" << RESET; break;
                    }

                    system("pause"); // Espera una tecla
                    system("cls");   // Limpia pantalla
                } while (opcionPaciente != 4);
                break;
            }
            case 2: {
                int opcionTriaje;
                do {
                    cout << ESMERALDA << "\n=== Sala de Triaje (Cola de Prioridad) ===\n" << RESET;
                    cout << "1. Encolar paciente por ID\n";
                    cout << "2. Desencolar y atender paciente\n";
                    cout << "3. Mostrar cola de triaje actual\n";
                    cout << "4. Volver al menu principal\n";
                    cout << "Seleccione una opcion (1-4): ";
                    if (!(cin >> opcionTriaje)) opcionTriaje = -1;
                    cin.clear();
                    cin.ignore(10000, '\n');

                    switch (opcionTriaje) {
                        case 1: encolarPacienteEnTriaje(); break;
                        case 2: desencolarYAtenderPaciente(); break;
                        case 3: mostrarColaTriaje(); break;
                        case 4: system("cls"); continue;
                        default: cout << ROJO_ALERTA << "Opcion invalida.\n" << RESET; break;
                    }

                    system("pause");
                    system("cls");
                } while (opcionTriaje != 4);
                break;
            }
            case 3: {
                int opcionInsumo;
                do {
                    cout << VIOLETA << "\n--- Gestor de Insumos Medicos ---\n" << RESET;
                    cout << "1. Asignar insumos a paciente (push)\n";
                    cout << "2. Liberar insumos (pop)\n";
                    cout << "3. Ver estado actual de insumos\n";
                    cout << "4. Volver al menu principal\n";
                    cout << "Seleccione una opcion (1-4): ";
                    if (!(cin >> opcionInsumo)) opcionInsumo = -1;
                    cin.clear();
                    cin.ignore(10000, '\n');

                    switch (opcionInsumo) {
                        case 1: asignarInsumos(); break;
                        case 2: liberarInsumos(); break;
                        case 3: estadoInsumos(); break;
                        case 4: system("cls"); continue;
                        default: cout << ROJO_ALERTA << "Opcion invalida.\n" << RESET; break;
                    }

                    system("pause");
                    system("cls");
                } while (opcionInsumo != 4);
                break;
            }
            case 4:
                cout << ESMERALDA << "Saliendo del programa...\n" << RESET;
                break;
        }

    } while (opcionPrincipal != 4);
}

// Función principal del programa
int main() {
    system("cls"); // Limpia la consola al iniciar para habilitar colores ANSI en Windows
    menu();        // Llama al menú principal
    return 0;
}