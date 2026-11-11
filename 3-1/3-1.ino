
const int RED_PIN = 9;
const int GREEN_PIN = 10;
const int BLUE_PIN = 11;
const int BUTTON_PIN = 2;

// 共陽極 RGB LED
const bool COMMON_ANODE = true;

// 五種顏色：粉紅、黃、綠、藍、白
const int colors[5][3] = {
  {255, 80, 150},
  {255, 220, 0},
  {0, 255, 0},
  {0, 100, 255},
  {255, 255, 255}
};

const int COLOR_COUNT = 5;

int colorIndex = 0;
int mode = 0;

bool lastButtonState = HIGH;
bool buttonState = HIGH;
bool longPressHandled = false;

unsigned long pressStartTime = 0;
unsigned long lastDebounceTime = 0;
unsigned long lastUpdateTime = 0;

const unsigned long debounceDelay = 50;
const unsigned long longPressTime = 800;

int brightness = 0;
int fadeAmount = 5;

bool ledOn = true;
int rainbowHue = 0;

void setColor(int r, int g, int b) {
  if (COMMON_ANODE) {
    r = 255 - r;
    g = 255 - g;
    b = 255 - b;
  }

  analogWrite(RED_PIN, r);
  analogWrite(GREEN_PIN, g);
  analogWrite(BLUE_PIN, b);
}

void turnOff() {
  setColor(0, 0, 0);
}

void showCurrentColor() {
  setColor(
    colors[colorIndex][0],
    colors[colorIndex][1],
    colors[colorIndex][2]
  );
}

// 按鈕控制
void handleButton() {
  bool reading = digitalRead(BUTTON_PIN);

  if (reading != lastButtonState) {
    lastDebounceTime = millis();
  }

  if (millis() - lastDebounceTime > debounceDelay) {
    if (reading != buttonState) {
      buttonState = reading;

      if (buttonState == LOW) {
        pressStartTime = millis();
        longPressHandled = false;
      } 
      else {
        if (!longPressHandled) {
          if (mode == 0) {
            colorIndex++;

            if (colorIndex >= COLOR_COUNT) {
              colorIndex = 0;
            }

            showCurrentColor();
          }
        }
      }
    }

    if (buttonState == LOW &&
        !longPressHandled &&
        millis() - pressStartTime >= longPressTime) {

      mode++;

      if (mode > 3) {
        mode = 0;
      }

      longPressHandled = true;

      lastUpdateTime = millis();
      brightness = 0;
      fadeAmount = 5;
      ledOn = true;
      rainbowHue = 0;
      colorIndex = 0;

      if (mode == 0) {
        showCurrentColor();
      } 
      else {
        turnOff();
      }
    }
  }

  lastButtonState = reading;
}

// 五色呼吸燈
void breathingMode() {
  if (millis() - lastUpdateTime >= 20) {
    lastUpdateTime = millis();

    brightness += fadeAmount;

    if (brightness >= 255) {
      brightness = 255;
      fadeAmount = -5;
    }

    if (brightness <= 0) {
      brightness = 0;
      fadeAmount = 5;

      colorIndex++;

      if (colorIndex >= COLOR_COUNT) {
        colorIndex = 0;
      }
    }

    int r = colors[colorIndex][0] * brightness / 255;
    int g = colors[colorIndex][1] * brightness / 255;
    int b = colors[colorIndex][2] * brightness / 255;

    setColor(r, g, b);
  }
}

// 五色閃爍燈
void blinkingMode() {
  if (millis() - lastUpdateTime >= 500) {
    lastUpdateTime = millis();

    if (ledOn) {
      turnOff();
    } 
    else {
      colorIndex++;

      if (colorIndex >= COLOR_COUNT) {
        colorIndex = 0;
      }

      showCurrentColor();
    }

    ledOn = !ledOn;
  }
}

// 彩虹燈
void rainbowMode() {
  if (millis() - lastUpdateTime >= 20) {
    lastUpdateTime = millis();

    rainbowHue++;

    if (rainbowHue >= 360) {
      rainbowHue = 0;
    }

    float h = rainbowHue / 60.0;
    int sector = (int)h;
    float f = h - sector;

    int r, g, b;

    switch (sector) {
      case 0:
        r = 255;
        g = 255 * f;
        b = 0;
        break;

      case 1:
        r = 255 * (1 - f);
        g = 255;
        b = 0;
        break;

      case 2:
        r = 0;
        g = 255;
        b = 255 * f;
        break;

      case 3:
        r = 0;
        g = 255 * (1 - f);
        b = 255;
        break;

      case 4:
        r = 255 * f;
        g = 0;
        b = 255;
        break;

      default:
        r = 255;
        g = 0;
        b = 255 * (1 - f);
        break;
    }

    setColor(r, g, b);
  }
}

void setup() {
  pinMode(RED_PIN, OUTPUT);
  pinMode(GREEN_PIN, OUTPUT);
  pinMode(BLUE_PIN, OUTPUT);
  pinMode(BUTTON_PIN, INPUT_PULLUP);

  showCurrentColor();
}

void loop() {
  handleButton();

  if (mode == 1) {
    breathingMode();
  }
  else if (mode == 2) {
    blinkingMode();
  }
  else if (mode == 3) {
    rainbowMode();
  }
}
