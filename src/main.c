#include <allegro5/allegro.h>
#include <allegro5/allegro_image.h>
#include <allegro5/allegro_audio.h>
#include <usrtypes.h>
#include <stdio.h>

#define CARD_WIDTH 98 
#define CARD_HEIGHT 146

#define FB_WIDTH 480
#define FB_HEIGHT 320
#define min(a, b) ((a) < (b) ? (a) : (b))

int main(int argc, char const *argv[])
{
    al_init();
    al_init_image_addon();
    al_install_audio();
    al_install_mouse();
    al_set_new_display_flags(ALLEGRO_RESIZABLE);
    ALLEGRO_DISPLAY* display = al_create_display(800, 800);
    ALLEGRO_EVENT_QUEUE* event_queue = al_create_event_queue();
    ALLEGRO_TIMER* timer = al_create_timer(1.0 / 60.0);

    al_register_event_source(event_queue, al_get_display_event_source(display));
    al_register_event_source(event_queue, al_get_timer_event_source(timer));
    al_register_event_source(event_queue, al_get_mouse_event_source());

    ALLEGRO_BITMAP* bm = al_load_bitmap("./res/test_card.png");
    ALLEGRO_BITMAP* fb = al_create_bitmap(FB_WIDTH, FB_HEIGHT);
    f32 fb_scale = 1.0f;
    s32 fb_x = 0, fb_y = 0;
    u8 tint = 0;

    s32 x = 0, y = 0;
    s32 ox = 0, oy = 0;
    b8 follow = false, in_card = false, got_offset = false;
    s32 mx = 0, my = 0;
    
    ALLEGRO_EVENT event;
    ALLEGRO_MOUSE_STATE mouse;
    b8 running = true, redraw = false;
    al_start_timer(timer);
    while (running)
    {
        al_wait_for_event(event_queue, &event);
        al_get_mouse_state(&mouse);

        switch (event.type)
        {
        case ALLEGRO_EVENT_DISPLAY_CLOSE:
            running = false;
            break;

        case ALLEGRO_EVENT_DISPLAY_RESIZE:
            al_acknowledge_resize(display);
            break;

        case ALLEGRO_EVENT_MOUSE_BUTTON_DOWN:
            if (mx >= x && mx <= x + CARD_WIDTH - 1 && my >= y && my <= y + CARD_HEIGHT - 1)
            {
                follow = true;

                if (!got_offset)
                {
                    ox = mx - x;
                    oy = my - y;
                }
            }
            break;

        case ALLEGRO_EVENT_MOUSE_BUTTON_UP:
            follow = false;
            break;

        case ALLEGRO_EVENT_TIMER:
            // update
            mx = (f32) (mouse.x - fb_x) / fb_scale;
            my = (f32) (mouse.y - fb_y) / fb_scale;

            if (follow)
            {
                // follow mouse
                x = mx - ox;
                y = my - oy;

                tint = 255;
            }

            if (mx >= x && mx <= x + CARD_WIDTH - 1 && my >= y && my <= y + CARD_HEIGHT - 1)
            {
                in_card = true;
                al_set_system_mouse_cursor(display, ALLEGRO_SYSTEM_MOUSE_CURSOR_MOVE);
                tint = 182;
            }
            else
            {
                in_card = false;
                al_set_system_mouse_cursor(display, ALLEGRO_SYSTEM_MOUSE_CURSOR_DEFAULT);
                tint = 128;
            }

            redraw = true;
            break;
        }

        if (redraw)
        {
            al_clear_to_color(al_map_rgb(0, 0, 0));
            
            al_set_target_bitmap(fb);
            al_clear_to_color(al_map_rgb(0, 0, 128));

            if (follow && in_card)
            {
                al_draw_tinted_scaled_bitmap(bm, al_map_rgb(tint, tint, tint), 0, 0, CARD_WIDTH, CARD_HEIGHT,
                    x - 4, y - 4, CARD_WIDTH + 8, CARD_HEIGHT + 8, 0);
            }
            else
            {
                al_draw_tinted_bitmap(bm, al_map_rgb(tint, tint, tint), x, y, 0);
            }

            al_set_target_bitmap(al_get_backbuffer(display));
            fb_scale = min((f32) al_get_display_width(display) / (f32) FB_WIDTH,
                (f32) al_get_display_height(display) / (f32) FB_HEIGHT);
            fb_x = ((f32) al_get_display_width(display) / 2) - (FB_WIDTH * fb_scale / 2);
            fb_y = ((f32) al_get_display_height(display) / 2) - (FB_HEIGHT * fb_scale / 2);
            al_draw_scaled_bitmap(fb, 
                0, 0, FB_WIDTH, FB_HEIGHT,
                fb_x,
                fb_y,
                FB_WIDTH * fb_scale, FB_HEIGHT * fb_scale, 0);

            al_flip_display();
            redraw = false;
        }
    }
    al_stop_timer(timer);

    al_destroy_bitmap(fb);
    al_destroy_bitmap(bm);
    al_unregister_event_source(event_queue, al_get_mouse_event_source());
    al_unregister_event_source(event_queue, al_get_timer_event_source(timer));
    al_unregister_event_source(event_queue, al_get_display_event_source(display));
    al_uninstall_audio();
    al_uninstall_mouse();
    al_destroy_timer(timer);
    al_destroy_event_queue(event_queue);
    al_destroy_display(display);
    al_shutdown_image_addon();

    return 0;
}
