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

class PPGnuPlot{
    public:
        PPGnuPlot();
        ~PPGnuPlot();

        template <class type> void operator()(type data, std::string title = "");

        void SetStyle(std::string plot_style);
        void SetXLabel(std::string label);
        void SetYLabel(std::string label);
        void ResetPlot();
        void PlotXY(
                double * x,
                double * y,
                int n,
                char * title
                ) ;
        void PlotOnce(
                char    *   title,
                char    *   style,
                char    *   label_x,
                char    *   label_y,
                double  *   x,
                double  *   y,
                int         n
                );
        void PlotSlope(
                double              a,
                double              b,
                char            *   title
                ) ;
        void Equation(std::string equation, std::string title) ;
        void Wait();
    private:
        enum{GP_MAX_TMP_FILES = 64, GP_TMP_NAME_SIZE = 512, GP_CMD_SIZE = 2048, 
             GP_TITLE_SIZE = 80, GP_EQ_SIZE = 512, PATH_MAXNAMESZ = 4096};

        std::string pstyle;

         /** Pipe to gnuplot process */
        FILE    * gnucmd ;

        /** Number of currently active plots */
        int       nplots ;
        /** Current plotting style */

        /** Name of temporary files */
        char      to_delete[GP_MAX_TMP_FILES][GP_TMP_NAME_SIZE] ;
        /** Number of temporary files */
        int       ntmp ;
       
        char * get_program_path(const char * pname);
        void init(void);
        void finish();
        void Command(const char *  cmd, ...);
};


#include "ppgnuplot.cpp"

#endif
