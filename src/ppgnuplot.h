#ifndef _PPGnuPlot_
#define _PPGnuPlot_

#include <string>
#include <iostream>
#include <vector>

#include <cstdio>
#include <cstdlib>
#include <cstdarg>
#include <unistd.h>


class PPGnuPlot{
    
    public:
        PPGnuPlot();
        ~PPGnuPlot();

        template <class type> void operator()(type data, std::string title = "");

        void SetStyle(std::string plot_style);
        void SetXLabel(std::string label);
        void SetYLabel(std::string label);
        void ResetPlot();
        template <class type> void Single(type data, std::string title = "");
        template <class type> void Pairs(type data, std::string title = "") ;
        void Equation(std::string equation, std::string title = "") ;
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
       
        std::string get_program_path(std::string pname);
        void init(void);
        void finish();
        void Command(std::string  cmd, ...);
};

#include "ppgnuplot.cpp"

#endif
