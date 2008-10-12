#include <iostream>
#include <stdlib.h>
#include <string.h>

#include "imagesource_util.h"
#include "progresstext.h"
#include "jpegsave.h"


int main(int argc,char **argv)
{
	try
	{
		if(argc==3)
		{
			ImageSource *is=ISLoadImage(argv[1]);
//			is=new ImageSource_Rotate(is,180);
			JPEGSaver ts(argv[2],is);
			ProgressText p;
			ts.SetProgress(&p);
			ts.Save();
			delete is;
		}
	}
	catch(const char *err)
	{
		cerr << "Error: " << err << endl;
	}
	return(0);
}
