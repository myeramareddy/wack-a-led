# Wack-a-LED

A play on Wack-a-Mole where there are 5 LED’s lighted in a random sequence and the hitting the correct photoresistor (which utilizes ADMUX) symbolizes wacking the LED. Upon starting the game, instructions are displayed (which utilizes custom characters) until the start button is hit. Along with these instructions, the top three scores are also displayed which are stored in EEPROM.The random sequence of these LED’s is computed and transferred using USART.

User Guide
- Controls
- Start Game - *
- Reset Game - #
- Pause - ‘C’
- Un-Pause - ‘D’

Rules & Conditions : Starts out with 2 lives, but can work upto 3 lives for every 10 consecutive hits.
If the players hits when the LCD displays ‘Don’t Hit LED’, then the player loses regardless of how many lives are left.
If the player hovers over any other photoresistor other than the one they are supposed to, the hit doesn’t count.
Sequence of LED’s speeds up every fourth LED by 200ms.

Technologies and Components
- AVR Studio 6
- Atmega32
- 16x2 LCD Screen
- Keypad
- Photo-Resistors
- LED’s
2 Microcontrollers


Demo


[![ScreenShot](http://img.youtube.com/vi/VIDEO-ID/0.jpg)](https://www.youtube.com/embed/2vQpdqegKcY)

