#pragma once

#include <LovyanGFX.hpp>

class LGFX : public lgfx::LGFX_Device
{

  lgfx::Panel_ILI9341 _panel;
  lgfx::Bus_SPI _bus;
  lgfx::Light_PWM _light;
  lgfx::Touch_FT5x06 _touch;

public:

  LGFX(void)
  {

    // SPI BUS CONFIGURATION
    {
      auto cfg = _bus.config();

      cfg.spi_host = SPI2_HOST;
      cfg.spi_mode = 3;

      cfg.freq_write = 80000000;
      cfg.freq_read  = 16000000;

      cfg.pin_sclk = 12;   // CHANGE
      cfg.pin_mosi = 11;   // CHANGE
      cfg.pin_miso = 13;

      cfg.pin_dc = 46;     // CHANGE

      _bus.config(cfg);
      _panel.setBus(&_bus);
    }


    // DISPLAY PANEL CONFIGURATION
    {
      auto cfg = _panel.config();

      cfg.pin_cs  = 10;     // CHANGE
      cfg.pin_rst = -1;     // CHANGE

      cfg.panel_width  = 240;
      cfg.panel_height = 320;

      cfg.memory_width  = 240;
      cfg.memory_height = 320;

      cfg.offset_rotation = 0;

      cfg.dummy_read_pixel = 8;
      cfg.dummy_read_bits  = 1;

      cfg.readable = false;
      cfg.invert = true;

      _panel.config(cfg);
    }


    // BACKLIGHT CONFIGURATION
    {
      auto cfg = _light.config();

      cfg.pin_bl = 45;       // CHANGE to your BL pin
      cfg.invert = false;

      cfg.freq = 44100;
      cfg.pwm_channel = 7;

      _light.config(cfg);

      _panel.setLight(&_light);
    }

    // TOUCH CONFIGURATION (FT6x36)
    {
      auto cfg = _touch.config();

      cfg.i2c_port = 0;
      cfg.i2c_addr = 0x38;

      cfg.pin_sda = 16;   // CHANGE if your board uses different pins
      cfg.pin_scl = 15;   // CHANGE if your board uses different pins

      cfg.pin_int = -1;
      cfg.pin_rst = -1;

      cfg.x_min = 0;
      cfg.x_max = 239;
      cfg.y_min = 0;
      cfg.y_max = 319;

      _touch.config(cfg);

      _panel.setTouch(&_touch);
    }

    setPanel(&_panel);

  }

};