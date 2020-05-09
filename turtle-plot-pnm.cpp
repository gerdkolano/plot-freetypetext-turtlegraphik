/* sudo apt install libfreetype6-dev  -y
 * pushd /usr/share/fonts/truetype
 * sudo ln -s ubuntu ubuntu-font-family
 * Hannos Bildschirme 20.5'' mal 11.5'',
 * ca 93 dpi in beide Richtungen in beide Richtungen
 * leto 1600x900
 * zoe 1920x1080
*/
#include <linux/fb.h>   // framebuffer
#include <sys/mman.h>   // framebuffer
#include <sys/ioctl.h>  // framebuffer
#include <fcntl.h>      // framebuffer open

#include <string.h>    // freetype
#include <ft2build.h>  // freetype
#include FT_FREETYPE_H
#include FT_GLYPH_H    // FT_Glyph

#include <stdlib.h> // calloc atoi atof exit
#include <stdio.h>  // fprintf fwrite
#include <unistd.h> // getcwd close
#include <errno.h>  // errno
#include <math.h>   // round ceil
#include <string>   // c++ string g++ --std=c++11 u32string
//using std::string;  // c++ string
#include <iostream> // cout cerr endl
#include <fstream>  // ofstream
#include <limits>       // std::numeric_limits
#include <iomanip>      // std::setprecision
#include <vector>
#include <initializer_list>
#include<cmath>         // pow
//#include <filesystem> // mkdir
#include <sys/stat.h>   // mkdir
using namespace std;

class pixeltypen {
  public:
    typedef struct {int x, y;} INTPAIR;
    typedef struct {double x, y;} DOUBLEPAIR;
    typedef short WORT;
    typedef struct {WORT red, green, blue;} RGB;
    typedef struct { int hor; int ver; int deep; int bpp; string memo; } TBILD;
    RGB blaeulich     = (RGB){ (WORT)0x7fff, (WORT)0xefff, (WORT)0xbfff};
    RGB hellgruen     = (RGB){ (WORT)0x0000, (WORT)0x2000, (WORT)0x0000};
    RGB gruen         = (RGB){ (WORT)0x0000, (WORT)0xffff, (WORT)0x0000};
    RGB rot           = (RGB){ (WORT)0xffff, (WORT)0x0000, (WORT)0x0000};
    RGB hell_tuerkis  = (RGB){ (WORT)0x0000, (WORT)0x7fff, (WORT)0x7fff};
    RGB tuerkis       = (RGB){ (WORT)0x0000, (WORT)0xffff, (WORT)0xffff};
    RGB magenta       = (RGB){ (WORT)0xffff, (WORT)0x0000, (WORT)0xffff};
    RGB gelb          = (RGB){ (WORT)0xffff, (WORT)0xffff, (WORT)0x0000};
    RGB weisz         = (RGB){ (WORT)0xffff, (WORT)0xffff, (WORT)0xffff};
    RGB strich_links  = (RGB){ (WORT)0xff00, (WORT)0xff00, (WORT)0xff00};
    RGB strich_rechts = (RGB){ (WORT)0x0000, (WORT)0xff00, (WORT)0xff00};
    RGB strich_oben   = (RGB){ (WORT)0xff00, (WORT)0x0000, (WORT)0xff00};
    RGB strich_unten  = (RGB){ (WORT)0xff00, (WORT)0xff00, (WORT)0x0000};
    // Rand
    RGB farbe_rand_links  = (RGB){ (WORT)0x0000, (WORT)0xff00, (WORT)0xff00};
    RGB farbe_rand_rechts = (RGB){ (WORT)0xff00, (WORT)0xff00, (WORT)0x0000};
    RGB farbe_rand_oben   = (RGB){ (WORT)0x0000, (WORT)0xff00, (WORT)0xff00};
    RGB farbe_rand_unten  = (RGB){ (WORT)0xff00, (WORT)0xff00, (WORT)0x0000};
};

// Definiere clear, plot3 und druck_pnm
// clear, plot3 und druck_pnm arbeiten auf der Zeichenfläche (TBILD)tbild
// (TBILD)tbild ist ein einziger string (von Bytes)
class cbildpnm : public pixeltypen {
  public:
  cbildpnm( int hor, int ver, int deep, string progname) {
    this->progname = progname;
    this->hor  = hor;
    this->ver  = ver;
    this->deep = deep;

    int bpp;
    bpp = sizeof(RGB);
    switch (deep) {
      case 65535: bpp = sizeof(RGB); break;
      case   255: bpp = 3;           break;
      default   : bpp = sizeof(RGB); break;
    }

    string t3ld( hor*ver*bpp, '\0');
    tbild = (TBILD){ hor, ver, deep, bpp, t3ld};
  }
  cbildpnm() {}
  ~cbildpnm() {}
  protected:
  int hor, ver, deep;
  TBILD tbild;
  public:
  string progname;
  void clear_grafik( char farbe) {
    std::fill_n(tbild.memo.begin(), tbild.memo.length(), farbe);
    // int bpp = sizeof(RGB);
    // tbild.memo( hor*ver*bpp, '\0');
  }
// xxd -s89 -c96 -g6 -a /tmp/turtle/turtle-004-cerprobe_brese.pnm | less -S

  void plot3( int xx, int yy, RGB farbe) {
    if (xx>=hor or yy>=ver or xx<0 or yy<0) { return; }

    int toffset;
    switch (deep) {
      case 65535:
        toffset = (xx + yy * hor) * sizeof(RGB); // sizeof(RGB) = 6
        tbild.memo[toffset+0] = farbe.red >> 8;
        tbild.memo[toffset+1] = farbe.red;
        tbild.memo[toffset+2] = farbe.green >> 8;
        tbild.memo[toffset+3] = farbe.green;
        tbild.memo[toffset+4] = farbe.blue >> 8;
        tbild.memo[toffset+5] = farbe.blue;
        break;
      case   255:
        toffset = (xx + yy * hor) * 3;
        tbild.memo[toffset+0] = (char)(0xff & farbe.red    );
        tbild.memo[toffset+1] = (char)(0xff & farbe.green  );
        tbild.memo[toffset+2] = (char)(0xff & farbe.blue   );
        break;
      default:
        break;
    }
  }

  void plor3( int xx, int yy, RGB farbe) {
    if (xx>=hor or yy>=ver or xx<0 or yy<0) { return; }

    int toffset;
    switch (deep) {
      case 65535:
        toffset = (xx + yy * hor) * sizeof(RGB); // sizeof(RGB) = 6
        tbild.memo[toffset+0] |= farbe.red >> 8;
        tbild.memo[toffset+1] |= farbe.red;
        tbild.memo[toffset+2] |= farbe.green >> 8;
        tbild.memo[toffset+3] |= farbe.green;
        tbild.memo[toffset+4] |= farbe.blue >> 8;
        tbild.memo[toffset+5] |= farbe.blue;
        break;
      case   255:
        toffset = (xx + yy * hor) * 3;
        tbild.memo[toffset+0] |= (char)(0xff & farbe.red    );
        tbild.memo[toffset+1] |= (char)(0xff & farbe.green  );
        tbild.memo[toffset+2] |= (char)(0xff & farbe.blue   );
        break;
      default:
        break;
    }
  }

  string erzeuge_verzeichnisse( string path) {
    int pos=path.find_last_of('/');
    string branch = path.substr(0,pos);
    if (branch.size() > 0) {
      erzeuge_verzeichnisse( branch);
      if (false) cout << "D010 mkdir( \"" << branch << "\") ";
      int status = mkdir( branch.c_str(), 0777);
      if (false) cout << (status>-1 ? "gemacht" : "war schon da") << endl;
    }
    return branch;
  }

  void druck_pnm( string dateiname3) {
    // std::cerr << "Z030 druck_pnm progname= " << this->progname << std::endl;
    ofstream druck_datei;
//  dateiname3.size();
    string filename( dateiname3);
//  filename.replace( filename.end()-7,filename.end()-4, "string");
    filename.replace( filename.end()-4,filename.end()-0, ".pnm");
    erzeuge_verzeichnisse( filename);
    druck_datei.open ( filename.c_str());
    if (druck_datei.is_open()) {
      /* ok, proceed with output */
      druck_datei
        << "P6"                   << endl
        << "# " << this->progname << endl
        << hor << " " << ver      << endl
        << deep                   << endl
        << tbild.memo
        ;
      druck_datei.close();
      cerr << "D020 " << filename << " geschrieben." << endl;
    } else {
      cerr << "D030 nicht geöffnet " << filename << endl;
    }
  }
};

class cframebuffer : public cbildpnm {
  public:
  cframebuffer( int hor, int ver, int deep, string progname) : cbildpnm( hor, ver, deep, progname) {}
  private:
    char *framebufferpointer;
    int fbfiledescriptor;
    long int screensize;
    bool debug_framebuffer;

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

      // Get variable screen information
      if (ioctl(fbfiledescriptor, FBIOGET_VSCREENINFO, &vinfo) == -1) {
        perror("E020 Error reading variable information");
        exit(3);
      }

      if (debug_framebuffer) fprintf( stderr, "FB24  vinfo.xres x vinfo.yres       %dx%d, %dbpp\n", vinfo.xres,    vinfo.yres,  vinfo.bits_per_pixel);
      if (debug_framebuffer) fprintf( stderr, "FB24  vinfo.xoffset x vinfo.yoffset %dx%d\n",        vinfo.xoffset, vinfo.yoffset);

      // Figure out the size of the screen in bytes
      INTPAIR HORxVER;
      bool host_johnny; host_johnny = false;
      if (host_johnny) {
        HORxVER.x = 1920;
        HORxVER.y = 1080;
        screensize = 1920 * 1080 * vinfo.bits_per_pixel / 8;
      } else {
        HORxVER.x = vinfo.xres;
        HORxVER.y = vinfo.yres;
        screensize = vinfo.xres * vinfo.yres * vinfo.bits_per_pixel / 8;
      }

      if (debug_framebuffer) fprintf( stderr, "FB26  screensize=%d*%d*%d vinfo.bits_per_pixel/8=%ld bytes\n",
          vinfo.xres, vinfo.yres, vinfo.bits_per_pixel/8, screensize);

      // Map the device to memory
      framebufferpointer = (char *)mmap(0, screensize, PROT_READ | PROT_WRITE, MAP_SHARED, fbfiledescriptor, 0);
      if (framebufferpointer == (void *) -1) {
        perror("E020 Error: failed to map framebuffer device to memory");
        exit(4);
      }
      if (debug_framebuffer) fprintf( stderr, "FB28  The framebuffer device was mapped to memory %p.\n", (void *)framebufferpointer);
      if (debug_framebuffer) fprintf( stderr, "FB30 Konstruktor fertig cframebuffer() this= %p\n\n", this);
      return HORxVER;

    }

  void plotf(int x, int y, int red, int green, int blue) {
    long int location = 0;

    // Figure out where in memory to put the pixel

    location = (x+vinfo.xoffset) * (vinfo.bits_per_pixel/8) +
               (y+vinfo.yoffset) * finfo.line_length;
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

class ctrue_type_font : public cframebuffer {
  public:
  ctrue_type_font( int hor, int ver, int deep, string progname) : cframebuffer( hor, ver, deep, progname) {
    this->textfarbe = weisz;
    this->textfenster_origin_x = 129; this->textfenster_origin_y = 129;
    this->textfenster_origin_x =   0; this->textfenster_origin_y =   0;
    this->textfenster_height = ver; this->textfenster_width = hor; // this->deep = deep; this->progname = progname;
    this->textfenster_speicher = erzeuge_textfenster( this->textfenster_width, this->textfenster_height);
    set_text_pt( 50); set_text_dpi( 100);

    clear_text();
if (false) {
    char text[] = "\101\344ABCdefäölüßghixyzLOKj";
    u32string text32 = chararray_to_u32string( text);
    this->main32( "/usr/share/fonts/truetype/ubuntu-font-family/UbuntuMono-R.ttf", text32,    15, 450,  22);

    u32string ein_32wort = U"AäöüßÄÖłżźśńęąEфывапролджэ"; // ruft plot3
    this->main32( "/usr/share/fonts/truetype/ubuntu-font-family/UbuntuMono-R.ttf", ein_32wort, 20, 500,  15);
    druck_pnm( "/tmp/turtle/turtle-001-ctrue_type_font-02.pnm");
}
  }
  ctrue_type_font() {}

  private:
  int textfenster_origin_x, textfenster_origin_y;
  int textfenster_height, textfenster_width;
  string fontdateiname, druckdateiname;
  RGB textfarbe;

  vector<string> textfenster_speicher;

  vector<string> erzeuge_textfenster( int textfenster_width, int textfenster_height) {
    vector<string> textfenster_image( textfenster_height);
    string zeile( textfenster_width, 97);
      for (int ii=0;ii<textfenster_height;ii++) {
        textfenster_image[ii] = zeile;
      }
      vector<string>::const_iterator iit;
      int nr;
      if (false) for (iit=textfenster_image.begin(),nr=0; iit!=textfenster_image.end(); iit++,nr++){
        cout << "textfenster_image.capacity() = " << textfenster_image.capacity() << endl;
        cout << "textfenster_image.size() = " << textfenster_image.size() << endl;
        cout << nr << " " << (*iit) << endl;
      }
    return textfenster_image;
  }

  int points, dpi;
  public:
  void set_text_pt( int points) { this->points = points; }
  void set_text_dpi( int dpi) { this->dpi = dpi; }
  
  u32string chararray_to_u32string( char *ein_text) {
    int lang = strlen( ein_text);
    u32string ein32wort( lang, 98);
    for (int n=0; n<lang; n++) {
      ein32wort[n] = ein_text[n] & 0x000000ff;
    }
    return ein32wort;
  }

  // ftview 36 /usr/share/fonts/truetype/ubuntu-font-family/Ubuntu-BI.ttf
  void set_text_font( string fontdateiname) { // Alle Fonts : fc-list | less
    this->fontdateiname = fontdateiname;
  }

  void set_textfarbe( RGB farbname) { // Alle Fonts : fc-list | less
    this->textfarbe = farbname;
  }

  void main32( u32string ein32wort, double xpos, double ypos, double winkel_deg) {
    main32( this->fontdateiname.c_str(), ein32wort, xpos, ypos, winkel_deg);
  }

  void draw_bitmap_nach_textfenster( FT_Bitmap*  bitmap, FT_Int      x, FT_Int      y) {
    FT_Int  i, j, p, q;
    FT_Int  x_max = x + bitmap->width;
    FT_Int  y_max = y + bitmap->rows;
  
    /* for simplicity, we assume that `bitmap->pixel_mode' */
    /* is `FT_PIXEL_MODE_GRAY' (i.e., not a bitmap font)   */
  
    for ( i = x, p = 0; i < x_max; i++, p++ ) {
      for ( j = y, q = 0; j < y_max; j++, q++ ) {
        if ( i < 0 || j < 0 || i >= textfenster_width || j >= this->textfenster_height ) continue;
        this->textfenster_speicher[j][i] |= bitmap->buffer[q * bitmap->width + p];
  
  /*
        plot3( i, j,
            textfenster_speicher[j][i] == 0 ? (RGB){ (WORT)0x0000, (WORT)0x0000, (WORT)0x0000}
                             : textfenster_speicher[j][i] < 128 ? (RGB){ (WORT)0x7000, (WORT)0x7fff, (WORT)0xffff}
                                                 : (RGB){ (WORT)0x0000, (WORT)0x7fff, (WORT)0x7fff}
            );
  
  */
      }
    }
  }
  
  void show_textfenster_as_ascii_graphik( void ) {
  
    int  i, j;
    for ( i = 0; i < this->textfenster_height; i++ ) {
      for ( j = 0; j < textfenster_width; j++ ) {
        int bu = 96 * textfenster_speicher[i][j] / 256 + 32;
        putc( bu==127 ? 32 : bu, stdout);
      } 
      putc( '\n', stdout);
    }
  }
  
  void show_textfenster_as_ascii( void ) {
    int  i, j;
    for ( i = 0; i < this->textfenster_height; i++ ) {
      for ( j = 0; j < textfenster_width; j++ )
        putc( textfenster_speicher[i][j] == 0 ? ' '
                                  : textfenster_speicher[i][j] < 128 ? '+'
                                                      : '*'
            , stdout);
      putc( '\n', stdout);
    }
  }
  
  void clear_text( void ) {
    for ( int i = 0; i < this->textfenster_height; i++ ) {
      for ( int j = 0; j < textfenster_width; j++ )
        textfenster_speicher[i][j] = 0;
    }
  }
  
  void plot3_von_textfenster_nach_tbild_monochrom( void ) {
    int  i, j;
    for ( i = 0; i < this->textfenster_height; i++ ) {
      for ( j = 0; j < this->textfenster_width; j++ )
//      plot3( j, i, 
        plor3( j+this->textfenster_origin_x, i+this->textfenster_origin_x,
            this->textfenster_speicher[i][j] == 0 ? hellgruen
                             : this->textfenster_speicher[i][j] < 128 ? textfarbe
                                                 : hell_tuerkis
            );
    }
  }
  
  void plot3_textfenster( void ) {
    int  i, j;
    for ( i = 0; i < this->textfenster_height; i++ ) {
      for ( j = 0; j < textfenster_width; j++ ) {
        unsigned char farbe8 = this->textfenster_speicher[i][j];
  //    if (farbe8==0) { plot3( j, i, (RGB){ 0, 0, 0}); return;}
        WORT red   = (((farbe8 & 7) + 1) * 32 - 1) * 256; farbe8 >>= 3;
        WORT green = (((farbe8 & 7) + 1) * 32 - 1) * 256; farbe8 >>= 3;
        WORT blue  = (((farbe8 & 7) + 1) * 64 - 1) * 256; farbe8 >>= 3;
        plot3( j, i, (RGB){ red, blue, green});
      }
    }
  }

            typedef struct  TGlyph_ {
              FT_UInt    index;  /* glyph index                  */
              FT_Vector  pos;    /* glyph origin on the baseline */
              FT_Glyph   image;  /* glyph image                  */
            
            } TGlyph, *PGlyph;
/*
 the compute_string_bbox function can now compute
 the bounding box of a transformed glyph string,
 which allows further code simplications.
*/

            void  compute_string_bbox( FT_BBox  *abbox, PGlyph glyphs, int num_glyphs) {
              FT_BBox  bbox;
            
            
              bbox.xMin = bbox.yMin =  32000;
              bbox.xMax = bbox.yMax = -32000;
            
              for ( int n = 0; n < num_glyphs; n++ ) {
                FT_BBox  glyph_bbox;
            
                FT_Glyph_Get_CBox( glyphs[n].image, ft_glyph_bbox_pixels, &glyph_bbox );
            
                if (glyph_bbox.xMin < bbox.xMin)
                  bbox.xMin = glyph_bbox.xMin;
            
                if (glyph_bbox.yMin < bbox.yMin)
                  bbox.yMin = glyph_bbox.yMin;
            
                if (glyph_bbox.xMax > bbox.xMax)
                  bbox.xMax = glyph_bbox.xMax;
            
                if (glyph_bbox.yMax > bbox.yMax)
                  bbox.yMax = glyph_bbox.yMax;
              }
            
              if ( bbox.xMin > bbox.xMax ) {
                bbox.xMin = 0;
                bbox.yMin = 0;
                bbox.xMax = 0;
                bbox.yMax = 0;
              }
            
              *abbox = bbox;
            }

  int main32(
      const char *textfenster_filename,
      u32string ein32wort,
      double xpos, double ypos, double winkel_deg
      ) {
    FT_Library    library;
    FT_Face       face;

    FT_GlyphSlot  slot;
    FT_Matrix     matrix;                 /* transformation matrix */
    FT_Vector     pen;                    /* untransformed origin  */
    FT_Error      error;

            /*
             We also translate each glyph image directly after it is loaded
             to its position on the baseline at load time.
             As we will see, this has several advantages.
             Here is our new glyph sequence loader.
            */
            #define MAX_GLYPHS 4321
            //FT_GlyphSlot  slot = face->glyph;  /* a small shortcut */
            FT_Bool       use_kerning;
            FT_UInt       previous;
            int           pen_x, pen_y;
            
            TGlyph        glyphs[MAX_GLYPHS];  /* glyphs table */
            PGlyph        glyph;               /* current glyph in table */
            FT_UInt       num_glyphs;

    double        angle;
    int           target_textfenster_width, target_textfenster_height;
    int           n, num_chars;

    num_chars     = ein32wort.size();
    angle         = ( winkel_deg / 360 ) * 3.14159 * 2;      /* use 25 degrees     */
    target_textfenster_height = this->textfenster_height;
    target_textfenster_width  = this->textfenster_width ;

    error = FT_Init_FreeType( &library );                    /* ... initialize library ... */
    if (error!=0) {
      fprintf( stderr, "error=%d FT_Init_FreeType\n", error);
      return errno;
    }
    error = FT_New_Face( library, textfenster_filename, 0, &face );   /* ... create face object ... */
    if (error!=0) {
      fprintf( stderr, "F020 error=%d FT_New_Face %s\n", error, textfenster_filename);
      return errno;
    }
    /* use 50pt at 100dpi */
    error = FT_Set_Char_Size( face, this->points * 64, 0, this->dpi, 0 );  /* ... set character size ... */
    if (error!=0) {
      fprintf( stderr, "error=%d FT_Set_Char_Size\n", error);
      return errno;
    }
    /* cmap selection omitted;                                        */
    /* for simplicity we assume that the font contains a Unicode cmap */

    slot = face->glyph;

    /* set up matrix */
    matrix.xx = (FT_Fixed)( cos( angle ) * 0x10000L );
    matrix.xy = (FT_Fixed)(-sin( angle ) * 0x10000L );
    matrix.yx = (FT_Fixed)( sin( angle ) * 0x10000L );
    matrix.yy = (FT_Fixed)( cos( angle ) * 0x10000L );

            pen_x = 0;   /* start at (0,0) */
            pen_y = 0;
            num_glyphs  = 0;
            use_kerning = FT_HAS_KERNING( face );
            previous    = 0;
            
            glyph = glyphs;
    bool debug_freetype = false;
/* ... load glyph sequence ... */
    for ( n = 0; n < num_chars; n++ ) {
      FT_ULong buchstabe = ein32wort[n];
      glyph->index = FT_Get_Char_Index( face, buchstabe /*text[n]*/ );
      if (debug_freetype)
        cerr
          << "B010"
          << " n"                 << setw(3) << n
          << " buchstabe " << hex << setw(3) << buchstabe
          << " glyph->index "     << setw(3) << glyph->index
          << " pen_x " << dec     << setw(3) << pen_x
          << endl
          ;
            
              if ( use_kerning && previous && glyph->index ) {
                FT_Vector  delta;
                FT_Get_Kerning( face, previous, glyph->index, FT_KERNING_DEFAULT, &delta );
                pen_x += delta.x >> 6;
//    cout << "B020 delta.x " << delta.x << endl;
              }
            
              /* store current pen position */
              glyph->pos.x = pen_x;
              glyph->pos.y = pen_y;
            
              error = FT_Load_Glyph( face, glyph->index, FT_LOAD_DEFAULT );
              if ( error ) { cout << "B030"  << endl; continue;}
            
              error = FT_Get_Glyph( face->glyph, &glyph->image );
              if ( error ) { cout << "B040"  << endl; continue;}
            
              /* translate the glyph image now */
              FT_Glyph_Transform( glyph->image, 0, &glyph->pos );
            
              pen_x   += slot->advance.x >> 6;
              previous = glyph->index;
            
              /* increment number of glyphs */
              glyph++;
    }
/* count number of glyphs loaded */
    num_glyphs = glyph - glyphs;

    FT_BBox  string_bbox;
/* get bbox of original glyph sequence */
    compute_string_bbox( &string_bbox, glyphs, num_glyphs);
if (false) cout << "B050"
  << " string_bbox.xMin " << string_bbox.xMin 
  << " string_bbox.xMax " << string_bbox.xMax 
  << " string_bbox.yMin " << string_bbox.yMin 
  << " string_bbox.yMax " << string_bbox.yMax 
  << endl;

/* ... set up `matrix' and `delta' ... */
/* compute string dimensions in integer pixels */
int string_width  = (string_bbox.xMax - string_bbox.xMin) / 64;
int string_height = (string_bbox.yMax - string_bbox.yMin) / 64;

/* set up start position in 26.6 Cartesian space */
FT_Vector     start;
start.x = ( ( target_textfenster_width  - string_width  ) / 2 ) * 64;
start.y = ( ( target_textfenster_height - string_height ) / 2 ) * 64;
start.x = (   target_textfenster_width  / 10                  ) * 64;
start.y = (   target_textfenster_height /  4                  ) * 64;
start.x = xpos * 64;
start.y = ( target_textfenster_height - ypos ) * 64;
start.y-= ( string_bbox.yMin ) * 64;

/* set up transform (a rotation here) */
matrix.xx = (FT_Fixed)( cos( angle ) * 0x10000L );
matrix.xy = (FT_Fixed)(-sin( angle ) * 0x10000L );
matrix.yx = (FT_Fixed)( sin( angle ) * 0x10000L );
matrix.yy = (FT_Fixed)( cos( angle ) * 0x10000L );

pen = start;

for ( n = 0; n < num_glyphs; n++ ) {
  /* create a copy of the original glyph */
  FT_Glyph image;
  error = FT_Glyph_Copy( glyphs[n].image, &image );
  if ( error ) {
    if (debug_freetype)
      cerr
        << "B020"
        << " n"                 << setw(3) << dec << n
        << " glyphs[n].index "  << setw(3) << hex << glyphs[n].index
        << endl;

    continue;
  }

  /* transform copy (this will also translate it to the */
  /* correct position                                   */
  FT_Glyph_Transform( image, &matrix, &pen );

  /* check bounding box; if the transformed glyph image      */
  /* is not in our target surface, we can avoid rendering it */
  FT_BBox  bbox;
  FT_Glyph_Get_CBox( image, ft_glyph_bbox_pixels, &bbox );
  if ( bbox.xMax < 0 || bbox.xMin >= target_textfenster_width  ||
       bbox.yMax < 0 || bbox.yMin >= target_textfenster_height ) { // bbox.xMax <= 0 lässt Leerschritte verschwinden 
    if (debug_freetype)
      cerr
        << "B021"
        << " n"                 << setw(3) << dec << n
        << " glyphs[n].index "  << setw(3) << hex << glyphs[n].index
        << " bbox.xMin "        << setw(3) << dec << bbox.xMin 
        << " bbox.xMax "        << setw(3) << dec << bbox.xMax 
        << endl;

    continue;
  }

  /* convert glyph image to bitmap (destroy the glyph copy!) */
  error = FT_Glyph_To_Bitmap(
            &image,
            FT_RENDER_MODE_NORMAL,
            0,                  /* no additional translation */
            1 );                /* destroy copy in "image"   */
  if ( !error ) {
    FT_BitmapGlyph  bit = (FT_BitmapGlyph)image;

    /* now, draw to our target surface (convert position) */
      if (debug_freetype)
        cerr
          << "B022"
          << " n"                 << setw(3) << dec << n
          << " glyphs[n].index "  << setw(3) << hex << glyphs[n].index
          << " bit->left "        << setw(3) << dec << bit->left
          << " bit->top "         << setw(3) << dec << bit->top
          << endl;
            
    draw_bitmap_nach_textfenster( &(bit->bitmap),
                 bit->left,
                 target_textfenster_height - bit->top );

    /* increment pen position --                       */
    /* we don't have access to a slot structure,       */
    /* so we have to use advances from glyph structure */
    /* (which are in 16.16 fixed float format)         */
    pen.x += image->advance.x >> 10;
    pen.y += image->advance.y >> 10;

    FT_Done_Glyph( image );
  }
}

    plot3_von_textfenster_nach_tbild_monochrom();
  //plot3_textfenster();
  //show_textfenster_as_ascii_graphik();

    FT_Done_Face    ( face );
    FT_Done_FreeType( library );

    return 0;
  }
  
  int main32_gute_erste_version(
      const char *textfenster_filename,
      u32string ein32wort,
      double xpos, double ypos, double winkel_deg
      ) {
    FT_Library    library;
    FT_Face       face;

    FT_GlyphSlot  slot;
    FT_Matrix     matrix;                 /* transformation matrix */
    FT_Vector     pen;                    /* untransformed origin  */
    FT_Error      error;

    double        angle;
    int           target_textfenster_height;
    int           n, num_chars;

    num_chars     = ein32wort.size();
    angle         = ( winkel_deg / 360 ) * 3.14159 * 2;      /* use 25 degrees     */
    target_textfenster_height = this->textfenster_height;

    error = FT_Init_FreeType( &library );                    /* initialize library */
    if (error!=0) {
      fprintf( stderr, "error=%d FT_Init_FreeType\n", error);
      return errno;
    }
    error = FT_New_Face( library, textfenster_filename, 0, &face );   /* create face object */
    if (error!=0) {
      fprintf( stderr, "F020 error=%d FT_New_Face %s\n", error, textfenster_filename);
      return errno;
    }
    /* use 50pt at 100dpi */
    error = FT_Set_Char_Size( face, this->points * 64, 0, this->dpi, 0 );  /* set character size */
    if (error!=0) {
      fprintf( stderr, "error=%d FT_Set_Char_Size\n", error);
      return errno;
    }
    /* cmap selection omitted;                                        */
    /* for simplicity we assume that the font contains a Unicode cmap */

    slot = face->glyph;

    /* set up matrix */
    matrix.xx = (FT_Fixed)( cos( angle ) * 0x10000L );
    matrix.xy = (FT_Fixed)(-sin( angle ) * 0x10000L );
    matrix.yx = (FT_Fixed)( sin( angle ) * 0x10000L );
    matrix.yy = (FT_Fixed)( cos( angle ) * 0x10000L );

    /* the pen position in 26.6 cartesian space coordinates; */
    /* start at (300,200) relative to the upper left corner  */
    /* start at (xpos,ypos) relative to the upper left corner  */
    pen.x = xpos * 64;
    pen.y = ( target_textfenster_height - ypos ) * 64;

    for ( n = 0; n < num_chars; n++ ) {
      FT_ULong buchstabe = ein32wort[n];
      /* set transformation */
      FT_Set_Transform( face, &matrix, &pen );

      /* load glyph image into the slot (erase previous one) */
      error = FT_Load_Char( face, buchstabe, FT_LOAD_RENDER );
      if ( error ) continue;                 /* ignore errors */

      /* now, draw to our target surface (convert position) */
      draw_bitmap_nach_textfenster( &slot->bitmap,
                   slot->bitmap_left,
                   target_textfenster_height - slot->bitmap_top );

      /* increment pen position */
      pen.x += slot->advance.x;
      pen.y += slot->advance.y;
    }

    plot3_von_textfenster_nach_tbild_monochrom();
  //plot3_textfenster();
  //show_textfenster_as_ascii_graphik();

    FT_Done_Face    ( face );
    FT_Done_FreeType( library );

    return 0;
  }
  
  int main32_FreeType2_Mono(
      const char *textfenster_filename,
      u32string ein32wort,
      double xpos, double ypos, double winkel_deg
      ) {
    FT_Library    library;
    FT_Face       face;

    FT_GlyphSlot  slot;
    FT_Matrix     matrix;                 /* transformation matrix */
    FT_Vector     pen;                    /* untransformed origin  */
    FT_Error      error;

    double        angle;
    int           target_textfenster_height;
    int           n, num_chars;

                int max_descent = 0, max_ascent = 0;

                struct worte_mit_masz {
                  int        ascent;
                  int        max_ascent;
                  int        max_descent;
                  FT_ULong   buchstabe;
                };
                vector<worte_mit_masz> wort_mit_masz;
                typedef struct Text_Bitmap_mit_Dimensions_ {
                    int            width;
                    int            height;
                    int            baseline;
                    // unsigned char *ascii_speicher;
                } Text_Bitmap_mit_Dimensions;
                Text_Bitmap_mit_Dimensions worte_mit_bbox;

    num_chars     = ein32wort.size();
    angle         = ( winkel_deg / 180.0 ) * 3.14159265358979323844 ;      /* use 25 degrees     */
    target_textfenster_height = this->textfenster_height;

    error = FT_Init_FreeType( &library );                    /* initialize library */
    if (error!=0) {
      fprintf( stderr, "error=%d FT_Init_FreeType\n", error);
      return errno;
    }
    error = FT_New_Face( library, textfenster_filename, 0, &face );   /* create face object */
    if (error!=0) {
      fprintf( stderr, "F020 error=%d FT_New_Face %s\n", error, textfenster_filename);
      return errno;
    }
    /* use 50pt at 100dpi */
    error = FT_Set_Char_Size( face, this->points * 64, 0, this->dpi, 0 );  /* set character size */
    if (error!=0) {
      fprintf( stderr, "error=%d FT_Set_Char_Size\n", error);
      return errno;
    }
    /* cmap selection omitted;                                        */
    /* for simplicity we assume that the font contains a Unicode cmap */

    slot = face->glyph;

    /* set up matrix */
    matrix.xx = (FT_Fixed)( cos( angle ) * 0x10000L );
    matrix.xy = (FT_Fixed)(-sin( angle ) * 0x10000L );
    matrix.yx = (FT_Fixed)( sin( angle ) * 0x10000L );
    matrix.yy = (FT_Fixed)( cos( angle ) * 0x10000L );

    /* the pen position in 26.6 cartesian space coordinates; */
    /* start at (300,200) relative to the upper left corner  */
    /* start at (xpos,ypos) relative to the upper left corner  */
    pen.x = xpos * 64;
    pen.y = ( target_textfenster_height - ypos ) * 64;

    for ( n = 0; n < num_chars; n++ ) {
      FT_ULong buchstabe = ein32wort[n];
      /* set transformation */
      FT_Set_Transform( face, &matrix, &pen );

      /* load glyph image into the slot (erase previous one) */
      error = FT_Load_Char( face, buchstabe, FT_LOAD_RENDER );
      if ( error ) continue;                 /* ignore errors */

                int descent = 0, ascent = 0, ascent_calc;
                int rows_minus_top = slot->bitmap.rows - slot->bitmap_top;
                if (descent < rows_minus_top) {descent = rows_minus_top; }
                if (slot->bitmap_top < slot->bitmap.rows) {
                    ascent_calc = slot->bitmap.rows;
                } else {
                    ascent_calc = slot->bitmap_top;
                }
                int ascent_calc_minus_descent = ascent_calc - descent;
                if (ascent < ascent_calc_minus_descent) { ascent = ascent_calc_minus_descent; }
   
                if (max_ascent  < ascent ) { max_ascent  = ascent;  }
                if (max_descent < descent) { max_descent = descent; }
   
                struct worte_mit_masz ein_wort_mit_masz;
                ein_wort_mit_masz.ascent      = ascent;
                ein_wort_mit_masz.max_ascent  = max_ascent;
                ein_wort_mit_masz.max_descent = max_descent;
                ein_wort_mit_masz.buchstabe   = buchstabe;
                wort_mit_masz.push_back( ein_wort_mit_masz);
   
                cout
                  << "M011"
                  << " buchstabe= "   << hex << buchstabe << dec
                  << " width= "       << setw(4) << slot->bitmap.width
                  << " rows= "        << setw(4) << slot->bitmap.rows
                  << " top= "         << setw(4) << slot->bitmap_top
                  << " descent= "     << setw(4) << descent
                  << " ascent= "      << setw(4) << ascent
                  << " max_descent= " << setw(4) << max_descent
                  << " max_ascent= "  << setw(4) << max_ascent
                  << endl;

      /* now, draw to our target surface (convert position) */
      draw_bitmap_nach_textfenster( &slot->bitmap,
                   slot->bitmap_left,
                   target_textfenster_height - slot->bitmap_top );

      /* increment pen position */
      pen.x += slot->advance.x;
      pen.y += slot->advance.y;
    }
    worte_mit_bbox.height = max_ascent + max_descent;
    // worte_mit_bbox.width  = width;
    worte_mit_bbox.baseline = max_descent;
    cout << "M011" << endl;

//    pen.y = max_ascent + max_descent - ascent - max_descent;
    pen.x = xpos * 64;
    pen.y = ( target_textfenster_height - ypos ) * 64;
//  wort_mit_masz[0].ascent -= 300;

    for ( n = 0; n < num_chars; n++ ) {
      FT_ULong buchstabe = ein32wort[n];
      /* set transformation */
      int retter_pen_y = pen.y;
//    pen.y += (wort_mit_masz[n].max_ascent - wort_mit_masz[n].ascent) * 64;
//    pen.y  = (worte_mit_bbox.height - wort_mit_masz[n].ascent + worte_mit_bbox.baseline) * 64;
      FT_Set_Transform( face, &matrix, &pen );

      /* load glyph image into the slot (erase previous one) */
      error = FT_Load_Char( face, wort_mit_masz[n].buchstabe, FT_LOAD_RENDER ); /* Ergebnis in face, verwende nur face->glyph als slot */
//    error = FT_Load_Char( face, buchstabe, FT_LOAD_RENDER );
      if ( error ) continue;                 /* ignore errors */

      /* now, draw to our target surface (convert position) */
      draw_bitmap_nach_textfenster( &slot->bitmap,
                   slot->bitmap_left,
                   target_textfenster_height - slot->bitmap_top );

      /* increment pen position */
      pen.x += slot->advance.x;
      pen.y = retter_pen_y;
      pen.y += slot->advance.y;
    }

    plot3_von_textfenster_nach_tbild_monochrom();
  //plot3_textfenster();
  //show_textfenster_as_ascii_graphik();

    FT_Done_Face    ( face );
    FT_Done_FreeType( library );

    return 0;
  }
  
};

#define SIGN(arg) (arg?arg>0?1:-1:0)
// definiere breseline

class cbrese : public ctrue_type_font {
  public:
  cbrese( int hor, int ver, int deep, string progname) : ctrue_type_font ( hor, ver, deep, progname) {
    this->film_mach_einzelne_bilder = false;
    this->film_dateinummer = 1000000;
    this->film_dateiname_praefix = "/tmp/turtle/turtle-brese-";
    string film_suffix( ".pnm"); this->film_suffix = film_suffix;
  }
  cbrese() {}

  private:
  bool film_mach_einzelne_bilder;
  int film_dateinummer;
  string film_verzeichnisname, film_dateiname_praefix, film_suffix;

  public:
  string film_pfadname_ganz() {
    return
        this->film_verzeichnisname
      + "/"
      + this->film_dateiname_praefix
      + to_string( this->film_dateinummer)
      + this->film_suffix
      ;
  }

  void film_machen( bool film_oder_nicht, string film_verzeichnis, string film_dateiname_praefix) {
    this->film_verzeichnisname = film_verzeichnis;
    this->film_dateiname_praefix = film_dateiname_praefix;
    this->film_dateinummer = 1000000;
    this->film_suffix = film_suffix = ".pnm";
    cerr << "B013 " << (film_oder_nicht?"film":"kein_film")
      << " film_pfadname_ganz= " << film_pfadname_ganz()
      << endl;
    if (film_oder_nicht) {
      this->film_mach_einzelne_bilder =  true;
    }
  }
// convert -delay 100 /tmp/turtle/turtle-003-cerprobe_turtle-10000??.pnm -loop 0 /tmp/turtle/turtle-003-cerprobe_turtle-1000999.gif
  void film_beenden() {
    cerr << "B014 "
      << "convert -delay 100 "
      << this->film_dateiname_praefix << "1??????.pnm"
      << " -loop 0 "
      << this->film_dateiname_praefix << "1999999.gif"
      << endl;
    this->film_mach_einzelne_bilder = false;;
  }
  void film_ein_bild( int x, int y, int xe, int ye, RGB farbe) {
    druck_pnm( film_pfadname_ganz());
    film_dateinummer++;
  }
  void breseline( DOUBLEPAIR pa, DOUBLEPAIR pe, RGB farbe) {
//  cerr << setprecision(5) << "B010 " << pa.x << " " << pa.y << " " << pe.x << " " << pe.y << endl;
    breseline( pa.x, pa.y, pe.x, pe.y, farbe);
  }
  void breseline( INTPAIR pa, INTPAIR pe, RGB farbe) {
//  cerr << setprecision(5) << "B012 " << pa.x << " " << pa.y << " " << pe.x << " " << pe.y << endl;
    breseline( pa.x, pa.y, pe.x, pe.y, farbe);
  }
  void breseline( int x, int y, int xe, int ye, RGB farbe) {
//  cerr << setprecision(5) << "B015 " << x << " " << y << " " << xe << " " << ye << endl;
    int m,e,   dx,dy,   s1,s2,   i, temp;
    bool austausch;
    dx=abs(xe-x);  dy=abs(ye-y);
    s1=SIGN(xe-x); s2=SIGN(ye-y);
    /* Vertausche dx und dy je nach Steigung der Strecke. */
    if (dy>dx) { temp=dx; dx=dy; dy=temp; austausch=true; } else austausch=false;
    m=2*dy;
    e=-dx;
    for (i=0; i<=dx; i++) {
      if (e>0) {
        if (austausch) x += s1; else y += s2;
        e -= 2*dx;
      }
      plot3( x, y, farbe);
      e += m;
      if (austausch) y += s2; else x += s1;
    }
    if (film_mach_einzelne_bilder) film_ein_bild(x, y, xe, ye, farbe);
  }

  void koperta_raster( bool mit_skala = false) {
    INTPAIR lo, ro,lu, ru;
    lu = (INTPAIR){      0, ver- 1};
    lo = (INTPAIR){      0,      0};
    ru = (INTPAIR){ hor- 1, ver- 1};
    ro = (INTPAIR){ hor   ,      0};
    // Kreuz
    breseline( lu, ro, weisz);
    breseline( lo, ru, weisz);
    // Rand
    breseline( lu, ru, farbe_rand_unten );
    breseline( ru, ro, farbe_rand_rechts);
    breseline( ro, lo, farbe_rand_oben  );
    breseline( lo, lu, farbe_rand_links );
    if (mit_skala) {
      // Skala
      set_text_font( "/usr/share/fonts/truetype/ubuntu-font-family/UbuntuMono-R.ttf");
      char labelc[] =  "0123456789";
      double hundert;
      hundert = 100;
      for (int yy=0; yy < ver; yy += hundert) {
        breseline( (INTPAIR){ 0          , yy}, (INTPAIR){ hor/100, yy}, strich_links );
        breseline( (INTPAIR){ hor-hor/100, yy}, (INTPAIR){ hor - 1, yy}, strich_rechts);
//      breseline( (INTPAIR){ 0          , yy}, (INTPAIR){ hor - 1, yy}, strich_rechts);
        snprintf( labelc, strlen( labelc), "%d", yy);
        u32string label = chararray_to_u32string( labelc);
        set_text_pt( 20); 
        main32( label, hor/100, yy,  0);
      }
      hundert = 200;
      for (int xx=0; xx < hor; xx += hundert) {
        breseline( (INTPAIR){ xx, 0          }, (INTPAIR){ xx, ver/100}, strich_oben  );
        breseline( (INTPAIR){ xx, ver-ver/100}, (INTPAIR){ xx, ver- 1 }, strich_unten );
//      breseline( (INTPAIR){ xx, 0          }, (INTPAIR){ xx, ver- 1 }, strich_unten );
        snprintf( labelc, strlen( labelc), "%d", xx);
        u32string label = chararray_to_u32string( labelc);
        set_text_pt( 20); 
        main32( label, xx, ver-ver/100,  0);
      }
    }
  }

};

// definiere die Abbildung der userworld(real-Koordinaten) auf die plotterwelt(int-Koordinaten)

class cabbild : public cbrese {
  // Der Kugelschreiber bewegt sich auf dem gerasterten Zeichenblatt.
  int kuli_min_x, kuli_max_x, kuli_min_y, kuli_max_y;
  double welt_min_x, welt_min_y, welt_max_x, welt_max_y;
  double abb_limit_min_x, abb_limit_min_y, abb_limit_max_x, abb_limit_max_y;
  double maszstab_x, maszstab_y;
  bool debug_skala;

  public:
  cabbild( int hor, int ver, int deep, string progname) : cbrese( hor, ver, deep, progname) {
    debug_skala = true;
    welt_min_x= -4000.0,
    welt_max_x=  4000.0,
    welt_min_y= -4000.0,
    welt_max_y=  4000.0;
    abb_limit_min_x= numeric_limits<double>::max();    //  4000.0,
    abb_limit_max_x= numeric_limits<double>::lowest(); // -4000.0,
    abb_limit_min_y= numeric_limits<double>::max();    //  4000.0,
    abb_limit_max_y= numeric_limits<double>::lowest(); // -4000.0;
    // Der Kugelschreiber bewgt sich auf dem gerasterten Zeichenblatt.
    kuli_min_x=0,
    kuli_max_x= hor,
    kuli_min_y=0,
    kuli_max_y= ver;
    if (false) fprintf( stderr, "A070 kuli_min_x=%d\n", kuli_min_x);
    if (false) fprintf( stderr, "A070 kuli_min_y=%d\n", kuli_min_y);
    if (false) fprintf( stderr, "A070 kuli_max_x=%d\n", kuli_max_x);
    if (false) fprintf( stderr, "A070 kuli_max_y=%d\n", kuli_max_y);
    if (false) fprintf( stderr, "A070 welt_min_x=%f\n", welt_min_x);
    if (false) fprintf( stderr, "A070 welt_min_y=%f\n", welt_min_y);
    if (false) fprintf( stderr, "A070 welt_max_x=%f\n", welt_max_x);
    if (false) fprintf( stderr, "A070 welt_max_y=%f\n", welt_max_y);
  }
  cabbild() {}
  void weltline(      double alt_ort_x   , double alt_ort_y   , double igel_ort_x   , double igel_ort_y , RGB igel_farbe) {
    breseline(      abbildx( alt_ort_x), abbildy( alt_ort_y), abbildx( igel_ort_x), abbildy( igel_ort_y),     igel_farbe);
  }
  void show_limits() {
    cerr
      << "...."
      << fixed << setprecision(5) << defaultfloat
      << " abb_limit_min=( " << abb_limit_min_x << ", " << abb_limit_min_y << ") "
      <<           " max=( " << abb_limit_max_x << ", " << abb_limit_max_y << ") "
      << endl;
  }
  void viewport() {
    maszstab_x = (kuli_max_x-kuli_min_x)/(welt_max_x-welt_min_x);
    maszstab_y = (kuli_max_y-kuli_min_y)/(welt_max_y-welt_min_y);
    if (false) fprintf( stderr, "maszstab_x=%f\n", maszstab_x);
    if (false) fprintf( stderr, "maszstab_y=%f\n", maszstab_y);
  }

  void userworld( double user_min_x, double user_min_y, double user_max_x, double user_max_y) {
    welt_min_x = user_min_x,
    welt_max_x = user_max_x,
    welt_min_y = user_min_y,
    welt_max_y = user_max_y;

    maszstab_x = (kuli_max_x-kuli_min_x)/(welt_max_x-welt_min_x);
    maszstab_y = (kuli_max_y-kuli_min_y)/(welt_max_y-welt_min_y);
//  maszstab_x /=2; maszstab_y /=2; // pnm-Format 255 oder 65535
    if (false) fprintf( stderr, "A030 kuli_min_x=%d\n", kuli_min_x);
    if (false) fprintf( stderr, "A030 kuli_min_y=%d\n", kuli_min_y);
    if (false) fprintf( stderr, "A030 kuli_max_x=%d\n", kuli_max_x);
    if (false) fprintf( stderr, "A030 kuli_max_y=%d\n", kuli_max_y);
    if (false) fprintf( stderr, "A030 welt_min_x=%f\n", welt_min_x);
    if (false) fprintf( stderr, "A030 welt_min_y=%f\n", welt_min_y);
    if (false) fprintf( stderr, "A030 welt_max_x=%f\n", welt_max_x);
    if (false) fprintf( stderr, "A030 welt_max_y=%f\n", welt_max_y);
    if (false) fprintf( stderr, "A030 maszstab_x=%f\n", maszstab_x);
    if (false) fprintf( stderr, "A030 maszstab_y=%f\n", maszstab_y);
  }

  INTPAIR abbild( DOUBLEPAIR welt_paar) {
    INTPAIR erg = (INTPAIR){ abbildx( welt_paar.x), abbildy( welt_paar.y)};
    return erg;
  }

  int abbildx( double welt_xx) {
    int erg;
    erg = kuli_min_x + round( (welt_xx-welt_min_x)*maszstab_x);
    if (false) fprintf( stderr, "A040   welt_xx=%4.2f --> abbildx=%d kuli_min_x=%d welt_min_x=%4.2f maszstab_x=%4.2f\n",
        welt_xx, erg, kuli_min_x, welt_min_x, maszstab_x);
    if (true) {
      if ( welt_xx<abb_limit_min_x) abb_limit_min_x= welt_xx;
      if ( welt_xx>abb_limit_max_x) abb_limit_max_x= welt_xx;
    }
    return erg;
  }

  int abbildy( double welt_yy) {
    int erg;
    erg = kuli_min_y + round( (welt_yy-welt_min_y)*maszstab_y);
    if (false) fprintf( stderr, "A040   welt_yy=%4.2f --> abbildy=%d kuli_min_y=%d welt_min_y=%4.2f maszstab_y=%4.2f\n",
        welt_yy, erg, kuli_min_y, welt_min_y, maszstab_y);
    if (true) {
      if ( welt_yy<abb_limit_min_y) abb_limit_min_y= welt_yy;
      if ( welt_yy>abb_limit_max_y) abb_limit_max_y= welt_yy;
    }
    return erg;
  }

  typedef  int (cabbild::*fatyp) (double);
  fatyp                  _fyptr;

  void koperta_welt( bool mit_raster = false) {
    if (mit_raster) koperta_raster( true);

    sskala skala_y = leiste_welt( welt_min_y, welt_max_y);
    if (debug_skala) cerr << endl << "S010y " << " welt_min_y= " << welt_min_y << " welt_max_y= " << welt_max_y
                             << " abb_min_y= " << abbildy( welt_min_y) << " abb_max_y= " << abbildy( welt_max_y) << endl;

    skala_y.zeichne_die_marken_ins_bild( this, 'y');

    if (debug_skala) cerr << "yyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyy koperta_welt" << endl;

    sskala skala_x = leiste_welt( welt_min_x, welt_max_x);
    if (debug_skala) cerr << endl << "S010x " << " welt_min_x= " << welt_min_x << " welt_max_x= " << welt_max_x
                             << " abb_min_x= " << abbildx( welt_min_x) << " abb_max_x= " << abbildx( welt_max_x) << endl;
    skala_x.zeichne_die_marken_ins_bild( this, 'x');

    if (debug_skala) cerr << "xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx koperta_welt" << endl;
  }

  struct sskala {
    double pov_pos; vector<double> marke;
    void zeichne_die_marken_ins_bild( cabbild *diss, char x_oder_y) {
//    int result4 = (*this.*funktion)( 1.0);
    // Skala
    diss->set_text_font( "/usr/share/fonts/truetype/ubuntu-font-family/UbuntuMono-R.ttf");
    char labelc[] =  "0123456789012345678901234567890123456789"; int label_size = sizeof(labelc);
    double schritt;

    if (diss->debug_skala) {
      cerr << "S050 " << x_oder_y << " ";
      for (int ii=0; ii<marke.size(); ii++) {
        cerr << setw(4) << setfill(' ') << marke[ii] << " ";
      }
      cerr
        << " pov_pos= "  << pov_pos
        << endl;

      cerr << "S051 " << x_oder_y << " ";
      for (int ii=0; ii<marke.size(); ii++) {
        cerr << setw(4) << setfill(' ') << marke[ii] * pov_pos << " ";
      }
      cerr << endl;
    }
      switch (x_oder_y) {
        case 'x':
            if (diss->debug_skala) {
              cerr << "S052 " << x_oder_y << " ";
              for (int ii=0; ii<marke.size(); ii++) {
                int x_koordinate_der_marke = diss->abbildx( marke[ii] * pov_pos);
                cerr << setw(4) << setfill(' ') << x_koordinate_der_marke << " ";
              }
              cerr << endl;
              cerr << "S022 " << x_oder_y << " ";
            }
            for (int ii=0; ii<marke.size(); ii++) {
              double mx = marke[ii] * pov_pos;
              int x_koordinate_der_marke = diss->abbildx( mx);
//            snprintf( labelc, label_size, "%4.0f %d", mx, x_koordinate_der_marke);
              snprintf( labelc, label_size, "%.0f"   , mx    );
              u32string label = diss->chararray_to_u32string( labelc);
//            cerr << label.c_str() << " ";
              diss->breseline( x_koordinate_der_marke,                0, x_koordinate_der_marke, diss->ver*1/100, diss->rot);
              diss->breseline( x_koordinate_der_marke, diss->ver*99/100, x_koordinate_der_marke, diss->ver,       diss->rot);
              diss->set_text_pt( 20);
              diss->main32( label, x_koordinate_der_marke, diss->ver-diss->ver/100.0, 10.0);
              if (diss->debug_skala) cerr << labelc << " ";
            }
            if (diss->debug_skala) cerr << endl;
          break;
        case 'y':
            if (diss->debug_skala) {
              cerr << "S052 " << x_oder_y << " ";
              for (int ii=0; ii<marke.size(); ii++) {
                int y_koordinate_der_marke = diss->abbildy( marke[ii] * pov_pos);
                cerr << setw(4) << setfill(' ') << y_koordinate_der_marke << " ";
              }
              cerr << endl;
              cerr << "S022 " << x_oder_y << " ";
            }
            for (int ii=0; ii<marke.size(); ii++) {
              int y_koordinate_der_marke = diss->abbildy( marke[ii] * pov_pos);
//            snprintf( labelc, label_size, "%4.0f %d", marke[ii] * pov_pos, y_koordinate_der_marke);
              snprintf( labelc, label_size, "%.0f"   , marke[ii] * pov_pos    );
              u32string label = diss->chararray_to_u32string( labelc);
//            cerr << label.c_str() << " ";
              diss->breseline( 0,                y_koordinate_der_marke, diss->hor*1/100, y_koordinate_der_marke, diss->gelb);
              diss->breseline( diss->hor*99/100, y_koordinate_der_marke, diss->hor,       y_koordinate_der_marke, diss->gelb);
              diss->set_text_pt( 20);
              diss->main32( label, diss->hor/100.0, y_koordinate_der_marke, 10.0);
              if (diss->debug_skala) cerr << labelc << " ";
            }
            if (diss->debug_skala) cerr << endl;
          break;
      }

    } 
  };

    sskala leiste_welt( double orig_min, double orig_max) {
      if (debug_skala) cerr << endl << "S010 " << " orig_min= " << orig_min << " orig_max= " << orig_max << endl;
      double rund_min, rund_max, rund_delta, orig_delta, orig_step;
      double mod_min, mod_max, mod_delta, mod_step;
      orig_delta = orig_max - orig_min;

      double log = floor( log10( orig_delta)) - 1.0;
      double pov_pos = exp10(  log);
      double pov_neg = exp10( -log);

      mod_min = orig_min * pov_neg; mod_max = orig_max * pov_neg; mod_delta = orig_delta * pov_neg;

      rund_min = floor( mod_min); rund_max = ceil( mod_max); rund_delta = rund_max - rund_min;

      orig_step = orig_delta/100.0 *  10.0; // soll 1 2 5 oder 10
      mod_step  = orig_step * pov_neg;
      if (false) {
        cerr << "orig_min=  " << orig_min << " orig_max= " << orig_max << " orig_delta= " << orig_delta << endl;
        cerr << "mod_min=   " << mod_min  << " mod_max=  " << mod_max  << " mod_delta=  " << mod_delta << " log= " << log << endl;
        cerr << "rund_min=  " << rund_min << " rund_max= " << rund_max << " rund_delta= " << rund_delta << endl;
        cerr << "orig_step= " << orig_step << endl;
        cerr << "mod_step=  " << mod_step  << endl;
        cerr << " pov_pos=" << pov_pos << endl;
      }

      sskala marken;
      marken = eine_markenleiste( rund_min, rund_max, (int)ceil(   mod_step), pov_pos);
      return marken;
    }

    sskala eine_markenleiste( double rund_min, double rund_max, int step, double pov_pos) {
      sskala marken;
      double step2510;
      switch (step) {
        case  1: step2510 =  1; break;
        case  2: 
        case  3: step2510 =  2; break;
        case  4:
        case  5:
        case  6: step2510 =  5; break;
        case  7: 
        case  8:
        case  9:
        case 10: step2510 = 10; break;
      default: break;
      }
      double links_shift;
      links_shift = floor( rund_min / step2510) * step2510;
      if (false) cerr
        << "step2510= "     << setw(3) << step2510
        << " links_shift= " << setw(3) << links_shift
        << " leiste= ";
      for (double markenkoordinate = links_shift; markenkoordinate < rund_max; markenkoordinate+=step2510) {
        marken.marke.push_back( markenkoordinate);
      }
      marken.pov_pos = pov_pos;
      return marken;
    }

};

// definiere titel

class ctitel : public cabbild {
  public:
  ctitel( int hor, int ver, int deep, string progname) : cabbild( hor, ver, deep, progname) {}
  ctitel() {}
  bool debug_ctitel;
  void drucke_einen_titel( u32string worte) {
    debug_ctitel = false;
    if (debug_ctitel) cerr << "T010 " << worte.c_str() << " " << (worte.size()>0?worte.c_str()[0]:'x') << endl;
    set_text_font( "/usr/share/fonts/truetype/ubuntu-font-family/UbuntuMono-R.ttf");
    // u32string worte = U"Titel AäöüßÄÖłżźśńęąEфывапролджэ";
    main32( "/usr/share/fonts/truetype/ubuntu-font-family/UbuntuMono-R.ttf", worte, hor/5, +60, 0);
  }
};

// definiere initturtle, move, turn, moveto usw.

class cturtle : public ctitel {
  public:
  enum class stiftart { AN, AUS};
  private:
  stiftart igel_art;
  DOUBLEPAIR igel_ort;
  double igel_richtung;
  RGB igel_farbe;
  
  public:
  struct igel_status {
    stiftart   igel_art;
    DOUBLEPAIR igel_ort;
    double     igel_richtung;
    RGB        igel_farbe;
  } igel_status;
  cturtle( int hor, int ver, int deep, string progname) : ctitel( hor, ver, deep, progname) { }
  cturtle() {}
  void turn( double angle) { igel_richtung += angle; igel_richtung = fmod( igel_richtung, 360.0);} 
  void turnto( double angle) { igel_richtung = angle; } 
  void moveto( DOUBLEPAIR punkt) {
    DOUBLEPAIR alt_ort = igel_ort;
    igel_ort = punkt;
    // to do : clipping 
    if (igel_art != stiftart::AUS) {
      if (false) fprintf( stderr, "L010 breseline( %4.2f %4.2f %4.2f %4.2f)\n", alt_ort.x, alt_ort.y, igel_ort.x, igel_ort.y);
      weltline( alt_ort.x,
                alt_ort.y,
                igel_ort.x,
                igel_ort.y,
                igel_farbe);
    }
  }
  
  void move( double dist) {
    moveto( (DOUBLEPAIR){ igel_ort.x + dist * cos( igel_richtung/180.0*M_PI),
                    igel_ort.y + dist * sin( igel_richtung/180.0*M_PI)}
            );
  }
  
  void initturtle( double turtle_min_x, double turtle_min_y, double turtle_max_x, double turtle_max_y) {
    igel_status.igel_ort = (DOUBLEPAIR){ 0.5*(turtle_min_x+turtle_max_x), 0.5*(turtle_min_y+turtle_max_y)};
    igel_ort = (DOUBLEPAIR){ 0.5*(turtle_min_x+turtle_max_x), 0.5*(turtle_min_y+turtle_max_y)};
    pencolor( tuerkis);
    penstyle( stiftart::AN);
    userworld( turtle_min_x, turtle_min_y, turtle_max_x, turtle_max_y);
//  moveto((DOUBLEPAIR){ 0.5*(turtle_min_x+turtle_max_x), 0.5*(turtle_min_y+turtle_max_y)});
//  moveto((DOUBLEPAIR){ 0.0, 0.0});
    turnto( 0.0);
    igel_art = stiftart::AN;
  }
  
  void pencolor( RGB farbe) { igel_farbe = farbe; } 
  void penstyle( stiftart art) { igel_art = art; } 
  stiftart penstyle() {     return igel_art;     }
  RGB  pencolor() {         return igel_farbe;   } 
  DOUBLEPAIR turtle_ort() { return igel_ort;     } 
  double turtle_angle() {   return igel_richtung;} 
  double turtle_x() {       return igel_ort.x;   } 
  double turtle_y() {       return igel_ort.y;   } 
  void closeturtle() {} 
};

class cerprobe_turtle : public cturtle {
  public:
  cerprobe_turtle( string progname, string dateiname, int hor, int ver, int deep) : cturtle( hor, ver, deep, progname) {
    std::cerr << "Z050  progname= " << progname << std::endl;
    // Zeichenfläche in int Pixelkoordinaten
    film_machen( false, "/data7/tmp/cerprobe_turtle", "");
    initturtle( 0.0, 0.0, 1920.0, 1080.0);
    pencolor( blaeulich);
    penstyle( stiftart::AN);
    moveto((DOUBLEPAIR){0000.0,0000.0});
    moveto((DOUBLEPAIR){0200.0,0300.0});
    moveto((DOUBLEPAIR){ 480.0, 270.0});
    move  (                     100.0 );
    moveto((DOUBLEPAIR){ 480.0, 540.0});
    moveto((DOUBLEPAIR){ 960.0,   0.0});
    moveto((DOUBLEPAIR){1440.0, 540.0});

    for (int count=0;count<5;count++) {
      move( 80);
      turn( 72.0);
    }
    turn( 180.0);
    for (int count=0;count<5;count++) {
      move( 80);
      turn( 72.0);
    }
    film_beenden();
    closeturtle();
    druck_pnm( "/tmp/turtle/turtle-003-cerprobe_turtle-99.pnm");
  }
};

class cerprobe_true_type_font : public ctrue_type_font {
  vector<string> textfenster;

  u32string chararray_to_u32string( char *ein_text) {
    int lang = strlen( ein_text);
    u32string ein32wort( lang, 98);
    for (int n=0; n<lang; n++) {
      ein32wort[n] = ein_text[n] & 0x000000ff;
    }
    return ein32wort;
  }
  void druck_eine_zeile( u32string wort, double xx, double yy, double ww) {
    set_text_font( "/usr/share/fonts/truetype/ubuntu-font-family/UbuntuMono-R.ttf");
    main32( wort, xx, yy,  ww+3);
    main32( "/usr/share/fonts/truetype/ubuntu-font-family/UbuntuMono-R.ttf", wort, xx, yy,  ww);
    druck_pnm( "/tmp/turtle/turtle-004-cerprobe_true_type_font-01.pnm");
  }
  void eine_methode_von_cerprobe_true_type_font_ruft( string wort) {
    cerr << "P010 wort=" << wort << " progname=\"" << progname << "\"" << endl;
    char text[] = "\101\344ABdeäölüß ttf-probe";
    u32string text32 = chararray_to_u32string( text);
    main32( "/usr/share/fonts/truetype/ubuntu-font-family/UbuntuMono-R.ttf", text32,    15, 600,  -7);
    druck_pnm( "/tmp/turtle/turtle-005-cerprobe_true_type_font-02.pnm");
  }

  public:
  cerprobe_true_type_font( string progname, string dateiname, int hor, int ver, int deep) : ctrue_type_font( hor, ver, deep, progname) {
    std::cerr << "Z052  cerprobe_true_type_font( string) progname= " << progname << std::endl;
    this->progname = progname;
  eine_methode_von_cerprobe_true_type_font_ruft( "cerprobe_true_type_font testet ctrue_type_font");
  druck_eine_zeile( U"EфывапролджэAäöüßÄÖłżźśńęą", 15, 600, -2);

    clear_grafik( 0);

    char text[] = "\101\344ABCdefäölüßghixyzLOKj";
    u32string text32 = chararray_to_u32string( text);
    main32( "/usr/share/fonts/truetype/ubuntu-font-family/UbuntuMono-R.ttf", text32,    15, 450,  22);

    u32string ein_32wort = U"AäöüßÄÖłżźśńęąEфывапролджэ"; // ruft plot3
    main32( "/usr/share/fonts/truetype/ubuntu-font-family/UbuntuMono-R.ttf", ein_32wort, 20, 500,  15);
    main32( "/usr/share/fonts/truetype/ubuntu-font-family/UbuntuMono-R.ttf", ein_32wort, 20, 600, -15);

    druck_pnm( "/tmp/turtle/turtle-006-erprobe_true_type_font-03.pnm");
  }
  
};

class csierpinski_ohne_turtle : public cturtle {
  RGB color;
  public:
  csierpinski_ohne_turtle( string progname, string dateiname, int hor, int ver, int deep) : cturtle( hor, ver, deep, progname) {
    std::cerr << "Z053  progname= " << progname << std::endl;
    // Zeichenfläche in int Pixelkoordinaten

//  initturtle( -2.0, -2.0, 5.47, 2.2);
    main_sierpinski();
//  closeturtle();

    druck_pnm( "/tmp/turtle/007-csierpinski_no_turtle_2.pnm");
  }
  void line( double xa, double ya, double xe, double ye) {
//  fprintf( stderr, "B020 %5.2f %5.2f %5.2f %5.2f %08x %08x %08x\n",
//      xa, ya, xe, ye, (unsigned int)this->color.red, (unsigned int)this->color.green, (unsigned int)this->color.blue);
    breseline( xa, ya, xe, ye, this->color);
  }
  void setcolor( int farbe) {
    WORT red, green, blue;
    switch (farbe%16) {
      case  0: red=0x0000; green=0x0000; blue=0x0000; break;
      case  1: red=0x0000; green=0x0000; blue=0x7fff; break;
      case  2: red=0x0000; green=0x7fff; blue=0x0000; break;
      case  3: red=0x0000; green=0x7fff; blue=0xffff; break;
      case  4: red=0x7fff; green=0x0000; blue=0x0000; break;
      case  5: red=0x7fff; green=0x0000; blue=0xffff; break;
      case  6: red=0x7fff; green=0xffff; blue=0x0000; break;
      case  7: red=0x7fff; green=0xffff; blue=0xffff; break;
      case  8: red=0x0000; green=0x0000; blue=0x0000; break;
      case  9: red=0x0000; green=0x0000; blue=0xffff; break;
      case 10: red=0x0000; green=0xffff; blue=0x0000; break;
      case 11: red=0x0000; green=0xffff; blue=0xffff; break;
      case 12: red=0xffff; green=0x0000; blue=0x0000; break;
      case 13: red=0xffff; green=0x0000; blue=0xffff; break;
      case 14: red=0xffff; green=0xffff; blue=0x0000; break;
      case 15: red=0xffff; green=0xffff; blue=0xffff; break;
      default: red=0x7fff; green=0x7fff; blue=0x7fff; break;
    }
    this->color = (RGB){ red, green, blue};
  }

  int getch() {}
  void initwindow( int maxx, int maxy){ initturtle( 0, 0, maxx, maxy);}
  void closegraph(){ closeturtle();}
  // https://www.geeksforgeeks.org/sierpinski-triangle-using-graphics/?ref=rp
  // C++ code to implement 
  // Sierpinski Triangle using Graphics 
  
//  #include <math.h> 
//  #include <stdlib.h> 
  
  // Defining a function to draw a triangle 
  // with thickness 'delta' 
  void triangle(double x, double y, double h, int colorVal) { 
   setcolor(colorVal % 15 + 1); 
  
   for (double delta = 0.0; delta > -5.0; delta -= 1.0) { 
    line(
     x - (h + delta) / sqrt(3.0), 
     y - (h + delta) / 3.0, 
     x + (h + delta) / sqrt(3.0), 
     y - (h + delta) / 3.0); 
    line(
     x - (h + delta) / sqrt(3.0), 
     y - (h + delta) / 3.0, 
     x, 
     y + 2.0 * (h + delta) / 3.0); 
    line(
     x, 
     y + 2.0 * (h + delta) / 3.0, 
     x + (h + delta) / sqrt(3.0), 
     y - (h + delta) / 3.0); 
   } 
  } 
  
  // Defining a function to draw 
  // an inverted triangle 
  // with thickness 'delta' 
  void trianglev2( double x, double y, double h, int colorVal) { 
   setcolor(colorVal % 15 + 1); 
  
   for (double delta = 0.0; delta > -1.0 + 5.0; delta -= 1.0) { 
  
    line(
     x - (h + delta) / sqrt(3.0), 
     y + (h + delta) / 3.0, 
     x + (h + delta) / sqrt(3.0), 
     y + (h + delta) / 3.0); 
    line(
     x - (h + delta) / sqrt(3.0), 
     y + (h + delta) / 3.0, 
     x, 
     y - 2.0 * (h + delta) / 3.0); 
    line(
     x, 
     y - 2.0 * (h + delta) / 3.0, 
     x + (h + delta) / sqrt(3.0), 
     y + (h + delta) / 3.0); 
   } 
  } 
  
  // A recursive function to draw out 
  // three adjacent smaller triangles 
  // while the height is greater than 5 pixels. 
  int drawTriangles ( double x, double y, double h, int colorVal) { 
  
   if (h < 5) { 
    return 0; 
   } 
  
   if (x > 0 && y > 0 && x < hor && y < ver) { 
    triangle(x, y, h, colorVal); 
   } 
  
   drawTriangles(x              , y - 2 * h / 3, h / 2, colorVal + 1); 
   drawTriangles(x - h / sqrt(3), y + h / 3    , h / 2, colorVal + 1); 
   drawTriangles(x + h / sqrt(3), y + h / 3    , h / 2, colorVal + 1); 
  
   return 0; 
  } 
  
  // Driver code 
  int main_sierpinski() { 
   initwindow(hor, ver); 
   trianglev2( hor / 2.0, 2.0 * ver / 3.0, ver, 2); 
  
   drawTriangles( hor / 2.0, 2.0 * ver / 3.0, ver / 2.0, 0); 
   getch(); 
   closegraph(); 
  
   return 0; 
  } 
};

class csierpinski_turtle : public cturtle {
  public:
  csierpinski_turtle( string progname, string dateiname, int hor, int ver, int deep) : cturtle( hor, ver, deep, progname) {
    std::cerr << "Z054  progname= " << progname << std::endl;
    // Zeichenfläche in int Pixelkoordinaten

    initturtle( -2.0, -2.0, 5.47, 2.2);
    film_machen( false, "/data7/tmp/sierpinski-turtle", "");
    // Zeichenfläche in int Pixelkoordinaten
    set_text_font( "/usr/share/fonts/truetype/dejavu/DejaVuSerif.ttf");
    set_text_pt( 40); set_text_dpi( 100);
    main32( U"  Wacław Sierpiński - Dreieck", 300, 1000, 60);
    set_text_font( "/usr/share/fonts/truetype/dejavu/DejaVuSerif.ttf");
    main32( U"Sierpinski-Dreiecke", 60, ver, 90);

    penstyle( stiftart::AUS);
    move( 2.0);
    turn( 90.0);
    move( 2.0);
    penstyle( stiftart::AN);
    turn(-90.0);
    pencolor( weisz);
    dreieck( 9.0);

    film_beenden();

    closeturtle();
    druck_pnm( "/tmp/turtle/008-csierpinski_turtle.pnm");
  }
  void dreieck( float breit) {
    for (int i=0;i<3;i++) {
      if (breit > 0.1) {
        turn( -120.0);
        move( breit/2.0);
        dreieck( breit/2.0);
      }
    }
  }

};

class cpythagoras_turtle : public cturtle {
  public:
  cpythagoras_turtle( string progname, string dateiname, int hor, int ver, int deep) : cturtle( hor, ver, deep, progname) {
    std::cerr << "Z055  progname= " << progname << std::endl;
    // Zeichenfläche in int Pixelkoordinaten
    set_text_font( "/usr/share/fonts/truetype/ubuntu-font-family/UbuntuMono-R.ttf");
    main32( U"Pythagoras-Baum", 70, ver-10, 0);
    set_text_font( "/usr/share/fonts/truetype/ubuntu-font-family/Ubuntu-BI.ttf");
    main32( U"Pythagoras-Baum", 50, ver, 90);

    // Zeichenfläche in double Weltkoordinaten
    initturtle( -2.0, -2.0, 5.47, 2.2);
    move( 0.2);
    /*
    move( 1.0);
    turn( 90.0);
//  penstyle( stiftart::AUS);
    move( 1.0);
    penstyle( stiftart::AN);
    turn(-180.0);
    */
    pencolor( weisz);
    haus_mit_dach( 0.5, +1.0);
    move   ( 0.6);
    haus_mit_dach( 0.5, -1.0);
    move   ( 0.6);
    haus_mit_dach( 0.5, +1.0);

    closeturtle();
    druck_pnm( "/tmp/turtle/turtle-009-cpythagoras_turtle.pnm");
  }
  void haus_mit_dach( float breit, float sign) {
    float min=0.1;
    move( breit);
    turn( +90.0 * sign);
    move( breit);
    turn( +90.0 * sign);
    move(  breit);
    move( -breit);

    turn(  -30.0 * sign);
    if (breit>2*min) haus_mit_dach(   breit*0.86602540378443864676, -sign); else move(   breit*0.86602540378443864676);
    turn(  +90.0 * sign);
    if (breit>min) haus_mit_dach(   breit*0.5, -sign); else move(   breit*0.5);
    turn(  -60.0 * sign);


    turn( +90.0 * sign);
    move( breit);
    turn( +90.0 * sign);
    move( breit);
  }
/*
    turn(   45.0);
    move(   breit*.7071067811865475244);
    turn(  -90.0);
    move(   breit*.7071067811865475244);
    turn(   45.0);
*/
    // breit^2 = breit^2 / 4.0 + x^2 
    // x^2 = breit^2 - breit^2 / 4.0
    // x^2 = breit^2 * 3.0 / 4.0
    // x   = sqrt( breit^2 * 3.0 / 4.0)
    // x   = sqrt( breit * breit * 3.0 / 4.0)
    // x   = breit * sqrt( 3.0 / 4.0)
    // x   = breit * sqrt( 0.75)
    // x   = breit * 0.86602540378443864676

};

class ckoch_turtle : public cturtle {
  float minimum;
  RGB koch_farbe_a, koch_farbe_b, koch_farbe_c, koch_farbe_d, koch_farbe_e;
  public:                                         // member initializer
  ckoch_turtle( string progname, string dateiname, int hor, int ver, int deep) : cturtle( hor, ver, deep, progname) {
    std::cerr << "Z056  progname= " << progname << std::endl;
    koch_farbe_a = (RGB){ (WORT)0xf000, (WORT)0xffff, (WORT)0xffff};
    koch_farbe_b = (RGB){ (WORT)0xffff, (WORT)0xf000, (WORT)0xffff};
    koch_farbe_c = (RGB){ (WORT)0xffff, (WORT)0xffff, (WORT)0xf000};
    koch_farbe_d = (RGB){ (WORT)0xf000, (WORT)0xffff, (WORT)0xf000};
    koch_farbe_e = (RGB){ (WORT)0xd000, (WORT)0xffff, (WORT)0xffff};
    minimum = 0.05;
    minimum = 0.30;
    minimum = 0.10;
    // Zeichenfläche in int Pixelkoordinaten

    film_machen( false, "/data7/tmp/koch2", "");

    initturtle( -2.0, -2.0, 5.47, 2.2);
    penstyle( stiftart::AUS);
    move(-2.7);

    kokurve( 1.8);

    penstyle( stiftart::AUS);
    move( 2.5);
    penstyle( stiftart::AN);
    turn( 315.0);

    kokurve( 2.5);

    kokurva();

    film_beenden();

    closeturtle();
    druck_pnm( "/tmp/turtle/turtle-010-ckoch_turtle.pnm");
  }

  int kokurva() {
    pencolor( koch_farbe_e);
    penstyle( stiftart::AUS);
    move( -0.35);
    turn( -90);
    move(  1.5);
    turn( -45);
    penstyle( stiftart::AN);
    for (int count=0;count<5;count++) {
      move( 0.4);
      turn( 72.0);
    }
    penstyle( stiftart::AUS);
    move( -0.7);
    turn(-90.0);
    move( -1.2);
    penstyle( stiftart::AN);
    koch( 1.8);
    turn( 90.0);
    koch( 1.8);
    turn( 90.0);
    pencolor( koch_farbe_b);
    koch( 1.8);
    turn( 90.0);
    koch( 1.8);
    turn( 90.0);
  }

  int kokurve( float eins_acht) {
    pencolor( koch_farbe_c);
    penstyle( stiftart::AN);
    if (false) for (int count=0;count<5;count++) {
      move( 0.4);
      turn( 72.0);
    }
    koch( eins_acht);
    turn( 90.0);
    koch( eins_acht);
    turn( 90.0);
    pencolor( koch_farbe_d);
    koch( eins_acht);
    turn( 90.0);
    koch( eins_acht);
    turn( 90.0);
  }

  void koch( float breit) {
      if (breit > minimum) {
            breit /= 3.0;
            koch( breit);
            turn(   60.0);
            koch( breit);
            turn( -120.0);
            koch( breit);
            turn(   60.0);
            koch( breit);
          } else {
            move( breit);                                         // member initializer
          }
    }

};

class cerprobe_brese : public cbrese {
  RGB farbe;
  public:
  cerprobe_brese() {
    std::cerr << "Z020 cerprobe_brese() progname= " << progname << std::endl << std::endl;
  }
  cerprobe_brese( string progname, string dateiname, int hor, int ver, int deep) : cbrese( hor, ver, deep, progname) {
    std::cerr << "Z057  cerprobe_brese( string) progname= " << progname << std::endl;
    plot3( 0, 0, (RGB){ (WORT)0xf0, (WORT)0x00, (WORT)0xa0});
    // Zeichne ein Kreuz aus Diagonalen
    breseline( 0, 0       ,     hor, ver*9/10, (RGB){ (WORT)0x00, (WORT)0xff, (WORT)0xff});
    breseline( 0,    ver,       hor, ver*1/10, (RGB){ (WORT)0xff, (WORT)0xff, (WORT)0x00});
/*
    INTPAIR lu = (INTPAIR){      0, ver- 1};
    INTPAIR ro = (INTPAIR){ hor- 2,      0};
//  breseline( lu.x, lu.y, ro.x, ro.y, (RGB){ (WORT)0x00, (WORT)0xff, (WORT)0xff});
*/
    koperta_raster( true);
    druck_pnm( "/tmp/turtle/turtle-013-cerprobe_brese.pnm");
  }
};

class cerprobe_brese_mit_ctrue_type_font : public cbrese {
  public:
  cerprobe_brese_mit_ctrue_type_font() {
    std::cerr << "Z022 cerprobe_brese_mit_ctrue_type_font() progname= " << progname << std::endl << std::endl;
  }
  cerprobe_brese_mit_ctrue_type_font( string progname, string dateiname, int hor, int ver, int deep) : cbrese( hor, ver, deep, progname) {
    std::cerr << "Z058  cerprobe_brese_mit_ctrue_type_font( string) progname= " << progname << std::endl;

    set_text_font( "/usr/share/fonts/truetype/dejavu/DejaVuSerif.ttf");
//  set_text_pt( 100); set_text_dpi( 400); main32( U"Ägp|", 0, 1079 - 194, 0.0/*Grad*/);
    set_text_pt( 100); set_text_dpi( 550); main32( U"Ägp|" , 0, 1079 -   0, 0.0/*Grad*/);
    set_text_pt( 50); set_text_dpi( 100);
      set_textfarbe( magenta);
//  main32( wort,                                            xx,  yy,   ww);
    main32( U"erprobe_brese_DejaVuSerif.ttf 50, 550, +13", 50, 550,  +13);
    druck_pnm( "/tmp/turtle/turtle-011-cerprobe_brese_mit_ctrue_type_font-01.pnm");

    set_text_font( "/usr/share/fonts/truetype/ubuntu-font-family/UbuntuMono-R.ttf");
      set_textfarbe( gelb);
    main32( U"cerprobe_brese_UbuntuMono-R.ttf 50, 550, -13", 50, 50,  -13);
    druck_pnm( "/tmp/turtle/turtle-012-cerprobe_brese_mit_ctrue_type_font-02.pnm");

    // Zeichne ein Kreuz aus Diagonalen
    breseline( 0, 0     , hor, ver, tuerkis);
    breseline( 0, ver, hor, 0     , magenta);

    koperta_raster( true);
    druck_pnm( "/tmp/turtle/turtle-014-cerprobe_brese_mit_ctrue_type_font-03.pnm");
  }
};

class cerprobe_abbild : public cabbild {
  public:
  cerprobe_abbild( string progname, string dateiname, int hor, int ver, int deep) : cabbild ( hor, ver, deep, progname) {
    // Zeichenfläche in int Pixelkoordinaten
    breseline( 0, 0                , hor*9/10, ver*8/10, tuerkis);
    breseline( 0, ver*8/10, hor*9/10, 0                , magenta);

    // Zeichenfläche in double Weltkoordinaten
    DOUBLEPAIR igel_ort = { +2.0, +2.0};
    DOUBLEPAIR  alt_ort = { -2.0, -2.0};
    userworld( alt_ort.x, alt_ort.y, igel_ort.x, igel_ort.y);

    breseline( abbild( alt_ort), abbild( igel_ort), weisz);

    // Zeichenfläche
/*
    fprintf( stderr, "A010 %f %f %f %f %d\n",
               line__von_mitte.x  ,
               line__von_mitte.y  ,
               line_nach_mitte.x ,
               line_nach_mitte.y ,
               igel_farbe
               );
    fprintf( stderr, "A012 %d %d %d %d %d\n",
               abbildx( line__von_mitte.x),
               abbildy( line__von_mitte.y),
               abbildx( line_nach_mitte.x),
               abbildy( line_nach_mitte.y),
               igel_farbe
               );
*/
    // Mittelsenkrechte in double Weltkoordinaten
    DOUBLEPAIR line__von_mitte = { (igel_ort.x + alt_ort.x) / 2.0, -2.0};
    DOUBLEPAIR line_nach_mitte = { (igel_ort.x + alt_ort.x) / 2.0, +2.0};

    breseline(
        abbildx( line__von_mitte.x),
        abbildy( line__von_mitte.y),
        abbildx( line_nach_mitte.x),
        abbildy( line_nach_mitte.y),
        weisz
        );

    druck_pnm( "/tmp/turtle/turtle-015-cerprobe_abbild.pnm");
  }
};

class cerprobe_bildpnm : public cbildpnm {
  RGB farbe;
  public:
  cerprobe_bildpnm() { }
  cerprobe_bildpnm( string progname, string dateiname, int hor, int ver, int deep) : cbildpnm ( hor, ver, deep, progname) {
    std::cerr << "Z059  cerprobe_bildpnm( string) progname= " << progname << std::endl;
    for (WORT xx=0; xx<4; xx++) {
      for (WORT yy=0; yy<3; yy++) {
        WORT rot   = 42 * xx;    // 0xf0 + xx;    
        WORT gruen = 42 * yy;    // 0xe0 + yy;
        WORT blau  = 42 * xx+yy; // 0xc0 + xx+yy;
        plot3( xx, yy, (RGB){ rot, blau, gruen});
        }
      }
      druck_pnm( "/tmp/turtle/turtle-016-cerprobe_bildpnm.pnm");
    }
  };

#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>
#include <fcntl.h>
#include <linux/fb.h>
#include <sys/mman.h>
#include <sys/ioctl.h>

class czufall {
#define DEBUG
#undef  DEBUG
int result;
int inkrement;

long mal( long a, long b, long mod){
  /* a hoechstens 1 000 000 000,
     jedenfalls darf a*2 keinen long-overflow verursachen.
  */
  long p=0;
  /* ergebnis == p+a*b */
  while (b>0) {
    if (b%2) { 
      /* if odd b */	  
      p += a;
      p %= mod;
    }
    a *= 2;
    a %= mod;
    b /= 2;
  }
  return p;
}

int prim( int cand) {
	int div;
	for (div=3; cand % div && div*div<cand; div += 2)
		;
	return cand % div;
}

int next_prime( int start) {
  if (start==1) start++; 
  if (!(start%2))
    start++;
  for (; !prim(start); start +=2)
    ;
  return start;
}

int malschlecht( int z, int f, int modul) {
/* z < 2 ^ 24 */
	unsigned char *f1;
	int p = 0, zeiger;
	f1 = (unsigned char*)&f;
	p = (z*(*f1)%modul);
	for (zeiger = 0; zeiger < 3; zeiger ++) {
		f1 += 1;
		p = (p<<8) % modul;
		p += (z*(*f1)%modul);
		p %= modul;
#ifdef DEBUG
		fprintf( stderr, "f1=%d p=%d\n", *f1, p);
#endif //  DEBUG
	}
	return p;
}

int mpotenz( int x, int n, int modul) {
	/*
** Donald Knuth, The Art of Computer Programming, Vol. 2,
** Seminumerical Algorithms, Page 442
*/
	int N,Y,Z;
	N=n;
	Y=1;
	Z=x;
	while (N) {
		/*
**      x hoch n == Y * Z hoch N
*/
		if (N%2)
			Y = mal( Y, Z, modul);
		N /= 2;
		Z = mal( Z, Z, modul);
	}
	return Y;
}

int next() {
  if ( inkrement == 2 ) inkrement = 4;
  else                  inkrement = 2;

  switch (result) {
    case 0:  result = 2; break;
    case 2:  result = 3; break;
    case 3:  result = 5; break;
    default: result += inkrement; break;
  }
  return result;
}

int faktor( int *N) {
	/*
** Donald Knuth, The Art of Computer Programming, Vol. 2,
** Seminumerical Algorithms, Page 364
*/
	int n;
	int q, r;
	int dk=next();
	n = *N;
	while (n!=1) {
		do {
			q=n/dk;
			r=n%dk;
			if (!r)
				break;
			dk=next();
		} while (q>dk);
		if (r) {
			*N=1;
			return n;
		}
		while (!(n%dk)) n /= dk;
		*N=n;
		return dk;
	}
	return n;
	/*
**      n == N / p1 * p2 *...* pt und
**      n hat keine Primfaktoren kleiner als dk
*/
}

int primitiv( int a, int p) {
	int q, p_minus_1, pot;
	for (;;a++){
#ifdef DEBUG
	  fprintf( stderr, "a=%d ", a);
#endif //  DEBUG
		if (!(a%p))
			continue;
		//next = anfang;
		result = 0;
		p_minus_1 = p - 1;
		while (p_minus_1 > 1) {
#ifdef DEBUG
			fprintf( stderr, "p_minus_1=%d\n", p_minus_1);
#endif //  DEBUG
			q = faktor( &p_minus_1);
#ifdef DEBUG
			fprintf( stderr, "q=%d\n", q);
#endif //  DEBUG
			pot = mpotenz( a, (p-1)/q, p);
#ifdef DEBUG
			fprintf( stderr, "q=%d (p-1)/q=%d pot=%d\n", q, (p-1)/q, pot);
#endif //  DEBUG
			if ( pot == 1) break;
		}
		if ( pot != 1) return  a;
	}
	fprintf( stderr, "keine primitive > a=%d modulo p=%d\n", a, p);
}

public:
int eine_primitive( int *zahl) {
/*
** Liefert zwei Zahlen als Grundlage
** fuer einen Pseudozufallszahlengenerator,
** der pseudozufaellig alle Werte
** in einem ganzzahligen Intervall annimmt.
*/
	int primitive_zahl;
	*zahl = next_prime( *zahl);
	primitive_zahl = primitiv( 1, *zahl);
	return primitive_zahl;
}

};

class capfelmann : public cturtle {
  public:
    capfelmann( string progname, string dateiname, int hor, int ver, int deep) : cturtle ( hor, ver, deep, progname) {
      std::cerr << "Z060  progname= " << progname << std::endl;
    INTPAIR HORxVER = mach_framebuffer();
      // Zeichenfläche in int Pixelkoordinaten
      int tief = 160;
      streifen(
          HORxVER,
          (DOUBLEPAIR){-1.800, -0.016875},
          (DOUBLEPAIR){-1.740, 0.016875},
          tief
          );
      druck_pnm( "/tmp/turtle/turtle-017-capfelmann-streifen-01.pnm");
      double re_min=-2.0, re_max = 0.5;
      DOUBLEPAIR imaginaer = mandelbrotparameter( HORxVER, re_min, re_max);
      streifen(
          HORxVER,
          (DOUBLEPAIR){re_min, imaginaer.x},
          (DOUBLEPAIR){re_max, imaginaer.y},
        tief
        );
      druck_pnm( "/tmp/turtle/turtle-017-capfelmann-streifen-02.pnm");
      huepf(
          HORxVER,
          (DOUBLEPAIR){re_min, imaginaer.x},
          (DOUBLEPAIR){re_max, imaginaer.y},
          tief
          );
      druck_pnm( "/tmp/turtle/turtle-018-capfelmann-huepf.pnm");
    }
    
  DOUBLEPAIR mandelbrotparameter( INTPAIR HORxVER, double re_min, double re_max) {
/*  Nur für framebuffer relevant
    struct fb_var_screeninfo vvinfo = instanz_cplotter0->get_vinfo();
    if (HORxVER.y  > vvinfo.yres) HORxVER.y  = vvinfo.yres;
    if (HORxVER.x > vvinfo.xres) HORxVER.x = vvinfo.xres;
    fprintf( stderr, "M010 apfelmann vinfo.xres=%d vinfo.yres=%d\n", vvinfo.xres, vvinfo.yres);
*/
    //aspect ratio nach Bildschrmabmessung HORxVER.x HORxVER.y
    double delta_im = (re_max-re_min) * HORxVER.y / HORxVER.x;
    double im_max = delta_im/2.0;
    double im_min = im_max-delta_im;
    im_max+=0.0, im_min+=0.0 ;
    fprintf( stderr, "M014 apfelmann HORxVER.x=%d HORxVER.y=%d %4.2f %4.2f\n", HORxVER.x, HORxVER.y, im_min, im_max);
    return (DOUBLEPAIR){im_min, im_max};
  }


  int mandelfolge( double p, double q, int rechentiefe) {
#define UNENDLICH 4.0
    double    r_quadrat,i_quadrat,realteil,imaginaerteil,betrag_quadrat;
    int      k;
    r_quadrat =
      i_quadrat =
      realteil =
      imaginaerteil = 0;
    k = 0;
    do {
      imaginaerteil = 2*realteil*imaginaerteil+q;
      realteil = r_quadrat-i_quadrat+p;
      k += 1;
      r_quadrat = realteil*realteil;
      i_quadrat = imaginaerteil*imaginaerteil;
      betrag_quadrat = r_quadrat+i_quadrat;
    } while ((betrag_quadrat<=UNENDLICH) && (k<rechentiefe));
    return k;
  }

  RGB toRGB( int farbe) {
    WORT red, green, blue;
    switch (farbe/10%8) {
      case  0: red=0x0000; green=0x0000; blue=0x0000; break;
      case  1: red=0x0000; green=0x0000; blue=0xffff; break;
      case  2: red=0x0000; green=0xffff; blue=0x0000; break;
      case  3: red=0x0000; green=0xffff; blue=0xffff; break;
      case  4: red=0xffff; green=0x0000; blue=0x0000; break;
      case  5: red=0xffff; green=0x0000; blue=0xffff; break;
      case  6: red=0xffff; green=0xffff; blue=0x0000; break;
      case  7: red=0xffff; green=0xffff; blue=0xffff; break;
      default: red=0x7fff; green=0x7fff; blue=0x7fff; break;
    }
    return (RGB){ red, green, blue};
  }

  int streifen( INTPAIR punkte,
                DOUBLEPAIR min, DOUBLEPAIR max,
                int rechentiefe) {
    int xx, yy;
    double    stepx,stepy;
    clear_grafik( 0);

    stepx = (max.x-min.x)/punkte.x;
    stepy = (max.y-min.y)/punkte.y;
    for (xx = 0; xx < punkte.x; xx++)
      for (yy = 0; yy < punkte.y; yy++) {
        int farbe = mandelfolge( min.x+xx*stepx, min.y+yy*stepy, rechentiefe);
        plot3( xx, yy, toRGB( farbe));
//      plotf( xx, yy,        farbe );
      }
  }

  int huepf( INTPAIR punkte,
             DOUBLEPAIR links_unten, DOUBLEPAIR rechts_oben,
             int rechentiefe) {
    int basis;
    int prim, x_mal_y, zaehler;
    int xx, yy, farbe, ort = 1;
    double  stepx,stepy;
    clear_grafik( 0);
  
    short minx, maxx, miny, maxy;
    int minort, maxort;
    minx=32767, maxx=-32768, miny=32767, maxy=-32768;
    minort=2000000000, maxort=-2000000000;
  
    stepx = (rechts_oben.x-links_unten.x)/punkte.x;
    stepy = (rechts_oben.y-links_unten.y)/punkte.y;
    /*
    ** Die Periode des Dezimalbruchs 1/798713 ist 798712 Ziffern lang.
    ** basis = 10;
    ** x_mal_y = 798713;
    */
    prim = x_mal_y = punkte.x * punkte.y;
    fprintf( stderr, "M030 x_mal_y=%d ", x_mal_y);
    czufall ein_zufall;
    basis = ein_zufall.eine_primitive( &prim);
    fprintf( stderr, "M032 basis=%d prim=%d\n", basis, prim);
    for (zaehler=1; zaehler < prim; zaehler++) {
      //while ((ort = basis * ort % prim) > x_mal_y);
      ort = basis * ort % prim;
      if (ort <= x_mal_y) {
        //fprintf( stderr, "z=%d,o=%d ", zaehler, ort);
        xx = (ort-1) % punkte.x; 
        yy = (ort-1) / punkte.x;
        if (1) {
          if (xx<minx) minx=xx;
          if (yy<miny) miny=yy;
          if (xx>maxx) maxx=xx;
          if (yy>maxy) maxy=yy;
          if (ort<minort) minort=ort;
          if (ort>maxort) maxort=ort;
        }
#ifdef DEBUG
        fprintf( stderr,"M034 x=%5d y=%5d\n", xx, yy);
#endif //  DEBUG
        if ( (farbe = mandelfolge( links_unten.x+xx*stepx, links_unten.y+yy*stepy, rechentiefe)) >=0) {
          plotf( xx, yy, farbe);
          plot3( xx, yy, toRGB( farbe));
        }
      }
    }
    if (true) {
      fprintf( stderr, "M036 minort=%d, maxort=%d, minx=%d maxx=%d miny=%d maxy=%d\n",
               minort, maxort, minx, maxx, miny, maxy);
    }
  }

};

/*
#!/usr/bin/perl
#https://perlmaven.com/replace-character-by-character
use strict;
use warnings;
use 5.010;
my %map;
my $text;
%map = (
    'X' => '-YF+XFX+FY-',
    'Y' => '+XF-YFY-FX+',
);
$text = 'X';
say $text;
$text =~ s/([XY])/$map{$1}/g;
say $text;
$text =~ s/([XY])/$map{$1}/g;
say $text;
$text =~ s/([XY])/$map{$1}/g;
say $text;
*/
class clindenmayer : public cturtle {
  // F -> F-H und H -> F+H
  // http://paulbourke.net/fractals/lsys/
  public:
    clindenmayer( int tiefe) { }
    clindenmayer( string progname, string dateiname, int hor, int ver, int deep, int tiefe) : cturtle ( hor, ver, deep, progname) {
      std::cerr << "Z061  progname= " << progname << std::endl;
      initturtle(    0.0,    0.0, 192.0, 108.0);
      pencolor( weisz);
      penstyle( stiftart::AN);
//    koperta_raster();
      koperta_welt();
    penstyle( stiftart::AN); move( -33.0); turn( -48.0); move( 13.0); turn(  24.0);
    gerundet = true;
      zeichne( dragon( tiefe) , 10.0);
    penstyle( stiftart::AN); moveto((DOUBLEPAIR){ 96.0, 54.0}); // move( -33.0); turn( -48.0); move( 13.0); turn(  24.0);
    gerundet = false;
      string gen;
      gen = "-YF+XFX+FY-"; // gut
      string geny( "+XF-YFY-FX+"); // Y -> "+XF-YFY-FX+" gut
      string genx( "-YF+XFX+FY-"); // X -> "-YF+XFX+FY-" gut
      gen = "-+XF-YFY-FX+F+-YF+XFX+FY-F-YF+XFX+FY-+F+XF-YFY-FX+-";
      gen = "-+-YF+XFX+FY-F-+XF-YFY-FX+F+XF-YFY-FX+-F-YF+XFX+FY-+F+-+XF-YFY-FX+F+-YF+XFX+FY-F-YF+XFX+FY-+F+XF-YFY-FX+-F-+XF-YFY-FX+F+-YF+XFX+FY-F-YF+XFX+FY-+F+XF-YFY-FX+-+F+-YF+XFX+FY-F-+XF-YFY-FX+F+XF-YFY-FX+-F-YF+XFX+FY-+-";
      gen = generator_hilbert( tiefe);

      zeichne( gen, 2.0);
      closeturtle();
      set_textfarbe( magenta);
      drucke_einen_titel( U"Lindenmayer erzeugt rekursiv Hilbert & Dragon");
      druck_pnm( "/tmp/turtle/019-000-clindenmayer.pnm");
    }

string generator_hilbert( int tief) {
  string geny( "+XF-YFY-FX+"); // Y -> "+XF-YFY-FX+"
  string genx( "-YF+XFX+FY-"); // X -> "-YF+XFX+FY-"
  string axiom( "X");
  string gen;
  string erg;
  gen.append( axiom);
//cout << gen << endl;
  for (int ii=0; ii<tief; ii++) {
    for (int nn=0; nn<gen.size(); nn++) {
      switch (gen[nn]) {
        case 'X': erg.append( genx);      break;
        case 'Y': erg.append( geny);      break;
        default : erg.append( gen, nn, 1); break;
      }
    }
    gen = erg;
    erg = "";
//  cout << gen << endl;
  }
  return gen;
}

  string FF( int depth) {   // F -> F-H
    if (depth < 1) {
      return "F";
    } else {
      return FF(depth - 1) + "-" + HH(depth - 1);
    }
  }

  string HH( int depth) {   // H -> F+H
    if (depth < 1) {
      return "H";
    } else {
      return FF(depth - 1) + "+" + HH(depth - 1);
    }
  }

  string dragon( int depth) {
    string drache = FF( depth + 1);
    cerr << "H012 dragon= " << drache << " depth= " << depth << endl;
    return drache;
  }

  double li_re;
  bool gerundet;

  int kante( double breit) {
    if (gerundet) {
      turn(   -90.0*li_re); penstyle( stiftart::AUS); move( breit/3.0);
      turn(   135.0*li_re); penstyle( stiftart::AN); move( breit/3.0*1.41421356); 
      turn(   -45.0*li_re);               move( breit/3.0);
                            penstyle( stiftart::AUS); move( breit/3.0);
    } else {
      move( breit);
    }
  }

  int zeichne( string gen, double breit) {
    li_re =   1.0;
    int num_chars = gen.size();
    for ( int n = 0; n < num_chars; n++ ) {
      switch (gen[n]) {
        case 'F': kante( breit); break;
        case 'H': kante( breit); break;
        case '+': turn(   90.0); li_re = -1.0; break;
        case '-': turn(  -90.0); li_re =  1.0; break;
        default : ; break;
      }
    }
//  cout << endl;
  }

};

class clindenmayer_2 : public cturtle {
  // F -> F-H und H -> F+H
  // http://paulbourke.net/fractals/lsys/
  public:
  double li_re;
  bool gerundet, zeige_gen;

struct chromosom;
    clindenmayer_2( string progname, string dateiname, int hor, int ver, int deep, int tiefe, bool alle_bilder) : cturtle ( hor, ver, deep, progname) {
      std::cerr << "Z062  progname= " << progname << std::endl;
      double skalierung = 1.0, breite = 1.0;
      initturtle(    0.0,    0.0, 192.0, 108.0);
      pencolor( weisz);
      penstyle( stiftart::AN);
//    koperta_raster();
      koperta_welt();
      string gen;
    DOUBLEPAIR mitte = (DOUBLEPAIR){  96.0, 54.0};
    turn(0.0);

    gerundet = false; zeige_gen = false;

//  chromosom snake-kolam;
    gerundet = false;
    show_limits();
    closeturtle();

    chromosom mein_chromosom; // = erzeuge_ein_chromosom();

/*
    mein_chromosom.clear();
    mein_chromosom.add( "axiom", );
    mein_chromosom.add( "X"    , );
    mein_chromosom.add( "Y"    , );
    mein_chromosom.add( "F"    , );
    mein_chromosom.add(            );
    tiefe =  ; turnto( 0.0); skalierung = 1.0;  breite = ; clear_grafik( 0); clear_text();
    zeichneC( , tiefe, mein_chromosom, skalierung, breite, "/tmp/turtle/019-001a.pnm", U" C 002a");

*/
if (alle_bilder) {
    mein_chromosom.clear();
    mein_chromosom.add( "axiom", "FX");
    mein_chromosom.add( "X"    , "X+YF+");
    mein_chromosom.add( "Y"    , "-FX-Y");
    mein_chromosom.add( 90.0       );
    tiefe = 7; turnto( 0.0); skalierung = 1.0  ;  breite =   2.85; clear_grafik( 0); clear_text();
    tiefe = 7; turnto( 0.0); skalierung = 0.5  ;  breite = 800.0  ; clear_grafik( 0); clear_text();
    tiefe = 5; turnto( 0.0); skalierung = 0.5  ;  breite = 200.0  ; clear_grafik( 0); clear_text();
    tiefe = 3; turnto( 0.0); skalierung = 0.5  ;  breite =  50.0  ; clear_grafik( 0); clear_text();
    tiefe = 2; turnto( 0.0); skalierung = 0.5  ;  breite =  25.0  ; clear_grafik( 0); clear_text();
    tiefe = 1; turnto( 0.0); skalierung = 0.5  ;  breite =  25.0  ; clear_grafik( 0); clear_text();
    tiefe = 0; turnto( 0.0); skalierung = 0.5  ;  breite =   6.25 ; clear_grafik( 0); clear_text();
    tiefe = 8; turnto( 0.0); skalierung = 0.5  ;  breite = 800.0  ; clear_grafik( 0); clear_text();
    tiefe = 8; turnto( 0.0); skalierung = 0.713;  breite = 100.0  ; clear_grafik( 0); clear_text();
    zeichneC( 48.0, 68.0, tiefe, mein_chromosom, skalierung, breite, "/tmp/turtle/019-001a.pnm", U"dragon C 001a");

}
if (alle_bilder) {
    mein_chromosom.clear();
    mein_chromosom.add( "axiom", "F");
    mein_chromosom.add( "F"    , "-F+F+F-F +-F+F+F-F +-F+F+F-F +-F+F+F-F +-F+F+F-F ");
    mein_chromosom.add( 72.0       );
    tiefe = 2; turnto( 0.0); skalierung = 1.0;  breite = 5.0; clear_grafik( 0); clear_text();
    zeichneC( 72.0, 48.0, tiefe, mein_chromosom, skalierung, breite, "/tmp/turtle/019-002a.pnm", U"fünfeckig C 002a");

    mein_chromosom.clear();
    mein_chromosom.add( "axiom", "F+F+F");
    mein_chromosom.add( "F"    , "F-G+H");
    mein_chromosom.add( "G"    , "F-G+H");
    mein_chromosom.add( "H"    , "F-G+H");
    mein_chromosom.add( 120.0      );
    tiefe = 6; turnto( 0.0); skalierung = 0.5;  breite = 148.0; clear_grafik( 0); clear_text();
    zeichneC( 120.0,  75.0, tiefe, mein_chromosom, skalierung, breite, "/tmp/turtle/019-003a.pnm", U"Dreiecke C 003a");

    mein_chromosom.clear();
    mein_chromosom.add( "axiom", "F+XF+F+XF");
    mein_chromosom.add( "X"    , " (XF-F+F-XF+F+XF-F+F-X) ");
    mein_chromosom.add( 90.0       );
    tiefe = 4; turnto( 0.0); skalierung = 1.0;  breite =  1.76;  // echo 'a=1.76;f=0.5; a/(f^4)' | bc -l clear_grafik( 0); clear_text();
    tiefe = 5; turnto( 0.0); skalierung = 0.5;  breite = 28.16; clear_grafik( 0); clear_text();
    zeichneC( 96.0,  0.0, tiefe, mein_chromosom, skalierung, breite, "/tmp/turtle/019-004a.pnm", U"Wacław Sierpiński - Quadrate C 004a");

    mein_chromosom.clear();
    mein_chromosom.add( "axiom", "X");
    mein_chromosom.add( "X"    , " (XFYFX+F+YFXFY-F-XFYFX) ");
    mein_chromosom.add( "Y"    , "YFXFY-F-XFYFX+F+YFXFY");
    mein_chromosom.add( 90.0       );
    tiefe = 4; turnto( 0.0); skalierung = 1.0 ;  breite =  1.2; clear_grafik( 0); clear_text();
    tiefe = 4; turnto( 0.0); skalierung = 0.35;  breite = 80.0; clear_grafik( 0); clear_text();
    zeichneC( 48.0,  10.0, tiefe, mein_chromosom, skalierung, breite, "/tmp/turtle/019-005a.pnm", U"Peano-Kurve C 005a");

    mein_chromosom.clear();
    mein_chromosom.add( "axiom", "X+X+X+X+X+X+X+X");
    mein_chromosom.add( "X"    , " (X+YF++YF-FX--FXFX-YF+X) ");
    mein_chromosom.add( "Y"    , " (-FX+YFYF++YF+FX--FX-YF) ");
    mein_chromosom.add( 45.0       );
    tiefe = 3; turnto( 0.0); skalierung = 1.0;  breite =  0.35; // drei Varianten mit gleichem Ergebnis clear_grafik( 0); clear_text();
    tiefe = 3; turnto( 0.0); skalierung = 0.5;  breite =  2.80; clear_grafik( 0); clear_text();
    tiefe = 3; turnto( 0.0); skalierung = 0.3;  breite = 12.96; clear_grafik( 0); clear_text();
    zeichneC( 144.0,  20.0, tiefe, mein_chromosom, skalierung, breite, "/tmp/turtle/019-006a.pnm", U"Hasan Hosam Schneeflocke C 006a");

    mein_chromosom.clear();
    mein_chromosom.add( "axiom", "YF");
    mein_chromosom.add( "X"    , " (YF+XF+Y) ");
    mein_chromosom.add( "Y"    , " (XF-YF-X) ");
    mein_chromosom.add( 60.0       );

    tiefe = 1; turnto( 0.0); skalierung = 0.5;  breite = 100.0; clear_grafik( 0); clear_text();
    zeichneC( 8.0, 100.0, tiefe, mein_chromosom, skalierung, breite, "/tmp/turtle/019-007a-1.pnm", U"Wacław_Sierpiński Dreieck - Arrowhead - Pfeilspitze C 007a 1");
    clear_text(); tiefe = 3; turnto( 0.0);
    zeichneC( 8.0, 100.0, tiefe, mein_chromosom, skalierung, breite, "/tmp/turtle/019-007a-1-3.pnm", U"Wacław Sierpiński Dreieck - Arrowhead - Pfeilspitze C 007a 1 3");
    clear_text(); tiefe = 5; turnto( 0.0);
    zeichneC( 8.0, 100.0, tiefe, mein_chromosom, skalierung, breite, "/tmp/turtle/019-007a-1-3-5.pnm", U"Wacław Sierpiński Dreieck - Arrowhead - Pfeilspitze C 007a 1 3 5");
    clear_text(); tiefe = 7; turnto( 0.0);
    zeichneC( 8.0, 100.0, tiefe, mein_chromosom, skalierung, breite, "/tmp/turtle/019-007a-1-3-5-7.pnm", U"Wacław Sierpiński Dreieck - Arrowhead - Pfeilspitze C 007a 1 3 5 7");

    tiefe = 2; turnto( 180.0); skalierung = 0.5;  breite = 100.0; clear_grafik( 0); clear_text();
    zeichneC( 108.0, 100.0, tiefe, mein_chromosom, skalierung, breite, "/tmp/turtle/019-007a-2.pnm", U"Wacław Sierpiński Dreieck - Arrowhead - Pfeilspitze C 007a 2");
    tiefe = 4; turnto( 180.0);
    clear_text(); zeichneC( 108.0, 100.0, tiefe, mein_chromosom, skalierung, breite, "/tmp/turtle/019-007a-2-4.pnm", U"Wacław Sierpiński Dreieck - Arrowhead - Pfeilspitze C 007a 2 4");
    tiefe = 6; turnto( 180.0);
    clear_text(); zeichneC( 108.0, 100.0, tiefe, mein_chromosom, skalierung, breite, "/tmp/turtle/019-007a-2-4-6.pnm", U"Wacław Sierpiński Dreieck - Arrowhead - Pfeilspitze C 007a 2 4 6");
    tiefe = 8; turnto( 180.0);
    clear_text(); zeichneC( 108.0, 100.0, tiefe, mein_chromosom, skalierung, breite, "/tmp/turtle/019-007a-2-4-6-8.pnm", U"Wacław Sierpiński Dreieck - Arrowhead - Pfeilspitze C 007a 2 4 6 8");

    mein_chromosom.clear();
    mein_chromosom.add( "axiom", "F++F++F");
    mein_chromosom.add( "F"    , "F-F++F-F");
    mein_chromosom.add( 60.0       );
    tiefe = 4; turnto( 0.0); skalierung = 0.3;  breite = 120.0; clear_grafik( 0); clear_text();
    zeichneC( 48.0,  30.0, tiefe, mein_chromosom, skalierung, breite, "/tmp/turtle/019-008a.pnm", U"Wacław Sierpiński Dreieck - Arrowhead - Pfeilspitze C 008a");

}
if (alle_bilder) {
    mein_chromosom.clear();
    mein_chromosom.add( "axiom", "F+F+F+F");
    mein_chromosom.add( "F"    , " (FF+F++F+F)");
    mein_chromosom.add( 90.0       );
    tiefe = 7; turnto( 0.0); skalierung = 0.3;  breite = 180.0; clear_grafik( 0); clear_text();
    zeichneC( 64.0, 8.0, tiefe, mein_chromosom, skalierung, breite, "/tmp/turtle/019-009a.pnm", U"Paul Bourke - Crystal C 009a");

    mein_chromosom.clear();
    mein_chromosom.add( "axiom", "F");
    mein_chromosom.add( "F"    , "F-F+F+F-F");
    mein_chromosom.add( 90.0       );
    tiefe = 4; turnto( 0.0); skalierung = 0.33;  breite = 170.0; clear_grafik( 0); clear_text();
    zeichneC( 14.0, 90.0, tiefe, mein_chromosom, skalierung, breite, "/tmp/turtle/019-010a.pnm", U"Paul Bourke - Crystal C 10a");

    mein_chromosom.clear();
    mein_chromosom.add( "axiom", "FF");
    mein_chromosom.add( "F"    , "F+F-F-F+F");
    mein_chromosom.add( 90.0       );
    tiefe = 4; turnto( 0.0); skalierung = 0.33;  breite = 40.0; clear_grafik( 0); clear_text();
    zeichneC( 50.0, 10.0, tiefe, mein_chromosom, skalierung, breite, "/tmp/turtle/019-011-1.pnm", U"Hasan Hosam - Crystal C 11-1 FF");

    mein_chromosom.clear();
    mein_chromosom.add( "axiom", "FF+FF");
    mein_chromosom.add( "F"    , "F+F-F-F+F");
    mein_chromosom.add( 90.0       );
    tiefe = 4; turnto( 0.0); skalierung = 0.33;  breite = 40.0; clear_grafik( 0); clear_text();
    zeichneC( 50.0, 10.0, tiefe, mein_chromosom, skalierung, breite, "/tmp/turtle/019-011-2.pnm", U"Hasan Hosam - Crystal C 11-2 FF+FF");

    mein_chromosom.clear();
    mein_chromosom.add( "axiom", "FF+FF+F");
    mein_chromosom.add( "F"    , "F+F-F-F+F");
    mein_chromosom.add( 90.0       );
    tiefe = 4; turnto( 0.0); skalierung = 0.33;  breite = 40.0; clear_grafik( 0); clear_text();
    zeichneC( 50.0, 10.0, tiefe, mein_chromosom, skalierung, breite, "/tmp/turtle/019-011-3.pnm", U"Hasan Hosam - Crystal C 11-3 FF+FF+F");

    mein_chromosom.clear();
    mein_chromosom.add( "axiom", "F");
    mein_chromosom.add( "F"    , "F+F-F-F+F");
    mein_chromosom.add( 90.0       );
    tiefe = 4; turnto( 0.0); skalierung = 0.33;  breite = 40.0; clear_grafik( 0); clear_text();
    zeichneC( 50.0, 10.0, tiefe, mein_chromosom, skalierung, breite, "/tmp/turtle/019-011-4.pnm", U"Hasan Hosam - Crystal C 11-4 F");

    mein_chromosom.clear();
    mein_chromosom.add( "axiom", "FF+FF+FF+FF");
    mein_chromosom.add( "F"    , "F+F-F-F+F");
    mein_chromosom.add( 90.0       );
    tiefe = 4; turnto( 0.0); skalierung = 0.33;  breite = 40.0; clear_grafik( 0); clear_text();
    zeichneC( 50.0, 10.0, tiefe, mein_chromosom, skalierung, breite, "/tmp/turtle/019-011-5.pnm", U"Hasan Hosam - Crystal C 11-5 FF+FF+FF+FF");

    mein_chromosom.clear();
    mein_chromosom.add( "axiom", "F+F+F+F");
    mein_chromosom.add( "F"    , "F+F-F-FFF+F+F-F");
    mein_chromosom.add( 90.0       );
    tiefe = 3; turnto( 0.0); skalierung = 0.33;  breite = 20.0; clear_grafik( 0); clear_text();
    zeichneC( 65.0, 50.0, tiefe, mein_chromosom, skalierung, breite, "/tmp/turtle/019-012-a.pnm", U"Paul Bourke - Quadratic Koch Island C 12a");

}
    mein_chromosom.clear();
    mein_chromosom.add( "axiom", "FXXXXXXXX----F----");
    mein_chromosom.add( "X"    , "(----F----+F)");
    mein_chromosom.add( 45.0       );
    tiefe = 2; turnto( 0.0); skalierung = 1.0;  breite = 20.0; clear_grafik( 0); clear_text();
    zeichneC( 171.0, 86.0, tiefe, mein_chromosom, skalierung, breite, "/tmp/turtle/019-013-a.pnm", U"Sternchen^ C 13a");
    zeichneC(  21.0, 21.0, tiefe, mein_chromosom, skalierung, breite, "/tmp/turtle/019-013-a.pnm", U"Sternchen. C 13a");

if (alle_bilder) {
    mein_chromosom.clear();
    mein_chromosom.add( "axiom", "F+F+F+F");
    mein_chromosom.add( "F"    , "F-FF+FF+F+F-F-FF+F+F-F-FF-FF+F");
    mein_chromosom.add( 90.0       );
    tiefe = 2; turnto( 0.0); skalierung = 0.3;  breite = 16.0; clear_grafik( 0); clear_text();
    zeichneC( 75.0, 28.0, tiefe, mein_chromosom, skalierung, breite, "/tmp/turtle/019-014-a.pnm", U"Paul Bourke - Quadratic Koch Island C 14a");

    mein_chromosom.clear();
    mein_chromosom.add( "axiom", "X+X+X+X+X+X+X+X");
    mein_chromosom.add( "X"    , "X+YF++YF-FX--FXFX-YF+X");
    mein_chromosom.add( "Y"    , "-FX+YFYF++YF+FX--FX-YF");
    mein_chromosom.add( 90.0       );
    tiefe = 4; turnto( 0.0); skalierung = 0.35;  breite = 64.0; clear_grafik( 0); clear_text();
    zeichneC( 75.0, 87.0, tiefe, mein_chromosom, skalierung, breite, "/tmp/turtle/019-015-a.pnm", U"Hasan Hosam - Quadratic Koch Island C 15a");

    mein_chromosom.clear();
    mein_chromosom.add( "axiom", "F+F+F+F");
    mein_chromosom.add( "F"    , "FF+F+F+F+FF");
    mein_chromosom.add( 90.0           );
    tiefe = 5; turnto( 0.0); skalierung = 0.3;  breite = 100.0; clear_grafik( 0); clear_text();
    zeichneC( 85.0, 8.0, tiefe, mein_chromosom, skalierung, breite, "/tmp/turtle/019-016-a.pnm", U"Paul Bourke - Board C 16a");

    mein_chromosom.clear();
    mein_chromosom.add( "axiom", "F+F+F+F");
    mein_chromosom.add( "F"    , "F+FF++F+F");
    mein_chromosom.add( 90.0       );
    tiefe = 7; turnto( 0.0); skalierung = 0.5;  breite = 18.0; clear_grafik( 0); clear_text();
    zeichneC( 138.0, 78.0, tiefe, mein_chromosom, skalierung, breite, "/tmp/turtle/019-017-a.pnm", U"Paul Bourke - Cross C 17a");

    mein_chromosom.clear();
    mein_chromosom.add( "axiom", "F+F+F+F");
    mein_chromosom.add( "F"    , "F+F-F+F+F");
    mein_chromosom.add( 90.0       );
    tiefe = 4; turnto( 0.0); skalierung = 0.40;  breite = 100.0; clear_grafik( 0); clear_text();
    zeichneC( 66.0, 23.0, tiefe, mein_chromosom, skalierung, breite, "/tmp/turtle/019-018-a.pnm", U"Paul Bourke - Cross 2 C 18a");

    mein_chromosom.clear();
    mein_chromosom.add( "axiom", "F++F++F++F++F");
    mein_chromosom.add( "F"    , "F++F++F|F-F++F");
    mein_chromosom.add( 36.0       );
    tiefe = 4; turnto( 0.0); skalierung = 1.0;  breite = 1.2; clear_grafik( 0); clear_text();
    zeichneC( 68.0, 10.0, tiefe, mein_chromosom, skalierung, breite, "/tmp/turtle/019-019-a.pnm", U"Pentaplexity C 19a");

    gerundet = true;
    mein_chromosom.clear();
    mein_chromosom.add( "axiom", "FX");
    mein_chromosom.add( "X"    , "X+YF+");
    mein_chromosom.add( "Y"    , "-FX-Y");
    mein_chromosom.add( 90.0           );
    tiefe = 6; turnto( 0.0); skalierung = 1.0;  breite = 7.0; clear_grafik( 0); clear_text();
    zeichneC( 128.0, 75.0, tiefe, mein_chromosom, skalierung, breite, "/tmp/turtle/019-020-a.pnm", U"Dragon C 20a");
    gerundet = false;

    mein_chromosom.clear();
    mein_chromosom.add( "axiom", "F+F+F+F");
    mein_chromosom.add( "F"    , "FF+F-F+F+FF");
    mein_chromosom.add( 90.0           );
    tiefe = 4; turnto( 0.0); skalierung = 0.86;  breite = 3.0; clear_grafik( 0); clear_text();
    zeichneC( 80.0, 31.0, tiefe, mein_chromosom, skalierung, breite, "/tmp/turtle/019-022-a.pnm", U"Tiles C 22a");

    mein_chromosom.clear();
//  mein_chromosom.add( "axiom", "F++F++F++F++ GGG ++GG-- F++F++F++F++ GGG ++GG-- F++F++F++F++");
//  mein_chromosom.add( "F"    , "->F-<F--FF--F->F<-");
    mein_chromosom.add( "axiom", "X [fff ++ff-- X] [fff --ff++ X] [++ff ++fff-- X] [--ff --fff++ X] [--ffff X] [++ffff X] ");
    mein_chromosom.add( "X"    , "Y++Y++Y++Y++");
    mein_chromosom.add( "Y"    , "->F-<F--FF--F->F<-");
    mein_chromosom.add( 45.0           );
    mein_chromosom.add( "skafak", 1.4142     );
    tiefe = 2 ; turnto( 0.0); skalierung = 1.0;  breite = 21.0;  breite = 5.0; clear_grafik( 0); clear_text();
    zeichneC( 96.0, 54.0, tiefe, mein_chromosom, skalierung, breite, "/tmp/turtle/019-021-a.pnm", U"fünfeckige Berliner Gehwegplatte C 21a");

    mein_chromosom.clear();
    mein_chromosom.add( "axiom", "F++F++F++F++");
    mein_chromosom.add( "F"    , "->F-<F--FF--F->F<-");
    mein_chromosom.add( 45.0           );
    mein_chromosom.add( "skafak", 1.4142     );
    tiefe = 2 ; turnto( 0.0); skalierung = 1.0;  breite = 21.0;  breite = 5.0; clear_grafik( 0); clear_text();
    zeichneC( 96.0, 54.0, tiefe, mein_chromosom, skalierung, breite, "/tmp/turtle/019-021-b.pnm", U"fünfeckige Berliner Gehwegplatte C 21b");

    mein_chromosom.clear();
    mein_chromosom.add( "axiom", "a");
    mein_chromosom.add( "F"    , ">F<");
    mein_chromosom.add( "a"    , "F[+x]Fb");
    mein_chromosom.add( "b"    , "F[-y]Fa");
    mein_chromosom.add( "x"    , "a");
    mein_chromosom.add( "y"    , "b");
    mein_chromosom.add( 45.0       );
    mein_chromosom.add( "skafak", 1.36       );
    tiefe = 12 ; turnto( -90.0); skalierung = 0.90;  breite = 1.0; clear_grafik( 0); clear_text();
    zeichneC( 96.0, 100.0, tiefe, mein_chromosom, skalierung, breite, "/tmp/turtle/019-032-a.pnm", U"L-System Leaf C 32a");

    mein_chromosom.clear();
    mein_chromosom.add( "axiom", "F+F+F+F");
    mein_chromosom.add( "F"    , "FF+F+F+F+F+F-F");
    mein_chromosom.add( 90.0           );
    tiefe = 4; turnto(0.0 ); skalierung = 0.5;  breite = 10.0; clear_grafik( 0); clear_text();
    zeichneC( 130.0, 9.0, tiefe, mein_chromosom, skalierung, breite, "/tmp/turtle/019-023-a.pnm", U"Rings C 23a");

    mein_chromosom.clear();
    mein_chromosom.add( "axiom", "XF");
    mein_chromosom.add( "X"    , "X+YF++YF-FX--FXFX-YF+");
    mein_chromosom.add( "Y"    , "-FX+YFYF++YF+FX--FX-Y");
    mein_chromosom.add( 60.0           );
    tiefe = 4; turnto( 0.0); skalierung = 1.0;  breite = 1.7; clear_grafik( 0); clear_text();
    zeichneC( 100.0, 4.0, tiefe, mein_chromosom, skalierung, breite, "/tmp/turtle/019-024-a.pnm", U"Hexagonal Gosper C 24a");

    mein_chromosom.clear();
    mein_chromosom.add( "axiom", "F");
    mein_chromosom.add( "F"    , "-F++F-");
    mein_chromosom.add( 45.0           );
    tiefe = 10 ; turnto( 0.0); skalierung = 1.0;  breite = 2.0; clear_grafik( 0); clear_text();
    zeichneC( 60.0, 80.0, tiefe, mein_chromosom, skalierung, breite, "/tmp/turtle/019-025-a.pnm", U"Lévy curve C 25a");

    mein_chromosom.clear();
    mein_chromosom.add( "axiom", "F--XF--F--XF");
    mein_chromosom.add( "X"    , "XF+F+XF--F--XF+F+X");
    mein_chromosom.add( 45.0           );
    tiefe = 5 ; turnto( 0.0); skalierung = 1.0;  breite = 0.9; clear_grafik( 0); clear_text();
    zeichneC( 96.0, 100.0, tiefe, mein_chromosom, skalierung, breite, "/tmp/turtle/019-026-a.pnm", U"Classic Sierpinski Curve C 26a");

    mein_chromosom.clear();
    mein_chromosom.add( "axiom", "-X--X");
    mein_chromosom.add( "X"    , "XFX--XFX");
    mein_chromosom.add( 45.0               );
    tiefe =  6; turnto( 0.0); skalierung = 0.437;  breite = 171.0; clear_grafik( 0); clear_text();
    zeichneC( 96.0, 107.0, tiefe, mein_chromosom, skalierung, breite, "/tmp/turtle/019-027-a.pnm", U"Krishna Anklets C 27a");

    mein_chromosom.clear();
    mein_chromosom.add( "axiom", "Y---Y");
    mein_chromosom.add( "X"    , "{F-F}{F-F}--[--X]{F-F}{F-F}--{F-F}{F-F}--");
    mein_chromosom.add( "Y"    , "f-F+X+F-fY");
    mein_chromosom.add( 60.0               );
    tiefe = 11; turnto( 90.0); skalierung = 1.0;  clear_grafik( 0); clear_text();
    zeichneC( 96.0, 2.0, tiefe, mein_chromosom, skalierung, 3.0, "/tmp/turtle/019-028-a.pnm", U"Mango Leaf C 28a");

    mein_chromosom.clear();
    mein_chromosom.add( "axiom", "F+XF+F+XF");
    mein_chromosom.add( "X"    , "X{F-F-F}+XF+F+X{F-F-F}+X");
    mein_chromosom.add( 90.0               );
    tiefe = 4; turnto( 45.0); skalierung = 1.0;  clear_grafik( 0); clear_text();
    zeichneC( 96.0, 4.0, tiefe, mein_chromosom, skalierung, 2.0, "/tmp/turtle/019-029-a.pnm", U"Snake Kolam C 29a");

    mein_chromosom.add( "axiom", "(-D--D)");
    mein_chromosom.add( "A"    , "F++FFFF--F--FFFF++F++FFFF--F");
    mein_chromosom.add( "B"    , "F--FFFF++F++FFFF--F--FFFF++F");
    mein_chromosom.add( "C"    , "BFA--BFA");
    mein_chromosom.add( "D"    , "CFC--CFC");
    mein_chromosom.add( 45.0               );
    tiefe = 3; turnto( 0.0); skalierung = 1.0;  clear_grafik( 0); clear_text();
    zeichneC( 96.0, 107.0, tiefe, mein_chromosom, skalierung, 3.95, "/tmp/turtle/019-030-a.pnm", U"Kolam C 30a");

    mein_chromosom.clear();
    mein_chromosom.add( "axiom", "F");
    mein_chromosom.add( "F"    , "FF+[+F-F-F]-[-F+F+F]");
    mein_chromosom.add( 27.5);
    tiefe = 4; turnto( -95.0); skalierung = 0.5; breite = 30.0; clear_grafik( 0); clear_text();
    film_machen( false, "/data7/tmp/Bush-31a", "");
    zeichneC( 96.0, 107.0, tiefe, mein_chromosom, skalierung, breite, "/tmp/turtle/019-031-a.pnm", U"Bush 31a");
    film_beenden();

}
if (alle_bilder) {
    mein_chromosom.clear();
    mein_chromosom.add( "axiom", "Y");
    mein_chromosom.add( "X"    , "X[-FFF][+FFF]FX");
    mein_chromosom.add( "Y"    , "YFX[+Y][-Y]");
    mein_chromosom.add( 25.7       );
    tiefe = 6; turnto( -90.0); skalierung = 1.0;  breite = 0.8; clear_grafik( 0); clear_text();
    zeichneC( 96.0, 100, tiefe, mein_chromosom, skalierung, breite, "/tmp/turtle/019-032a.pnm", U"Bush C 032a");

    mein_chromosom.clear();
    mein_chromosom.add( "axiom", "F");
    mein_chromosom.add( "F"    , "F[+FF][-FF]F[-F][+F]F");
    mein_chromosom.add( 35.0       );
    tiefe = 5; turnto( -90.0); skalierung = 1.0;  breite = 0.44; clear_grafik( 0); clear_text();
    zeichneC( 96.0, 108.0, tiefe, mein_chromosom, skalierung, breite, "/tmp/turtle/019-033a.pnm", U"Bush C 033a");

    mein_chromosom.clear();
    mein_chromosom.add( "axiom", "VZFFF");
    mein_chromosom.add( "V"    , "[+++W][---W]YV");
    mein_chromosom.add( "W"    , "+X[-W]Z");
    mein_chromosom.add( "X"    , "-W[+X]Z");
    mein_chromosom.add( "Y"    , "YZ");
    mein_chromosom.add( "Z"    , "[-FFF][+FFF]F");
    mein_chromosom.add( 20.0       );
    tiefe = 8; turnto( -90.0); skalierung = 1.0;  breite = 4.5; clear_grafik( 0); clear_text();
    zeichneC( 96.0, 108.0, tiefe, mein_chromosom, skalierung, breite, "/tmp/turtle/019-034a.pnm", U"Saupe C 034a");

    mein_chromosom.clear();
    mein_chromosom.add( "axiom", "FX");
    mein_chromosom.add( "X"    , ">[-GX]+HX");
    mein_chromosom.add( "skafak", 1.0);
    mein_chromosom.add( "skafak", 0.9);
    mein_chromosom.add( 50.0       );
    tiefe = 3; turnto( -90.0); skalierung = 1.0;  breite = 5.00001; clear_grafik( 0); clear_text();
    zeichneC( 96.0, 54.0, tiefe, mein_chromosom, skalierung, breite, "/tmp/turtle/019-035a.pnm", U"Blumenkohl C 035a");

    mein_chromosom.clear();
    mein_chromosom.add( "axiom", "X");
    mein_chromosom.add( "F"    , "FF");
    mein_chromosom.add( "X"    , "F[+X]F[-X]+X");
    mein_chromosom.add( 20.0       );
    tiefe = 7; turnto( -90.0); skalierung = 1.0;  breite = 0.4; clear_grafik( 0); clear_text();
    zeichneC( 96.0, 108.0, tiefe, mein_chromosom, skalierung, breite, "/tmp/turtle/019-036a.pnm", U"Sticks C 036a");

    mein_chromosom.clear();
    mein_chromosom.add( "axiom", "aF");
    mein_chromosom.add( "a"    , "FFFFFv[+++h][---q]fb");
    mein_chromosom.add( "b"    , "FFFFFv[+++h][---q]fc");
    mein_chromosom.add( "c"    , "FFFFFv[+++fa]fd"     );
    mein_chromosom.add( "d"    , "FFFFFv[+++h][---q]fe");
    mein_chromosom.add( "e"    , "FFFFFv[+++h][---q]fg");
    mein_chromosom.add( "g"    , "FFFFFv[---fa]fa"     );
    mein_chromosom.add( "h"    , "ifFF"                );
    mein_chromosom.add( "i"    , "fFFF[--m]j"          );
    mein_chromosom.add( "j"    , "fFFF[--n]k"          );
    mein_chromosom.add( "k"    , "fFFF[--o]l"          );
    mein_chromosom.add( "l"    , "fFFF[--p]"           );
    mein_chromosom.add( "m"    , "fFn"                 );
    mein_chromosom.add( "n"    , "fFo"                 );
    mein_chromosom.add( "o"    , "fFp"                 );
    mein_chromosom.add( "p"    , "fF"                  );
    mein_chromosom.add( "q"    , "rfF"                 );
    mein_chromosom.add( "r"    , "fFFF[++m]s"          );
    mein_chromosom.add( "s"    , "fFFF[++n]t"          );
    mein_chromosom.add( "t"    , "fFFF[++o]u"          );
    mein_chromosom.add( "u"    , "fFFF[++p]"           );
    mein_chromosom.add( "v"    , "Fv"                  );
    mein_chromosom.add( 12.0       );
//  tiefe = 20; turnto( -90.0); skalierung = 1.0;  breite = 0.5; clear_grafik( 0); clear_text();
//  zeichneC( 96.0, 118.0, tiefe, mein_chromosom, skalierung, breite, "/tmp/turtle/019-037a.pnm", U"algae C 037a");
    tiefe = 20; turnto( 0.0); skalierung = 1.0;  breite = 0.45; clear_grafik( 0); clear_text();
    zeichneC( 0.0, 54.0, tiefe, mein_chromosom, skalierung, breite, "/tmp/turtle/019-037a.pnm", U"algae C 037a");

    mein_chromosom.clear();
    mein_chromosom.add( "axiom", "aF"                     );
    mein_chromosom.add( "a"    , "FFFFFy[++++n][----t]fb" );
    mein_chromosom.add( "b"    , "+FFFFFy[++++n][----t]fc");
    mein_chromosom.add( "c"    , "FFFFFy[++++n][----t]fd" );
    mein_chromosom.add( "d"    , "-FFFFFy[++++n][----t]fe");
    mein_chromosom.add( "e"    , "FFFFFy[++++n][----t]fg" );
    mein_chromosom.add( "g"    , "FFFFFy[+++fa]fh"        );
    mein_chromosom.add( "h"    , "FFFFFy[++++n][----t]fi" );
    mein_chromosom.add( "i"    , "+FFFFFy[++++n][----t]fj");
    mein_chromosom.add( "j"    , "FFFFFy[++++n][----t]fk" );
    mein_chromosom.add( "k"    , "-FFFFFy[++++n][----t]fl");
    mein_chromosom.add( "l"    , "FFFFFy[++++n][----t]fm" );
    mein_chromosom.add( "m"    , "FFFFFy[---fa]fa"        );
    mein_chromosom.add( "n"    , "ofFFF"                  );
    mein_chromosom.add( "o"    , "fFFFp"                  );
    mein_chromosom.add( "p"    , "fFFF[-s]q"              );
    mein_chromosom.add( "q"    , "fFFF[-s]r"              );
    mein_chromosom.add( "r"    , "fFFF[-s]"               );
    mein_chromosom.add( "s"    , "fFfF"                   );
    mein_chromosom.add( "t"    , "ufFFF"                  );
    mein_chromosom.add( "u"    , "fFFFv"                  );
    mein_chromosom.add( "v"    , "fFFF[+s]w"              );
    mein_chromosom.add( "w"    , "fFFF[+s]x"              );
    mein_chromosom.add( "x"    , "fFFF[+s]"               );
    mein_chromosom.add( "y"    , "Fy"                     );
    mein_chromosom.add(  12.0                             );
    tiefe = 24; turnto( 0.0); skalierung = 1.0;  breite = 0.45; clear_grafik( 0); clear_text();
    zeichneC( 0.0, 24.0, tiefe, mein_chromosom, skalierung, breite, "/tmp/turtle/019-038a.pnm", U"algae C 038a");

    mein_chromosom.clear();
    mein_chromosom.add( "axiom", "F");
    mein_chromosom.add( "X"    , "+FY");
    mein_chromosom.add( "Y"    , "-FX");
    mein_chromosom.add( "F"    , "FF-[XY]+[XY]");
    mein_chromosom.add( 22.5       );
    tiefe = 8; turnto( 0.0); skalierung = 1.0;  breite = 0.38; clear_grafik( 0); clear_text();
    zeichneC( 0.0, 54.0, tiefe, mein_chromosom, skalierung, breite, "/tmp/turtle/019-039a.pnm", U"Weed C 039a");

}
    mein_chromosom.clear();
    mein_chromosom.add( "axiom", "+WF--XF---YF--ZF");
    mein_chromosom.add( "W"    , "YF++ZF----XF[-YF----WF]++");
    mein_chromosom.add( "X"    , "+YF--ZF[---WF--XF]+");
    mein_chromosom.add( "Y"    , "-WF++XF[+++YF++ZF]-");
    mein_chromosom.add( "Z"    , "--YF++++WF[+ZF++++XF]--XF");
    mein_chromosom.add( "F"    , "");
    mein_chromosom.add( 36.0       );
    tiefe = 3; turnto( 0.0); skalierung = 1.0;  breite = 10.0; clear_grafik( 0); clear_text();
    zeichneC( 50.0, 50.0, tiefe, mein_chromosom, skalierung, breite, "/tmp/turtle/019-040a.pnm", U"Weitz C 040a");

if (alle_bilder) {
    zeige_gen = true;
    zeige_gen = false;
}

/*
    mein_chromosom.clear();
    mein_chromosom.add( "axiom", );
    mein_chromosom.add( "X"    , );
    mein_chromosom.add( "Y"    , );
    mein_chromosom.add( "F"    , );
    mein_chromosom.add(            );
    tiefe =  ; turnto( 0.0); skalierung = 1.0;  breite = ; clear_grafik( 0); clear_text();
    zeichneC( , tiefe, mein_chromosom, skalierung, breite, "/tmp/turtle/019-041a.pnm", U" C 041a");

*/

    }

/* ein chromosom besteht aus einem winkel, einem axiom und einigen regeln.
 * die regeln sind weisungen wie "D -> CFC--CFC"
 * jede regel hat als steuer das wort "axiom" oder einen Buchstaben sowie ein muster
*/
struct chromosom {
  struct sregel { char steuer; string muster;};
//vector<string> chromosom;
  double winkel;
  double length_scale_factor;
  sregel axiom;
  vector<sregel> regel;
  void clear() { length_scale_factor = 1.0; winkel = 0.0; axiom.steuer = ' '; axiom.muster = "";regel.clear();}
  void show() {
    cout
      << "Sk10 length_scale_factor= " << length_scale_factor
      << " winkel= " << winkel
      << " axiom.muster= " << axiom.muster
      << endl;
  }
  void add( double ein_winkel) { winkel = ein_winkel; }
  void add( string steuer, double ein_wert) {
    if (steuer == "skafak") length_scale_factor = ein_wert;
    if (steuer == "winkel") winkel              = ein_wert;
  }
  void add( string steuer, string eine_muster) {
    sregel ein_chrom;
    ein_chrom.steuer = steuer[0];
    ein_chrom.muster = eine_muster;
    if (steuer == "axiom")
      axiom = ein_chrom;
    else
      regel.push_back( ein_chrom);
  }

  template <class T>
  vector<sregel> func2( std::initializer_list<T> list ) {
    for( auto elem : list ) {
      regel.push_back( elem);
      std::cout << elem << std::endl ;
    }
  //  return liste;
  }
};

template <class T>
vector<string> func2( std::initializer_list<T> list ) {
  vector<string> liste;
      for( auto elem : list ) {
        liste.push_back( elem);
        std::cout << elem << std::endl ;
      }
      return liste;
}

  string generatorC( int tief, chromosom ein_chrom) {
    string gen;
    string erg;
    gen.append( ein_chrom.axiom.muster);
    // cout << "G010 " << tief << " " << gen << endl;
    for (int ii=0; ii<tief; ii++) {
      for (int nn=0; nn<gen.size(); nn++) {
        // gen[nn] enthält einen Buchstaben.
        // Anhand dieses Buchstabens muss eine regel augewählt werden.
        // Anhand dieses Buchstabens muss diejenige regel augewählt werden,
        // deren steuer gleich diesem Buchstaben ist.
/*
        switch (gen[nn]) {
          case 'X': erg.append( ein_chrom.regel[0].muster);      break;
          default : erg.append( gen, nn, 1); break;
        }
        for (int jj=0; jj<ein_chrom.regel.size(); jj++) {
          if (gen[nn] == ein_chrom.regel[jj].steuer) {
            erg.append( ein_chrom.regel[jj].muster);
          } else {
            erg.append( gen, nn, 1);
          }
        }
*/
        string ein_muster = " "; ein_muster[0] = gen[nn];
        for (int jj=0; jj<ein_chrom.regel.size(); jj++) {
          if (gen[nn] == ein_chrom.regel[jj].steuer) {
            ein_muster = ein_chrom.regel[jj].muster;
          }
        }
        erg.append( ein_muster);
      }
      gen = erg;
      erg = "";
      if (zeige_gen) cout << "G090 " << ii << " " << gen << endl;
    }
    return gen;
  }

void zeichneC( double startx, double starty, int tiefe, chromosom ein_cchrom, double skalierung, double breite, string zieldatei, u32string bildname) {
//gerundet = false;
  koperta_welt(); pencolor( weisz); penstyle( stiftart::AN);
  DOUBLEPAIR startpunkt = (DOUBLEPAIR){ startx, starty};
  double winkelretter = turtle_angle();
  RGB farbe = pencolor();
  pencolor ( gruen);
  penstyle( stiftart::AUS); // Kennzeichne Startpunkt
  moveto( startpunkt);
  pencolor ( farbe);
  penstyle( stiftart::AN); // Kennzeichne Startpunkt

  string gen = generatorC( tiefe, ein_cchrom);
  findAndReplaceAll( gen, "+-", "  ");
  double skalierte_breite = breite * pow( skalierung, tiefe);
//cout << "Z011 zeichne skalierte_breite= " << skalierte_breite << " zeichne ein_cchrom.length_scale_factor= " << ein_cchrom.length_scale_factor << endl;
  zeichne( gen, skalierte_breite, ein_cchrom.winkel, ein_cchrom.length_scale_factor);
  set_textfarbe( hell_tuerkis);
  drucke_einen_titel( bildname);

  farbe = pencolor();
  pencolor ( rot);
  penstyle( stiftart::AUS);  // Kennzeichne Endpunkt
  moveto( startpunkt);
  pencolor ( farbe);

  turnto( winkelretter);
  druck_pnm( zieldatei);
}

  int zeichne( string gen, double breit, double winkel, double length_scale_factor) {
//  cout << "Z012 zeichne breit= " << breit << " zeichne length_scale_factor= " << length_scale_factor << endl;
    struct zustand {
      double winkel;
      DOUBLEPAIR ort;
      void zeig_zustand() {
        cerr << "V010 winkel= " << winkel << " ort.x= " << ort.x << " ort.y= " << ort.y << endl;
      }
    };
    vector<zustand> state;
    stiftart retter;

    li_re =   1.0;
    li_re =  -1.0;
//  double length_scale_factor =1.41421356;
    zustand zuszus;
    int num_chars = gen.size();
    if (gerundet) cerr << "K012 zeichne gerundet li_re= " << li_re << " gen= " << gen << endl;
    for ( int n = 0; n < num_chars; n++ ) {
      switch (gen[n]) {
        case 'f': retter = penstyle(); penstyle( stiftart::AUS);
                  move( breit); penstyle( retter);break;
        case 'F': pencolor((RGB){ (WORT)0xfffe, (WORT)0xfdfc, (WORT)0xcbfa});kante( breit); break;
        case 'G': pencolor((RGB){ (WORT)0xfffe, (WORT)0x00fc, (WORT)0xcbfa});kante( breit); break;
        case 'H': pencolor((RGB){ (WORT)0x00fe, (WORT)0xfdfc, (WORT)0xfffa});kante( breit); break;

        case 'A': pencolor((RGB){ (WORT)0x00fe, (WORT)0xfdfc, (WORT)0xfffa});kante( breit); break;
        case 'B': pencolor((RGB){ (WORT)0x00fe, (WORT)0xfdfc, (WORT)0xfffa});kante( breit); break;
        case 'C': pencolor((RGB){ (WORT)0x00fe, (WORT)0xfdfc, (WORT)0xfffa});kante( breit); break;
        case 'D': pencolor((RGB){ (WORT)0x00fe, (WORT)0xfdfc, (WORT)0xfffa});kante( breit); break;

        case '+': turn(   winkel); li_re = -1.0; break;
        case '-': turn(  -winkel); li_re =  1.0; break;
        case '|': turn(    180.0);               break;
        case '>': breit *= length_scale_factor;         break;
        case '<': breit /= length_scale_factor;         break;
        case '[': zuszus = { turtle_angle(), turtle_ort()};
                  state.push_back(zuszus);
                  break;
        case ']': if ( !state.empty()) {
                    zustand letzter = state.back();
                    bool debug_zustand=false; if (debug_zustand) letzter.zeig_zustand();
                    turnto( letzter.winkel);
                    retter = penstyle(); penstyle( stiftart::AUS); moveto( letzter.ort); penstyle( retter);
                    state.pop_back();
                  }
                  break;
        default : break;
      }
//    cout << "K011 zeichne breit= " << breit << " zeichne length_scale_factor= " << length_scale_factor << endl;
    }
//  cout << endl;
  }

  int kante( double breit) {
    if (gerundet) {
//    cerr << "K011 kante li_re= " << li_re << endl;
      turn(   -90.0*li_re); penstyle( stiftart::AUS); pencolor(        weisz); move( breit/3.0);
      turn(   135.0*li_re); penstyle( stiftart::AN);  pencolor(      magenta); move( breit/3.0*1.41421356); 
      turn(   -45.0*li_re);                           pencolor(         gelb); move( breit/3.0);
                            penstyle( stiftart::AUS); pencolor( hell_tuerkis); move( breit/3.0);
      turn(   -90.0*li_re); penstyle( stiftart::AUS); pencolor(        weisz); move( breit/3.0);
    } else {
      move( breit);
    }
  }

void findAndReplaceAll(std::string & data, std::string toSearch, std::string replaceStr) {
  // Get the first occurrence
  size_t pos = data.find(toSearch);
 
  // Repeat till end is reached
  while( pos != std::string::npos) {
    // Replace this occurrence of Sub String
    data.replace(pos, toSearch.size(), replaceStr);
    // Get the next occurrence from the current position
    pos =data.find(toSearch, pos + replaceStr.size());
  }
}

};

int main (int argc, char *argv[]) {
  int opt, tief=4; double links=-2.0, rechts=1.0;
  pixeltypen::INTPAIR punkte = (pixeltypen::INTPAIR) {1920, 1080};
  while ((opt = getopt(argc, argv, "l:r:t:h:v:")) != -1) {
    switch (opt) {
    case 'l': links    = atof(optarg); break;
    case 'r': rechts   = atof(optarg); break;
    case 't': tief     = atoi(optarg); break;
    case 'h': punkte.x = atoi(optarg); break;
    case 'v': punkte.y = atoi(optarg); break;
    default: /* '?' */
      fprintf(stderr, "Usage: )%s [-t tief] [-l links] [-r rechts] name\n", argv[0]);
      exit( EXIT_FAILURE);
    }
  }

#define HANNO_PATH_MAX 1000
   char cwd[HANNO_PATH_MAX];
   if (getcwd(cwd, sizeof(cwd)) != NULL) {
     fprintf( stderr, "Z000 Erzeuger: %s/%s\n", cwd, argv[0]);
   } else {
     cwd[0] = '\0';
     perror("getcwd() error");
   }
  
  string pathname( cwd);
  string progname( argv[0]);
bool alle_proben = false;
if (alle_proben) {
  clindenmayer_2                       l2( "clindenmayer_2 " + pathname + "/" + progname, "", 1920, 1080, 65535, tief, alle_proben);
}
if (alle_proben) {
  clindenmayer                         l3( tief);
  clindenmayer                         l4( "clindenmayer "   + pathname + "/" + progname, "", 1920, 1080, 65535, 3);
  cerprobe_true_type_font            rufl( "erpr_ttf " + pathname + "/" + progname, "", 1280, 960, 65535);

  cerprobe_bildpnm                   rufa( "plott "    + pathname + "/" + progname, "",    4,    3,   255);
  cerprobe_brese_mit_ctrue_type_font rufB( "brese "    + pathname + "/" + progname, "", 1920, 1080, 65535);
  cerprobe_brese                     rufb( "brese "    + pathname + "/" + progname, "", 1920, 1080,   255);
  cerprobe_abbild                    rufc( "abbild "   + pathname + "/" + progname, "", 1920, 1080,   255);
  cerprobe_turtle                    rufd( "turtle "   + pathname + "/" + progname, "", 1920, 1080, 65535);
  ckoch_turtle                       rufe( "koch "     + pathname + "/" + progname, "", 1920, 1080, 65535);
  cpythagoras_turtle                 ruff( "pytha "    + pathname + "/" + progname, "", 1920, 1080, 65535);
}
if (alle_proben) {
  csierpinski_turtle                 rufg( "sierp "    + pathname + "/" + progname, "", 1920, 1080, 65535);
  csierpinski_ohne_turtle            rufh( "sierp2 "   + pathname + "/" + progname, "", 1600,  900, 65535);
}
  capfelmann                         rufi( "apfel "    + pathname + "/" + progname, "", 1920, 1080, 65535);
if (alle_proben) {
}
/*
  ctrue_type_font                    rufk( "ttf "      + pathname + "/" + progname, "", 1280, 960, 65535);
  cerprobe_bildpnm *rufp = new cerprobe_bildpnm( pathname + "/" + progname); 
*/
  cerr << "M999 Fertig" << endl << endl;
  return 0;
}
