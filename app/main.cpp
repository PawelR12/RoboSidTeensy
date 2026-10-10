#include <Arduino.h>
// #include <HardwareSerial.h>

extern "C" int main(void)
{
// #ifdef USING_MAKEFILE

	// To use Teensy 4.0 without Arduino, simply put your code here.
	// For example:

    Serial.begin(9600); // USB is always 12 or 480 Mbit/sec

	pinMode(13, OUTPUT);
	while (1) {
        Serial.println("hello");
		digitalWriteFast(13, HIGH);
		delay(1500);
		digitalWriteFast(13, LOW);
		delay(500);
	}
}

// #else
// 	// Arduino's main() function just calls setup() and loop()....
// 	setup();
// 	while (1) {
// 		loop();
// 		yield();
// 	}
// #endif
// }

// https://forum.pjrc.com/index.php?threads/75084
// https://forum.pjrc.com/index.php?threads/77937/page-3#post-368518
#if 1
namespace __gnu_cxx {
void __verbose_terminate_handler()
{
	while (1) asm ("WFI");
}
}
#endif
