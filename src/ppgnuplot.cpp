#ifndef P_tmpdir
#define P_tmpdir "."


/*---------------------------------------------------------------------------
                                Defines
 ---------------------------------------------------------------------------*/
#include <string>
#include <iostream>
#include <vector>

#include <cstdio>
#include <cstdlib>
#include <cstdarg>
#include <unistd.h>


#include "ppgnuplot.h"

#endif

/*---------------------------------------------------------------------------
                            Function codes
 ---------------------------------------------------------------------------*/

PPGnuPlot::PPGnuPlot()
:pstyle("points"), gnucmd(), nplots(), to_delete(), to_delete_aux(){
    init();
}

PPGnuPlot::~PPGnuPlot(){
    finish();
}

/*-------------------------------------------------------------------------*/
/**
  @brief	Find out where a command lives in your PATH.
  @param	pname Name of the program to look for.
  @return	pointer to statically allocated character string.

  This is the C equivalent to the 'which' command in Unix. It parses
  out your PATH environment variable to find out where a command
  lives. The returned character string is statically allocated within
  this function, i.e. there is no need to free it. Beware that the
  contents of this string will change from one call to the next,
  though (as all static variables in a function).

  The input character string must be the name of a command without
  prefixing path of any kind, i.e. only the command name. The returned
  string is the path in which a command matching the same name was
  found.

  Examples (assuming there is a prog named 'hello' in the cwd):

  @verbatim
  get_program_path("hello") returns "."
  get_program_path("ls") returns "/bin"
  get_program_path("csh") returns "/usr/bin"
  get_program_path("/bin/ls") returns NULL
  @endverbatim
  
 */
/*-------------------------------------------------------------------------*/
std::string PPGnuPlot::get_program_path(std::string pname)
{
    int         i, j, lg;
    char    *   path;
    static char buf[PATH_MAXNAMESZ];

    /* Trivial case: try in CWD */
    sprintf(buf, "./%s", pname.c_str()) ;
    if (access(buf, X_OK)==0) {
        sprintf(buf, ".");
        return buf ;
    }
    /* Try out in all paths given in the PATH variable */
    buf[0] = 0;
    path = getenv("PATH") ;
    if (path!=NULL) {
        for (i=0; path[i]; ) {
            for (j=i ; (path[j]) && (path[j]!=':') ; j++);
            lg = j - i;
            strncpy(buf, path + i, lg);
            if (lg == 0) buf[lg++] = '.';
            buf[lg++] = '/';
            strcpy(buf + lg, pname.c_str());
            if (access(buf, X_OK) == 0) {
                /* Found it! */
                break ;
            }
            buf[0] = 0;
            i = j;
            if (path[i] == ':') i++ ;
        }
    } else {
		fprintf(stderr, "PATH variable not set\n");
	}
    /* If the buffer is still empty, the command was not found */
    if (buf[0] == 0) return NULL ;
    /* Otherwise truncate the command name to yield path only */
    lg = strlen(buf) - 1 ;
    while (buf[lg]!='/') {
        buf[lg]=0 ;
        lg -- ;
    }
    buf[lg] = 0;
    return buf ;
}

/*-------------------------------------------------------------------------*/
/**
  @brief	Opens up a gnuplot session, ready to receive commands.
  @return	Newly allocated gnuplot control structure.

  This opens up a new gnuplot session, ready for input. The struct
  controlling a gnuplot session should remain opaque and only be
  accessed through the provided functions.

  The session must be closed using finish().
 */
/*--------------------------------------------------------------------------*/

void PPGnuPlot::init(void){

    if (getenv("DISPLAY") == NULL) {
        fprintf(stderr, "cannot find DISPLAY variable: is it set?\n") ;
    }
	if (get_program_path("gnuplot") == "") {
		fprintf(stderr, "cannot find gnuplot in your PATH");
        //TODO trow exception
	}

    /* 
     * Structure initialization:
     */
    nplots = 0 ;
    //SetStyle("points");

    gnucmd = popen("gnuplot", "w") ;
    if (gnucmd == NULL) {
        fprintf(stderr, "error starting gnuplot\n") ;
        //TODO trow exceptio!!!
    }
}


/*-------------------------------------------------------------------------*/
/**
  @brief	Closes a gnuplot session previously opened by init()
  @param	handle Gnuplot session control handle.
  @return	void

  Kills the child PID and deletes all opened temporary files.
  It is mandatory to call this function to close the handle, otherwise
  temporary files are not cleaned and child process might survive.

 */
/*--------------------------------------------------------------------------*/

void PPGnuPlot::finish()
{
    if (pclose(gnucmd) == -1) {
        fprintf(stderr, "problem closing communication to gnuplot\n") ;
        return ;
    }
    for (auto iten: to_delete) {
        remove(iten.c_str()) ;
    }
    for (auto iten: to_delete_aux) {
        remove(iten.c_str()) ;
    }
}


/*-------------------------------------------------------------------------*/
/**
  @brief	Sends a command to an active gnuplot session.
  @param	handle Gnuplot session control handle
  @param	cmd    Command to send, same as a printf statement.

  This sends a string to an active gnuplot session, to be executed.
  There is strictly no way to know if the command has been
  successfully executed or not.
  The command syntax is the same as printf.

  Examples:

  @code
  Command(g, "plot %d*x", 23.0);
  Command(g, "plot %g * cos(%g * x)", 32.0, -3.0);
  @endcode

  Since the communication to the gnuplot process is run through
  a standard Unix pipe, it is only unidirectional. This means that
  it is not possible for this interface to query an error status
  back from gnuplot.
 */
/*--------------------------------------------------------------------------*/

void PPGnuPlot::Command(std::string  cmd, ...)
{   
    cmd += "\n";
    fputs(cmd.c_str(), gnucmd) ;
    fflush(gnucmd) ;
}


/*-------------------------------------------------------------------------*/
/**
  @brief	Change the plotting style of a gnuplot session.
  @param	h Gnuplot session control handle
  @param	plot_style Plotting-style to use (character string)
  @return	void

  The provided plotting style is a character string. It must be one of
  the following:

  - lines
  - points
  - linespoints
  - impulses
  - dots
  - steps
  - errorbars
  - boxes
  - boxeserrorbars
 */
/*--------------------------------------------------------------------------*/

void PPGnuPlot::SetStyle(std::string plot_style) 
{
    
    if ((plot_style != "lines") &&
        (plot_style != "points") &&
        (plot_style != "linespoints") &&
        (plot_style != "impulses") &&
        (plot_style != "dots") &&
        (plot_style != "steps") &&
        (plot_style != "errorbars") &&
        (plot_style != "boxes") &&
        (plot_style != "boxerrorbars")) {
        fprintf(stderr, "warning: unknown requested style: using points\n") ;
        pstyle = "points" ;
    } else {
        pstyle = plot_style;
    }
    
}

void PPGnuPlot::SetPointType(unsigned type)
{
        pstyle = "points pt "+std::to_string(type);
}

/*-------------------------------------------------------------------------*/
/**
  @brief	Sets the x label of a gnuplot session.
  @param	h Gnuplot session control handle.
  @param	label Character string to use for X label.
  @return	void

  Sets the x label for a gnuplot session.
 */
/*--------------------------------------------------------------------------*/

void PPGnuPlot::SetXLabel(std::string label)
{
    Command("set xlabel \"" + label + "\"");
}


/*-------------------------------------------------------------------------*/
/**
  @brief	Sets the y label of a gnuplot session.
  @param	h Gnuplot session control handle.
  @param	label Character string to use for Y label.
  @return	void

  Sets the y label for a gnuplot session.
 */
/*--------------------------------------------------------------------------*/

void PPGnuPlot::SetYLabel(std::string label)
{
    Command("set ylabel \"" + label + "\"");
}

void PPGnuPlot::SetXRange(double min, double max)
{
    std::string s_min(std::to_string(min));
    std::string s_max(std::to_string(max));
    Command("set xrange [" + s_min + ":" + s_max + "]");
}



void PPGnuPlot::SetYRange(double min, double max)
{
    std::string s_min(std::to_string(min));
    std::string s_max(std::to_string(max));
    Command("set yrange [" + s_min + ":" + s_max + "]");
}

void PPGnuPlot::SetLine(double x1, double y1, double x2, double y2)
{
    std::string sx1(std::to_string(x1));
    std::string sx2(std::to_string(x2));
    std::string sy1(std::to_string(y1));
    std::string sy2(std::to_string(y2));
    Command("set arrow nohead from " + sx1 + "," + sy1 + " to " + sx2 + "," + sy2);
}

void PPGnuPlot::SavePNG(std::string file_name){
    Command("set term png");
    Command("set output \"" + file_name + "\"");
}

/*-------------------------------------------------------------------------*/
/**
  @brief	Resets a gnuplot session (next plot will erase previous ones).
  @param	h Gnuplot session control handle.
  @return	void

  Resets a gnuplot session, i.e. the next plot will erase all previous
  ones.
 */
/*--------------------------------------------------------------------------*/

void PPGnuPlot::Reset()
{
    to_delete_aux.insert(to_delete_aux.end(), to_delete.begin(), to_delete.end());
    to_delete.erase(to_delete.begin(), to_delete.end());
    nplots = 0;
}

/*-------------------------------------------------------------------------*/
/**
  @brief	Plot a 2d graph from a list of points.
  @param	handle		Gnuplot session control handle.
  @param	x			Pointer to a list of x coordinates.
  @param	y			Pointer to a list of y coordinates.
  @param	n			Number of doubles in x (assumed the same as in y).
  @param	title		Title of the plot.
  @return	void

  Plots out a 2d graph from a list of points. Provide points through a list
  of x and a list of y coordinates. Both provided arrays are assumed to
  contain the same number of values.

  @code
    ctrl    *h ;
	double			x[50] ;
	double			y[50] ;
    int             i ;

    h = init() ;
    for (i=0 ; i<50 ; i++) {
        x[i] = (double)(i)/10.0 ;
        y[i] = x[i] * x[i] ;
    }
    plot_xy(h, x, y, 50, "parabola") ;
    sleep(2) ;
    finish(h) ;
  @endcode
 */
/*--------------------------------------------------------------------------*/

void PPGnuPlot::Equation(
    std::string equation,
    std::string title
)
{
  std::string plot;
    if (title == "") {
        title = " no title ";
    }else{
      title = " title \"" + title + "\"";
    } 

    if (nplots > 0) {
        plot = "replot ";
    } else {
        plot = "plot ";
    }

    Command(plot + equation + title + " with " + pstyle);
    std::cout << plot + equation + title + " with " + pstyle << std::endl; 
    nplots++ ;
}

void PPGnuPlot::Wait(){
    std::cerr << "Press Enter to continue...";
    char enter;
    do {
        std::cin.get(enter);
    } while ( enter != '\n' );
}

void PPGnuPlot::Wait(double time){
    std::cerr << "Let's wait " << time << " seconds..." << std::endl;
    usleep(1000*time);
}

