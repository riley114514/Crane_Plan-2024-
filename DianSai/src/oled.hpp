#include <SPI.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

// OLED FeatherWing buttons map to different pins depending on board.
// The I2C (Wire) bus may also be different.

#define BUTTON_A 15
#define BUTTON_B 32
#define BUTTON_C 14
#define WIRE Wire


class Oled
{
public:
    void OLED_Init(void)
    {
        this->display = Adafruit_SSD1306(128, 32, &WIRE);
        display.begin(SSD1306_SWITCHCAPVCC, 0x3C);
    }

    void Show_Text(float y, float z,float distance, int graph)
    {
        this->display.setTextSize(1);
        this->display.setTextColor(SSD1306_WHITE);
        this->display.setCursor(0, 0);
        this->display.print("The graph is : ");
        switch (graph)
        {
        case 1:
            this->display.println("Circle");
            break;
        case 2:
            this->display.println("Square");
            break;
        case 3:
            this->display.println("Triangle");
            break;
        default:
            break;
        }

        this->display.setCursor(6, 0);
        this->display.print("The Distance is : ");
        this->display.println(distance);
        this->display.setCursor(12, 0);
        this->display.print("Center Axis (y,z) is : ");
        this->display.print(y);
        this->display.print(",");
        this->display.println(z);
        this->display.display();
    }

    void Clear_All(void)
    {
        this->display.clearDisplay();

    }
    Adafruit_SSD1306 display;
};
