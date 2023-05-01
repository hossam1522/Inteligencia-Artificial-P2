#include "../Comportamientos_Jugador/jugador.hpp"
#include "motorlib/util.h"

#include <iostream>
#include <cmath>
#include <set>
#include <stack>
#include <queue>
#include <cmath>


/* ..................................Declaración nivel 0................................................ */
list<Action> AnchuraSoloJugador(const stateN0 &inicio, const ubicacion &final, const vector<vector<unsigned char> > &mapa);
bool CasillaTransitable(const ubicacion &x, const vector<vector<unsigned char> > &mapa);
ubicacion NextCasilla(const ubicacion &pos);
stateN0 apply(const Action &a, const stateN0 &st, const vector<vector<unsigned char> > &mapa);
bool Find(const stateN0 &item, const list<stateN0> &lista);
bool Find(const stateN0 &item, const list<nodeN0> &lista);
void AnularMatriz(vector<vector<unsigned char>> &matriz);

/* ..................................Declaración nivel 1................................................ */
bool SON_aLaVista(const stateN0 &st);
list<Action> AnchuraAmbos(const stateN0 &inicio, const ubicacion &final, const vector<vector<unsigned char> > &mapa);

/* ..................................Declaración nivel 2................................................ */
nodeN2 apply(const Action &a, const nodeN2 &n, const vector<vector<unsigned char> > &mapa);
struct ComparaCoste{
	bool operator()(const nodeN2 &n1, const nodeN2 &n2) const{
		return n1.coste > n2.coste;
	}
};
list<Action> DijkstraSoloJugador(const stateN0 &inicio, const ubicacion &final, const vector<vector<unsigned char> > &mapa);

/* ..................................Declaración nivel 3................................................ */


/* ..................................Declaración nivel 4................................................ */



// Este es el método principal que se piden en la practica.
// Tiene como entrada la información de los sensores y devuelve la acción a realizar.
// Para ver los distintos sensores mirar fichero "comportamiento.hpp"
Action ComportamientoJugador::think(Sensores sensores)
{
	Action accion = actIDLE;

	if (sensores.nivel != 4){
		if (!hayPlan){
			// Invocar al método de búsqueda
			cout << "Calculando un nuevo plan..." << endl;
			c_state.jugador.f = sensores.posF;
			c_state.jugador.c = sensores.posC;
			c_state.jugador.brujula = sensores.sentido;
			c_state.sonambulo.f = sensores.SONposF;
			c_state.sonambulo.c = sensores.SONposC;
			c_state.sonambulo.brujula = sensores.SONsentido;
			/* if (sensores.terreno[0] == 'K')
				c_state.tiene_bikini = true;
			else	
				c_state.tiene_bikini = false;
			if (sensores.terreno[0] == 'D')
				c_state.tiene_zapatillas = true;
			else	
				c_state.tiene_zapatillas = false; */
			goal.f = sensores.destinoF;
			goal.c = sensores.destinoC;

			switch (sensores.nivel){
				case 0:
					plan = AnchuraSoloJugador(c_state, goal, mapaResultado);
					break;
				case 1:
					plan = AnchuraAmbos(c_state, goal, mapaResultado);
					break;
				case 2:
					plan = DijkstraSoloJugador(c_state, goal, mapaResultado);
					break;
				case 3:
					cout << "Nivel 3 no implementado" << endl;
					break;
			}
			if (plan.size() > 0){
				VisualizaPlan(c_state, plan);
				hayPlan = true;
			}
		}
		if (hayPlan and plan.size()>0){
			cout << "Ejecutando la siguiente acción del plan" << endl;
			accion = plan.front();
			plan.pop_front();
		}
		if (plan.size()== 0){
			cout << "Se completó el plan" << endl;
			hayPlan = false;
		}

	}
	else{
		cout << "Nivel 4 no implementado" << endl;
	}

	return accion;
}

/* ..................................Implementación nivel 0................................................ */

int ComportamientoJugador::interact(Action accion, int valor)
{
	return false;
}

list<Action> AnchuraSoloJugador(const stateN0 &inicio, const ubicacion &final, const vector<vector<unsigned char> > &mapa){
	nodeN0 current_node;
	list<nodeN0> frontier;
	set<nodeN0> explored;
	list<Action> plan;
	current_node.st = inicio;
	bool SolutionFound = (current_node.st.jugador.f == final.f && current_node.st.jugador.c == final.c);
	frontier.push_back(current_node);

	while (!frontier.empty() && !SolutionFound){
		frontier.pop_front();
		explored.insert(current_node);

		// Generar hijo actFORWARD
		nodeN0 child_forward = current_node;
		child_forward.st = apply(actFORWARD, current_node.st, mapa);
		if (child_forward.st.jugador.f == final.f && child_forward.st.jugador.c == final.c){
			child_forward.secuencia.push_back(actFORWARD);
			current_node = child_forward;
			SolutionFound = true;
		}
		else if (explored.find(child_forward) == explored.end()){
			child_forward.secuencia.push_back(actFORWARD);
			frontier.push_back(child_forward);
		}

		if (!SolutionFound){
			// Generar hijo actTURN_L
			nodeN0 child_turnl = current_node;
			child_turnl.st = apply(actTURN_L, current_node.st, mapa);
			if (explored.find(child_turnl) == explored.end()){
				child_turnl.secuencia.push_back(actTURN_L);
				frontier.push_back(child_turnl);
			}

			// Generar hijo actTURN_R
			nodeN0 child_turnr = current_node;
			child_turnr.st = apply(actTURN_R, current_node.st, mapa);
			if (explored.find(child_turnr) == explored.end()){
				child_turnr.secuencia.push_back(actTURN_R);
				frontier.push_back(child_turnr);
			}
		}

		if (!SolutionFound && !frontier.empty()){
			current_node = frontier.front();
			while (!frontier.empty() && explored.find(current_node) != explored.end()){
				frontier.pop_front();
				if (!frontier.empty())
					current_node = frontier.front();
			}
		}
	}

	if (SolutionFound){
		plan = current_node.secuencia;
	}

	return plan;
}

bool CasillaTransitable(const ubicacion &x, const vector<vector<unsigned char> > &mapa){
	return (mapa[x.f][x.c] != 'P' && mapa[x.f][x.c] != 'M');
}

ubicacion NextCasilla(const ubicacion &pos){
	ubicacion next = pos;
	switch (pos.brujula){
		case norte:
			next.f = pos.f - 1;
			break;
		case noreste:
			next.f = pos.f - 1;
			next.c = pos.c + 1;
			break;
		case este:
			next.c = pos.c + 1;
			break;
		case sureste:
			next.f = pos.f + 1;
			next.c = pos.c + 1;
			break;
		case sur:
			next.f = pos.f + 1;
			break;
		case suroeste:
			next.f = pos.f + 1;
			next.c = pos.c - 1;
			break;
		case oeste:
			next.c = pos.c - 1;
			break;
		case noroeste:
			next.f = pos.f - 1;
			next.c = pos.c - 1;
			break;
		default:
			break;
	}

	return next;
}

stateN0 apply(const Action &a, const stateN0 &st, const vector<vector<unsigned char> > &mapa){
	stateN0 st_result = st;
	ubicacion sig_ubicacion;
	switch (a){
		case actFORWARD:
			sig_ubicacion = NextCasilla(st.jugador);
			if (CasillaTransitable(sig_ubicacion, mapa) && !(sig_ubicacion.f == st.sonambulo.f && sig_ubicacion.c == st.sonambulo.c)){
				st_result.jugador = sig_ubicacion;
			}
			break;
		case actTURN_L:
			st_result.jugador.brujula = static_cast<Orientacion>((st.jugador.brujula + 6) % 8);
			break;
		case actTURN_R:
			st_result.jugador.brujula = static_cast<Orientacion>((st.jugador.brujula + 2) % 8);
			break;
		case actSON_FORWARD:
			sig_ubicacion = NextCasilla(st.sonambulo);
			if (CasillaTransitable(sig_ubicacion, mapa) && !(sig_ubicacion.f == st.jugador.f && sig_ubicacion.c == st.jugador.c)){
				st_result.sonambulo = sig_ubicacion;
			}
			break;
		case actSON_TURN_SL:
			st_result.sonambulo.brujula = static_cast<Orientacion>((st.sonambulo.brujula + 7) % 8);
			break;
		case actSON_TURN_SR:
			st_result.sonambulo.brujula = static_cast<Orientacion>((st.sonambulo.brujula + 1) % 8);
			break;
	}
	return st_result;
}

bool Find(const stateN0 &item, const list<stateN0> &lista){
	auto it = lista.begin();
	while (it != lista.end() && !(*it == item))
		it++;

	return (!(it == lista.end()));
}

bool Find(const stateN0 &item, const list<nodeN0> &lista){
	auto it = lista.begin();
	while (it != lista.end() && !(it->st == item))
		it++;

	return (!(it == lista.end()));
}

void AnularMatriz(vector<vector<unsigned char>> &matriz){
	for (int i = 0; i < matriz.size(); i++){
		for (int j = 0; j < matriz[i].size(); j++){
			matriz[i][j] = 0;
		}
	}
}


/* ..................................Implementación nivel 1................................................ */

bool SON_aLaVista(const stateN0 &st){
	bool aLaVista = false;
	ubicacion pos = st.jugador;

	switch (pos.brujula){
		case norte:
		  if (pos.f - st.sonambulo.f == 1 && abs(st.sonambulo.c - pos.c) <= 1)
				aLaVista = true;
			else if (pos.f - st.sonambulo.f == 2 && abs(st.sonambulo.c - pos.c) <= 2)
				aLaVista = true;
			else if (pos.f - st.sonambulo.f == 3 && abs(st.sonambulo.c - pos.c) <= 3)
				aLaVista = true;
			break;
		case noreste:
			if (pos.f - st.sonambulo.f <=3 && st.sonambulo.c - pos.c <= 3 && pos.f - st.sonambulo.f >= 1 && st.sonambulo.c - pos.c >= 1)
				aLaVista = true;
			break;
		case este:
			if (st.sonambulo.c - pos.c == 1 && abs(st.sonambulo.f - pos.f) <= 1)
				aLaVista = true;
			else if (st.sonambulo.c - pos.c == 2 && abs(st.sonambulo.f - pos.f) <= 2)
				aLaVista = true;
			else if (st.sonambulo.c - pos.c == 3 && abs(st.sonambulo.f - pos.f) <= 3)
				aLaVista = true;
			break;
		case sureste:
			if (st.sonambulo.f - pos.f <= 3 && st.sonambulo.f - pos.f >= 1 && st.sonambulo.c - pos.c <= 3 && st.sonambulo.c - pos.c >= 1)
				aLaVista = true;
			break;
		case sur:
			if (st.sonambulo.f - pos.f == 1 && abs(st.sonambulo.c - pos.c) <= 1)
				aLaVista = true;
			else if (st.sonambulo.f - pos.f == 2 && abs(st.sonambulo.c - pos.c) <= 2)
				aLaVista = true;
			else if (st.sonambulo.f - pos.f == 3 && abs(st.sonambulo.c - pos.c) <= 3)
				aLaVista = true;
			break;
		case suroeste:
			if (st.sonambulo.f - pos.f <= 3 && st.sonambulo.f - pos.f >= 1 && pos.c - st.sonambulo.c <= 3 && pos.c - st.sonambulo.c >= 1)
				aLaVista = true;
			break;
		case oeste:
			if (pos.c - st.sonambulo.c == 1 && abs(st.sonambulo.f - pos.f) <=1)
				aLaVista = true;
			else if (pos.c - st.sonambulo.c == 2 && abs(st.sonambulo.f - pos.f) <= 2)
				aLaVista = true;
			else if (pos.c - st.sonambulo.c == 3 && abs(st.sonambulo.f - pos.f) <= 3)
				aLaVista = true;
			break;
		case noroeste:
			if (pos.f - st.sonambulo.f <= 3 && pos.f - st.sonambulo.f >= 1 && pos.c - st.sonambulo.c <= 3 && pos.c - st.sonambulo.c >= 1)
				aLaVista = true;
			break;
	}

	return aLaVista;
}

list<Action> AnchuraAmbos(const stateN0 &inicio, const ubicacion &final, const vector<vector<unsigned char> > &mapa){
	nodeN1 current_node;
	list<nodeN1> frontier;
	set<nodeN1> explored;
	list<Action> plan;
	current_node.st = inicio;
	bool SolutionFound = (current_node.st.sonambulo.f == final.f && current_node.st.sonambulo.c == final.c);
	frontier.push_back(current_node);

	while (!frontier.empty() && !SolutionFound){
		frontier.pop_front();
		explored.insert(current_node);

		if (!SolutionFound){

			if (SON_aLaVista(current_node.st)){

				// Generar hijo actSON_FORWARD
				nodeN1 child_SON_forward = current_node;
				child_SON_forward.st = apply(actSON_FORWARD, current_node.st, mapa);
				if (child_SON_forward.st.sonambulo.f == final.f && child_SON_forward.st.sonambulo.c == final.c){
					child_SON_forward.secuencia.push_back(actSON_FORWARD);
					current_node = child_SON_forward;
					SolutionFound = true;
				}
				else if (explored.find(child_SON_forward) == explored.end()){
					child_SON_forward.secuencia.push_back(actSON_FORWARD);
					frontier.push_back(child_SON_forward);
				}

				// Generar hijo actSON_TURN_SL
				nodeN1 child_SON_turnsl = current_node;
				child_SON_turnsl.st = apply(actSON_TURN_SL, current_node.st, mapa);
				if (explored.find(child_SON_turnsl) == explored.end() ){
					child_SON_turnsl.secuencia.push_back(actSON_TURN_SL);
					frontier.push_back(child_SON_turnsl);
				}

				// Generar hijo actSON_TURN_SR
				nodeN1 child_SON_turnsr = current_node;
				child_SON_turnsr.st = apply(actSON_TURN_SR, current_node.st, mapa);
				if (explored.find(child_SON_turnsr) == explored.end() ){
					child_SON_turnsr.secuencia.push_back(actSON_TURN_SR);
					frontier.push_back(child_SON_turnsr);
				}

			} else {

				// Generar hijo actFORWARD
				nodeN1 child_forward = current_node;
				child_forward.st = apply(actFORWARD, current_node.st, mapa);
				if (SON_aLaVista(child_forward.st)){
					child_forward.secuencia.push_back(actFORWARD);
					frontier.push_back(child_forward);
					current_node = child_forward;
				}
				else if (explored.find(child_forward) == explored.end()){
					child_forward.secuencia.push_back(actFORWARD);
					frontier.push_back(child_forward);
				}

				// Generar hijo actTURN_L
				nodeN1 child_turnl = current_node;
				child_turnl.st = apply(actTURN_L, current_node.st, mapa);
				if (explored.find(child_turnl) == explored.end() && !SON_aLaVista(current_node.st)){
					child_turnl.secuencia.push_back(actTURN_L);
					frontier.push_back(child_turnl);
				}

				// Generar hijo actTURN_R
				nodeN1 child_turnr = current_node;
				child_turnr.st = apply(actTURN_R, current_node.st, mapa);
				if (explored.find(child_turnr) == explored.end() && !SON_aLaVista(current_node.st) ){
					child_turnr.secuencia.push_back(actTURN_R);
					frontier.push_back(child_turnr);
				}

			}
		}

		if (!SolutionFound && !frontier.empty()){
			current_node = frontier.front();
			while (!frontier.empty() && explored.find(current_node) != explored.end()){
				frontier.pop_front();
				if (!frontier.empty())
					current_node = frontier.front();
			}
		}
	}

	if (SolutionFound){
		plan = current_node.secuencia;
	}
	else {
		cout << "No se ha encontrado solución" << endl;
	}

	return plan;
}


/* ..................................Implementación nivel 2................................................ */

nodeN2 apply(const Action &a, const nodeN2 &n, const vector<vector<unsigned char> > &mapa){
	/* nodeN2 n_result = n;
	ubicacion sig_ubicacion;
	switch (a){
		case actFORWARD:
			sig_ubicacion = NextCasilla(n.n.st.jugador);
			if (CasillaTransitable(sig_ubicacion, mapa) && !(sig_ubicacion.f == n.n.st.sonambulo.f && sig_ubicacion.c == n.n.st.sonambulo.c)){
				n_result.n.st.jugador = sig_ubicacion;
				if (mapa[sig_ubicacion.f][sig_ubicacion.c] == 'K' && !n.n.st.tiene_bikini)
					n_result.n.st.tiene_bikini = true;
				if (mapa[sig_ubicacion.f][sig_ubicacion.c] == 'D' && !n.n.st.tiene_zapatillas)
					n_result.n.st.tiene_zapatillas = true;
			}
			if (mapa[n_result.n.st.jugador.f][n_result.n.st.jugador.c] == 'A')
				if (n_result.n.st.tiene_bikini)
					n_result.coste += 10;
				else
					n_result.coste += 100;
			else if (mapa[n_result.n.st.jugador.f][n_result.n.st.jugador.c] == 'B')
				if (n_result.n.st.tiene_zapatillas)
					n_result.coste += 15;
				else
					n_result.coste += 50;
			else if (mapa[n_result.n.st.jugador.f][n_result.n.st.jugador.c] == 'T')
				n_result.coste += 2;
			else
				n_result.coste += 1;
			break;

		case actTURN_L:
			n_result.n.st.jugador.brujula = static_cast<Orientacion>((n.n.st.jugador.brujula + 6) % 8);
			if (mapa[n_result.n.st.jugador.f][n_result.n.st.jugador.c] == 'A')
				if (n_result.n.st.tiene_bikini)
					n_result.coste += 5;
				else
					n_result.coste += 25;
			else if (mapa[n_result.n.st.jugador.f][n_result.n.st.jugador.c] == 'B')
				if (n_result.n.st.tiene_zapatillas)
					n_result.coste += 1;
				else
					n_result.coste += 5;
			else if (mapa[n_result.n.st.jugador.f][n_result.n.st.jugador.c] == 'T')
				n_result.coste += 2;
			else
				n_result.coste += 1;
			break;

		case actTURN_R:
			n_result.n.st.jugador.brujula = static_cast<Orientacion>((n.n.st.jugador.brujula + 2) % 8);
			if (mapa[n_result.n.st.jugador.f][n_result.n.st.jugador.c] == 'A')
				if (n_result.n.st.tiene_bikini)
					n_result.coste += 5;
				else
					n_result.coste += 25;
			else if (mapa[n_result.n.st.jugador.f][n_result.n.st.jugador.c] == 'B')
				if (n_result.n.st.tiene_zapatillas)
					n_result.coste += 1;
				else
					n_result.coste += 5;
			else if (mapa[n_result.n.st.jugador.f][n_result.n.st.jugador.c] == 'T')
				n_result.coste += 2;
			else
				n_result.coste += 1;
			break;

		case actSON_FORWARD:
			sig_ubicacion = NextCasilla(n.n.st.sonambulo);
			if (CasillaTransitable(sig_ubicacion, mapa) && !(sig_ubicacion.f == n.n.st.jugador.f && sig_ubicacion.c == n.n.st.jugador.c)){
				n_result.n.st.sonambulo = sig_ubicacion;
				if (mapa[sig_ubicacion.f][sig_ubicacion.c] == 'K')
					n_result.n.st.tiene_bikini = true;
				if (mapa[sig_ubicacion.f][sig_ubicacion.c] == 'D')
					n_result.n.st.tiene_zapatillas = true;
			}
			if (mapa[n_result.n.st.sonambulo.f][n_result.n.st.sonambulo.c] == 'A')
				if (n_result.n.st.tiene_bikini)
					n_result.coste += 10;
				else
					n_result.coste += 100;
			else if (mapa[n_result.n.st.sonambulo.f][n_result.n.st.sonambulo.c] == 'B')
				if (n_result.n.st.tiene_zapatillas)
					n_result.coste += 15;
				else
					n_result.coste += 50;
			else if (mapa[n_result.n.st.sonambulo.f][n_result.n.st.sonambulo.c] == 'T')
				n_result.coste += 2;
			else
				n_result.coste += 1;
			break;

		case actSON_TURN_SL:
			n_result.n.st.sonambulo.brujula = static_cast<Orientacion>((n.n.st.sonambulo.brujula + 7) % 8);
			if (mapa[n_result.n.st.sonambulo.f][n_result.n.st.sonambulo.c] == 'A')
				if (n_result.n.st.tiene_bikini)
					n_result.coste += 2;
				else
					n_result.coste += 7;
			else if (mapa[n_result.n.st.sonambulo.f][n_result.n.st.sonambulo.c] == 'B')
				if (n_result.n.st.tiene_zapatillas)
					n_result.coste += 1;
				else
					n_result.coste += 3;
			else if (mapa[n_result.n.st.sonambulo.f][n_result.n.st.sonambulo.c] == 'T')
				n_result.coste += 1;
			else
				n_result.coste += 1;
			break;

		case actSON_TURN_SR:
			n_result.n.st.sonambulo.brujula = static_cast<Orientacion>((n.n.st.sonambulo.brujula + 1) % 8);
			if (mapa[n_result.n.st.sonambulo.f][n_result.n.st.sonambulo.c] == 'A')
				if (n_result.n.st.tiene_bikini)
					n_result.coste += 2;
				else
					n_result.coste += 7;
			else if (mapa[n_result.n.st.sonambulo.f][n_result.n.st.sonambulo.c] == 'B')
				if (n_result.n.st.tiene_zapatillas)
					n_result.coste += 1;
				else
					n_result.coste += 3;
			else if (mapa[n_result.n.st.sonambulo.f][n_result.n.st.sonambulo.c] == 'T')
				n_result.coste += 1;
			else
				n_result.coste += 1;
			break;
	}
	return n_result; */

	nodeN2 n_result = n;
	ubicacion sig_ubicacion;
	switch (a){
		case actFORWARD:
			sig_ubicacion = NextCasilla(n.n.st.jugador);
			if (CasillaTransitable(sig_ubicacion, mapa) && !(sig_ubicacion.f == n.n.st.sonambulo.f && sig_ubicacion.c == n.n.st.sonambulo.c)){
				n_result.n.st.jugador = sig_ubicacion;
				if (mapa[sig_ubicacion.f][sig_ubicacion.c] == 'K' && !n.tiene_bikini)
					n_result.tiene_bikini = true;
				if (mapa[sig_ubicacion.f][sig_ubicacion.c] == 'D' && !n.tiene_zapatillas)
					n_result.tiene_zapatillas = true;
			}
			if (mapa[n_result.n.st.jugador.f][n_result.n.st.jugador.c] == 'A')
				if (n_result.tiene_bikini)
					n_result.coste += 10;
				else
					n_result.coste += 100;
			else if (mapa[n_result.n.st.jugador.f][n_result.n.st.jugador.c] == 'B')
				if (n_result.tiene_zapatillas)
					n_result.coste += 15;
				else
					n_result.coste += 50;
			else if (mapa[n_result.n.st.jugador.f][n_result.n.st.jugador.c] == 'T')
				n_result.coste += 2;
			else
				n_result.coste += 1;
			break;

		case actTURN_L:
			n_result.n.st.jugador.brujula = static_cast<Orientacion>((n.n.st.jugador.brujula + 6) % 8);
			if (mapa[n_result.n.st.jugador.f][n_result.n.st.jugador.c] == 'A')
				if (n_result.tiene_bikini)
					n_result.coste += 5;
				else
					n_result.coste += 25;
			else if (mapa[n_result.n.st.jugador.f][n_result.n.st.jugador.c] == 'B')
				if (n_result.tiene_zapatillas)
					n_result.coste += 1;
				else
					n_result.coste += 5;
			else if (mapa[n_result.n.st.jugador.f][n_result.n.st.jugador.c] == 'T')
				n_result.coste += 2;
			else
				n_result.coste += 1;
			break;

		case actTURN_R:
			n_result.n.st.jugador.brujula = static_cast<Orientacion>((n.n.st.jugador.brujula + 2) % 8);
			if (mapa[n_result.n.st.jugador.f][n_result.n.st.jugador.c] == 'A')
				if (n_result.tiene_bikini)
					n_result.coste += 5;
				else
					n_result.coste += 25;
			else if (mapa[n_result.n.st.jugador.f][n_result.n.st.jugador.c] == 'B')
				if (n_result.tiene_zapatillas)
					n_result.coste += 1;
				else
					n_result.coste += 5;
			else if (mapa[n_result.n.st.jugador.f][n_result.n.st.jugador.c] == 'T')
				n_result.coste += 2;
			else
				n_result.coste += 1;
			break;

		case actSON_FORWARD:
			sig_ubicacion = NextCasilla(n.n.st.sonambulo);
			if (CasillaTransitable(sig_ubicacion, mapa) && !(sig_ubicacion.f == n.n.st.jugador.f && sig_ubicacion.c == n.n.st.jugador.c)){
				n_result.n.st.sonambulo = sig_ubicacion;
				if (mapa[sig_ubicacion.f][sig_ubicacion.c] == 'K')
					n_result.tiene_bikini = true;
				if (mapa[sig_ubicacion.f][sig_ubicacion.c] == 'D')
					n_result.tiene_zapatillas = true;
			}
			if (mapa[n_result.n.st.sonambulo.f][n_result.n.st.sonambulo.c] == 'A')
				if (n_result.tiene_bikini)
					n_result.coste += 10;
				else
					n_result.coste += 100;
			else if (mapa[n_result.n.st.sonambulo.f][n_result.n.st.sonambulo.c] == 'B')
				if (n_result.tiene_zapatillas)
					n_result.coste += 15;
				else
					n_result.coste += 50;
			else if (mapa[n_result.n.st.sonambulo.f][n_result.n.st.sonambulo.c] == 'T')
				n_result.coste += 2;
			else
				n_result.coste += 1;
			break;

		case actSON_TURN_SL:
			n_result.n.st.sonambulo.brujula = static_cast<Orientacion>((n.n.st.sonambulo.brujula + 7) % 8);
			if (mapa[n_result.n.st.sonambulo.f][n_result.n.st.sonambulo.c] == 'A')
				if (n_result.tiene_bikini)
					n_result.coste += 2;
				else
					n_result.coste += 7;
			else if (mapa[n_result.n.st.sonambulo.f][n_result.n.st.sonambulo.c] == 'B')
				if (n_result.tiene_zapatillas)
					n_result.coste += 1;
				else
					n_result.coste += 3;
			else if (mapa[n_result.n.st.sonambulo.f][n_result.n.st.sonambulo.c] == 'T')
				n_result.coste += 1;
			else
				n_result.coste += 1;
			break;

		case actSON_TURN_SR:
			n_result.n.st.sonambulo.brujula = static_cast<Orientacion>((n.n.st.sonambulo.brujula + 1) % 8);
			if (mapa[n_result.n.st.sonambulo.f][n_result.n.st.sonambulo.c] == 'A')
				if (n_result.tiene_bikini)
					n_result.coste += 2;
				else
					n_result.coste += 7;
			else if (mapa[n_result.n.st.sonambulo.f][n_result.n.st.sonambulo.c] == 'B')
				if (n_result.tiene_zapatillas)
					n_result.coste += 1;
				else
					n_result.coste += 3;
			else if (mapa[n_result.n.st.sonambulo.f][n_result.n.st.sonambulo.c] == 'T')
				n_result.coste += 1;
			else
				n_result.coste += 1;
			break;
	}
	return n_result;
}

list<Action> DijkstraSoloJugador(const stateN0 &inicio, const ubicacion &final, const vector<vector<unsigned char> > &mapa){
	nodeN2 current_node;
	priority_queue<nodeN2, vector<nodeN2>, ComparaCoste> frontier;
	set<nodeN2> explored;
	list<Action> plan;
	current_node.n.st = inicio;
	bool SolutionFound = (current_node.n.st.jugador.f == final.f && current_node.n.st.jugador.c == final.c);
	frontier.push(current_node);

	while (!frontier.empty() && !SolutionFound){
		frontier.pop();
		explored.insert(current_node);

		// Generar hijo actFORWARD
		nodeN2 child_forward = current_node;
		child_forward = apply(actFORWARD, current_node, mapa);
		if (child_forward.n.st.jugador.f == final.f && child_forward.n.st.jugador.c == final.c){
			child_forward.n.secuencia.push_back(actFORWARD);
			current_node = child_forward;
			SolutionFound = true;
		}
		else if (explored.find(child_forward) == explored.end()){
			child_forward.n.secuencia.push_back(actFORWARD);
			frontier.push(child_forward);
		} /* else {
			auto it = explored.find(child_forward);
			if (it->n == child_forward.n && (it->tiene_bikini != child_forward.tiene_bikini or 
			it->tiene_zapatillas != child_forward.tiene_zapatillas)){
				child_forward.n.secuencia.push_back(actFORWARD);
				frontier.push(child_forward);
			}
		} */

		if (!SolutionFound){
			// Generar hijo actTURN_L
			nodeN2 child_turnl = current_node;
			child_turnl = apply(actTURN_L, current_node, mapa);
			if (explored.find(child_turnl) == explored.end()){
				child_turnl.n.secuencia.push_back(actTURN_L);
				frontier.push(child_turnl);
			} /* else {
				auto it = explored.find(child_turnl);
				if (it->n == child_turnl.n && (it->tiene_bikini != child_turnl.tiene_bikini or
				it->tiene_zapatillas != child_turnl.tiene_zapatillas)){
					child_turnl.n.secuencia.push_back(actTURN_L);
					frontier.push(child_turnl);
				}
			} */

			// Generar hijo actTURN_R
			nodeN2 child_turnr = current_node;
			child_turnr = apply(actTURN_R, current_node, mapa);
			if (explored.find(child_turnr) == explored.end()){
				child_turnr.n.secuencia.push_back(actTURN_R);
				frontier.push(child_turnr);
			} /* else {
				auto it = explored.find(child_turnr);
				if (it->n == child_turnr.n && (it->tiene_bikini != child_turnr.tiene_bikini or
				it->tiene_zapatillas != child_turnr.tiene_zapatillas)){
					child_turnr.n.secuencia.push_back(actTURN_R);
					frontier.push(child_turnr);
				}
			} */
		}

		if (!SolutionFound && !frontier.empty()){
			current_node = frontier.top();
			while (!frontier.empty() && explored.find(current_node) != explored.end()){
				/* auto it = explored.find(current_node);
				if (it->n == current_node.n && (it->tiene_bikini == current_node.tiene_bikini &&
				it->tiene_zapatillas == current_node.tiene_zapatillas && it->coste <= current_node.coste)){ */
					frontier.pop();
					if (!frontier.empty())
						current_node = frontier.top();
				//}
			}
		}
	}

	if (SolutionFound){
		plan = current_node.n.secuencia;
	}

	return plan;
}

/* ..................................Implementación nivel 3................................................ */


/* ..................................Implementación nivel 4................................................ */


/* ..................................Implementación desde hpp.............................................. */

void ComportamientoJugador::VisualizaPlan(const stateN0 &st, const list<Action> &plan){
	AnularMatriz(mapaConPlan);
	stateN0 cst = st;

	auto it = plan.begin();
	while (it != plan.end()){
		switch (*it){
			case actFORWARD:
				cst.jugador = NextCasilla(cst.jugador);
				mapaConPlan[cst.jugador.f][cst.jugador.c] = 1;
				break;
			case actTURN_R:
				cst.jugador.brujula = (Orientacion)((cst.jugador.brujula + 2) % 8);
				break;
			case actTURN_L:
				cst.jugador.brujula = (Orientacion)((cst.jugador.brujula + 6) % 8);
				break;
			case actSON_FORWARD:
				cst.sonambulo = NextCasilla(cst.sonambulo);
				mapaConPlan[cst.sonambulo.f][cst.sonambulo.c] = 2;
				break;
			case actSON_TURN_SR:
				cst.sonambulo.brujula = (Orientacion)((cst.sonambulo.brujula + 1) % 8);
				break;
			case actSON_TURN_SL:
				cst.sonambulo.brujula = (Orientacion)((cst.sonambulo.brujula + 7) % 8);
				break;
		}
		it++;
	}
}


