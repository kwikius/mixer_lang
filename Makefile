

.PHONY: all mixer_lang_lib clean_mixer_lang_lib taranis_joystick \
clean_taranis_joystick clean

all: mixer_lang_lib taranis_joystick

mixer_lang_lib:
	make -C ./src

clean_mixer_lang_lib:
	make -C ./src clean 

taranis_joystick : 
	make -C ./examples/TaranisUSBJoystick

clean_taranis_joystick : 
	make -C ./examples/TaranisUSBJoystick clean

clean:
	make -C ./examples/TaranisUSBJoystick clean
	make -C ./src clean




