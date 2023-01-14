all:
	gcc oledStats.cpp ssd1306.cpp -o oledStats -std=c++11

clean:
	rm -f oled

