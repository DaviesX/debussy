#include <stdlib.h>
#include <window.h>


int main(int argc, char* argv[])
{
        struct window win;
        window_init(&win, &argc, &argv);
        window_run(&win);
        window_free(&win);
        return 0;
}
