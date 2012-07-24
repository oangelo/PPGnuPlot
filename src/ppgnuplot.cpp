

/*-------------------------------------------------------------------------*/
/**
  @file		i.c
  @author	N. Devillard
  @date	Sep 1998
  @version	$Revision: 2.10 $
  @brief	C interface to gnuplot.

  gnuplot is a freely available, command-driven graphical display tool for
  Unix. It compiles and works quite well on a number of Unix flavours as
  well as other operating systems. The following module enables sending
  display requests to gnuplot through simple C calls.
  
*/
/*--------------------------------------------------------------------------*/

/*
	$Id: i.c,v 2.10 2003/01/27 08:58:04 ndevilla Exp $
	$Author: ndevilla $
	$Date: 2003/01/27 08:58:04 $
	$Revision: 2.10 $
 */

/*---------------------------------------------------------------------------
                                Includes
 ---------------------------------------------------------------------------*/

#include "ppgnuplot.h"
#ifndef P_tmpdir
#define P_tmpdir "."


/*---------------------------------------------------------------------------
                                Defines
 ---------------------------------------------------------------------------*/

#endif

/*---------------------------------------------------------------------------
                            Function codes
 ---------------------------------------------------------------------------*/

PPGnuPlot::PPGnuPlot(): pstyle("points"), my_handle(init()){
       
}

PPGnuPlot::~PPGnuPlot(){
    finish(my_handle);
}

template<class type> inline void PPGnuPlot::operator()(type data, std::string title)
{
    ctrl    *   handle = my_handle;
    size_t i ;
	int		tmpfd ;
    char    name[128] ;
    char    cmd[GP_CMD_SIZE] ;
    char    line[GP_CMD_SIZE] ;


	if (handle==NULL || (data.size() < 1)) return ;

    /* Open one more temporary file? */
    if (handle->ntmp == GP_MAX_TMP_FILES - 1) {
        fprintf(stderr,
                "maximum # of temporary files reached (%d): cannot open more",
                GP_MAX_TMP_FILES) ;
        return ;
    }

    /* Open temporary file for output   */
	sprintf(name, "%s/gnuplot-i-XXXXXX", P_tmpdir);
    if ((tmpfd = mkstemp(name)) == -1) {
        fprintf(stderr,"cannot create temporary file: exiting plot") ;
        return ;
    }

    /* Store file name in array for future deletion */
    strcpy(handle->to_delete[handle->ntmp], name) ;
    handle->ntmp ++ ;
    /* Write data to this file  */
    for (i=0 ; i < data.size(); i++) {
		sprintf(line, "%g\n", data[i]);
		write(tmpfd, line, strlen(line));
    }
    close(tmpfd) ;

    /* Command to be sent to gnuplot    */
    if (handle->nplots > 0) {
        strcpy(cmd, "replot") ;
    } else {
        strcpy(cmd, "plot") ;
    }
    
    if (title == "") {
        sprintf(line, "%s \"%s\" with %s", cmd, name, pstyle.c_str()) ;
    } else {
        sprintf(line, "%s \"%s\" title \"%s\" with %s", cmd, name,
                      title.c_str(), pstyle.c_str()) ;
    }

    /* send command to gnuplot  */
    Command(line) ;
    handle->nplots++ ;
    return ;
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
char * PPGnuPlot::get_program_path(const char * pname)
{
    int         i, j, lg;
    char    *   path;
    static char buf[PATH_MAXNAMESZ];

    /* Trivial case: try in CWD */
    sprintf(buf, "./%s", pname) ;
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
            strcpy(buf + lg, pname);
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

ctrl * PPGnuPlot::init(void){
    ctrl*  handle = NULL;

    if (getenv("DISPLAY") == NULL) {
        fprintf(stderr, "cannot find DISPLAY variable: is it set?\n") ;
    }
	if (get_program_path("gnuplot")==NULL) {
		fprintf(stderr, "cannot find gnuplot in your PATH");
		return NULL ;
	}

    /* 
     * Structure initialization:
     */
    handle = new ctrl;
    handle->nplots = 0 ;
    handle->ntmp = 0 ;
    //setstyle("points");

    handle->gnucmd = popen("gnuplot", "w") ;
    if (handle->gnucmd == NULL) {
        fprintf(stderr, "error starting gnuplot\n") ;
        free(handle) ;
        return NULL ;
    }
    return handle;
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

void PPGnuPlot::finish(ctrl * handle)
{
    int     i ;
	
    if (pclose(handle->gnucmd) == -1) {
        fprintf(stderr, "problem closing communication to gnuplot\n") ;
        return ;
    }
    if (handle->ntmp) {
        for (i=0 ; i<handle->ntmp ; i++) {
            remove(handle->to_delete[i]) ;
        }
    }
    delete handle;
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

void PPGnuPlot::Command(const char *  cmd, ...)
{   
    ctrl * handle = my_handle;
    va_list ap ;
    char    local_cmd[GP_CMD_SIZE];

    va_start(ap, cmd);
    vsprintf(local_cmd, cmd, ap);
    va_end(ap);

    strcat(local_cmd, "\n");

    fputs(local_cmd, handle->gnucmd) ;
    fflush(handle->gnucmd) ;
    return ;
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

void PPGnuPlot::setstyle(std::string plot_style) 
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


/*-------------------------------------------------------------------------*/
/**
  @brief	Sets the x label of a gnuplot session.
  @param	h Gnuplot session control handle.
  @param	label Character string to use for X label.
  @return	void

  Sets the x label for a gnuplot session.
 */
/*--------------------------------------------------------------------------*/

void PPGnuPlot::set_xlabel(const char * label)
{
    char    cmd[GP_CMD_SIZE] ;

    sprintf(cmd, "set xlabel \"%s\"", label) ;
    Command(cmd) ;
    return ;
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

void PPGnuPlot::set_ylabel(const char * label)
{
    char    cmd[GP_CMD_SIZE] ;

    sprintf(cmd, "set ylabel \"%s\"", label) ;
    Command(cmd) ;
    return ;
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

void PPGnuPlot::resetplot()
{
    ctrl * h = my_handle;
    int     i ;
    if (h->ntmp) {
        for (i=0 ; i<h->ntmp ; i++) {
            remove(h->to_delete[i]) ;
        }
    }
    h->ntmp = 0 ;
    h->nplots = 0 ;
    return ;
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

void PPGnuPlot::plot_xy(
	double			*	x,
	double			*	y,
    int                 n,
    char            *   title
)
{
    ctrl    *   handle = my_handle;
    int     i ;
	int		tmpfd ;
    char    name[128] ;
    char    cmd[GP_CMD_SIZE] ;
    char    line[GP_CMD_SIZE] ;

	if (handle==NULL || x==NULL || y==NULL || (n<1)) return ;

    /* Open one more temporary file? */
    if (handle->ntmp == GP_MAX_TMP_FILES - 1) {
        fprintf(stderr,
                "maximum # of temporary files reached (%d): cannot open more",
                GP_MAX_TMP_FILES) ;
        return ;
    }

    /* Open temporary file for output   */
	sprintf(name, "%s/gnuplot-i-XXXXXX", P_tmpdir);
    if ((tmpfd=mkstemp(name))==-1) {
        fprintf(stderr,"cannot create temporary file: exiting plot") ;
        return ;
    }
    /* Store file name in array for future deletion */
    strcpy(handle->to_delete[handle->ntmp], name) ;
    handle->ntmp ++ ;

    /* Write data to this file  */
    for (i=0 ; i<n; i++) {
        sprintf(line, "%g %g\n", x[i], y[i]) ;
		write(tmpfd, line, strlen(line));
    }
    close(tmpfd) ;

    /* Command to be sent to gnuplot    */
    if (handle->nplots > 0) {
        strcpy(cmd, "replot") ;
    } else {
        strcpy(cmd, "plot") ;
    }
    
    if (title == NULL) {
        sprintf(line, "%s \"%s\" with %s", cmd, name, pstyle.c_str());
    } else {
        sprintf(line, "%s \"%s\" title \"%s\" with %s", cmd, name,
                      title, pstyle.c_str()) ;
    }

    /* send command to gnuplot  */
    Command(line) ;
    handle->nplots++ ;
    return ;
}



/*-------------------------------------------------------------------------*/
/**
  @brief	Open a new session, plot a signal, close the session.
  @param	title	Plot title
  @param	style	Plot style
  @param	label_x	Label for X
  @param	label_y	Label for Y
  @param	x		Array of X coordinates
  @param	y		Array of Y coordinates (can be NULL)
  @param	n		Number of values in x and y.
  @return

  This function opens a new gnuplot session, plots the provided
  signal as an X or XY signal depending on a provided y, waits for
  a carriage return on stdin and closes the session.

  It is Ok to provide an empty title, empty style, or empty labels for
  X and Y. Defaults are provided in this case.
 */
/*--------------------------------------------------------------------------*/

void PPGnuPlot::plot_once(
	char	*	title,
	char	*	style,
	char	*	label_x,
	char	*	label_y,
	double	*	x,
	double	*	y,
	int			n
)
{
	ctrl	*	handle ;

	if (x==NULL || n<1) return ;

	if ((handle = init()) == NULL) return ;
	if (style!=NULL) {
		setstyle(style);
	} else {
		setstyle("lines");
	}
	if (label_x!=NULL) {
		set_xlabel(label_x);
	} else {
		set_xlabel("X");
	}
	if (label_y!=NULL) {
		set_ylabel(label_y);
	} else {
		set_ylabel("Y");
	}
	if (y==NULL) {
		//plot_x(x, n, title);
	} else {
		plot_xy(x, y, n, title);
	}
	printf("press ENTER to continue\n");
	while (getchar()!='\n') {}
	finish(handle);
	return ;
}




/*-------------------------------------------------------------------------*/
/**
  @brief	Plot a slope on a gnuplot session.
  @param	handle		Gnuplot session control handle.
  @param	a			Slope.
  @param	b			Intercept.
  @param	title		Title of the plot.
  @return	void

  Plot a slope on a gnuplot session. The provided slope has an
  equation of the form y=ax+b

  Example:

  @code
    ctrl    *   h ;
    double              a, b ;

    h = init() ;
    plot_slope(h, 1.0, 0.0, "unity slope") ;
    sleep(2) ;
    finish(h) ;
  @endcode
 */
/*--------------------------------------------------------------------------*/
    

void PPGnuPlot::plot_slope(
    double              a,
    double              b,
    char            *   title
)
{
    ctrl    *   handle = my_handle;
    char    stitle[GP_TITLE_SIZE] ;
    char    cmd[GP_CMD_SIZE] ;

    if (title == NULL) {
        strcpy(stitle, "no title") ;
    } else {
        strcpy(stitle, title) ;
    }

    if (handle->nplots > 0) {
        sprintf(cmd, "replot %g * x + %g title \"%s\" with %s",
                      a, b, title, pstyle.c_str()) ;
    } else {
        sprintf(cmd, "plot %g * x + %g title \"%s\" with %s",
                      a, b, title, pstyle.c_str()) ;
    }
    Command(cmd) ;
    handle->nplots++ ;
    return ;
}


/*-------------------------------------------------------------------------*/
/**
  @brief	Plot a curve of given equation y=f(x).
  @param	h			Gnuplot session control handle.
  @param	equation	Equation to plot.
  @param	title		Title of the plot.
  @return	void

  Plots out a curve of given equation. The general form of the
  equation is y=f(x), you only provide the f(x) side of the equation.

  Example:

  @code
        ctrl    *h ;
        char            eq[80] ;

        h = init() ;
        strcpy(eq, "sin(x) * cos(2*x)") ;
        plot_equation(h, eq, "sine wave", normal) ;
        finish(h) ;
  @endcode
 */
/*--------------------------------------------------------------------------*/

void PPGnuPlot::Equation(
    char            *   equation,
    char            *   title
)
{
    ctrl    *   h = my_handle;
    char    cmd[GP_CMD_SIZE];
    char    plot_str[GP_EQ_SIZE] ;
    char    title_str[GP_TITLE_SIZE] ;

    if (title == NULL) {
        strcpy(title_str, "no title") ;
    } else {
        strcpy(title_str, title) ;
    }
    if (h->nplots > 0) {
        strcpy(plot_str, "replot") ;
    } else {
        strcpy(plot_str, "plot") ;
    }

    sprintf(cmd, "%s %s title \"%s\" with %s", 
                  plot_str, equation, title_str, pstyle.c_str()) ;
    Command(cmd) ;
    h->nplots++ ;


}

void PPGnuPlot::Wait(){
    std::cout << "Press Enter to continue...";
    char enter;
    do {
        std::cin.get(enter);
    } while ( enter != '\n' );
}

