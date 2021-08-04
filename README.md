# 2021_ESP32_DOIT_VSC_JTAG
Test esp-prog JTAG board on ESP32 DO-IT dev. board

1. The file in /include/include.rar contains some different versions of C-header files.
  Why I isolated these C-header files ?
  Because there existed several different versions of .cpp/.h from different authors and they are not always compatible by each other.
  We may accidently use author-A's cpp files but include a wrong header files for it, so the program always fails.
  Such issue costs me a lot of time to try to modify original cpp source to fit or find a correct pair of cpp/h to pass the compiler. 
  
2. The esp-prog JTAG board was bought from internet, openOCD is good enough for it.
  But you needs to find correct parameters from internet to let it work well with your development boards.
  That means you may search the internet to find the keyword parameters and its values to fill in the configuration file in specific directory,
     that's also a time consuming job.

3. Thanks for your patience to read my words.  May you have a good luck & a good day !
4. 
