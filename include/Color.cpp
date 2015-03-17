//-----------------------------------------------------------------------------------//
//              Windows Graphics Programming: Win32 GDI and DirectDraw               //
//                             ISBN  0-13-086985-6                                   //
//                                                                                   //
//  Written            by  Yuan, Feng                             www.fengyuan.com   //
//  Copyright (c) 2000 by  Hewlett-Packard Company                www.hp.com         //
//  Published          by  Prentice Hall PTR, Prentice-Hall, Inc. www.phptr.com      //
//                                                                                   //
//  FileName   : color.cpp						                                     //
//  Description: Color space conversion                                              //
//  Version    : 1.00.000, May 31, 2000                                              //
//-----------------------------------------------------------------------------------//

#define STRICT
#define WIN32_LEAN_AND_MEAN

#include <windows.h>
#include <assert.h>
#include <tchar.h>

#include "Color.h"

// lightness  [0..1]
// saturation [0..1]
// hue		  [0..359)
void KColor::ToHLS(void)
{
	double mn, mx; 
	int	   major;
	
	if ( red < green )
	{
		mn = red; mx = green; major = Green;
	}
	else
	{
		mn = green; mx = red; major = Red;
	}

	if ( blue < mn )
		mn = blue;
	else if ( blue > mx )
	{
		mx = blue; major = Blue;
	}
	
    if ( mn==mx ) 
    {
		lightness    = mn/255;
		saturation   = 0;
		hue          = 0; 
    }   
    else 
    { 
	    lightness = (mn+mx) / 510;

		if ( lightness <= 0.5 )
			saturation = (mx-mn) / (mn+mx); 
		else
			saturation = (mx-mn) / (510-mn-mx);

		switch ( major )
		{
			case Red  : hue = (green-blue) * 60 / (mx-mn) + 360; break;
			case Green: hue = (blue-red) * 60  / (mx-mn) + 120;  break;
			case Blue : hue = (red-green) * 60 / (mx-mn) + 240;
		}

		if (hue >= 360)
			hue = hue - 360;
    }
}

unsigned char Value(double m1, double m2, double h)
{
	while (h >= 360) h -= 360;
	while (h <    0) h += 360;
 
	if (h < 60) 
		m1 = m1 + (m2 - m1) * h / 60;   
	else if (h < 180) 
		m1 = m2;
	else if (h < 240) 
		m1 = m1 + (m2 - m1) * (240 - h) / 60;      
                   
	return (unsigned char)(m1 * 255);
}


void KColor::ToRGB(void)
{
    if (saturation == 0)
    {
	    red = green = blue = (unsigned char) (lightness*255);
    }
    else
    {
		double m1, m2;
         
		if ( lightness <= 0.5 )
			m2 = lightness + lightness * saturation;  
		else                     
			m2 = lightness + saturation - lightness * saturation;
      
		m1 = 2 * lightness - m2;   
      
		red   = Value(m1, m2, hue + 120);   
		green = Value(m1, m2, hue);
		blue  = Value(m1, m2, hue - 120);
    }
}

