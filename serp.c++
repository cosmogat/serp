// 27-10-2015
// billy
// serp.c++
#include <iostream>
#include <cstdlib>
#include <cstring>
#include <ctime>
#include <vector>
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_ttf.h>
#include <SDL2/SDL_mixer.h>
using namespace std;

void imprError(const char * mis);
SDL_Texture * carregarTextura(const char * ruta, SDL_Renderer * ren);
void renderTextura(SDL_Texture * tex, SDL_Renderer * ren, int x, int y, int w, int h);
void renderTextura(SDL_Texture * tex, SDL_Renderer * ren, SDL_Rect dst, SDL_Rect * clip = nullptr);
void renderTextura(SDL_Texture * tex, SDL_Renderer * ren, int x, int y, SDL_Rect * clip = nullptr);
SDL_Texture * renderText(const char * missatge, const char * ruta, SDL_Color color, int tamany, SDL_Renderer * ren);

struct PuntSerp {
  int x;
  int y;
  int o;
  int g;
  PuntSerp(int a = 0, int b = 0, int c = 0, int d = 0);
  PuntSerp & operator=(const PuntSerp & p);
};

int main (int num_arg, char * vec_arg[]) {
  // Iniciacio de SDL i carrega d'arxius
  int pos_x = 100, pos_y = 100, tam_x = 1024, tam_y = 576;
  int desp = 30, tam_serp = 30;
  srand(time(NULL));
  if (SDL_Init(SDL_INIT_EVERYTHING) < 0) {
    imprError("Error en SDL_Init");
    return 1;
  }
  atexit(SDL_Quit);
  if ((IMG_Init(IMG_INIT_PNG) & IMG_INIT_PNG) != IMG_INIT_PNG) {
    imprError("Error en IMG_Init");
    return 2;
  }
  atexit(IMG_Quit);
  if (TTF_Init() != 0) {
    imprError("Error en TTF_Init");
    return 3;
  }
  atexit(TTF_Quit);
  SDL_Window * fin;
  if (num_arg > 1 && strcmp(vec_arg[1], "-f") == 0) {
    SDL_DisplayMode actual;
    SDL_GetCurrentDisplayMode(0, &actual);
    tam_x = actual.w;
    tam_y = actual.h;
    fin = SDL_CreateWindow("Serp...", pos_x, pos_y, tam_x, tam_y, SDL_WINDOW_FULLSCREEN_DESKTOP);
    SDL_ShowCursor(SDL_DISABLE);
  }
  else {
    fin = SDL_CreateWindow("Serp...", pos_x, pos_y, tam_x, tam_y, SDL_WINDOW_SHOWN);
  }
  if (fin == nullptr) {
    imprError("Error en SDL_CreateWindow");
    return 4;
  }
  SDL_Renderer * ren = SDL_CreateRenderer(fin, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
  if (ren == nullptr) {
    SDL_DestroyWindow(fin);
    imprError("Error en SDL_CreateRenderer");
    return 5;
  }
  SDL_Texture * fons = carregarTextura("./img/fons.png", ren);
  SDL_Texture * serp = carregarTextura("./img/serp.png", ren);
  SDL_Texture * poma = carregarTextura("./img/poma.png", ren);
  if (fons == nullptr || serp == nullptr || poma == nullptr) {
    SDL_DestroyTexture(fons);
    SDL_DestroyTexture(serp);
    SDL_DestroyRenderer(ren);
    SDL_DestroyWindow(fin);
    imprError("Error en CarregarTextura");
    return 6;
  }
  // Càlculs previs
  SDL_Rect serps[4][16];
  SDL_Rect pomes[7];
  for (int i = 0; i < 4; i++) {
    for (int j = 0; j < 16; j++) {
      serps[i][j].w = serps[i][j].h = tam_serp;
      serps[i][j].y = i * tam_serp;
      serps[i][j].x = j * tam_serp;
    }
  }
  for (int i = 0; i < 7; i++) {
    pomes[i].w = pomes[i].h = tam_serp;
    pomes[i].x = i * tam_serp;
    pomes[i].y = 0;
  }
  bool eixir = false;
  bool joc_pausat = false;
  bool menjat = false;
  int max_x = tam_x - (tam_x % tam_serp), max_y = tam_y - (tam_y % tam_serp);
  int x, y, ori, ant_ori, gir, tipus, num_pomes, tipus_poma, nivell;
  int poma_x = 0, poma_y = 0;
  int retras = 100, temps_ms = 0, temps_poma = 0;
  SDL_Event e;
  unsigned int menu = 0;
  int puntuacio = 0, pantalla = 0, lletra_sel = 0;
  vector<PuntSerp> posi;
  // Bucle principal
  while (!eixir) {
    if (pantalla == 0) {
      while (SDL_PollEvent(&e)) {
	if (e.type == SDL_QUIT)
	  eixir = true;
	else if (e.type == SDL_KEYDOWN) {
	  if (e.key.keysym.sym == SDLK_w or e.key.keysym.sym == SDLK_UP)
	    menu = (menu - 1) % 3;
	  else if (e.key.keysym.sym == SDLK_s or e.key.keysym.sym == SDLK_DOWN)
	    menu = (menu + 1) % 3;
	  else if (e.key.keysym.sym == SDLK_SPACE or e.key.keysym.sym == SDLK_RETURN) {
	    if (menu == 0) {
	      pantalla = 1;
	      x = (max_x >> 1) - ((max_x >> 1) % tam_serp);
	      y = (max_y >> 1) - ((max_y >> 1) % tam_serp);
	      ori = ant_ori = gir = tipus = temps_ms = puntuacio = temps_poma = tipus_poma = num_pomes = 0;
	      nivell = 1;
	      retras = 100;
	      menjat = true;
	      posi.resize(4);
	      posi[posi.size() - 1] = PuntSerp(x, y, 0, 0); 
	      for (unsigned int i = 0; i < (posi.size() - 1); i++) {
		posi[i].x = x;
		posi[i].y = y + ((3 - i) * tam_serp);
		posi[i].g = posi[i].o = 0;
	      }
	    }
	    else if (menu == 1) {
	      pantalla = 2;
	    }
	    else if (menu == 2)
	      eixir = true;
	  }
	}
      }
    }
    else if (pantalla == 1) {
      // bucle d'events
      while (SDL_PollEvent(&e)) {
	if (e.type == SDL_QUIT)
	  eixir = true;
	else if (e.type == SDL_KEYDOWN) {
	  if ((e.key.keysym.sym == SDLK_w or e.key.keysym.sym == SDLK_UP) && ori != 2 && !joc_pausat)
	    ori = 0;
	  else if ((e.key.keysym.sym == SDLK_s or e.key.keysym.sym == SDLK_DOWN) && ori != 0 && !joc_pausat)
	    ori = 2;
	  else if ((e.key.keysym.sym == SDLK_a or e.key.keysym.sym == SDLK_LEFT) && ori != 3 && !joc_pausat)
	    ori = 1;
	  else if ((e.key.keysym.sym == SDLK_d or e.key.keysym.sym == SDLK_RIGHT) && ori != 1 && !joc_pausat)
	    ori = 3;
	  else if (e.key.keysym.sym == SDLK_t)
	    tipus = (tipus + 1) % 4;
	  else if (e.key.keysym.sym == SDLK_SPACE or e.key.keysym.sym == SDLK_RETURN)
	    joc_pausat = !joc_pausat;
	  else if (e.key.keysym.sym == SDLK_q && joc_pausat) {
	    pantalla = 0;
	  }
	}
      }
      // dinamica del joc
      if (!joc_pausat) {
	// comprovar poma
	if (poma_x == x && poma_y == y) {
	  posi.resize(posi.size() + 1);
	  posi[posi.size() - 1] = posi[posi.size() - 2];
	  if (tipus_poma == 0)
	    puntuacio++;
	  else if (tipus_poma < 4)
	    puntuacio += 2;
	  else if (tipus_poma == 4)
	    puntuacio += 5;
	  else if (tipus_poma == 5)
	    puntuacio += 10;
	  else {
	    puntuacio += 20;
	    nivell++;
	    temps_ms = 0;
	    num_pomes = -1;
	    posi.resize(4);
	  }
	  num_pomes++;
	  menjat = true;
	  temps_poma = temps_ms;
	}
	else if (temps_ms > (temps_poma + 10000)) {
	  menjat = true;
	  temps_poma = temps_ms;
	}
	// orientacio de la serp
	if (ori == 0)
	  y -= desp;
	else if (ori == 1)
	  x -= desp;
	else if (ori == 2)
	  y += desp;
	else if (ori == 3)
	  x += desp;
	// bordes
	if (y > max_y)
	  y = 0;
	else if (x > max_x)
	  x = 0;
	else if (y < 0)
	  y = max_y;
	else if (x < 0)
	  x = max_x;
	gir = 0;
	if (ant_ori != ori) {
	  switch (ant_ori) {
	  case 0:
	    if (ori == 1)
	      gir = 4;
	    else
	      gir = 1;
	    break;
 	  case 1:
	    if (ori == 0)
	      gir = 2;
	    else
	      gir = 1;
	    break;
	  case 2:
	    if (ori == 1)
	      gir = 3;
	    else
	      gir = 2;
	    break;
	  case 3:
	    if (ori == 0)
	      gir = 3;
	    else
	      gir = 4;
	    break;
	  default:
	    break;
	  }
	}
	ant_ori = ori;
	posi[posi.size() - 1].g = gir;
	for (unsigned int i = 1; i < posi.size(); i++)
	  posi[i - 1] = posi[i];
	posi[posi.size() - 1] = PuntSerp(x, y, ori, 0);
      }
      // generar poma
      if (menjat) {
	poma_x = (rand() % (max_x / tam_serp)) * tam_serp;
	poma_y = (rand() % (max_y / tam_serp)) * tam_serp;
	if (num_pomes > 20) {
	  int aux = rand() % 2;
	  if (aux == 0)
	    tipus_poma = 6;
	  else
	    tipus_poma = 5;
	}
	else if (num_pomes > 10) {
	  int aux = rand() % 2;
	  if (aux == 0)
	    tipus_poma = 4;
	  else
	    tipus_poma = rand() % 4;
	}
	else if (num_pomes > 5)
	  tipus_poma = rand() % 4;
	else
	  tipus_poma = 0;
	menjat = false;
      }
      // calcul de la colisio
      bool colisio = false;
      for (unsigned int i = 0; i < (posi.size() - 1) && !colisio; i++) {
	if (x == posi[i].x && y == posi[i].y)
	  colisio = true;
      }
      if (colisio or temps_ms > 300000) {
      	pantalla = 0;
      }
    }
    else if (pantalla == 2) {
      while (SDL_PollEvent(&e)) {
	if (e.type == SDL_QUIT)
	  eixir = true;
	else if (e.type == SDL_KEYDOWN) {
	  if (e.key.keysym.sym == SDLK_SPACE or e.key.keysym.sym == SDLK_RETURN)
	    eixir = true;
	  else if (e.key.keysym.sym == SDLK_w or e.key.keysym.sym == SDLK_a or e.key.keysym.sym == SDLK_UP or e.key.keysym.sym == SDLK_LEFT) {
	    lletra_sel--;
	    if (lletra_sel < 0)
	      lletra_sel = 0;
	  }
	  else if (e.key.keysym.sym == SDLK_s or e.key.keysym.sym == SDLK_d or e.key.keysym.sym == SDLK_DOWN or e.key.keysym.sym == SDLK_RIGHT) {
	    lletra_sel++;
	    if (lletra_sel > 25)
	      lletra_sel = 25;
	  }
	}
      }
    }
    // dibuixar
    SDL_RenderClear(ren);
    SDL_RenderCopy(ren, fons, NULL, NULL);
    SDL_Color color_blanc = {255, 255, 255, 255};
    SDL_Color color_verd = {0, 255, 0, 255};
    SDL_Color color_roig = {255, 0, 0, 255};
    SDL_Color color_groc ={255, 255, 0, 255};
    if (pantalla == 0) {
      SDL_Texture * text_super = renderText("Super Serp", "./ttf/emulogic.ttf", color_roig, 80, ren);
      SDL_Texture * text_jugar = renderText("Jugar", "./ttf/emulogic.ttf", menu == 0 ? color_groc : color_blanc, 25, ren);
      SDL_Texture * text_recor = renderText("Records", "./ttf/emulogic.ttf", menu == 1 ? color_groc : color_blanc, 25, ren);
      SDL_Texture * text_eixir = renderText("Eixir", "./ttf/emulogic.ttf", menu == 2 ? color_groc : color_blanc, 25, ren);
      for (int i = 250; i < 1100; i += tam_serp) {
	renderTextura(serp, ren, i, 190, &serps[0][3]);
	renderTextura(serp, ren, i, 40, &serps[0][1]);
      }
      for (int i = 70; i < 190; i += tam_serp)
	renderTextura(serp, ren, 220, i, &serps[0][2]);
      renderTextura(serp, ren, 220, 40, &serps[0][4]);
      renderTextura(serp, ren, 220, 190, &serps[0][5]);
      renderTextura(serp, ren, 1116, 40, &serps[0][7]);
      renderTextura(serp, ren, 1116, 190, &serps[0][6]);
      renderTextura(serp, ren, 1116, 70, &serps[0][12]);
      renderTextura(serp, ren, 1116, 130, &serps[0][8]);
      renderTextura(serp, ren, 1116, 160, &serps[0][0]);
      renderTextura(text_super, ren, 0.20 * tam_x, 0.1 * tam_y);
      renderTextura(text_jugar, ren, 0.45 * tam_x, 0.5 * tam_y);
      renderTextura(text_recor, ren, 0.45 * tam_x, 0.6 * tam_y);
      renderTextura(text_eixir, ren, 0.45 * tam_x, 0.7 * tam_y);
    }
    else if (pantalla == 1) {
      char cad_punts[15];
      char cad_nivel[15];
      char cad_temps[15];
      sprintf(cad_punts, "Punts: %.5d", puntuacio);
      sprintf(cad_nivel, "Nivell: %.2d", nivell);
      sprintf(cad_temps, "Temps: %.3d", (int) (300 - (((float) temps_ms) * 0.001)));
      SDL_Texture * text_punts = renderText(cad_punts, "./ttf/emulogic.ttf", color_blanc, 25, ren);
      SDL_Texture * text_nivel = renderText(cad_nivel, "./ttf/emulogic.ttf", color_verd, 25, ren);
      SDL_Texture * text_temps = renderText(cad_temps, "./ttf/emulogic.ttf", color_roig, 25, ren);
      if (puntuacio > 99999)
	puntuacio = 99999;

      renderTextura(text_punts, ren, 0.03 * tam_x, 0.03 * tam_y);
      renderTextura(text_temps, ren, 0.40 * tam_x, 0.03 * tam_y);
      renderTextura(text_nivel, ren, 0.73 * tam_x, 0.03 * tam_y);
      renderTextura(poma, ren, poma_x, poma_y, &pomes[tipus_poma]);
      renderTextura(serp, ren, posi[0].x, posi[0].y, &serps[tipus][posi[1].o + 12]);
      for (unsigned int i = 1; i < posi.size() - 1; i++)
	renderTextura(serp, ren, posi[i].x, posi[i].y, &serps[tipus][posi[i].g == 0 ? posi[i].o : 3 + posi[i].g]);
      renderTextura(serp, ren, posi[posi.size() - 1].x, posi[posi.size() - 1].y, &serps[tipus][posi[posi.size() - 1].o + 8]);
      if (joc_pausat) {
	SDL_Texture * text_pausa = renderText("Joc pausat", "./ttf/emulogic.ttf", color_blanc, 25, ren);
	renderTextura(text_pausa, ren, 0.4 * tam_x, 0.5 * tam_y);
      }
    }
    else if (pantalla == 2) {
      char lletra[1];
      for (int i = 65; i < 75; i++) {
	sprintf(lletra, "%c", (char) i);
	SDL_Texture * text_lletra = renderText(lletra, "./ttf/emulogic.ttf", (lletra_sel + 65) == i ? color_roig : color_blanc, 25, ren);
	renderTextura(text_lletra, ren, 60 * (i - 58), 400);
      }
      for (int i = 75; i < 85; i++) {
	sprintf(lletra, "%c", (char) i);
	SDL_Texture * text_lletra = renderText(lletra, "./ttf/emulogic.ttf", (lletra_sel + 65) == i ? color_roig : color_blanc, 25, ren);
	renderTextura(text_lletra, ren, 60 * (i - 68), 500);
      }
      for (int i = 85; i < 91; i++) {
	sprintf(lletra, "%c", (char) i);
	SDL_Texture * text_lletra = renderText(lletra, "./ttf/emulogic.ttf", (lletra_sel + 65) == i ? color_roig : color_blanc, 25, ren);
	renderTextura(text_lletra, ren, 60 * (i - 76), 600);
      }
    }
    SDL_RenderPresent(ren);
    SDL_Delay(retras);
    if (!joc_pausat)
      temps_ms += retras;
  }
  // Tancament de SDL
  SDL_DestroyTexture(poma);
  SDL_DestroyTexture(fons);
  SDL_DestroyTexture(serp);
  SDL_DestroyRenderer(ren);
  SDL_DestroyWindow(fin);
  return 0;
}

void imprError(const char * mis) {
  cerr << mis << ": " << SDL_GetError() << endl;
}

SDL_Texture * carregarTextura(const char * ruta, SDL_Renderer * ren) {
  SDL_Texture * textura = IMG_LoadTexture(ren, ruta);
  if (textura == nullptr)
    imprError("Error en IMG_LoadTexture ");
  return textura;
}

void renderTextura(SDL_Texture * tex, SDL_Renderer * ren, int x, int y, int w, int h) {
  SDL_Rect dest;
  dest.x = x;
  dest.y = y;
  dest.w = w;
  dest.h = h;
  SDL_RenderCopy(ren, tex, NULL, &dest);
}

void renderTextura(SDL_Texture * tex, SDL_Renderer * ren, SDL_Rect dst, SDL_Rect * clip) {
  SDL_RenderCopy(ren, tex, clip, &dst);
}

void renderTextura(SDL_Texture * tex, SDL_Renderer * ren, int x, int y, SDL_Rect * clip) {
  SDL_Rect dst;
  dst.x = x;
  dst.y = y;
  if (clip != nullptr) {
    dst.w = clip->w;
    dst.h = clip->h;
  }
  else
    SDL_QueryTexture(tex, NULL, NULL, &dst.w, &dst.h);
  renderTextura(tex, ren, dst, clip);
}

SDL_Texture * renderText(const char * missatge, const char * ruta, SDL_Color color, int tamany, SDL_Renderer * ren) {
  TTF_Font * font = TTF_OpenFont(ruta, tamany);
  SDL_Surface * super = nullptr;
  SDL_Texture * textu = nullptr;
  if (font != nullptr) {
    super = TTF_RenderText_Blended(font, missatge, color);
    if (super != nullptr) {
      textu = SDL_CreateTextureFromSurface(ren, super);
      SDL_FreeSurface(super);
    }
    TTF_CloseFont(font);
  }
  return textu;
}

PuntSerp::PuntSerp(int a, int b, int c, int d) {
  x = a;
  y = b;
  o = c;
  g = d;
}

PuntSerp & PuntSerp::operator=(const PuntSerp & p) {
  x = p.x;
  y = p.y;
  o = p.o;
  g = p.g;
  return (*this);
}
