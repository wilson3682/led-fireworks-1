#include <Adafruit_NeoPixel.h>

//ARDUINO
// const int STRIP_PIN = 6;      // Digital pin connected to DIN on the led strip
// const int NUM_PIXELS = 20;    // How many individual LEDs are connected on the strip
// const int WORM_LENGTH = 3;    // How long a single light of worms is

//ESP32
const int STRIP_PIN = 32;   // Digital pin connected to DIN on the led strip
const int NUM_PIXELS = 120; // How many individual LEDs are connected on the strip
const int WORM_LENGTH = 12; // How long a single light of worms is

Adafruit_NeoPixel pixels(NUM_PIXELS, STRIP_PIN, NEO_GRB + NEO_KHZ800); //set up pixels object

uint32_t red = pixels.Color(100, 0, 0);        // rgb(100,0,0)
uint32_t orange = pixels.Color(100, 50, 0);    // rgb(100,50,0)
uint32_t yellow = pixels.Color(100, 100, 0);   // rgb(100,100,0)
uint32_t green = pixels.Color(0, 100, 0);      // rgb(0,100,0)
uint32_t light_blue = pixels.Color(0, 0, 100); // rgb(0,100,100)
uint32_t blue = pixels.Color(0, 0, 100);       // rgb(0,0,100)
uint32_t indigo = pixels.Color(150, 0, 150);   // rgb(50,0,100)
uint32_t magenta = pixels.Color(150, 0, 150);  // rgb(100,0,100)

const int NUM_COLOURS = 8;
uint32_t colorList[NUM_COLOURS]{red, orange, yellow, green, light_blue, blue, indigo, magenta};

/**
  * returns a random colour from a pre-defined list
  */
uint32_t getRandomColor()
{
    int index = random(NUM_COLOURS);
    return colorList[index];
}

typedef struct
{
    int *head;
    uint32_t *color;
} Worm;

const int NUM_MAX_WORMS = 10;

int head0 = -1;
int head1 = -1;
int head2 = -1;
int head3 = -1;
int head4 = -1;
int head5 = -1;
int head6 = -1;
int head7 = -1;
int head8 = -1;
int head9 = -1;
uint32_t color0 = getRandomColor();
uint32_t color1 = getRandomColor();
uint32_t color2 = getRandomColor();
uint32_t color3 = getRandomColor();
uint32_t color4 = getRandomColor();
uint32_t color5 = getRandomColor();
uint32_t color6 = getRandomColor();
uint32_t color7 = getRandomColor();
uint32_t color8 = getRandomColor();
uint32_t color9 = getRandomColor();

Worm worms[NUM_MAX_WORMS] = {
    {&head0, &color0},
    {&head1, &color1},
    {&head2, &color2},
    {&head3, &color3},
    {&head4, &color4},
    {&head5, &color5},
    {&head6, &color6},
    {&head7, &color7},
    {&head8, &color8},
    {&head9, &color9},
};

/**
 * Returns whether the worm head provided
 */
bool isWormOnStrip(Worm worm)
{
    return *worm.head >= 0;
}

/**
 * Returns whether the worm is past the end of the strip
 */
bool isWormPastStrip(Worm worm)
{
    return (*worm.head - WORM_LENGTH) >= NUM_PIXELS;
}

volatile int lastStart = 0;

/**
 * Handles settings a worm head to pixel 0 and assigning it a new colour.
 * Uses a debounce implementation to prevent starting worms too close together
 * as a result of fluctuating triggers.
 */
void startWorm()
{
    if (millis() - lastStart <= 300)
    {
        return;
    }

    Serial.println("\nTrying to start a worm");

    // find the first worm not on the strip and add it to the strip
    for (int i = 0; i < NUM_MAX_WORMS; i++)
    {
        Worm worm = worms[i];
        if (!isWormOnStrip(worm))
        {
            Serial.println("Starting worm at index " + String(i));
            *worm.head = 0;
            *worm.color = getRandomColor();
            break;
        }
    }
    lastStart = millis();
}

/**
 * Fills a worm based on the provided head index and colour
 */
void fillWorm(Worm worm)
{
    int wormHeadIdx = *worm.head;
    uint32_t wormColor = *worm.color;

    int count = wormHeadIdx < WORM_LENGTH - 1 ? wormHeadIdx + 1 : WORM_LENGTH;
    int theoreticalWormTail = wormHeadIdx - WORM_LENGTH + 1;
    int wormTail = theoreticalWormTail < 0 ? 0 : theoreticalWormTail;
    if (wormTail - 1 >= 0)
    {
        pixels.setPixelColor(wormTail - 1, 0, 0, 0);
    }

    pixels.fill(wormColor, wormTail, count);
}

/**
 * Handles checking each worm to fill, and incrementing the worm where necessary.
 */
void tickWorms()
{
    for (int i = 0; i < NUM_MAX_WORMS; i++)
    {
        Worm worm = worms[i];
        if (isWormOnStrip(worm))
        {
            // fill the worm and move it for the next tick
            fillWorm(worm);
            *worm.head += 1;
        }
        if (isWormPastStrip(worm))
        {
            // reset the worm
            *worm.head = -1;
        }
    }
    pixels.show();
}