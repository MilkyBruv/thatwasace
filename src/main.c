#include <allegro5/allegro.h>
#include <allegro5/allegro_image.h>
#include <allegro5/allegro_audio.h>
#include <allegro5/allegro_acodec.h>
#include <allegro5/allegro_font.h>
#include <allegro5/allegro_ttf.h>
#include <usrtypes.h>
#include <stdio.h>

#define CARD_WIDTH 98 
#define CARD_HEIGHT 146

#define TOTAL_CARDS 3
#define CARD_SCALE 2.0f

#define FB_WIDTH (240 * 3)
#define FB_HEIGHT (160 * 3)
#define min(a, b) ((a) < (b) ? (a) : (b))

typedef struct card
{
    ALLEGRO_BITMAP* bm;
    ALLEGRO_BITMAP* overlay_bm;
    u8 tint;
    s32 cx, cy, x1, tl_x, tl_y, tr_x, tr_y, bl_x, bl_r, br_x, br_y; // rectangle positions (corners and center)
    b8 follow, entered, got_offset;
    s32 ox, oy;
    u8 layer;
} card_t;

int main(int argc, char const *argv[])
{
    al_init();
    al_init_image_addon();
    al_init_font_addon();
    al_init_ttf_addon();
    al_install_audio();
    al_install_mouse();
    al_init_acodec_addon();

    al_set_new_display_flags(ALLEGRO_RESIZABLE);
    ALLEGRO_DISPLAY* display = al_create_display(800, 800);
    ALLEGRO_EVENT_QUEUE* event_queue = al_create_event_queue();
    ALLEGRO_TIMER* timer = al_create_timer(1.0 / 60.0);

    al_register_event_source(event_queue, al_get_display_event_source(display));
    al_register_event_source(event_queue, al_get_timer_event_source(timer));
    al_register_event_source(event_queue, al_get_mouse_event_source());

    // i have 1 whole font just for me
    ALLEGRO_FONT* font = al_load_ttf_font("./res/font/carnevalee-freakshow/Carnevalee Freakshow.ttf", 32, ALLEGRO_TTF_MONOCHROME);

    // i have 3 whole sounds just for me
    al_reserve_samples(3);
    ALLEGRO_SAMPLE* up_sample = al_load_sample("./res/up2.wav");
    ALLEGRO_SAMPLE* down_sample = al_load_sample("./res/down2.wav");
    ALLEGRO_SAMPLE* info_sample = al_load_sample("./res/info.wav");

    ALLEGRO_BITMAP* fb = al_create_bitmap(FB_WIDTH, FB_HEIGHT);
    f32 fb_scale = 1.0f;
    s32 fb_x = 0, fb_y = 0;
    s32 mx = 0, my = 0;
    b8 in_card = false;

    card_t cards[TOTAL_CARDS];
    cards[0] = (card_t) {
        .bm = al_load_bitmap("./res/card1.png"),
        .overlay_bm = al_create_bitmap(CARD_WIDTH * CARD_SCALE, CARD_HEIGHT * CARD_SCALE),
        .x = 0, .y = 0, .ox = 0, .oy = 0, .entered = false, .follow = false, .got_offset = false, .tint = 128
    };
    cards[1] = (card_t) {
        .bm = al_load_bitmap("./res/card2.png"),
        .overlay_bm = al_create_bitmap(CARD_WIDTH * CARD_SCALE, CARD_HEIGHT * CARD_SCALE),
        .x = 100, .y = 0, .ox = 0, .oy = 0, .entered = false, .follow = false, .got_offset = false, .tint = 128
    };
    cards[2] = (card_t) {
        .bm = al_load_bitmap("./res/card2.png"),
        .overlay_bm = al_create_bitmap(CARD_WIDTH * CARD_SCALE, CARD_HEIGHT * CARD_SCALE),
        .x = 200, .y = 0, .ox = 0, .oy = 0, .entered = false, .follow = false, .got_offset = false, .tint = 128
    };
    
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
            for (size_t i = 0; i < TOTAL_CARDS; i++)
            {
                if (cards[i].entered)
                {
                    if (event.mouse.button == 1)
                    {
                        cards[i].follow = true;
                        cards[i].layer = TOTAL_CARDS - 1;

                        // Move rest of cards down beneath
                        for (size_t j = 0; j < TOTAL_CARDS; j++)
                        {
                            if (i != j && cards[j].layer - 1 >= 0)
                            {
                                cards[j].layer--;
                            }
                        }

                        if (!cards[i].got_offset)
                        {
                            cards[i].ox = mx - cards[i].x;
                            cards[i].oy = my - cards[i].y;
                            al_play_sample(up_sample, 1.0f, 0.0f, 1.0f, ALLEGRO_PLAYMODE_ONCE, NULL);
                        }
                    }
                    else if (event.mouse.button == 2)
                    {
                        al_play_sample(info_sample, 1.0f, 0.0f, 1.0f, ALLEGRO_PLAYMODE_ONCE, NULL);
                    }
                    break;
                }
            }
            
            break;

        case ALLEGRO_EVENT_MOUSE_BUTTON_UP:
            if (event.mouse.button != 1) { break; }
            for (size_t i = 0; i < TOTAL_CARDS; i++)
            {
                cards[i].follow = false;
            }
            al_play_sample(down_sample, 1.0f, 0.0f, 1.0f, ALLEGRO_PLAYMODE_ONCE, NULL);
            break;

        case ALLEGRO_EVENT_TIMER:
            // update
            mx = (f32) (mouse.x - fb_x) / fb_scale;
            my = (f32) (mouse.y - fb_y) / fb_scale;

            for (size_t i = 0; i < TOTAL_CARDS; i++)
            {
                if (cards[i].follow)
                {
                    // follow mouse
                    cards[i].x = mx - cards[i].ox;
                    cards[i].y = my - cards[i].oy;

                    cards[i].tint = 255;
                }

                if (mx >= cards[i].x && mx <= cards[i].x + (CARD_WIDTH * CARD_SCALE) - 1 && 
                    my >= cards[i].y && my <= cards[i].y + (CARD_HEIGHT * CARD_SCALE) - 1)
                {
                    cards[i].entered = true;
                    in_card = true;
                    al_set_system_mouse_cursor(display, ALLEGRO_SYSTEM_MOUSE_CURSOR_MOVE);
                    if (!cards[i].follow) cards[i].tint = 182;
                }
                else
                {
                    cards[i].entered = false;
                    al_set_system_mouse_cursor(display, ALLEGRO_SYSTEM_MOUSE_CURSOR_DEFAULT);
                    cards[i].tint = 128;
                }
            }

            for (size_t i = 0; i < TOTAL_CARDS; i++)
            {
                if (cards[i].entered)
                {
                    // al_set_system_mouse_cursor(display, ALLEGRO_SYSTEM_MOUSE_CURSOR_MOVE);
                    // al_set_system_mouse_cursor(display, ALLEGRO_SYSTEM_MOUSE_CURSOR_DEFAULT);
                }
            }
            

            redraw = true;
            break;
        }

        if (redraw)
        {
            al_clear_to_color(al_map_rgb(0, 0, 0));
            
            al_set_target_bitmap(fb);
            al_clear_to_color(al_map_rgb(72, 0, 255));

            for (size_t l = 0; l < max_layer; l++)
            {
                for (size_t i = 0; i < TOTAL_CARDS; i++)
                {
                    // if (cards[i].layer != l) { continue; }
                    if (cards[i].follow && cards[i].entered)
                    {
                        al_draw_tinted_scaled_bitmap(cards[i].bm, al_map_rgb(cards[i].tint, cards[i].tint, cards[i].tint), 
                            0, 0, CARD_WIDTH, CARD_HEIGHT,
                            cards[i].x - 4, cards[i].y - 4, (CARD_WIDTH * CARD_SCALE) + 8, (CARD_HEIGHT * CARD_SCALE) + 8, 0);
                    }
                    else
                    {
                        al_draw_tinted_scaled_bitmap(cards[i].bm, al_map_rgb(cards[i].tint, cards[i].tint, cards[i].tint), 
                            0, 0, CARD_WIDTH, CARD_HEIGHT,
                            cards[i].x, cards[i].y, CARD_WIDTH * CARD_SCALE, CARD_HEIGHT * CARD_SCALE, 0);
                    }

                    al_draw_multiline_textf(font, al_map_rgb(cards[i].tint, cards[i].tint, cards[i].tint), 
                        cards[i].x + 16, cards[i].y + ((CARD_HEIGHT * CARD_SCALE) / 2) + 16, CARD_WIDTH, 32, 0, "%d, %d", cards[i].x, cards[i].y);
                }
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

    al_destroy_sample(up_sample);
    al_destroy_sample(down_sample);
    al_destroy_sample(info_sample);
    al_destroy_bitmap(fb);
    al_destroy_bitmap(cards[0].bm);
    al_destroy_bitmap(cards[1].bm);
    al_destroy_font(font);
    al_unregister_event_source(event_queue, al_get_mouse_event_source());
    al_unregister_event_source(event_queue, al_get_timer_event_source(timer));
    al_unregister_event_source(event_queue, al_get_display_event_source(display));
    al_destroy_timer(timer);
    al_destroy_event_queue(event_queue);
    al_destroy_display(display);
    al_uninstall_audio();
    al_uninstall_mouse();
    al_shutdown_ttf_addon();
    al_shutdown_font_addon();
    al_shutdown_image_addon();

    return 0;
}
