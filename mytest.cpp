#include "mg0.h"

class my_application : mg0::application {

protected:
    int Draw_Line(int x0, int y0, // starting position
                  int x1, int y1, // ending position
                  UCHAR color,    // color index
                  UCHAR *vb_start, int lpitch) // video buffer and memory pitch
    {
    // this function draws a line from xo,yo to x1,y1 using differential error
    // terms (based on Bresenahams work)

    int dx,             // difference in x's
        dy,             // difference in y's
        dx2,            // dx,dy * 2
        dy2,
        x_inc,          // amount in pixel space to move during drawing
        y_inc,          // amount in pixel space to move during drawing
        error,          // the discriminant i.e. error i.e. decision variable
        index;          // used for looping

    // pre-compute first pixel address in video buffer
    vb_start = vb_start + x0 + y0*lpitch;

    // compute horizontal and vertical deltas
    dx = x1-x0;
    dy = y1-y0;

    // test which direction the line is going in i.e. slope angle
    if (dx>=0)
       {
       x_inc = 1;

       } // end if line is moving right
    else
       {
       x_inc = -1;
       dx    = -dx;  // need absolute value

       } // end else moving left

    // test y component of slope

    if (dy>=0)
       {
       y_inc = lpitch;
       } // end if line is moving down
    else
       {
       y_inc = -lpitch;
       dy    = -dy;  // need absolute value

       } // end else moving up

    // compute (dx,dy) * 2
    dx2 = dx << 1;
    dy2 = dy << 1;

    // now based on which delta is greater we can draw the line
    if (dx > dy)
       {
       // initialize error term
       error = dy2 - dx;

       // draw the line
       for (index=0; index <= dx; index++)
           {
           // set the pixel
           *vb_start = color;

           // test if error has overflowed
           if (error >= 0)
              {
              error-=dx2;

              // move to next line
              vb_start+=y_inc;

    	   } // end if error overflowed

           // adjust the error term
           error+=dy2;

           // move to the next pixel
           vb_start+=x_inc;

           } // end for

       } // end if |slope| <= 1
    else
       {
       // initialize error term
       error = dx2 - dy;

       // draw the line
       for (index=0; index <= dy; index++)
           {
           // set the pixel
           *vb_start = color;

           // test if error overflowed
           if (error >= 0)
              {
              error-=dy2;

              // move to next line
              vb_start+=x_inc;

              } // end if error overflowed

           // adjust the error term
           error+=dx2;

           // move to the next pixel
           vb_start+=y_inc;

           } // end for

       } // end else |slope| > 1

    // return success
    return(1);

    } // end Draw_Line

    void runtime() {
        // lock primary buffer
        DDRAW_INIT_STRUCT(super.ddsd);

        if (FAILED(lpddsprimary->Lock(NULL, &super.ddsd,
                                      DDLOCK_WAIT | DDLOCK_SURFACEMEMORYPTR,
                                      NULL)))
            return(0);

        // draw 1000 random lines
        for (int index=0; index < 1000; index++)
        {
            Draw_Line(rand()%super.screen_width, rand()%super.screen_height,
                      rand()%super.screen_width, rand()%super.screen_height,
                      rand()%256,
                      (UCHAR *)super.ddsd.lpSurface, super.ddsd.lPitch);

        } // end for index

        // unlock primary buffer
        if (FAILED(super.lpddsprimary->Unlock(NULL)))
           return(0);

        // wait a sec
        Sleep(33);

    }
}

DECLARE_MAIN(my_application)
