#include <iostream>
#include <SDL3/SDL.h>
#include <cmath>
using namespace std;

const double pi = acos(-1.0); //gibt genau pi

const double FOV = pi / 3;  //zB
const int X = 640;    //zB für BILDSCHIRMBREITE projektion
const int Y = 360;    //zB
//const float D_X_MAP = 25.;
//const float D_X_MAP = 25.;
const double d_alpha = 60.0f / X;
const float H = 35;

//Startposition und Orientierung des Spielers festlegen
float S_Pos[2] = { 350.,200. };
float orientierung[2] = { 0.,1. };  //90°
float winkel_orientierung = 90; // grad
float speed = 100.0f; //Pixel/s geschwindigkeit Spieler

const double d = (X / 2) / tan(FOV / 2); //Distanz des Spielers zur Projektionsebene
const int Projektion_versch_x = 600;
const int Projektion_versch_y = 180;



int init_SDL(SDL_Window*& window, SDL_Renderer*& renderer);
void SDL_schliessen(SDL_Window*& window, SDL_Renderer*& renderer);

struct wand {
	int farbe;
	SDL_Surface* textur;

};

/*class Wand {
private:
	//float d; //distanz vom Spieler zur Projektionsebene 
	float dw; //distanz vom Spieler zur Wand
	float H; //Höhe der Wand
	float h; //Projektionshöhe
	bool vertikal; //Schnittpunkt für Schatten, wenn fals, dann horizontal
public:
	void berechne_h() {
		h = H * (d / dw);
	}
	float get_dw();


};*/

//d korrigieren
void korrektur(double d, double alpha) {
	double d_korr = d * cos(alpha);
}

//DDA-Algorithmus
/*void DDA(float alpha, float DX, float DY) {
	float dx = cos(alpha);
	float dy = sin(alpha);
	float e_vec[2] = { dx, dy };
	float SideDistX = DX / dx;
	float SideDistY = DY / dy;

}*/

int main() {

	SDL_Window* window = nullptr;
	SDL_Renderer* renderer = nullptr;

	init_SDL(window, renderer);


	int Spielfeld[20][20];
	for (int i = 0; i < 20; i++) {
		for (int j = 0; j < 20; j++) {
			if (i == 0 || j == 0 || i == 19 || j == 19) {
				Spielfeld[i][j] = 1; //Rahmenindex
			}
			else if ((i >= 2 && i < 6) && (j >= 13 && j < 18)) {
				Spielfeld[i][j] = 2;
			}
			else if ((i >= 10 && i < 17) && (j >= 4 && j < 6)) {
				Spielfeld[i][j] = 3;
			}

			else {
				Spielfeld[i][j] = 0;
			}
		}
	}
	//struct array anlegen

	wand waende[4];
	waende[0].farbe = 0;
	waende[1].farbe = 1;
	waende[2].farbe = 2;
	waende[3].farbe = 3;

	//Farben anlegen
	SDL_Color farben[4] = { { 70,80,140,255 },{ 133,139,137,255 },{ 170,140,20,255 },{ 120,190,190,255 } };

	//Texturen laden 
	SDL_Surface* textur1 = SDL_LoadBMP("Texturen/wall1.bmp");
	SDL_Surface* textur2 = SDL_LoadBMP("Texturen/wall2.bmp");

	waende[0].textur = nullptr;
	waende[1].textur = textur1;
	waende[2].textur = textur2;
	waende[3].textur = textur2;

	bool running = true;
	SDL_Event event;
	Uint64 lastTime = SDL_GetTicks();

	while (running) {
		//Delta Time berechnen
		Uint64 currentTime = SDL_GetTicks();
		float deltaTime = (currentTime - lastTime) / 1000.0f;
		lastTime = currentTime;

		while (SDL_PollEvent(&event)) {
			if (event.type == SDL_EVENT_QUIT) {  //wenn fenster geschlossen wird...
				running = false;           //...ablauf stoppen
			}
		}
		//double buffering
		SDL_SetRenderDrawColor(renderer, 0, 0, 0, 0);
		SDL_RenderClear(renderer);


		//Spielkarte Draufsicht erzeugen
		for (int x = 0; x < 20; x++) {
			for (int y = 0; y < 20; y++) {
				SDL_FRect rect;
				rect.x = (x) * 25.0f;   // Spalte * Blockgröße
				rect.y = (y) * 25.0f;   // Zeile  * Blockgröße
				rect.w = 24.0f;       // Breite //24, damit Ränder sichtbar sind
				rect.h = 24.0f;       // Höhe


				//Indizes des Arrays abfragen um Wandtyp zu erfahren
				switch (Spielfeld[x][y]) { //umgekehrt! da zeilen y-werte sind und spalten x-werte
				case 0:
					SDL_SetRenderDrawColor(renderer, 0, 0, 0, 0); //Schwarz
					break;
				case 1:
					SDL_SetRenderDrawColor(renderer, 0, 0, 255, 255);// Blau
					break;
				case 2:
					SDL_SetRenderDrawColor(renderer, 0, 255, 0, 255); // Grün
					break;
				case 3:
					SDL_SetRenderDrawColor(renderer, 100, 10, 60, 255);

				}
				SDL_RenderFillRect(renderer, &rect);
			}
		}
		SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255);

		//S_Pos[1] += 0.5; // schritt unten
		//S_Pos[0] += 0.5; // schritt rechts
		//Player bewegen
		const bool* keystate = SDL_GetKeyboardState(nullptr);

		if (keystate[SDL_SCANCODE_UP]) {
			float S_Pos_tmp[2] = { S_Pos[0],S_Pos[1] };
			S_Pos[0] += speed * deltaTime * orientierung[0];
			S_Pos[1] += speed * deltaTime * orientierung[1];
			//Wanderkennung und durchgang verbieten
			if (Spielfeld[int(S_Pos[0] / 25)][int(S_Pos[1] / 25)] != 0) { //wenn die Position dahingehen würde
				S_Pos[0] = S_Pos_tmp[0];
				S_Pos[1] = S_Pos_tmp[1];
			}

		}
		if (keystate[SDL_SCANCODE_DOWN]) {
			//if (Spielfeld[int(S_Pos[0] + orientierung[0] / 25)][int(S_Pos[1] + orientierung[1] / 25)] != 0) { //wenn die Position dahingehen würde
				//speed = 0;
			//}
			float S_Pos_tmp[2] = { S_Pos[0],S_Pos[1] };
			S_Pos[1] -= speed * deltaTime * orientierung[1];
			S_Pos[0] -= speed * deltaTime * orientierung[0];
			//Wanderkennung und durchgang verbieten
			if (Spielfeld[int(S_Pos[0] / 25)][int(S_Pos[1] / 25)] != 0) { //wenn die Position dahingehen würde
				S_Pos[0] = S_Pos_tmp[0];
				S_Pos[1] = S_Pos_tmp[1];
			}
		}
		if (keystate[SDL_SCANCODE_LEFT]) { winkel_orientierung -= speed * deltaTime; }
		if (keystate[SDL_SCANCODE_RIGHT]) { winkel_orientierung += speed * deltaTime; }
		orientierung[0] = cos(winkel_orientierung * pi / 180);
		orientierung[1] = sin(winkel_orientierung * pi / 180);




		// Player
		SDL_FRect spieler;
		spieler.x = S_Pos[0] - 4;
		spieler.y = S_Pos[1] - 4;
		spieler.w = 8.;
		spieler.h = 8.;
		SDL_RenderFillRect(renderer, &spieler);

		float alpha_left = winkel_orientierung + 30;
		float tmp_alpha = alpha_left;
		float end_x = S_Pos[0] + 150 * orientierung[0];
		float end_y = S_Pos[1] + 150 * orientierung[1];
		/*SDL_SetRenderDrawColor(renderer, 255, 130, 67, 255);

		SDL_RenderLine(renderer, S_Pos[0], S_Pos[1], end_x, end_y);

		SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255);

		end_x = S_Pos[0] + 150 * cos(alpha_left * pi / 180);
		end_y = S_Pos[1] + 150 * sin(alpha_left * pi / 180);
		SDL_RenderLine(renderer, S_Pos[0], S_Pos[1], end_x, end_y);

		SDL_SetRenderDrawColor(renderer, 0, 255, 0, 255);
		float alpha_right = winkel_orientierung - 30;
		end_x = S_Pos[0] + 150 * cos(alpha_right * pi / 180);
		end_y = S_Pos[1] + 150 * sin(alpha_right * pi / 180);
		SDL_RenderLine(renderer, S_Pos[0], S_Pos[1], end_x, end_y);*/

		//SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
		int count = 0; //count, um nur alle n Strahlen, nur einen zu erzeugen

		float FOV_alpha = 30;

		for (int i = 0; i < X; i++) { //Für zwischenabstände n+1 mal schleife durchführen	
			float strahlen[2] = { cos(tmp_alpha * pi / 180), sin(tmp_alpha * pi / 180) };

			//DDA Algorithmus zur Wanderkennung 
			float dx = strahlen[0];//Rechtsanteil
			float dy = strahlen[1];//Untenanteil
			int mapX = int(S_Pos[0] / 25); //Feld Spieler x-Position
			int mapY = int(S_Pos[1] / 25); //Feld Spieler y-Position
			int stepX; //Richung Feldwechsel in x
			int stepY; //Richtung Feldwechsel in y 
			int wandtyp;

			if (dx < 0) {
				stepX = -1;
			}
			else {
				stepX = 1;
			}
			if (dy < 0) {
				stepY = -1;
			}
			else {
				stepY = 1;
			}
			float deltaDistX = 25 / fabs(dx); //Strahl-Länge zwischen 2 senkrechtenLinien 
			float deltaDistY = 25 / fabs(dy); //STrahllänge zwischen 2 waagrechten Linien 
			//25 = Feldgröße in Pixeln 
			//Bei dx = 0 kein Absturz, float / 0 = inf

			//Abstand zur nächsten Linie:
			float sideDistX;
			float sideDistY;
			if (stepX == 1) { //Strahl nach Rechts -> rechte Kante 
				sideDistX = ((mapX + 1) * 25 - S_Pos[0]) / fabs(dx);
			}
			else { //Strahl nach links -> linke Kante 
				sideDistX = (S_Pos[0] - mapX * 25) / fabs(dx);
			}
			if (stepY == 1) { //Srahl nach unten -> untere Kante
				sideDistY = ((mapY + 1) * 25 - S_Pos[1]) / fabs(dy);
			}
			else { //Strahl nach oben -> obere Kante
				sideDistY = (S_Pos[1] - mapY * 25) / fabs(dy);
			}

			//Sprung Schleife zur nächsten Wand
			bool getroffen = false; //Wanddetektor
			bool vertikal = false; //Für Schattierung
			float dist_dda = 0; //Spieler -> Wand Abstand

			while (getroffen == false) { //Wdh bis Wand getroffen 
				//Welche Wand liegt näher?
				if (sideDistX < sideDistY) { //Zuerst Senkrechte
					dist_dda = sideDistX;
					sideDistX += deltaDistX;
					mapX += stepX; //Schritt nach rechts/links
					vertikal = true;
				}
				else { //Zuerst Waagrechte
					dist_dda = sideDistY;
					sideDistY += deltaDistY;
					mapY += stepY; //Schritt nach oben/unten 
					vertikal = false;
				}
				if (Spielfeld[mapX][mapY] != 0) { //neues Feld = Wand?
					getroffen = true;
					wandtyp = Spielfeld[mapX][mapY];
				}
			}

			//Treffpunkt aus DDA-Distanz berechnen:
			end_x = S_Pos[0] + dist_dda * dx;
			end_y = S_Pos[1] + dist_dda * dy;

			float Treffer_im_Feld;
			if (vertikal == true) {
				Treffer_im_Feld = fmod(end_y, 25); //senkrechte Wand 
			}
			else {
				Treffer_im_Feld = fmod(end_x, 25); //waagrechte Wand 
			}

			//passende Spalte im Texturbild
			SDL_Surface* tex = waende[wandtyp].textur;

			int  texX = int(Treffer_im_Feld / 25 * tex->w);
			//naiver algorithmus zur Wand erkennung

		   /*for (int dt = 1; dt < 500; dt++) {
			   end_x = S_Pos[0] + dt * cos(tmp_alpha * pi / 180);
			   end_y = S_Pos[1] + dt * sin(tmp_alpha * pi / 180);

			   //end_x = S_Pos[0] + dt * strahlen[0];
			   //end_y = S_Pos[1] + dt * strahlen[1];
			   if (Spielfeld[int(end_x / 25)][int(end_y / 25)] != 0) { // Wand gefunden */
			if (count % 16 == 0) {//jeden 16. Strahl zeichnen
				SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
				SDL_RenderLine(renderer, S_Pos[0], S_Pos[1], end_x, end_y);
			}
			/*break;
		}

	}

	float d_w = sqrt(pow(end_x - S_Pos[0], 2) + pow(end_y - S_Pos[1], 2));
	if (i == 320) {
		cout << "naiv: " << d_w << " DDA: " << dist_dda << endl;
	}*/
			float d_korr = dist_dda * cos(FOV_alpha * pi / 180);
			int h_w = int(H * d / d_korr);
			int oben_unten = (Y - h_w) / 2; //Höhe für Boden oder Wand

			for (int j = 0; j < Y; j++) {
				if (j < oben_unten)
				{

					SDL_SetRenderDrawColor(renderer, farben[0].r, farben[0].g, farben[0].b, farben[0].a);
					SDL_RenderPoint(renderer, X - i + Projektion_versch_x, j + Projektion_versch_y);
				}
				if (j >= oben_unten && j < oben_unten + h_w)
				{
					/* ALTE VERSION: Einfarbige Wand
					SDL_Color c;
					c = farben[waende[wandtyp].farbe]; //farbnummer ist im structarray hinterlegt und bestimmt SDL_farbarray

					if (vertikal == true) {
						SDL_SetRenderDrawColor(renderer, c.r,c.g,c.b,c.a);//helle Seite
					}
					else {
						SDL_SetRenderDrawColor(renderer, c.r*3/5, c.g * 3 / 5, c.b * 3 / 5, c.a * 3 / 5);//dunkle Seite
					}
					SDL_RenderPoint(renderer, X - i + Projektion_versch_x, j + Projektion_versch_y);*/

					//Zeilen für Texturen 
					int texY = (j - oben_unten) * tex->h / h_w;

					Uint8 r, g, b, a;
					SDL_ReadSurfacePixel(tex, texX, texY, &r, &g, &b, &a);

					//Schattenseite abdunkeln
					if (vertikal == false) {
						r = r * 3 / 5;
						g = g * 3 / 5;
						b = b * 3 / 5;
					}
					SDL_SetRenderDrawColor(renderer, r, g, b, 255);
					SDL_RenderPoint(renderer, X - i + Projektion_versch_x, j + Projektion_versch_y);
				}
				if (j >= oben_unten + h_w && j < oben_unten * 2 + h_w)
				{
					SDL_SetRenderDrawColor(renderer, 50, 40, 50, 200);
					SDL_RenderPoint(renderer, X - i + Projektion_versch_x, j + Projektion_versch_y);
				}
			}




			count++;
			FOV_alpha -= d_alpha;
			//DDA
			//end_x = S_Pos[0] + 150 * strahlen[0];
			//end_y = S_Pos[1] + 150 * strahlen[1];
			tmp_alpha -= d_alpha;
			//SDL_SetRenderDrawColor(renderer, 255, 130, 67, 255);
			//SDL_RenderLine(renderer, S_Pos[0], S_Pos[1], end_x, end_y);
		}



		SDL_RenderPresent(renderer);
		SDL_Delay(1000 / 24);
	}
	
	//Speicher der Textur freigeben 
	SDL_DestroySurface(textur1);
	SDL_DestroySurface(textur2);
	SDL_schliessen(window, renderer);

	return 0;
}



int init_SDL(SDL_Window*& window, SDL_Renderer*& renderer) {  //es wird die Referenz des Zeigers auf den struct übergeben
	// 1) Initialisierung
	if (SDL_Init(SDL_INIT_VIDEO)) {
		cout << "window initialization is succesful.\n";
	}
	else {
		cout << SDL_GetError();
		SDL_Quit();
		return -1;
	}
	// 2) create window
	window = nullptr;
	window = SDL_CreateWindow("SDL3 Start", 1280, 720, SDL_WINDOW_VULKAN | SDL_WINDOW_HIGH_PIXEL_DENSITY);
	if (window == nullptr) {
		cout << "error creating window\n";
		SDL_Quit();
		return -1;
	}
	else {
		cout << "window created sucessfully\n";
	}
	//create Renderer
	renderer = SDL_CreateRenderer(window, nullptr);
	if (renderer == nullptr) {
		cout << "ERROR: couldn't open renderer\n";
		SDL_DestroyWindow(window);
		SDL_DestroyRenderer(renderer);
		SDL_Quit();
	}
}
void SDL_schliessen(SDL_Window*& window, SDL_Renderer*& renderer) {
	SDL_DestroyRenderer(renderer);
	SDL_DestroyWindow(window);
	SDL_Quit();
}