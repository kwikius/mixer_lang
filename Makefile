

.PHONY: all mixer_lang_lib clean_mixer_lang_lib taranis_joystick \
clean_taranis_joystick clean test clean_test stm32f405

all: mixer_lang_lib taranis_joystick

mixer_lang_lib:
	make -C ./src

clean_mixer_lang_lib:
	make -C ./src clean 

taranis_joystick : 
	make -C ./examples/TaranisUSBJoystick

clean_taranis_joystick : 
	make -C ./examples/TaranisUSBJoystick clean

test:
	make -C ./test

clean_test :
	make -C ./test clean

stm32f405:
	make AERFLITE=True -C ./src -f make_stm32f405.mk

clean_stm32f405:
	make AERFLITE=True -C ./src -f make_stm32f405.mk clean

run_easystar : taranis_joystick
	./examples/TaranisUSBJoystick/bin/taranis_mixer.exe ./mixers/generic/EasyStar.mix

run_flyingwing : taranis_joystick
	./examples/TaranisUSBJoystick/bin/taranis_mixer.exe ./mixers/generic/flyingwing.mix

run_glider : taranis_joystick
	./examples/TaranisUSBJoystick/bin/taranis_mixer.exe ./mixers/generic/glider.mix

clean:
	make -C ./examples/TaranisUSBJoystick clean
	make -C ./src clean




