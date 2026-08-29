# 🟢 Matrix Rain Cyberpunk UI (ESP32-S3 + ST7789 + XPT2046)

<p align="center">
  <img src="https://img.shields.io/badge/Board-ESP32--S3%20N16R8-red?style=for-the-badge&logo=expressif" alt="ESP32-S3">
  <img src="https://img.shields.io/badge/Display-ST7789%20240x320-blue?style=for-the-badge" alt="ST7789">
  <img src="https://img.shields.io/badge/Touch-XPT2046-green?style=for-the-badge" alt="XPT2046">
  <img src="https://img.shields.io/badge/Author-MD%20RAZ-orange?style=for-the-badge" alt="MD RAZ">
  <img src="https://img.shields.io/badge/Language-C%2B%2B%20%2F%20Arduino-00599C?style=for-the-badge&logo=cplusplus" alt="C++">
</p>

---

## 📌 প্রজেক্ট পরিচিতি (Project Overview)
**Matrix Rain Cyberpunk UI** হলো সাইবারপাঙ্ক ও হ্যাকার ভাইবযুক্ত একটি ইন্টারঅ্যাক্টিভ ডিজিটাল ম্যাট্রিক্স কোড রেইন প্রজেক্ট। ESP32-S3 এবং ২.৮" TFT ডিসপ্লে ব্যবহার করে বিখ্যাত *'The Matrix'* মুভির মতো আইকনিক সবুজ রঙের কোড ড্রপ অ্যানিমেশন ডিসপ্লেতে ফুটিয়ে তোলা হয়েছে।

এই প্রজেক্টে রয়েছে সম্পূর্ণ টাচস্ক্রিন কন্ট্রোল, লাইভ কালার প্যালেট চেইঞ্জার, পোর্ট্রেট (Portrait) ও ল্যান্ডস্কেপ (Landscape) মোড সুইচ করার সুবিধা, এবং অ্যানিমেশনের স্পিড, ট্রেইল লেন্থ ও ব্লিঙ্ক রেট অ্যাডজাস্ট করার জন্য একটি ফুল কাস্টম সেটিংস মেনু।

---

## ✨ প্রধান ফিচারসমূহ (Key Features)

- 🟩 **আইকনিক ডিজিটাল ম্যাট্রিক্স রেইন:** অপটিমাইজড Adafruit GFX ইঞ্জিন ব্যবহার করে স্মুথ অ্যানিমেশন।
- 🎨 **৬টি কালার থিম/প্যালেট (Color Palettes):**
  - `GREEN` (ক্লাসিক্যাল ম্যাট্রিক্স গ্রিন)
  - `AMBER` (ওয়ার্ম রেট্রো অ্যাম্বার)
  - `ICE` (কুল সাইবার আইস ব্লু)
  - `MAGENTA` (ফিউচারিস্টিক নিয়ন পিঙ্ক/ম্যাজেন্টা)
  - `CRIMSON` (ডার্ক রেড ক্রিমসন)
  - `GHOST` (মনোক্রোম হোয়াইট গোটিক)
- 📱 **পোর্ট্রেট ও ল্যান্ডস্কেপ ডুয়াল মোড:** সরাসরি টাচ মেনু থেকেই Portrait এবং Landscape Orientation চেঞ্জ করার সুবিধা।
- 🎛️ **ইন্টারঅ্যাক্টিভ টাচ সেটিং স্ক্রিন (Touch UI):**
  - **SPEED Slider:** ৩০% থেকে ২২০% পর্যন্ত বৃষ্টির বেগ নিয়ন্ত্রণের ব্যবস্থা।
  - **TRAIL Slider:** বৃষ্টির অক্ষরের লেজ কতটুকু লম্বা হবে তা নির্ধারণ করা (৪ থেকে ২০)।
  - **BLINK Slider:** স্ক্রিনে অক্ষরের ঝিলিক/ব্লিঙ্কিং ফ্রিকোয়েন্সি কমানো-বাড়ানো।
- 🔘 **স্মার্ট বটম বার (Quick Control Bar):**
  - **PREV / NEXT:** এক টাচেই কালার থিম পরিবর্তন।
  - **SETUP:** সেটিংস স্ক্রিনে যাওয়া।
  - স্ক্রিনের যেকোনো ফাঁকা জায়গায় ট্যাপ করে বটম বার হাইড/শো করার টগল সিস্টেম।
- 🎯 **বিল্ট-ইন টাচ ক্যালিব্রেশন মোড:** টাচ ইনপুট নিখুঁত করার জন্য কোডেই সিয়াল মনিটর ভিত্তিক ক্যালিব্রেশন মোড রয়েছে।

---

## 🧰 প্রয়োজনীয় উপাদান (Hardware Components)

1. **ESP32-S3 N16R8 Development Board** (16MB Flash / 8MB PSRAM)
2. **ST7789 240x320 2.8" SPI TFT Display**
3. **XPT2046 Touch Controller Module** (TFT ডিসপ্লের সাথে ইন্টিগ্রেটেড)
4. **MPU6500 / MPU6050 Module** (I2C)
5. **USB-C ডাটা কেবল**
6. **Connecting Wires**

---

## 📚 প্রয়োজনীয় সফটওয়্যার ও লাইব্রেরি (Libraries Required)

Arduino IDE-তে নিচের লাইব্রেরিগুলো ইনস্টল করা থাকতে হবে:

| লাইব্রেরির নাম (Library Name) | কাজ | Library Manager Search Term |
| :--- | :--- | :--- |
| **Adafruit GFX Library** | ডিসপ্লে গ্রাফিক্স প্রসেসিং | `Adafruit GFX` |
| **Adafruit ST7735 and ST7789 Library** | ST7789 TFT ড্রাইভার | `Adafruit ST7789` |
| **XPT2046_Touchscreen** (by Paul Stoffregen) | টাচ প্যানেল কন্ট্রোল | `XPT2046_Touchscreen` |
| **SPI.h & Wire.h** | SPI ও I2C প্রোটোকল | ESP32 Core-এর সাথে ডিফল্ট থাকে |

---

## 🔌 পিন টু পিন কানেকশন (Detailed Pinout Diagram)

### 🖥️ ১. TFT Display (ST7789) -> ESP32-S3 Connection
| TFT Display Pin | ESP32-S3 GPIO Pin | কাজ |
| :--- | :--- | :--- |
| **VCC** | **3.3V** | পাওয়ার সাপ্লাই |
| **GND** | **GND** | গ্রাউন্ড |
| **CS** | **GPIO 10** | TFT Chip Select |
| **RST** | **GPIO 8** | Reset Pin |
| **DC** | **GPIO 9** | Data/Command Select |
| **MOSI / SDA** | **GPIO 11** | SPI Data Input |
| **SCK / SCL** | **GPIO 13** | SPI Clock Input |
| **LED / BLK** | **GPIO 5** (বা 3.3V) | ব্যাকলাইট পিন |
| **MISO** | **GPIO 12** | SPI MISO |

---

### 👆 ২. Touch Controller (XPT2046) -> ESP32-S3 Connection
*(TFT Display এর পেছনের টাচ পিনগুলো)*

| Touch Pin | ESP32-S3 GPIO Pin | কাজ |
| :--- | :--- | :--- |
| **TCLK / T_CLK** | **GPIO 13** | SPI Clock (TFT এর SCK এর সাথে শেয়ার করবে) |
| **T_CS** | **GPIO 7** | Touch Chip Select |
| **T_DIN** | **GPIO 11** | SPI Data In (TFT এর MOSI এর সাথে শেয়ার করবে) |
| **T_DO** | **GPIO 12** | SPI Data Out (TFT এর MISO এর সাথে শেয়ার করবে) |
| **T_IRQ** | **GPIO 6** (ঐচ্ছিক) | Touch Interrupt Pin |

---

### 🧭 ৩. Gyro/Accelerometer Sensor (MPU6500/6050) -> ESP32-S3
| MPU Pin | ESP32-S3 GPIO Pin | কাজ |
| :--- | :--- | :--- |
| **VCC** | **3.3V** | پاور |
| **GND** | **GND** | গ্রাউন্ড |
| **SDA** | **GPIO 16** | I2C Data Line |
| **SCL** | **GPIO 15** | I2C Clock Line |

---

## 🛠️ টাচ ফিক্সিং ও ক্যালিব্রেশন (Touch Calibration & Orientation)

### 🔄 ল্যান্ডস্কেপ মোডে টাচ উল্টো কাজ করলে:
কোডের **৮১ ও ৮২ নম্বর লাইনে** সংজ্ঞায়িত এক্সিস ইনভার্সন ফ্ল্যাগ পরিবর্তন করুন:
```cpp
const bool INVERT_LANDSCAPE_X = true;   // ডানে/বামে উল্টো হলে এটি true করুন
const bool INVERT_LANDSCAPE_Y = false;  // উপরে/নিচে উল্টো হলে এটি true করুন
```

### 🎯 র টাচ ক্যালিব্রেশন (Raw Calibration):
টাচ প্রেসের লোকেশন নিখুঁত না হলে কোডের **৪১ নম্বর লাইনে**:
```cpp
#define RUN_CALIBRATION 1
```
করে কোড আপলোড করুন। এরপরে সিরিয়াল মনিটরে স্ক্রিনের কোণায় কোণায় চাপ দিয়ে Min/Max X ও Y মানগুলো সংগ্রহ করুন এবং কোডের ৪২-৪৫ নম্বর লাইনে আপডেট করে আবার `RUN_CALIBRATION 0` করে আপলোড দিন।

---

## ⚙️ আর্ডুইনো আইডিই আপলোড সেটআপ (Arduino IDE Setup)

1. **Board:** `ESP32S3 Dev Module`
2. **PSRAM:** `Enabled` / `OPI PSRAM`
3. **Flash Size:** `16MB (128Mb)`
4. **Partition Scheme:** `16M Flash (3MB APP/9.9MB FATFS)`
5. **Port:** আপনার সংযুক্ত কম পোর্ট সিলেক্ট করে `Upload` চাপুন।

---

## 📺 ভিডিও টিউটোরিয়াল ও চ্যানেল সাবস্ক্রাইব (YouTube & Support)

প্রজেক্টটির ফুল মেকিং ও গাইড দেখতে এবং এই ধরনের আরও সাইবারপাঙ্ক ও রোবোটিক্স প্রজেক্টের টিউটোরিয়াল পেতে চ্যানেলটি সাবস্ক্রাইব করুন!

<p align="center">
  <a href="https://www.youtube.com/@razfriday" target="_blank">
    <img src="https://img.shields.io/badge/YouTube-Subscribe%20Now-red?style=for-the-badge&logo=youtube" alt="YouTube Channel">
  </a>
</p>

👉 **ইউটিউব চ্যানেল লিংক:** [https://www.youtube.com/@razfriday](https://www.youtube.com/@razfriday)

প্রোজেক্টটি আপনার কাজে আসলে গিটহাবে একটি **Star (⭐)** দিয়ে পাশে থাকবেন। Happy Hacking! 🟢✨
