#ifndef _PP
#define _PP

#include <string>
#include <iostream>
#include <vector>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <stdarg.h>




enum{GP_MAX_TMP_FILES = 64, GP_TMP_NAME_SIZE = 512, GP_CMD_SIZE = 2048, GP_TITLE_SIZE = 80, GP_EQ_SIZE = 512, PATH_MAXNAMESZ = 4096};
/** Define P_tmpdir if not defined (this is normally a POSIX symbol) */
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
        template <class type> void operator()(type data, std::string title);
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


#include "ppgnuplot.cpp"

#endif
