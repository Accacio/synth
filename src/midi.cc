
// #include <sys/soundcard.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdio.h>
#include <iostream>
#include <cstdlib>
#include <poll.h>
// #include <sys/types.h>

#define  MIDI_DEVICE  "/dev/snd/midiC2D0"
enum midiinput {
NOTE_ON = 144,
NOTE_OFF = 128,
CONTROL_CODE = 176,
AFTERTOUCH = 208,
};


int main(void) {
   unsigned char inpacket[4];

   int seqfd = open(MIDI_DEVICE, O_RDONLY);
   // printf("Error: cannot open %d\n", seqfd);
   if (seqfd == -1) {
       printf("Error: cannot open %s\n", MIDI_DEVICE);
       exit(1);
   }
	struct pollfd pfd = { seqfd, POLLIN, 0 };

   while (1) {
		// int ret = poll(&pfd,1,5);
		// if (ret > 0){

         read(seqfd, &inpacket, 1);

         //    // print the MIDI byte if this input packet contains one
         if (inpacket[0] == NOTE_ON) {
            read(seqfd, &inpacket, 3);
            printf("Note on: %d , Velocity: %d, Channel: %d\n",inpacket[0], inpacket[1],inpacket[2]);
            // printf("received MIDI byte: %d %d %d %d \n",inpacket[0],inpacket[1], inpacket[2],inpacket[3]);
         }
         if (inpacket[0] == NOTE_OFF) {
            read(seqfd, &inpacket, 3);
            printf("Note off: %d , Velocity: %d, Channel: %d\n",inpacket[0], inpacket[1],inpacket[2]);
            // printf("received MIDI byte: %d %d %d %d \n",inpacket[0],inpacket[1], inpacket[2],inpacket[3]);
         }
         if (inpacket[0] == CONTROL_CODE) {
            read(seqfd, &inpacket, 3);
            printf("Code: %d , Angle: %f, Channel: %d\n",inpacket[0],(double) inpacket[1]/127*360,inpacket[2]);
            if (inpacket[0]==118) return 0;
         }
         if (inpacket[0] == AFTERTOUCH) {
            read(seqfd, &inpacket, 1);
            printf("Aftertouch: %d \n",inpacket[0]);
         }
      // }

       // printf("nothing polled\n");
      }

   return 0;
}
