#include<stdlib.h>
#include"/gtk/gtk.h"

int main (int argc, char **argv)
{
/* .Initialisation de GTK+ */
gtk_init (&argc, &argv);

/* Creation de la fenetre principale de notre application */

/*Lancement de la boucle principale */
gtk_main();
return EXIT_SUCCESS;
}
