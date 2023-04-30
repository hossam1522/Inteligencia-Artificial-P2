#ifndef COMPORTAMIENTOJUGADOR_H
#define COMPORTAMIENTOJUGADOR_H

#include "comportamientos/comportamiento.hpp"

#include <list>
#include <cmath>

struct stateN0{
  ubicacion jugador;
  ubicacion sonambulo;

  bool operator== (const stateN0 &x) const{
    if (jugador == x.jugador && sonambulo.f == x.sonambulo.f && sonambulo.c == x.sonambulo.c)
      return true;
    else
      return false;
  }
};

struct nodeN0{
  stateN0 st;
  list<Action> secuencia;

  bool operator== (const nodeN0 &n) const{
    return (st == n.st);
  }

  bool operator< (const nodeN0 &n) const{
    if (st.jugador.f < n.st.jugador.f)
      return true;
    else if (st.jugador.f == n.st.jugador.f && st.jugador.c < n.st.jugador.c)
    return true;
    else if (st.jugador.f == n.st.jugador.f && st.jugador.c == n.st.jugador.c && st.jugador.brujula < n.st.jugador.brujula)
      return true;
    else
      return false;
  }
};

struct nodeN1{
  stateN0 st;
  list<Action> secuencia;

  bool operator== (const nodeN0 &n) const{
    return (st == n.st);
  }

  bool operator< (const nodeN1 &n) const{
    if (st.sonambulo.f+st.jugador.f < n.st.sonambulo.f+n.st.jugador.f)
      return true;
    else if (st.sonambulo.f+st.jugador.f == n.st.sonambulo.f+n.st.jugador.f && st.sonambulo.c+st.jugador.c < n.st.sonambulo.c+n.st.jugador.c)
     return true;
    else if (st.sonambulo.f+st.jugador.f == n.st.sonambulo.f+n.st.jugador.f &&
             st.sonambulo.c+st.jugador.c == n.st.sonambulo.c+n.st.jugador.c &&
             (st.sonambulo.brujula+st.jugador.brujula) < (n.st.sonambulo.brujula+n.st.jugador.brujula))
      return true;
    else
      return false;
  }
};

struct nodeN2{
  stateN0 st;
  list<Action> secuencia;
  int coste;

  bool operator== (const nodeN0 &n) const{
    return (st == n.st);
  }

  bool operator< (const nodeN2 &n) const{
    return (coste < n.coste);
  }
};


class ComportamientoJugador : public Comportamiento {
  public:
    ComportamientoJugador(unsigned int size) : Comportamiento(size) {
      // Inicializar Variables de Estado
    }
    ComportamientoJugador(std::vector< std::vector< unsigned char> > mapaR) : Comportamiento(mapaR) {
      // Inicializar Variables de Estado
      hayPlan = false;
    }
    ComportamientoJugador(const ComportamientoJugador & comport) : Comportamiento(comport){}
    ~ComportamientoJugador(){}

    Action think(Sensores sensores);
    int interact(Action accion, int valor);
    void VisualizaPlan(const stateN0 &st, const list<Action> &plan);


  private:
    // Declarar Variables de Estado
    list<Action> plan;
    bool hayPlan;
    stateN0 c_state;
    ubicacion goal;


};

#endif
