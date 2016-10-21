# include <SDL/SDL.h>
# include <SDL/SDL_image.h>
# include <err.h>

# include "pixel_operations.h"

void wait_for_keypressed(void) {
  SDL_Event             event;
  // Infinite loop, waiting for event
  for (;;) {
    // Take an event
    SDL_PollEvent( &event );
    // Switch on event type
    switch (event.type) {
    // Someone pressed a key -> leave the function
    case SDL_KEYDOWN: return;
    default: break;
    }
  // Loop until we got the expected event
  }
}


void init_sdl(void) {
  // Init only the video part
  if( SDL_Init(SDL_INIT_VIDEO)==-1 ) {
    // If it fails, die with an error message
    errx(1,"Could not initialize SDL: %s.\n", SDL_GetError());
  }
  // We don't really need a function for that ...
}


SDL_Surface* load_image(char *path) {
  SDL_Surface          *img;
  // Load an image using SDL_image with format detection
  img = IMG_Load(path);
  if (!img)
    // If it fails, die with an error message
    errx(3, "can't load %s: %s", path, IMG_GetError());
  return img;
}


SDL_Surface* display_image(SDL_Surface *img) {
  SDL_Surface          *screen;
  // Set the window to the same size as the image
  screen = SDL_SetVideoMode(img->w, img->h, 0, SDL_SWSURFACE|SDL_ANYFORMAT);
  if ( screen == NULL ) {
    // error management
    errx(1, "Couldn't set %dx%d video mode: %s\n",
         img->w, img->h, SDL_GetError());
  }
 
  /* Blit onto the screen surface */
  if(SDL_BlitSurface(img, NULL, screen, NULL) < 0)
    warnx("BlitSurface error: %s\n", SDL_GetError());
 
  // Update the screen
  SDL_UpdateRect(screen, 0, 0, img->w, img->h);
 
  // wait for a key
  wait_for_keypressed();
 
  // return the screen for further uses
  return screen;
}

int Clamp(float component)
        {
            return component < 0 ? 0 : (component > 255 ? 255 : (int)(component));
        }

int IsValid(int x, int y, SDL_Surface* img)
        {
	    if(x >= 0 && x < img ->w && y >= 0 && y < img->h )
		return 1;
	    else
		return 0;
        }

SDL_Surface* Convolute(SDL_Surface* img)
        {
	    float mask[3][3] = { {  0, -1, 0  },
                               { -1,  5, -1 },
                               {  0, -1, 0  } };
            int offset = 1;
            SDL_Surface* res = img;

            for (int y = 0; y < img->h; ++y)
            {
                for (int x = 0; x < img->w; ++x)
                {
                    float acc[3] = { 0, 0, 0 };
		    Uint32 c;
                    for (int dy = -offset; dy <= offset; ++dy)
                    {
                        for (int dx = -offset; dx <= offset; ++dx)
                        {
                            if (IsValid(x + dx, y + dy, img))
                            {
                                c = getpixel(img, x + dx, y + dy);
                                float coef = mask[dy + offset][dx + offset];
				Uint8 r, g, b;
				SDL_GetRGB(c, img ->format, &r, &g, &b);
                                acc[0] += r * coef;
                                acc[1] += g * coef;
                                acc[2] += b * coef;
                            }
                        }
                    }
		    Uint32 pxl = getpixel(img, x, y);
		    pxl = SDL_MapRGB(img->format, acc[0], acc[1], acc[2]);
		    putpixel(res, x , y, c);
                    //res.SetPixel(x, y, Color.FromArgb(Clamp(acc[0]), Clamp(acc[1]), Clamp(acc[2])));
                }
            }

            return res;
        }

SDL_Surface* RLSA(SDL_Surface* img, int mode, int tab[img->h][img->w][2]) //mode == 0 if horizontal, 1 if vertical
{
	int c;
	int neigh;
	int prevNeigh = 0;
	Uint32 pxl;
	int y;
	if (!mode) //if horizontal rlsa
	{
		for (int i = 0; i < img->h; ++i){
			for (int j = 0;j < img->w; ++j){  //horizontal path
				if (tab[i][j][0] != 0)	//if current pixel is white
				{
					neigh = 1 + prevNeigh; //there are at least (1 + number of previous consecutive white pixels) neighbours
					if (j < (img->w) - 1) //if not right before the end of line
					{
						c = 0;
						y = j + 1;								//we look up to the 4 next pixels
						while ((c < 4) && (tab[i][y][0] != 0) && (j < img->w)){                 //to see if they are white
							neigh++;					  //if yes and no black pixel in between, we increment
							c++;						  //the number of neighbouring white pixels
							y++;
						}
					}
					if (neigh <= 4) //if it's part of a short space
						pxl = getpixel(img, i, j);
						pxl = SDL_MapRGB(img->format, 0, 0, 0); //we set current pixel to black
					prevNeigh++; //in any case, it means the next pixel will have 1 more white neighbour
				}
				else
					prevNeigh = 0; //if black pixel, we break the chain of white pixels
			}
			prevNeigh = 0; //end of line, the next pixel won't be part of the current chain of white pixels
		}
	}
	else
	{
		for (int i = 0; i < img->w; ++i){ //vertical path
                        for (int j = 0;j < img->h; ++j){
                                if (tab[j][i][0] != 0)
                                {
                                        neigh = 1 + prevNeigh;
					if (j < (img->h) - 1) //if not right before end of column
					{
                                        	c = 0;
                                        	y = j + 1;
						while ((c < 4) && (tab[y][i][0] != 0) && (j < img->h)){
                                                	neigh++;
                                                	c++;
                                                	y++;
                                        	}
					}
                                        if (neigh <= 4)
						pxl = getpixel(img, j, i);
                                                pxl = SDL_MapRGB(img->format, 0, 0, 0);
                                        prevNeigh++;
                                }
                                else
                                        prevNeigh = 0;
                        }
			prevNeigh = 0; //end of column
                }
	}
	return img;
}

SDL_Surface* FusAnd(SDL_Surface* img,int tabhor[img->h][img->w][2], int tabver[img->h][img->w][2])
{
	SDL_Surface* res = SDL_CreateRGBSurface(0, img->w, img->h, 32, 0, 0, 0, 0);
	Uint32 respxl;
	for (int i = 0;i < img->h;i++){
		for (int j = 0;j < img->w;j++){
			respxl = getpixel(res, i, j);
			if (tabhor[i][j][0] != tabver[i][j][0]) //if current pixel doesn't have the same color in both images
				respxl = SDL_MapRGB(res->format, 255, 255, 255); //we set its color to white in the result image
			else
				respxl = SDL_MapRGB(res->format, tabhor[i][j][0], tabhor[i][j][0], tabhor[i][j][0]); //else, we set it to the color they have in common (it could have been tabver[i][j][0])
		}
	}
	return res;
}

int main(int argc, char *argv[])
{
   if (argc<2)
	errx(1 , "No image.");
   init_sdl();
   SDL_Surface* img = load_image(argv[1]);
   display_image(img);
   img = Convolute(img);
   display_image(img);
   for (int x = 0; x < img->w; x++)
   {
	for(int y = 0; y <img->h; y++)
        {
		Uint32 pxl = getpixel(img, x, y);
		Uint8 r, g, b;
		SDL_GetRGB(pxl , img->format, &r, &g, &b);
		float gray = 0.3 * r + 0.59 * g + 0.11 * b;
		pxl = SDL_MapRGB(img->format, gray, gray, gray);
		putpixel(img, x, y, pxl);
 	}
   }
   display_image(img);
   for (int x = 0; x < img->w; x++)
   {
        for(int y = 0; y <img->h; y++)
        {
                Uint32 pxl = getpixel(img, x, y);
                Uint8 r, g, b;
                SDL_GetRGB(pxl , img->format, &r, &g, &b);
                float bin = 0.3 * r + 0.59 * g + 0.11 * b;
		if(bin <= 127)
			bin = 0;
		else
			bin = 255;
                pxl = SDL_MapRGB(img->format, bin, bin, bin);
                putpixel(img, x, y, pxl);
        }
   }
   display_image(img);
   printf("1\n");
   Uint8 r, g, b;
   int tab[img->h][img->w][2];
   for (int i = 0;i < img->h;i++){
        for (int j = 0;j < img->w;j++){
                tab[i][j][1] = 0;
                SDL_GetRGB(getpixel(img, i, j), img->format, &r, &g, &b);
                tab[i][j][0] = r;
        }
   }
   SDL_Surface* hor = RLSA(img, 0, tab);
   display_image(hor);
   printf("2\n");
   SDL_Surface* ver = RLSA(img, 1, tab);
   display_image(ver);
   printf("3\n");
   Uint8 hr, hg, hb;
   Uint8 vr, vg, vb;
   int tabhor[hor->h][hor->w][2];
   int tabver[ver->h][ver->w][2];
   for (int i = 0;i < hor->h;i++){
        for (int j = 0;j < hor->w;j++){
                tabhor[i][j][1] = 0;
		tabver[i][j][1] = 0;
                SDL_GetRGB(getpixel(hor, i, j), hor->format, &hr, &hg, &hb);
		SDL_GetRGB(getpixel(ver, i, j), ver->format, &vr, &vg, &vb);
                tabhor[i][j][0] = hr;
		tabver[i][j][0] = vr;
        }
   }
   SDL_Surface* res = FusAnd(img, tabhor, tabver);
   display_image(res);
   printf("4\n");
   Uint8 rr, rg, rb;
   int tabres[res->h][res->w][2];
   for (int i = 0;i < res->h;i++){
        for (int j = 0;j < res->w;j++){
                tabres[i][j][1] = 0;
                SDL_GetRGB(getpixel(res, i, j), res->format, &rr, &rg, &rb);
                tabres[i][j][0] = rr;
        }
   }
   res = RLSA(res, 0, tabres);
   printf("5\n");
   display_image(res);
   printf("6\n");
   SDL_FreeSurface(res);
   printf("7\n");
   SDL_FreeSurface(ver);
   printf("8\n");
   SDL_FreeSurface(hor);
   printf("9\n");
   SDL_FreeSurface(img);
   return 0;
}
