#include <allegro5/allegro.h>
#include <allegro5/allegro_image.h>
#include <allegro5/allegro_audio.h>
#include <allegro5/allegro_acodec.h>
#include <allegro5/allegro_font.h>
#include <allegro5/allegro_ttf.h>
#include <allegro5/allegro_primitives.h>
#include <math.h>
#include <usrtypes.h>
#include <stdio.h>

#define CARD_WIDTH 98 
#define CARD_HEIGHT 146

// #define TOTAL_CARDS 6
#define CARD_SCALE 2.0f
#define START_ANGLE -0.4f
#define END_ANGLE 0.4f

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
    f32 angle;
} card_t;

card_t create_card(ALLEGRO_BITMAP* bm, s32 x, s32 y);
void sort_cards(card_t cards[]);
void rotate_point(s32* x, s32* y, s32 cx, s32 cy, f32 angle);
b8 lines_hit(s32 x1, s32 y1, s32 x2, s32 y2, s32 x3, s32 y3, s32 x4, s32 y4);

u32 TOTAL_CARDS = 6;

int main(int argc, char const *argv[])
{
    al_init();
    al_init_image_addon();
    al_init_font_addon();
    al_init_ttf_addon();
    al_install_audio();
    al_install_mouse();
    al_init_acodec_addon();
    al_init_primitives_addon();

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

    card_t cards[32];
    ALLEGRO_BITMAP* bm1 = al_load_bitmap("./res/card1.png");
    ALLEGRO_BITMAP* bm2 = al_load_bitmap("./res/card2.png");
    cards[0] = create_card(bm1, 0, 0);
    cards[1] = create_card(bm1, 0, 0);
    cards[2] = create_card(bm1, 0, 0);
    cards[3] = create_card(bm1, 0, 0);
    cards[4] = create_card(bm1, 0, 0);
    cards[5] = create_card(bm1, 0, 0);
    f32 angle = 0.0f;
    sort_cards(cards);

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

            for (size_t i = TOTAL_CARDS - 1; i > -1; i--)
            {
                // if ()
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
                    cards[i].cx, cards[i].cy, cards[i].angle, 0);
                al_draw_textf(font, al_map_rgb(255, 255, 255), 8, 8, 0, "%f", angle);

                al_draw_line(FB_WIDTH / 2, 0, FB_WIDTH / 2, FB_HEIGHT, al_map_rgb(255, 255, 255), 3);
            }

            al_draw_pixel(FB_WIDTH / 2, FB_HEIGHT / 2, al_map_rgb(255, 255, 255));

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
    al_destroy_bitmap(bm1);
    al_destroy_bitmap(bm2);
    al_destroy_font(font);
    al_unregister_event_source(event_queue, al_get_mouse_event_source());
    al_unregister_event_source(event_queue, al_get_timer_event_source(timer));
    al_unregister_event_source(event_queue, al_get_display_event_source(display));
    al_destroy_timer(timer);
    al_destroy_event_queue(event_queue);
    al_destroy_display(display);
    al_uninstall_audio();
    al_uninstall_mouse();
    al_shutdown_primitives_addon();
    al_shutdown_ttf_addon();
    al_shutdown_font_addon();
    al_shutdown_image_addon();

    return 0;
}

card_t create_card(ALLEGRO_BITMAP* bm, s32 x, s32 y)
{
    card_t new_card = {
        .bm = bm, .cx = x, .cy = y, .entered = false, .got_offset = false, .follow = false,
        .tl_x = x - (CARD_WIDTH / 2), .tl_y = y - (CARD_HEIGHT / 2),
        .tr_x = x + (CARD_WIDTH / 2), .tr_y = y - (CARD_HEIGHT / 2),
        .bl_x = x - (CARD_WIDTH / 2), .bl_y = y + (CARD_HEIGHT / 2),
        .br_x = x + (CARD_WIDTH / 2), .br_y = y + (CARD_HEIGHT / 2),
        .angle = 0.0f
    };

    return new_card;
}

void sort_cards(card_t cards[])
{
    f32 angle_increment = (f32) (END_ANGLE - START_ANGLE) / (f32) (TOTAL_CARDS - 1);
    f32 angle = START_ANGLE;
    u32 x_increment = 50;
    s32 x = (FB_WIDTH / 2) - ((TOTAL_CARDS * x_increment) / 2) + 25;
    s32 y_increment = 5;
    
    // Apply spread
    if (TOTAL_CARDS % 2 == 0) // if number of cards is even
    {
        // to the left
        for (size_t i = (TOTAL_CARDS / 2) - 1; i != -1; i--)
        {
            cards[i].cy = y_increment + 400;
            printf("left (%lld): %d\n", i, y_increment);
            y_increment += y_increment + 5;
        }

        // to the right
        y_increment = 5;

        for (size_t i = TOTAL_CARDS / 2; i < TOTAL_CARDS; i++)
        {
            cards[i].cy = y_increment + 400;
            printf("right (%lld): %d\n", i, y_increment);
            y_increment += y_increment + 5;
            
        }
    }
    
    // apply angle
    for (size_t i = 0; i < TOTAL_CARDS; i++)
    {
        cards[i].angle = angle;
        cards[i].cx = x;
        angle += angle_increment;
        x += x_increment;

        // set card corners
        cards[i].tl_x = cards[i].cx - (CARD_WIDTH / 2); cards[i].tl_y = cards[i].cy - (CARD_HEIGHT / 2);
        cards[i].tr_x = cards[i].cx + (CARD_WIDTH / 2); cards[i].tr_y = cards[i].cy - (CARD_HEIGHT / 2);
        cards[i].bl_x = cards[i].cx - (CARD_WIDTH / 2); cards[i].bl_y = cards[i].cy + (CARD_HEIGHT / 2);
        cards[i].br_x = cards[i].cx + (CARD_WIDTH / 2); cards[i].br_y = cards[i].cy + (CARD_HEIGHT / 2);

    }
}

void rotate_point(s32* x, s32* y, s32 cx, s32 cy, f32 angle)
{
    f32 s = sinf(angle);
    f32 c = cosf(angle);

    *x -= cx;
    *y -= cy;

    f32 new_x = *x * c - *y * s;
    f32 new_y = *x * s + *y * c;

    *x = new_x + cx;
    *y = new_y + cy;
}

b8 lines_hit(s32 x1, s32 y1, s32 x2, s32 y2, s32 x3, s32 y3, s32 x4, s32 y4)
{
    f32 den = ((x2 - x1) * (y4 - y3)) - ((y2 - y1) * (x4 - x3));
    f32 num1 = ((y1 - y3) * (x4 - x3)) - ((x1 - x3) * (y4 - y3));
    f32 num2 = ((y1 - y3) * (x2 - x1)) - ((x1 - x3) * (y2 - y1));

    if (den == 0.0f)
    {
        return num1 == 0.0f && num2 == 0.0f;
    }

    f32 r = num1 / den;
    f32 s = num2 / den;

    return (r >= 0.0f && r <= 1.0f) && (s >= 0.0f && s <= 1.0f);
}
