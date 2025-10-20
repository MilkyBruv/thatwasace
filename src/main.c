#include <allegro5/allegro.h>
#include <allegro5/allegro_image.h>
#include <usrtypes.h>
#include <stdio.h>

int main(int argc, char const *argv[])
{
    al_init();
    al_init_image_addon();
    ALLEGRO_DISPLAY* display = al_create_display(800, 800);
    ALLEGRO_EVENT_QUEUE* event_queue = al_create_event_queue();
    ALLEGRO_TIMER* timer = al_create_timer(1.0 / 60.0);

    al_register_event_source(event_queue, al_get_display_event_source(display));
    al_register_event_source(event_queue, al_get_timer_event_source(timer));

    ALLEGRO_BITMAP* bm = al_load_bitmap("../res/test_card.png");
    
    ALLEGRO_EVENT event;
    b8 running = true;
    b8 redraw = false;
    while (running)
    {
        al_wait_for_event(event_queue, &event);

        switch (event.type)
        {
        case ALLEGRO_EVENT_DISPLAY_CLOSE:
            running = false;
            break;

        case ALLEGRO_EVENT_TIMER:
            redraw = true;
        
        default:
            break;
        }

        if (al_is_event_queue_empty(event_queue) && redraw)
        {
            al_clear_to_color(al_map_rgb(0, 0, 255));

            al_draw_bitmap(bm, 0, 0, 0);

            al_flip_display();
            redraw = false;
        }
    }
    
    al_destroy_bitmap(bm);
    al_destroy_timer(timer);
    al_destroy_event_queue(event_queue);
    al_destroy_display(display);
    al_shutdown_image_addon();

    return 0;
}
