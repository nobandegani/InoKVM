#include "mouse_utils.h"

void MouseUtils::setup(){
  Serial.println("Mouse start");
  Mouse.begin();
}

void MouseUtils::loop(){
	if (random_move){
		rand_mouse_move();
	}
}

void MouseUtils::move(int InX, int InY) {
    Serial.print("Mouse move requested: ");
    Serial.print(InX);
    Serial.print(",");
    Serial.println(InY);

    while (InX != 0 || InY != 0) {
        int8_t stepX = 0;
        int8_t stepY = 0;

        if (InX > 127)      { stepX = 127;  InX -= 127; }
        else if (InX < -127){ stepX = -127; InX += 127; }
        else                { stepX = InX;  InX = 0;    }

        if (InY > 127)      { stepY = 127;  InY -= 127; }
        else if (InY < -127){ stepY = -127; InY += 127; }
        else                { stepY = InY;  InY = 0;    }

        Serial.print("Mouse moved: ");
        Serial.print(stepX);
        Serial.print(",");
        Serial.println(stepY);

        Mouse.move(stepX, stepY);
        delay(1);  // small delay to avoid buffer issues
    }
}

void MouseUtils::move2(int8_t InX, int8_t InY) { 
	uint8_t m_delay = 2;

	// Calculate how far we want to move the mouse in each direction
	int16_t x_dest = random(80) + 20;
	int16_t y_dest = random(80) + 20;
	float slope     = y_dest / (float)x_dest;

	//Serial.printf("0,0 to %d,%d = slope %0.3f\n", x_dest, y_dest, slope);

	// Previous x,y coordinates
	int16_t prev_x = 0;
	int16_t prev_y = 0;

	// Is the move up or left
	bool up   = random(2);
	bool left = random(2);

	// We calculate each X-coordinate and move the mouse accordingly
	// essentially one x-pixel at a time
	for (int i = 0; i < x_dest; i++) {
		float x = i;
		float y = slope * x;

		// If it's left or up we flip the sign to negative
		if (left) { x *= -1; }
		if (up)   { y *= -1; }

		// Calculate the difference in units this mini-move should be
		int8_t diff_x = int(x) - prev_x;
		int8_t diff_y = int(y) - prev_y;

		if (prev_x || prev_y) {
			Mouse.move(diff_x, diff_y);
			delay(m_delay);
		}

		// Store the x,y coordinates for next time
		prev_x = int(x);
		prev_y = int(y);
	}

	if (left) { x_dest *= -1; }
	if (up)   { y_dest *= -1; }

	//Serial.println("Mouse moved: %d,%d\r\n", x_dest, y_dest);

	return;
}

/*
#define MOUSE_LEFT     0x01
#define MOUSE_RIGHT    0x02
#define MOUSE_MIDDLE   0x04
#define MOUSE_BACKWARD 0x08
#define MOUSE_FORWARD  0x10
#define MOUSE_ALL      0x1F
*/
void MouseUtils::mouse_click(String event){
	if (event = "left"){
		Mouse.click(MOUSE_LEFT);
		return;
	}
	if (event = "right"){
		Mouse.click(MOUSE_RIGHT);
		return;
	}
	if (event = "double"){
		Mouse.click(MOUSE_LEFT);
		delay(40);
		Mouse.click(MOUSE_LEFT);
		return;
	}
}

// --------------------------------------------- rand_mouse_move ---------------------------------------------
void MouseUtils::rand_mouse_move() { 
	// Delay between each mouse move... if we move on average 60 times
	// this works out to just about 1/10th of a second for the whole
	// move. Set this to 0 to be instantaneous
	uint8_t my_delay = 2;

	// Calculate how far we want to move the mouse in each direction
	int16_t x_dest = random(80) + 20;
	int16_t y_dest = random(80) + 20;
	float slope     = y_dest / (float)x_dest;

	//Serial.printf("0,0 to %d,%d = slope %0.3f\n", x_dest, y_dest, slope);

	// Previous x,y coordinates
	int16_t prev_x = 0;
	int16_t prev_y = 0;

	// Is the move up or left
	bool up   = random(2);
	bool left = random(2);

	// We calculate each X-coordinate and move the mouse accordingly
	// essentially one x-pixel at a time
	for (int i = 0; i < x_dest; i++) {
		float x = i;
		float y = slope * x;

		// If it's left or up we flip the sign to negative
		if (left) { x *= -1; }
		if (up)   { y *= -1; }

		// Calculate the difference in units this mini-move should be
		int8_t diff_x = int(x) - prev_x;
		int8_t diff_y = int(y) - prev_y;

		if (prev_x || prev_y) {
			Mouse.move(diff_x, diff_y);
			delay(my_delay);
		}

		// Store the x,y coordinates for next time
		prev_x = int(x);
		prev_y = int(y);
	}

	if (left) { x_dest *= -1; }
	if (up)   { y_dest *= -1; }

	Serial.printf("Mouse moved: %d,%d\r\n", x_dest, y_dest);
	return;
}