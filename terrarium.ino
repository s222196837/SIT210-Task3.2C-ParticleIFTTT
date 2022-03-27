// The following line is optional, but recommended in most firmware.
// It allows your code to run before the cloud is connected.
SYSTEM_THREAD(ENABLED);

// This uses the USB serial port for debugging logs.
SerialLogHandler logHandler;

// This is where your LED is plugged in. The other side goes to a resistor
// connected to GND.
const pin_t LED_PIN = D6;

// This is where your photoresistor or phototransistor is plugged in.
const pin_t SENSOR_PIN = A0;

// This is the light threshold for determining sufficient daylight
const int MIN_AMBIENT_LIGHT = 75;
int threshold = MIN_AMBIENT_LIGHT;

// Here we are declaring the integer variable analogvalue, which we will
// use later to store the value of the photoresistor or phototransistor.
int analogvalue;

// Variabless holding our current and previous determination of daytime.
int daytime = -1;
int previous = -1;

// This function is used to switch the LED on or off
int ledToggle(int enable) {
    if (enable) {
        digitalWrite(LED_PIN, HIGH);
        return 1;
    }
    digitalWrite(LED_PIN, LOW);
    return 0;
}

void setup() {
    // First, declare all of our pins. This lets our device know which ones
    // will be used for outputting voltage, and which ones will read
    // incoming voltage.
    pinMode(LED_PIN, OUTPUT); // Our LED pin is output (lighting up the LED)
    digitalWrite(LED_PIN, HIGH);

    // We are going to declare Particle.variable() here so that we can
    // access these values relating to the photosensor from the cloud.
    Particle.variable("analogvalue", analogvalue);
    Particle.variable("threshold", threshold);
    Particle.variable("daytime", daytime);
}

void loop() {
    // Check to see what the value of the photoresistor or phototransistor is
    // and store it in the int variable analogvalue
    analogvalue = analogRead(SENSOR_PIN);

    // This prints the value to the USB debugging serial port (for optional
    // debugging purposes)
    Log.info("analogvalue=%d", analogvalue);

    // Implement the "sufficient daylight" sensing logic - if the value of
    // ambient light sensor is above a threshold we determine its daytime.
    daytime = (analogvalue > threshold);

    // Switch the LED on/off and send notification to the IFTTT service if
    // we determine the daylight state has just changed.  Note: switch the
    // LED *off* during the daytime and *on* during the night time.
    if (daytime != previous) {
        previous = daytime;
        ledToggle(!daytime);
        String changed = String(daytime);
        String rawdata = String(analogvalue);
        Particle.publish("Daytime", changed, PRIVATE);
        Particle.publish("Light", rawdata, PRIVATE);
    }

    // This delay is to prevent reading the sensor more often than needed
    delay(5000ms);
}
