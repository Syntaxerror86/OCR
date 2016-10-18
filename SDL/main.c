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
   SDL_FreeSurface(img);
   return 0;
}
