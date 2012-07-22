#ifndef _PPGnuPlot_
#define _PPGnuPlot_

#include <string>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <stdarg.h>


/** Maximal number of simultaneous temporary files */
#define GP_MAX_TMP_FILES    64
/** Maximal size of a temporary file name */
#define GP_TMP_NAME_SIZE    512


typedef struct _GNUPLOT_CTRL_ {
    /** Pipe to gnuplot process */
    FILE    * gnucmd ;

    /** Number of currently active plots */
    int       nplots ;
    /** Current plotting style */

    /** Name of temporary files */
    char      to_delete[GP_MAX_TMP_FILES][GP_TMP_NAME_SIZE] ;
    /** Number of temporary files */
    int       ntmp ;
} gnuplot_ctrl ;

/********************* base class for PPGnuPlot*************************/
class PPGnuPlot{
    public:
        PPGnuPlot();
        
        ~PPGnuPlot();

        void gnuplot_cmd(const char *  cmd, ...);

        void gnuplot_setstyle(std::string plot_style);

        void gnuplot_set_xlabel(const  char * label);

        void gnuplot_set_ylabel(const char * label);

        void gnuplot_resetplot();

        void gnuplot_plot_x(double * d, int n, char * title);

        void gnuplot_plot_xy(
                double * x,
                double * y,
                int n,
                char * title
                ) ;

        void gnuplot_plot_once(
                char    *   title,
                char    *   style,
                char    *   label_x,
                char    *   label_y,
                double  *   x,
                double  *   y,
                int         n
                );

        void gnuplot_plot_slope(
                double              a,
                double              b,
                char            *   title
                ) ;

        void gnuplot_plot_equation(char * equation, char * title) ;


    private:
        std::string pstyle;
        gnuplot_ctrl* my_handle;

        char * gnuplot_get_program_path(const char * pname);
        gnuplot_ctrl * gnuplot_init(void);
        void gnuplot_close(gnuplot_ctrl * handle);
};




#endif
