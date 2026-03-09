# Project 06 - Event groups

The main idea was to have a power button, that on press the system starts to initialize the peripherals (before that it stays in stand by), the peripherals must initialize correctly so that a buzzer generates a beep to follow with the launching of the running tasks. Putting into practice the event groups mechanics.

## Issues

The main issue here was the change of arquitecture, as the tasks had to be initialized from a startup manager task, and handling the errors in case a peripheral is not working properly. Although it was challenging mentally to understand it was easy to apply as the code is well structured in modules.
