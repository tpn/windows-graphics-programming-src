//-------------------------------------------------------------------------//
//          Windows Graphics Programming: Win32 GDI and DirectDraw         //
//                        ISBN  0-13-086985-6                              //
//                                                                         //
//  Modified by: Yuan, Feng                             www.fengyuan.com   //
//  Changes    : C++, exception, in-memory source, BGR byte order          //
//  Version    : 1.00.000, May 31, 2000                                    //
//-------------------------------------------------------------------------//

/* jconfig.vc --- jconfig.h for Microsoft Visual C++ on Windows 95 or NT. */
/* see jconfig.doc for explanations */

#define HAVE_UNSIGNED_CHAR
#define HAVE_UNSIGNED_SHORT
#undef CHAR_IS_UNSIGNED

/* Define "boolean" as unsigned char, not int, per Windows custom */
typedef unsigned char boolean;

#ifdef JPEG_INTERNALS

#undef RIGHT_SHIFT_IS_UNSIGNED

#endif /* JPEG_INTERNALS */

