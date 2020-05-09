#include <stdio.h>
#include <string.h>
#include <iostream> // cout cerr endl

#include <linux/fb.h>   // framebuffer
#include <sys/mman.h>   // framebuffer
#include <sys/ioctl.h>  // framebuffer
#include <fcntl.h>      // framebuffer open
#include <stdlib.h>     // exit
#include <unistd.h>     // close

#include <string>
using namespace std;

class pixeltypen {
  public:
    typedef struct {int x, y;} INTPAIR;
};

class cframebuffer : public pixeltypen {
  public:
  cframebuffer( int hor, int ver, int deep, string progname) {}
  private:
    char *framebufferpointer;
    int fbfiledescriptor;
    long int screensize;
    bool debug_framebuffer;
    INTPAIR HORxVER;

  public:
    struct fb_var_screeninfo vinfo;
    struct fb_fix_screeninfo finfo;

    struct fb_var_screeninfo get_vinfo() { return vinfo;}
    struct fb_fix_screeninfo get_finfo() { return finfo;}

    ~cframebuffer() {
      if (debug_framebuffer) fprintf( stderr, "FB97  The framebuffer device %p closing.\n", (void *)framebufferpointer);
      if (framebufferpointer == 0) return;
      munmap(framebufferpointer, screensize);
      close(fbfiledescriptor);
      if (debug_framebuffer) fprintf( stderr, "FB99 Destruktor fertig cframebuffer() this= %p\n", (void *)this);
    }
    cframebuffer() {
    }
    INTPAIR mach_framebuffer() {
      debug_framebuffer = true;
      // Open the file for reading and writing
      fbfiledescriptor = open("/dev/fb0", O_RDWR);
      if (fbfiledescriptor == -1) {
        perror("E020 Error: cannot open framebuffer device");
        exit(1);
      }
      if (debug_framebuffer) fprintf( stderr, "FB20  The framebuffer device was opened successfully.\n");

      // Get fixed screen information
      if (ioctl(fbfiledescriptor, FBIOGET_FSCREENINFO, &finfo) == -1) {
        perror("E020 Error reading fixed information");
        exit(2);
      }

      if (debug_framebuffer) fprintf( stderr, "FB22  finfo.line_length=%d finfo.mmio_len=%d\n", finfo.line_length, finfo.mmio_len);
      if (debug_framebuffer)
 cerr << "FB22" << endl
 << " finfo.id            = " <<        finfo.id             << endl
 << " finfo.smem_start    = " << hex << finfo.smem_start     << endl
 << " finfo.smem_len      = " << dec << finfo.smem_len       << endl
 << " finfo.type          = " <<        finfo.type           << endl
 << " finfo.type_aux      = " <<        finfo.type_aux       << endl
 << " finfo.visual        = " <<        finfo.visual         << endl
 << " finfo.xpanstep      = " <<        finfo.xpanstep       << endl
 << " finfo.ypanstep      = " <<        finfo.ypanstep       << endl
 << " finfo.ywrapstep     = " <<        finfo.ywrapstep      << endl
 << " finfo.line_length   = " <<        finfo.line_length    << endl
 << " finfo.mmio_start    = " <<        finfo.mmio_start     << endl
 << " finfo.mmio_len      = " <<        finfo.mmio_len       << endl
 << " finfo.accel         = " <<        finfo.accel          << endl
 << " finfo.capabilities  = " <<        finfo.capabilities   << endl
 << " finfo.reserved[2]   = " <<        finfo.reserved[2]    << endl
 << endl;
/*
        char id[16];                    // identification string eg "TT Builtin" //
        unsigned long smem_start;       // Start of frame buffer mem //
                                        // (physical address) //
        __u32 smem_len;                 // Length of frame buffer mem //
        __u32 type;                     // see FB_TYPE_/                //
        __u32 type_aux;                 // Interleave for interleaved Planes //
        __u32 visual;                   // see FB_VISUAL_/              // 
        __u16 xpanstep;                 // zero if no hardware panning  //
        __u16 ypanstep;                 // zero if no hardware panning  //
        __u16 ywrapstep;                // zero if no hardware ywrap    //
        __u32 line_length;              // length of a line in bytes    //
unsigned long mmio_start;       // Start of Memory Mapped I/O   //
        __u32 mmio_len;                 // Length of Memory Mapped I/O  //
        __u32 accel;                    // Indicate to driver which     //
        __u16 capabilities;             // see FB_CAP_/                 //
        __u16 reserved[2];              // Reserved for future compatibility //
*/
      // Get variable screen information
      if (ioctl(fbfiledescriptor, FBIOGET_VSCREENINFO, &vinfo) == -1) {
        perror("E020 Error reading variable information");
        exit(3);
      }

      if (debug_framebuffer) fprintf( stderr, "FB24  vinfo.xres x vinfo.yres       %dx%d, %dbpp\n", vinfo.xres,    vinfo.yres,  vinfo.bits_per_pixel);
      if (debug_framebuffer) fprintf( stderr, "FB24  vinfo.xoffset x vinfo.yoffset %dx%d\n",        vinfo.xoffset, vinfo.yoffset);

      // Figure out the size of the screen in bytes
      screensize = vinfo.xres * vinfo.yres * vinfo.bits_per_pixel / 8;

//    fprintf( stderr, "FB26 screensize=%ld bytes\n", screensize);
      if (debug_framebuffer) fprintf( stderr, "FB26  screensize=%d*%d*%d vinfo.bits_per_pixel/8=%ld bytes\n",
          vinfo.xres, vinfo.yres, vinfo.bits_per_pixel/8, screensize);

      INTPAIR HORxVER;
      bool host_johnny; host_johnny = false;
      if (host_johnny) {
        HORxVER.x = 1920; HORxVER.y = 1080;screensize = 1920 * 1080 * vinfo.bits_per_pixel / 8;
      } else {
        HORxVER.x = vinfo.xres; HORxVER.y = vinfo.yres;
      }

      // Map the device to memory
      framebufferpointer = (char *)mmap(0, screensize, PROT_READ | PROT_WRITE, MAP_SHARED, fbfiledescriptor, 0);
      if (framebufferpointer == (void *) -1) {
        perror("E020 Error: failed to map framebuffer device to memory");
        exit(4);
      }
      if (debug_framebuffer) fprintf( stderr, "FB28  The framebuffer device was mapped to memory %p.\n", (void *)framebufferpointer);
      if (debug_framebuffer) fprintf( stderr, "FB30 Konstruktor fertig cframebuffer() this= %p\n\n", this);
      this->HORxVER = HORxVER;
      return HORxVER;

    }

  void plotf(int x, int y, int red, int green, int blue) {
    if (x < 0 || y < 0 || x >= this->HORxVER.x || y >= this->HORxVER.y) return;
    long int location = 0;

    // Figure out where in memory to put the pixel

    location = (x+vinfo.xoffset) * (vinfo.bits_per_pixel/8) +
               (y+vinfo.yoffset) * finfo.line_length;
  //if (location >=5242880) return; // 1280*1024 // 1280*1024*4 = 5242880
  //if (location >=5245440) return; // 1280*1024 // 1280*1024*4 = 5242880
    if (vinfo.bits_per_pixel == 32) {
      // Pointer-Arithmetik ist hier die gewöhnliche Arithmetik, weil framebufferpointer auf char zeigt.
      *(framebufferpointer + location)     = blue;
      *(framebufferpointer + location + 1) = green;
      *(framebufferpointer + location + 2) = red;
      *(framebufferpointer + location + 3) = 0;      // No transparency
      //location += 4;
//    fprintf( stderr, "FB40 x=%4d y=%4d farbe=%06x\n", x, y, farbe);
    } else  { 
      fprintf( stderr, "FB88 vinfo.bits_per_pixel != 32 sondern %d\n", vinfo.bits_per_pixel);
      return;
    }
  }

  void plotf( int x, int y, int farbe) {
    int red, green, blue; 
    switch (farbe/10%8) {
      case  0: red=  0; green=  0; blue=  0; break;
      case  1: red=  0; green=  0; blue=254; break;
      case  2: red=  0; green=254; blue=  0; break;
      case  3: red=  0; green=254; blue=254; break;
      case  4: red=254; green=  0; blue=  0; break;
      case  5: red=254; green=  0; blue=254; break;
      case  6: red=254; green=254; blue=  0; break;
      case  7: red=254; green=254; blue=254; break;
      default: red=128; green=128; blue=128; break;
    }
    plotf( x, y, red, green, blue); 
    // *((unsigned short int*)(framebufferpointer + location)) = tt;
  }

};

class clies_pnm : public cframebuffer {
enum { start, magicP, kommentar,
  erwarte_width,
  erwarte_height,
  erwarte_bitanzahl,
  erwarte_farben,
  fehler, ende
};

  
  public:
    clies_pnm() : cframebuffer() {}
    void nun_lies_pnm( FILE *eindatei) {
      INTPAIR HORxVER = mach_framebuffer();
      int plot_x, plot_y;
      plot_x=0, plot_y=0;
      bool debug_liespnm;
      debug_liespnm = false;
      char buchstabe;
      int zustand;
      int width, height, bitanzahl, p_art;
      width=0, height=0, bitanzahl=0, p_art = 0;
      zustand = start;
  
      fread( &buchstabe, 1, 1, eindatei);
      while (!feof(eindatei)) {
        while (zustand != erwarte_farben && buchstabe == '#') {
          if (debug_liespnm) fprintf( stdout, "kommentar.%d!%c ", zustand, buchstabe);
          fread( &buchstabe, 1, 1, eindatei);
          while (!feof(eindatei)) {
            if (debug_liespnm) fprintf( stdout, "_%c", buchstabe);
            if (buchstabe == '\r' || buchstabe == '\n') { fread( &buchstabe, 1, 1, eindatei);break;}
            fread( &buchstabe, 1, 1, eindatei);
          }
        }
        if (debug_liespnm) fprintf( stdout, ".%d!%c ", zustand, buchstabe);
        switch (zustand) {
          case magicP:
            if (buchstabe == ' ' || buchstabe == '\t' || buchstabe == '\r' || buchstabe == '\n') {
              zustand = erwarte_width;
              break;
            }
            if (buchstabe >= '0' && buchstabe <= '9') {
              p_art = buchstabe - '0';
              zustand = magicP;
              break;
            }
            zustand = fehler; goto schluss;
            break;
          case start:
            if (buchstabe == 'P') { zustand = magicP;} else { zustand = fehler; goto schluss;}
            break;
          case kommentar:
            if (buchstabe == '\n') { zustand = erwarte_width;}
            break;
          case erwarte_width:
            if (buchstabe >= '0' && buchstabe <= '9') {
              width = 10 * width + buchstabe - '0';
              zustand = erwarte_width;
            }
            if (buchstabe == ' ' || buchstabe == '\t' || buchstabe == '\r' || buchstabe == '\n') {
              zustand = erwarte_height;
            }
            break;
          case erwarte_height:
            if (buchstabe >= '0' && buchstabe <= '9') {
              height = 10 * height + buchstabe - '0';
              zustand = erwarte_height;
            }
            if (buchstabe == ' ' || buchstabe == '\t' || buchstabe == '\r' || buchstabe == '\n') {
              zustand = erwarte_bitanzahl;
            }
            break;
          case erwarte_bitanzahl:
            if (buchstabe >= '0' && buchstabe <= '9') {
              bitanzahl = 10 * bitanzahl + buchstabe - '0';
              zustand = erwarte_bitanzahl;
            }
            if (buchstabe == ' ' || buchstabe == '\t' || buchstabe == '\r' || buchstabe == '\n') {
      if (true or debug_liespnm) fprintf( stdout, "L050 width=    %d height   =%d bitanzahl=%d p_art=%d\n", width, height, bitanzahl, p_art);
      if (true or debug_liespnm) fprintf( stdout, "L060 HORxVER.x=%d HORxVER.y=%d\n", HORxVER.x, HORxVER.y);
              zustand = erwarte_farben;
            }
            break;
          case erwarte_farben:
            int rot, gruen, blau, transparenz; size_t anz_gelesen;
            rot=0, gruen=0, blau=0, transparenz=0;
            int byteanzahl;
            switch (bitanzahl) {
              default   : byteanzahl=2; break;
              case 65535: byteanzahl=2; break;
              case   255: byteanzahl=1; break;
            }
            while (1) {
              anz_gelesen =  fread( &rot        , 1, byteanzahl, eindatei);
              if (anz_gelesen < 1) break;
              anz_gelesen =  fread( &gruen      , 1, byteanzahl, eindatei);
              anz_gelesen =  fread( &blau       , 1, byteanzahl, eindatei);
              if (false) fprintf( stdout, "%d %d %d\n", rot, gruen, blau);
              plot_x ++ ; if (plot_x >= width) {plot_x = 0; plot_y ++;};
              plotf( plot_x, plot_y, rot, gruen, blau);
            }
            break;
          default:
            break;
  
        }
        fread( &buchstabe, 1, 1, eindatei);
      }
schluss:
  if (debug_liespnm) fprintf( stdout, "\n");
    }
};

int main (int argc, char *argv[]) {
  FILE *eindatei;
  if (!strcmp(argv[1], "-")) {
    eindatei = stdin;
  } else {
    eindatei = fopen( argv[1], "r");
  }
  clies_pnm leser;
  leser.nun_lies_pnm( eindatei);
  return 0;
}

