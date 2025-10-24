#include <allegro5/allegro.h>
#include <allegro5/allegro_image.h>
#include <allegro5/allegro_audio.h>
#include <allegro5/allegro_acodec.h>
#include <allegro5/allegro_font.h>
#include <allegro5/allegro_ttf.h>
#include <math.h>
#include <usrtypes.h>
#include <stdio.h>

#define CARD_WIDTH 98 
#define CARD_HEIGHT 146

#define TOTAL_CARDS 1
#define CARD_SCALE 2.0f

#define FB_WIDTH (240 * 3)
#define FB_HEIGHT (160 * 3)
#define min(a, b) ((a) < (b) ? (a) : (b))

typedef struct card
{
    ALLEGRO_BITMAP* bm;
    ALLEGRO_BITMAP* overlay_bm;
    u8 tint;
    s32 cx, cy, x1, tl_x, tl_y, tr_x, tr_y, bl_x, bl_y, br_x, br_y; // rectangle positions (corners and center)
    b8 follow, entered, got_offset;
} card_t;

card_t create_card(const char* bm_path, s32 x, s32 y);

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
    cards[0] = create_card("./res/card1.png", FB_WIDTH / 2, FB_HEIGHT / 2);
    // cards[1] = create_card("./res/card2.png", 200, 200);
    f32 angle = 0.0f;

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
            angle += M_PI / 16;
            for (size_t i = 0; i < TOTAL_CARDS; i++)
            {
                // if (cards[i].entered)
                // {
                //     if (event.mouse.button == 1)
                //     {
                //         cards[i].follow = true;

                //         if (!cards[i].got_offset)
                //         {
                            // cards[i].cx = mx;
                            // cards[i].cy = my;
                //             al_play_sample(up_sample, 1.0f, 0.0f, 1.0f, ALLEGRO_PLAYMODE_ONCE, NULL);
                //         }
                //     }
                //     else if (event.mouse.button == 2)
                //     {
                //         al_play_sample(info_sample, 1.0f, 0.0f, 1.0f, ALLEGRO_PLAYMODE_ONCE, NULL);
                //     }
                //     break;
                // }
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
                // 
            }

            redraw = true;
            break;
        }

        if (redraw)
        {
            al_clear_to_color(al_map_rgb(0, 0, 0));
            
            al_set_target_bitmap(fb);
            al_clear_to_color(al_map_rgb(72, 0, 255));

            for (size_t i = 0; i < TOTAL_CARDS; i++)
            {
                // al_draw_tinted_scaled_rotated_bitmap(cards[i].bm, al_map_rgb(255, 255, 255),
                //     cards[i].cx, cards[i].cy, 
                //     cards[i].tl_x + CARD_WIDTH, cards[i].tl_y + CARD_HEIGHT, 
                //     CARD_SCALE, CARD_SCALE, angle, 0);

                al_draw_rotated_bitmap(cards[i].bm,
                    CARD_WIDTH / 2, CARD_HEIGHT / 2, 
                    cards[i].tl_x, cards[i].tl_y, angle, 0);

                al_draw_pixel(cards[i].cx, cards[i].cy, al_map_rgb(255, 0, 0));
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
    // al_destroy_bitmap(cards[1].bm);
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

card_t create_card(const char *bm_path, s32 x, s32 y)
{
    card_t new_card = {
        .bm = al_load_bitmap(bm_path), .cx = x, .cy = y, .entered = false, .got_offset = false, .follow = false,
        .tl_x = x - (CARD_WIDTH / 2), .tl_y = y - (CARD_HEIGHT / 2),
        .tr_x = x + (CARD_WIDTH / 2), .tr_y = y - (CARD_HEIGHT / 2),
        .bl_x = x - (CARD_WIDTH / 2), .bl_y = y + (CARD_HEIGHT / 2),
        .br_x = x + (CARD_WIDTH / 2), .br_y = y + (CARD_HEIGHT / 2),
    };

    return new_card;
}
