# epuck_mini_projet_g14
Robotics project for EPFL MICRO-315: Embedded systems and robotics course
---------------------
---------------------
By Parsa Khorasani & Martin Rollet - Group 14

Github link : https://github.com/MartinRollet/epuck_mini_projet_g14.git
---------------------
---------------------

*** Highlights ***

- Control robot's movement in four directions through voice recognition commanding system with 4 key words
- Deploy a functional Neural Net on E-puck2 robot
- Advanced sound processing unit capable of noise rejection 
- Obstacle detection during mouvment


!!! Attention !!!

To launch the program on the e-puck you MUST read at least once the USB serial port from the e-puck2 after downloading the program on it. Once it's done, the robot will work alone. This need comes from used libraries.

To insure correct implementation of the neural netwrok NNOM library and parts of CMSIS NN and CMSIS DSP are required. 
The "lib" folder besides the project folder (standard directory placement for practical exercises), already contains these libraries along with an adequate makefile which adds the required files from them. Thus, it must be used to satisfy  all dependencies and build the project correctly! 


--- External libraries ---

* NNOM: Neural Network on Microcontroller
	  https://github.com/majianjia/nnom

* CMSIS DSP & NN :
	  https://github.com/ARM-software/CMSIS_5


