#define LCD_MOSI MOSI
#define LCD_MISO -1
#define LCD_SCLK SCK
#define LCD_CS SS
#define LCD_DC 47
#define LCD_RST 21
#define LCD_BCKL 48

#include <Arduino.h>
#include <SPI.h>
#include <tft_i2c.hpp>
#include <st7789.hpp>
#include <gfx_cpp14.hpp>

// generated with fontgen
#include "Maziro.h"
// generated with fontgen
#include "Bm437_ATI_9x16.h"
#include "Bm437_Acer_VGA_8x8.h"
#include "image3.h"

using namespace arduino;
using namespace gfx;

using bus_type = tft_spi_ex<0,5,7,-1,6,SPI_MODE0>;

using lcd_type = st7789<240,240,4,8,9,bus_type,1>;

lcd_type lcd;


using lcd_color = color<rgb_pixel<16>>;

using bmp_type = bitmap<decltype(lcd)::pixel_type>;
using bmp_color = color<typename bmp_type::pixel_type>;
using mono_bmp_type = bitmap<gsc_pixel<1>>;
using mono_bmp_color = color<typename mono_bmp_type::pixel_type>;
size16 mono_bmp_size;
uint8_t *mono_buf;
int mono_x;

uint8_t r = 0;
int rd = 1;
uint8_t g = 127;
int gd = 1;
uint8_t b = 255;
int bd = -1;

const char *mono_text =
    "GFX Demo - Copyright (C) 2022 by honey the codewitch - MIT license  ";

// declare the bitmap
constexpr static const size16 bmp_size(16, 16);
uint8_t bmp_buf[bmp_type::sizeof_buffer(bmp_size)];
bmp_type bmp(bmp_size, bmp_buf);
mono_bmp_type mask_buf[mono_bmp_type::sizeof_buffer(bmp_size)];
mono_bmp_type mask(bmp_size,mask_buf);
void bmp_demo() {
    lcd.clear(lcd.bounds());

    // draw stuff

    // fill with the transparent color
    typename bmp_type::pixel_type tpx = bmp_color::cyan;
    bmp.fill(bmp.bounds(), tpx);

    // bounding info for the face
    srect16 bounds(0, 0, bmp_size.width - 1, (bmp_size.height - 1));
    rect16 ubounds(0, 0, bounds.x2, bounds.y2);

    // draw the face
    draw::filled_ellipse(bmp, bounds, bmp_color::yellow);

    // draw the left eye
    srect16 eye_bounds_left(spoint16(bounds.width() / 5, bounds.height() / 5),
                            ssize16(bounds.width() / 5, bounds.height() / 3));
    draw::filled_ellipse(bmp, eye_bounds_left, bmp_color::black);

    // draw the right eye
    srect16 eye_bounds_right(
        spoint16(bmp_size.width - eye_bounds_left.x1 - eye_bounds_left.width(),
                 eye_bounds_left.y1),
        eye_bounds_left.dimensions());
    draw::filled_ellipse(bmp, eye_bounds_right, bmp_color::black);

    // draw the mouth
    srect16 mouth_bounds =
        bounds.inflate(-bounds.width() / 7, -bounds.height() / 8).normalize();
    // we need to clip part of the circle we'll be drawing
    srect16 mouth_clip(mouth_bounds.x1,
                       mouth_bounds.y1 + mouth_bounds.height() / (float)1.6,
                       mouth_bounds.x2, mouth_bounds.y2);

    draw::ellipse(bmp, mouth_bounds, bmp_color::black, &mouth_clip);
    
    mask.clear(mask.bounds());
    draw::filled_ellipse(mask, bounds, bmp_color::white);

    using sprite_type = sprite<typename bmp_type::pixel_type>;

    sprite_type sprite(bmp_size,bmp_buf,mask_buf);
    int dx = 1;
    int dy = 2;
    lcd.clear(lcd.bounds());
    // now we're going to draw the bitmap to the lcd instead, animating it
    int i = 0;
    srect16 r = (srect16)sprite.bounds().center(lcd.bounds());
    while (i < 150) {
        draw::suspend(lcd);
        draw::filled_rectangle(lcd, r, lcd_color::black);
        srect16 r2 = r.offset(dx, dy);
        if (!((srect16)lcd.bounds()).contains(r2)) {
            if (r2.x1 < 0 || r2.x2 > lcd.bounds().x2)
                dx = -dx;
            if (r2.y1 < 0 || r2.y2 > lcd.bounds().y2)
                dy = -dy;
            r = r.offset(dx, dy);
        } else
            r = r2;
        draw::sprite(lcd,point16(r.x1,r.y1),sprite);
        draw::resume(lcd);
        delay(10);
        ++i;
    }
    delay(1000);
}
// produced by request
void scroll_text_demo() {
    lcd.clear(lcd.bounds());
    const font &f = Bm437_ATI_9x16_FON;
    const char *text = "(C) 2021\r\nby HTCW";
    ssize16 text_size = f.measure_text((ssize16)lcd.dimensions(), text);
    srect16 text_rect =
        srect16(spoint16((lcd.dimensions().width - text_size.width) / 2,
                         (lcd.dimensions().height - text_size.height) / 2),
                text_size);
    int16_t text_start = text_rect.x1;
    bool first = true;
    while (true) {
        draw::suspend(lcd);
        draw::filled_rectangle(lcd, text_rect, lcd_color::black);
        if (text_rect.x2 >= 320) {
            draw::filled_rectangle(lcd,
                                   text_rect.offset(-lcd.dimensions().width, 0),
                                   lcd_color::black);
        }

        text_rect = text_rect.offset(2, 0);
        draw::text(lcd, text_rect, text, f, lcd_color::old_lace,
                   lcd_color::black, false);
        if (text_rect.x2 >= lcd.dimensions().width) {
            draw::text(lcd, text_rect.offset(-lcd.dimensions().width, 0), text,
                       f, lcd_color::old_lace, lcd_color::black, false);
        }
        if (text_rect.x1 >= lcd.dimensions().width) {
            text_rect = text_rect.offset(-lcd.dimensions().width, 0);
            first = false;
        }
        draw::resume(lcd);
        if (!first && text_rect.x1 >= text_start)
            break;
    }
}

void lines_demo() {
#ifdef LCD_EPAPER
    draw::suspend(lcd);
#endif
    draw::filled_rectangle(lcd, (srect16)lcd.bounds(), lcd_color::white);
    const open_font &f = Maziro_ttf;
    const font &f2 = Bm437_ATI_9x16_FON;
    const char *text = "GFX";
    const char *text2 =
        (lcd.dimensions().width > 200) ? "honey the codewitch" : "HTCW";
    int height = min(lcd.dimensions().width, lcd.dimensions().height) / 2;
    const float scale = f.scale(height);
    srect16 text_rect =
        srect16(spoint16(0, 0),
                f.measure_text((ssize16)lcd.dimensions(), {0, 0}, text, scale));
    draw::text(lcd, text_rect.center((srect16)lcd.bounds()), {0, 0}, text, f,
               scale, lcd_color::blue, lcd_color::white, true, true);
    if (lcd.dimensions().height > 32) {
        text_rect = srect16(spoint16(0, 0),
                            f2.measure_text((ssize16)lcd.dimensions(), text2));
        uint16_t offsy = -(text_rect.height() / 2 + 16);
        if (lcd.dimensions().width > 200) {
            offsy = text_rect.height() + (text_rect.height() / 2)+16;
        }
        srect16 sr = text_rect.center((srect16)lcd.bounds()).offset(0, offsy);
        draw::text(lcd, sr, text2, f2, lcd_color::black);
    }

    for (int i = 1; i < 100; i += 4) {
        // calculate our extents
        srect16 r(
            i * (lcd.dimensions().width / 100.0),
            i * (lcd.dimensions().height / 100.0),
            lcd.dimensions().width - i * (lcd.dimensions().width / 100.0) - 1,
            lcd.dimensions().height - i * (lcd.dimensions().height / 100.0) -
                1);

        draw::line(lcd, srect16(0, r.y1, r.x1, lcd.dimensions().height - 1),
                   lcd_color::red);
        draw::line(lcd, srect16(r.x2, 0, lcd.dimensions().width - 1, r.y2),
                   lcd_color::yellow);
        draw::line(lcd, srect16(0, r.y2, r.x1, 0), lcd_color::orange);
        draw::line(lcd,
                   srect16(lcd.dimensions().width - 1, r.y1, r.x2,
                           lcd.dimensions().height - 1),
                   lcd_color::green);
    }

#ifdef LCD_EPAPER
    draw::resume(lcd);
#endif

    delay(500);
}
void alpha_demo() {

    draw::filled_rectangle(lcd, (srect16)lcd.bounds(), lcd_color::black);

    for (int y = 0; y < lcd.dimensions().height; y += 16) {
        for (int x = 0; x < lcd.dimensions().width; x += 16) {
            if (0 != ((x + y) % 32)) {
                draw::filled_rectangle(lcd,
                                       srect16(spoint16(x, y), ssize16(16, 16)),
                                       lcd_color::white);
            }
        }
    }
    randomSeed(millis());

    rgba_pixel<32> px;
    spoint16 tpa[3];
    const uint16_t sw =
        min(lcd.dimensions().width, lcd.dimensions().height) / 4;
    for (int i = 0; i < 30; ++i) {
        px.channel<channel_name::R>((rand() % 256));
        px.channel<channel_name::G>((rand() % 256));
        px.channel<channel_name::B>((rand() % 256));
        px.channel<channel_name::A>(50 + rand() % 156);
        srect16 sr(0, 0, rand() % sw + sw, rand() % sw + sw);
        sr.offset_inplace(rand() % (lcd.dimensions().width - sr.width()),
                          rand() % (lcd.dimensions().height - sr.height()));
        switch (rand() % 4) {
            case 0:
                draw::filled_rectangle(lcd, sr, px);
                break;
            case 1:
                draw::filled_rounded_rectangle(lcd, sr, .1, px);
                break;
            case 2:
                draw::filled_ellipse(lcd, sr, px);
                break;
            case 3:
                tpa[0] = {int16_t(((sr.x2 - sr.x1) / 2) + sr.x1), sr.y1};
                tpa[1] = {sr.x2, sr.y2};
                tpa[2] = {sr.x1, sr.y2};
                spath16 path(3, tpa);
                draw::filled_polygon(lcd, path, px);
                break;
        }
    }

    delay(2000);
}

void setup() {
    Serial.begin(115200);
    
    if (lcd.dimensions().height > 1 && lcd.dimensions().height < 16) {
        size_t len = strlen(mono_text);
        const font &f = Bm437_Acer_VGA_8x8_FON;
        mono_bmp_size = size16(f.measure_text({int16_t(len * f.average_width()),
                                               (int16_t)f.height()},
                                              mono_text)
                                   .width,
                               lcd.dimensions().height);
        mono_buf = (uint8_t *)malloc(bmp_type::sizeof_buffer(mono_bmp_size));
        mono_bmp_type bmp(mono_bmp_size, mono_buf);
        bmp.clear(bmp.bounds());
        mono_x = 0;
        int cy = (bmp.dimensions().height - f.height()) / 2;
        draw::text(bmp,
                   srect16(spoint16(0, cy),
                           ssize16(bmp.dimensions().width, f.height())),
                   mono_text, f, mono_bmp_color::white);
    }
}
void loop() {
  lines_demo();
  // these are both animated, so the e-paper display can't support them.
  scroll_text_demo();
  bmp_demo();

  // won't look great on a limited color display
  alpha_demo();

  image3_jpg_stream.seek(0);
  draw::image(lcd, srect16(0, 0, 335, 255).center((srect16)lcd.bounds()),
              &image3_jpg_stream);

  delay(2000);
}
