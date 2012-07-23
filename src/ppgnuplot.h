#ifndef _PP
#define _PP

#include <string>
#include <iostream>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <stdarg.h>


/** Maximal number of simultaneous temporary files */
#define GP_MAX_TMP_FILES    64
/** Maximal size of a temporary file name */
#define GP_TMP_NAME_SIZE    512


typedef struct _CTRL_ {
    /** Pipe to gnuplot process */
    FILE    * gnucmd ;

    /** Number of currently active plots */
    int       nplots ;
    /** Current plotting style */

    /** Name of temporary files */
    char      to_delete[GP_MAX_TMP_FILES][GP_TMP_NAME_SIZE] ;
    /** Number of temporary files */
    int       ntmp ;
} ctrl ;

/********************* base class for PPGnuPlot*************************/
class PPGnuPlot{
    public:
        PPGnuPlot();
        ~PPGnuPlot();

        void setstyle(std::string plot_style);
        void set_xlabel(const  char * label);
        void set_ylabel(const char * label);
        void resetplot();
        void plot_x(double * d, int n, char * title);
        void plot_xy(
                double * x,
                double * y,
                int n,
                char * title
                ) ;
        void plot_once(
                char    *   title,
                char    *   style,
                char    *   label_x,
                char    *   label_y,
                double  *   x,
                double  *   y,
                int         n
                );
        void plot_slope(
                double              a,
                double              b,
                char            *   title
                ) ;
        void Equation(char * equation, char * title) ;
        void Wait();
    private:
        std::string pstyle;
        ctrl* my_handle;

        char * get_program_path(const char * pname);
        ctrl * init(void);
        void finish(ctrl * handle);
        void Command(const char *  cmd, ...);
};




#endif
