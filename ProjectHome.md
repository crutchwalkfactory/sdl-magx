Port SDL library for MAGX phone:
Motorola VE66/EM35/ZN5/Z6/Z6W/EM30/E8/VA76/U9/...

Video driver based on:

- qtopia video driver;

- video driver whith IPU for MPlayer by Alexey Kuznetsov;

- code and header files of phone kernel;

This video driver supported:

- Hardware acceleration ( FB + IPU ) for rotation and scaling image;

- Surface is in video memory (IPU memory);

- Double-buffered video mode;


Audio driver:

- Based on audio driver for EZX;

- Use AAL function for audio out;