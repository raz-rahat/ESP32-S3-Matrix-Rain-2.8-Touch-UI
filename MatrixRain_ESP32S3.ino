/* ================================================================
   MATRIX RAIN  —  ESP32-S3-N16R8 + 2.8" 240x320 (ST7789) + XPT2046 
   by MD RAZ

   ESP32-S3 N16R8
   ST7789 240x320
   XPT2046 Touch
   MPU6500 / MPU6050 FAMILY

   TFT:
   VCC      3.3V
   GND      GND
   CS       GPIO 10
   RST      GPIO 8
   DC       GPIO 9
   MOSI     GPIO 11
   SCK      GPIO 13
   LED      3.3V
   MISO     GPIO 12
   TOUCH:
   TCLK     GPIO 13
   T_CS     GPIO 7
   T_DIN    GPIO 11
   T_DO     GPIO 12
   T_IRQ    GPIO 6
   MPU:
   SDA      GPIO 16
   SCL      GPIO 15
   ADDRESS  0x68

   ================================================================ */

#include <Arduino.h>
#include <math.h>
#include <SPI.h>
#include <Adafruit_GFX.h>
#include <Adafruit_ST7789.h>
#include <XPT2046_Touchscreen.h>

// ---------------- PIN MAP (Touchpad_V2.ino থেকে) ----------------
#define TFT_SCK   13
#define TFT_MISO  12
#define TFT_MOSI  11
#define TFT_CS    10
#define TFT_DC     9
#define TFT_RST    8
#define TFT_LED    5
#define TOUCH_CS   7
// #define TOUCH_IRQ  6      // যদি T_IRQ ওয়্যার করা থাকে, আনকমেন্ট করো

Adafruit_ST7789 tft(&SPI, TFT_CS, TFT_DC, TFT_RST);
#ifdef TOUCH_IRQ
XPT2046_Touchscreen touch(TOUCH_CS, TOUCH_IRQ);
#else
XPT2046_Touchscreen touch(TOUCH_CS);
#endif

#define BL_PIN     5          // ব্যাকলাইট। 3V3 এ লাগানো থাকলে -1 করে দিন

/* ---- টাচ ক্যালিব্রেশন ------------------------------------------
   Touchpad_V2.ino থেকে নেওয়া raw মান (একই বোর্ড/একই টাচ প্যানেল ধরে)।
   টাচ জায়গামতো না লাগলে RUN_CALIBRATION 1 করে আপলোড করো, Serial
   Monitor-এ কোনায় কোনায় চাপলে raw X/Y প্রিন্ট হবে — সেগুলো বসিয়ে
   আবার 0 করে দাও। */
#define RUN_CALIBRATION 0
const int RAW_X_MIN = 430;
const int RAW_X_MAX = 3706;
const int RAW_Y_MIN = 310;
const int RAW_Y_MAX = 3679;

/* ---- গ্রিড ---- */
#define CHAR_W    12
#define CHAR_H    16
#define BAR_H     48
#define MAX_COLS  27
#define MAX_ROWS  20

/* ---- প্যালেট ---- */
struct Palette { const char* name; uint16_t head, g1, g2, g3; };
const Palette PAL[] = {
  { "GREEN",   0xDFFF, 0x07E0, 0x0480, 0x0220 },
  { "AMBER",   0xFFFF, 0xFD20, 0x9A80, 0x4920 },
  { "ICE",     0xFFFF, 0x07FF, 0x0492, 0x0209 },
  { "MAGENTA", 0xFFFF, 0xF81F, 0x9010, 0x4008 },
  { "CRIMSON", 0xFFFF, 0xF800, 0x9000, 0x4000 },
  { "GHOST",   0xFFFF, 0xC618, 0x7BEF, 0x39E7 }
};
const int NPAL = sizeof(PAL) / sizeof(PAL[0]);

/* ---- অবস্থা ---- */
int  palIdx   = 0;
int  speedPct = 100;   // 30 – 220
int  trailMax = 13;    // 4 – 20
int  blinkAmt = 4;     // 0 – 12
uint8_t rot   = 0;     // 0 = portrait, 1 = landscape
bool barVisible = true;

enum Mode { RAIN, SETTINGS };
Mode mode = RAIN;

/* ---- লেআউট ---- */
int W, H, COLS, ROWS, activeRows;
int SX, SW, SY[3], OY, OH, BY, BH, CY;

/* ---- রেইন ডাটা ---- */
char  grid[MAX_COLS][MAX_ROWS];
float headPos[MAX_COLS], spd[MAX_COLS];
int   tailLen[MAX_COLS], lastRow[MAX_COLS];

const char charset[] = "0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZ:.=*+-<>|#$@%&/\\";
const int  charsetLen = sizeof(charset) - 1;
inline char rndChar() { return charset[random(charsetLen)]; }

const char* SLIDER_NAME[3] = { "SPEED", "TRAIL", "BLINK" };
const int   SLIDER_MIN[3]  = { 30, 4, 0 };
const int   SLIDER_MAX[3]  = { 220, 20, 12 };
int sliderVal(int i) { return i == 0 ? speedPct : (i == 1 ? trailMax : blinkAmt); }
void setSlider(int i, int v) { if (i == 0) speedPct = v; else if (i == 1) trailMax = v; else blinkAmt = v; }

/* ================= টাচ ম্যাপিং (raw -> screen, rotation-aware) =================
   rot==0 (পোর্ট্রেট)  : Touchpad_V2 এর মতোই direct mapping (প্রমাণিত কাজ করে)
   rot==1 (ল্যান্ডস্কেপ): axis swap করা — যদি টাচ উল্টো/আয়না মনে হয়, নিচের
                          INVERT_LANDSCAPE_X / _Y টা true/false করে দেখো। */
const bool INVERT_LANDSCAPE_X = false;
const bool INVERT_LANDSCAPE_Y = true;

void mapTouch(int rawX, int rawY, int &x, int &y) {
  if (rot == 0) {
    x = map(rawX, RAW_X_MIN, RAW_X_MAX, 0, W - 1);
    y = map(rawY, RAW_Y_MIN, RAW_Y_MAX, 0, H - 1);
  } else {
    x = map(rawY, RAW_Y_MIN, RAW_Y_MAX, 0, W - 1);
    y = map(rawX, RAW_X_MIN, RAW_X_MAX, 0, H - 1);
    if (INVERT_LANDSCAPE_X) x = W - 1 - x;
    if (INVERT_LANDSCAPE_Y) y = H - 1 - y;
  }
  x = constrain(x, 0, W - 1);
  y = constrain(y, 0, H - 1);
}

#define TFT_BLACK ST77XX_BLACK

/* ---- TFT_eSPI-স্টাইল drawString()/setTextDatum() শিম (Adafruit_GFX-এর উপরে) ----
   আসল TFT_eSPI কোডের UI ফাংশনগুলো (drawBar, drawSettings ইত্যাদি) অপরিবর্তিত
   রাখতে এই ছোট wrapper যোগ করা হয়েছে, যাতে বড় রিরাইট করতে না হয়। */
enum { TL_DATUM, TC_DATUM, TR_DATUM, MC_DATUM, BC_DATUM };
int _textDatum = TL_DATUM;
void setTextDatum(int d) { _textDatum = d; }

void drawString(const String &s, int x, int y, int tftFontNum) {
  uint8_t sz = (tftFontNum >= 4) ? 3 : 2;   // TFT_eSPI ফন্ট 2/4 -> GFX টেক্সট সাইজ আনুমানিক
  tft.setTextSize(sz);
  int16_t x1, y1; uint16_t w, h;
  tft.getTextBounds(s, 0, 0, &x1, &y1, &w, &h);
  int cx = x, cy = y;
  switch (_textDatum) {
    case TC_DATUM: cx -= w / 2; break;
    case TR_DATUM: cx -= w; break;
    case MC_DATUM: cx -= w / 2; cy -= h / 2; break;
    case BC_DATUM: cx -= w / 2; cy -= h; break;
    default: break; // TL_DATUM
  }
  tft.setCursor(cx, cy);
  tft.print(s);
}

/* ================= লেআউট ================= */
void applyRotation() {
  // rot==0 -> Touchpad_V2 এর প্রমাণিত rotation(2), portrait 240x320
  // rot==1 -> landscape, 320x240 (একই দিক-90 ডিগ্রি ঘোরানো)
  tft.setRotation(rot == 0 ? 2 : 3);
  W = tft.width();  H = tft.height();
  COLS = W / CHAR_W; if (COLS > MAX_COLS) COLS = MAX_COLS;
  ROWS = H / CHAR_H; if (ROWS > MAX_ROWS) ROWS = MAX_ROWS;

  SX = 16; SW = W - 32;
  int y0  = (rot == 0) ? 58 : 38;
  int gap = (rot == 0) ? 48 : 38;
  for (int i = 0; i < 3; i++) SY[i] = y0 + i * gap;
  OY = y0 + 3 * gap;
  OH = (rot == 0) ? 34 : 30;
  BY = H - ((rot == 0) ? 68 : 50);
  BH = (rot == 0) ? 36 : 30;
  CY = H - 16;
  updateActiveRows();
}

void updateActiveRows() {
  activeRows = barVisible ? (H - BAR_H) / CHAR_H : ROWS;
  if (activeRows > ROWS) activeRows = ROWS;
}

/* ================= রেইন ================= */
void resetColumn(int c) {
  headPos[c] = -(float)random(0, activeRows);
  spd[c]     = random(25, 110) / 100.0f;
  tailLen[c] = random(max(3, (trailMax * 45) / 100), trailMax + 1);
  lastRow[c] = -999;
}

void rainFont() { tft.setTextSize(2); setTextDatum(TL_DATUM); }

void drawCell(int c, int r, char ch, uint16_t color) {
  if (r < 0 || r >= activeRows) return;
  tft.setTextColor(color, TFT_BLACK);
  tft.setCursor(c * CHAR_W, r * CHAR_H);
  tft.print(ch);
}

void clearCell(int c, int r) {
  if (r < 0 || r >= activeRows) return;
  tft.fillRect(c * CHAR_W, r * CHAR_H, CHAR_W, CHAR_H, TFT_BLACK);
}

void startRain(bool clearAll) {
  mode = RAIN;
  if (clearAll) tft.fillScreen(TFT_BLACK);
  rainFont();
  for (int c = 0; c < COLS; c++) {
    for (int r = 0; r < MAX_ROWS; r++) grid[c][r] = ' ';
    resetColumn(c);
  }
  if (barVisible) drawBar();
}

void rainStep() {
  const Palette &P = PAL[palIdx];
  rainFont();
  for (int c = 0; c < COLS; c++) {
    headPos[c] += spd[c] * (speedPct / 100.0f);
    int cur = (int)floorf(headPos[c]);
    if (cur != lastRow[c]) {
      lastRow[c] = cur;
      if (cur - 1 >= 0) drawCell(c, cur - 1, grid[c][cur - 1], P.g1);
      if (cur - 3 >= 0) drawCell(c, cur - 3, grid[c][cur - 3], P.g2);
      if (cur - 6 >= 0) drawCell(c, cur - 6, grid[c][cur - 6], P.g3);
      if (cur >= 0 && cur < activeRows) {
        grid[c][cur] = rndChar();
        drawCell(c, cur, grid[c][cur], P.head);
      }
      int t = cur - tailLen[c];
      if (t >= 0 && t < activeRows) { clearCell(c, t); grid[c][t] = ' '; }
      if (cur - tailLen[c] > activeRows) resetColumn(c);
    }
  }
  for (int i = 0; i < blinkAmt; i++) {
    int c = random(COLS), r = random(activeRows);
    if (grid[c][r] != ' ') { grid[c][r] = rndChar(); drawCell(c, r, grid[c][r], P.g2); }
  }
}

/* ================= নিচের বার ================= */
void drawBar() {
  const Palette &P = PAL[palIdx];
  int by = H - BAR_H, third = W / 3;

  tft.fillRect(0, by, W, BAR_H, TFT_BLACK);
  tft.drawFastHLine(0, by, W, P.g3);
  tft.drawFastVLine(third, by + 8, BAR_H - 16, P.g3);
  tft.drawFastVLine(third * 2, by + 8, BAR_H - 16, P.g3);

  int cy = by + BAR_H / 2;
  setTextDatum(MC_DATUM);
  tft.setTextColor(P.g1, TFT_BLACK);

  // PREV
  tft.fillTriangle(third / 2 - 26, cy, third / 2 - 16, cy - 7, third / 2 - 16, cy + 7, P.g1);
  drawString("<<<", third / 2 + 6, cy, 2);
  // SETTINGS
  tft.setTextColor(P.head, TFT_BLACK);
  drawString("SETUP", third + third / 2, cy, 2);
  // NEXT
  tft.setTextColor(P.g1, TFT_BLACK);
  drawString(">>>", third * 2 + third / 2 - 6, cy, 2);
  tft.fillTriangle(third * 2 + third / 2 + 26, cy, third * 2 + third / 2 + 16, cy - 7,
                   third * 2 + third / 2 + 16, cy + 7, P.g1);
}

void toggleBar() {
  barVisible = !barVisible;
  tft.fillRect(0, H - BAR_H, W, BAR_H, TFT_BLACK);
  updateActiveRows();
  for (int c = 0; c < COLS; c++) {
    for (int r = activeRows; r < MAX_ROWS; r++) grid[c][r] = ' ';
    if (headPos[c] > activeRows + tailLen[c]) resetColumn(c);
  }
  if (barVisible) drawBar();
}

void changePalette(int dir) {
  palIdx = (palIdx + dir + NPAL) % NPAL;
  startRain(true);
  // নতুন রঙের নাম এক ঝলক
  setTextDatum(MC_DATUM);
  tft.setTextColor(PAL[palIdx].head, TFT_BLACK);
  drawString(PAL[palIdx].name, W / 2, 24, 4);
  delay(650);
  tft.fillRect(0, 8, W, 34, TFT_BLACK);
  for (int c = 0; c < COLS; c++) for (int r = 0; r < 3; r++) grid[c][r] = ' ';
}

/* ================= সেটিংস স্ক্রিন ================= */
void drawSlider(int i) {
  const Palette &P = PAL[palIdx];
  int y = SY[i], ty = y + 18;
  int v = sliderVal(i), mn = SLIDER_MIN[i], mx = SLIDER_MAX[i];
  int knob = SX + (int)((long)(v - mn) * SW / (mx - mn));

  tft.fillRect(0, y - 4, W, 42, TFT_BLACK);
  setTextDatum(TL_DATUM);
  tft.setTextColor(P.g1, TFT_BLACK);
  drawString(SLIDER_NAME[i], SX, y, 2);

  char buf[8];
  if (i == 0) snprintf(buf, sizeof(buf), "%d%%", v);
  else        snprintf(buf, sizeof(buf), "%d", v);
  setTextDatum(TR_DATUM);
  tft.setTextColor(P.head, TFT_BLACK);
  drawString(buf, SX + SW, y, 2);

  tft.fillRoundRect(SX, ty, SW, 6, 3, 0x2124);
  tft.fillRoundRect(SX, ty, max(6, knob - SX), 6, 3, P.g1);
  tft.fillCircle(knob, ty + 3, 8, P.head);
  tft.drawCircle(knob, ty + 3, 8, P.g2);
}

void drawOrientation() {
  const Palette &P = PAL[palIdx];
  int half = (SW - 8) / 2;
  tft.fillRect(0, OY - 20, W, OH + 22, TFT_BLACK);
  setTextDatum(TL_DATUM);
  tft.setTextColor(P.g1, TFT_BLACK);
  drawString("ORIENTATION", SX, OY - 18, 2);

  for (int i = 0; i < 2; i++) {
    int x = SX + i * (half + 8);
    bool on = (rot == i);
    tft.fillRoundRect(x, OY, half, OH, 4, on ? P.g1 : TFT_BLACK);
    tft.drawRoundRect(x, OY, half, OH, 4, on ? P.g1 : P.g2);
    setTextDatum(MC_DATUM);
    tft.setTextColor(on ? TFT_BLACK : P.g1, on ? P.g1 : TFT_BLACK);
    drawString(i == 0 ? "PORTRAIT" : "ROTATE", x + half / 2, OY + OH / 2, 2);
  }
}

void drawSettings() {
  const Palette &P = PAL[palIdx];
  tft.fillScreen(TFT_BLACK);

  setTextDatum(TC_DATUM);
  tft.setTextColor(P.head, TFT_BLACK);
  drawString("SETTINGS", W / 2, 10, 4);
  tft.setTextColor(P.g2, TFT_BLACK);
  drawString(PAL[palIdx].name, W / 2, (rot == 0) ? 38 : 34, 2);

  for (int i = 0; i < 3; i++) drawSlider(i);
  drawOrientation();

  tft.fillRoundRect(SX, BY, SW, BH, 4, TFT_BLACK);
  tft.drawRoundRect(SX, BY, SW, BH, 4, P.g1);
  setTextDatum(MC_DATUM);
  tft.setTextColor(P.g1, TFT_BLACK);
  tft.fillTriangle(SX + 18, BY + BH / 2, SX + 28, BY + BH / 2 - 6, SX + 28, BY + BH / 2 + 6, P.g1);
  drawString("BACK TO MATRIX", W / 2 + 8, BY + BH / 2, 2);

  setTextDatum(BC_DATUM);
  tft.setTextColor(P.g2, TFT_BLACK);
  drawString("BY MD RAZ", W / 2, CY + 12, 2);
}

void settingsTouch(uint16_t tx, uint16_t ty, bool freshPress) {
  // স্লাইডার — চেপে ধরে টানা যাবে
  for (int i = 0; i < 3; i++) {
    if (ty > SY[i] + 2 && ty < SY[i] + 40) {
      int mn = SLIDER_MIN[i], mx = SLIDER_MAX[i];
      int v = mn + (int)((long)((int)tx - SX) * (mx - mn) / SW);
      if (v < mn) v = mn; if (v > mx) v = mx;
      if (v != sliderVal(i)) { setSlider(i, v); drawSlider(i); }
      return;
    }
  }
  if (!freshPress) return;

  int half = (SW - 8) / 2;
  if (ty > OY && ty < OY + OH) {
    int newRot = (tx < SX + half + 4) ? 0 : 1;
    if (newRot != rot) { rot = newRot; applyRotation(); drawSettings(); }
    return;
  }
  if (ty > BY && ty < BY + BH) { startRain(true); return; }
}

/* ================= টাচ রাউটিং ================= */
bool wasTouch = false;
uint32_t lastPress = 0;

void handleTouch() {
  uint16_t tx = 0, ty = 0;
  bool t = touch.touched();
  if (t) {
    TS_Point p = touch.getPoint();
    int mx, my;
    mapTouch(p.x, p.y, mx, my);
    tx = mx; ty = my;
  }
  bool fresh = (t && !wasTouch && millis() - lastPress > 180);
  if (fresh) lastPress = millis();

  if (mode == RAIN) {
    if (fresh) {
      if (barVisible && ty >= H - BAR_H) {
        int third = W / 3;
        if (tx < third)            changePalette(-1);
        else if (tx < third * 2)   { mode = SETTINGS; drawSettings(); }
        else                       changePalette(1);
      } else {
        toggleBar();
      }
    }
  } else if (t) {
    settingsTouch(tx, ty, fresh);
  }
  wasTouch = t;
}

/* ================= setup / loop ================= */
void setup() {
  Serial.begin(115200);
  delay(300);
#if BL_PIN >= 0
  pinMode(BL_PIN, OUTPUT);
  digitalWrite(BL_PIN, HIGH);
#endif

  SPI.begin(TFT_SCK, TFT_MISO, TFT_MOSI, -1);

  tft.init(240, 320);
  tft.invertDisplay(false);   // রঙ invert (কালো<->সাদা উল্টে) দেখালে এটা true/false টগল করো
  randomSeed(esp_random());

  touch.begin(SPI);
  touch.setRotation(0);

#if RUN_CALIBRATION
  tft.setRotation(2);
  tft.fillScreen(ST77XX_BLACK);
  tft.setTextColor(ST77XX_GREEN);
  tft.setTextSize(1);
  tft.setCursor(20, 150);
  tft.print("Touch corners - watch Serial Monitor");
  Serial.println("--- RAW TOUCH CALIBRATION ---");
  Serial.println("Touch each corner, note the min/max X and Y you see below.");
  while (true) {
    if (touch.touched()) {
      TS_Point p = touch.getPoint();
      Serial.printf("raw X=%d  Y=%d\n", p.x, p.y);
      delay(150);
    }
  }
#endif

  applyRotation();

  // বুট স্প্ল্যাশ
  tft.fillScreen(TFT_BLACK);
  setTextDatum(MC_DATUM);
  tft.setTextColor(PAL[0].g1, TFT_BLACK);
  drawString("MATRIX RAIN", W / 2, H / 2 - 14, 4);
  tft.setTextColor(PAL[0].g2, TFT_BLACK);
  drawString("BY MD RAZ", W / 2, H / 2 + 16, 2);
  delay(1200);

  startRain(true);
}

uint32_t lastFrame = 0;

void loop() {
  handleTouch();
  if (mode == RAIN && millis() - lastFrame >= 25) {
    lastFrame = millis();
    rainStep();
  }
}
