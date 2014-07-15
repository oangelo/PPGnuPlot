#ifndef _PPGnuPlot_
#define _PPGnuPlot_

#include <string>
#include <iostream>
#include <vector>

#include <cstdio>
#include <cstdlib>
#include <cstdarg>
#include <unistd.h>
#include <cstring>

class PPGnuPlot{
    
    public:
        PPGnuPlot();
        ~PPGnuPlot();

        template <class type> 
            void operator()(type data, std::string title = "");

        void SetStyle(std::string plot_style);
        void SetXLabel(std::string label);
        void SetYLabel(std::string label);
        void SetXRange(double min, double max);
        void SetYRange(double min, double max);
        void SetLine(double x1, double y1, double x2, double y2);
        void SetPointType(unsigned type);
        void Reset();
        template <class type> 
            void Single(const type & data, std::string title = "");
        template <class type> 
            void Pairs(const type & data, std::string title = "") ;
        void Equation(std::string equation, std::string title = "") ;
        void SavePNG(std::string file_name);
        void Wait();
        void Wait(double time);
        
    private:
        enum{GP_MAX_TMP_FILES = 64, GP_TMP_NAME_SIZE = 512, 
             GP_CMD_SIZE = 2048, GP_TITLE_SIZE = 80, 
             GP_EQ_SIZE = 512, PATH_MAXNAMESZ = 4096};

        //do not let the class to be copyed
        PPGnuPlot(const PPGnuPlot&);
        PPGnuPlot& operator=(const PPGnuPlot&);

        std::string pstyle;
         /** Pipe to gnuplot process */
        FILE    * gnucmd ;
        /** Number of currently active plots */
        int       nplots ;
        /** Name of temporary files */
        std::vector<std::string> to_delete;
        //Do not remove files until the object is done
        //this is needed for movies
        std::vector<std::string> to_delete_aux;
       
        std::string get_program_path(std::string pname);
        void init(void);
        void finish();
        void Command(std::string  cmd, ...);
};

template<class type> void PPGnuPlot::operator()(type data, std::string title){
  std::cerr<< "Operator not implemented for this type" << std::endl;
}

template<> void PPGnuPlot::operator()<std::vector<double>>(std::vector<double> data, std::string title){
  this->Single<std::vector<double>>(data, title);
}

template<> void PPGnuPlot::operator()<std::vector<std::vector<double>>>\
(std::vector<std::vector<double>> data, std::string title){
  this->Pairs<std::vector<std::vector<double>>>(data, title);
}

template<> void PPGnuPlot::operator()<std::vector<std::vector<unsigned>>>\
(std::vector<std::vector<unsigned>> data, std::string title){
  this->Pairs<std::vector<std::vector<unsigned>>>(data, title);
}

template<class type> inline void PPGnuPlot::Single(const type & data, std::string title)
{
    size_t i ;
	int		tmpfd ;
    char    name[128] ;
    char    cmd[GP_CMD_SIZE] ;
    char    line[GP_CMD_SIZE] ;


    /* Open one more temporary file? */
    if (to_delete.size() == GP_MAX_TMP_FILES - 1) {
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
    to_delete.push_back(name);
    
    /* Write data to this file  */
    for (i=0 ; i < data.size(); i++) {
		sprintf(line, "%g\n", data[i]);
		write(tmpfd, line, strlen(line));
    }
    close(tmpfd) ;

    /* Command to be sent to gnuplot    */
    if (nplots > 0) {
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
    nplots++ ;
    return ;
}

template <class type>
void PPGnuPlot::Pairs(
    const type & data,
    std::string title
)
{
	int		tmpfd;
    char    name[128];
    char    cmd[GP_CMD_SIZE];
    char    line[GP_CMD_SIZE];

	if (data.size() == 0) return ;

    /* Open one more temporary file? */
    if (to_delete.size() == GP_MAX_TMP_FILES - 1) {
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
    to_delete.push_back(name);

    /* Write data to this file  */
    for (size_t i = 0; i < data.size(); i++) {
        double x = data[i][0]; 
        double y = data[i][1];
        sprintf(line, "%g %g\n", x, y) ;
		write(tmpfd, line, strlen(line));
    }
    close(tmpfd) ;

    /* Command to be sent to gnuplot    */
    if (nplots > 0) {
        strcpy(cmd, "replot") ;
    } else {
        strcpy(cmd, "plot") ;
    }
    
    if (title == "") {
        sprintf(line, "%s \"%s\" with %s notitle", cmd, name, pstyle.c_str());
    } else {
        sprintf(line, "%s \"%s\" title \"%s\" with %s", cmd, name,
                      title.c_str(), pstyle.c_str()) ;
    }

    /* send command to gnuplot  */
    Command(line) ;
    nplots++ ;
    return ;
}

#endif
