// Composite.cpp
// Copyright (c) 2007 by Alastair M. Robinson
// A little program to composite a collection of images
// Scale down the result and save as JPEG.
// Suitable for adding copyright messages or site logos to images.

#include <iostream>
#include <getopt.h>
#include <stdio.h>
#include <stdlib.h>

#include "jpegsave.h"
#include "progresstext.h"
#include "layoutrectangle.h"
#include "imagesource_util.h"
#include "imagesource_montage.h"
#include "imagesource_promote.h"
#include "imagesource_rotate.h"

using namespace std;

struct
{
	int s,l;	// Short-edge, Long-edge dimensions.
	int q;		// Output quality factor.
	bool tl,tr,bl,br;
	int rotation;
	char *stampfile;
} myvars;


void ParseOptions(int argc,char *argv[])
{
	static struct option long_options[] =
	{
		{"help",no_argument,NULL,'h'},
		{"version",no_argument,NULL,'v'},
		{"shortedge",required_argument,NULL,'s'},
		{"longedge",required_argument,NULL,'l'},
		{"quality",required_argument,NULL,'q'},
		{"rotation",required_argument,NULL,'r'},
		{"stamp",required_argument,NULL,'t'},
		{0, 0, 0, 0}
	};

	while(1)
	{
		int c;
		c = getopt_long(argc,argv,"hvs:l:q:t:r:",long_options,NULL);
		if(c==-1)
			break;
		switch (c)
		{
			case 'h':
				throw 0;
				break;
			case 'v':
				cerr << "Composite 0.1 - test program from PhotoPrint" << endl;
				throw 0;
				break;
			case 'l':
				myvars.l=atoi(optarg);
				break;
			case 's':
				myvars.s=atoi(optarg);
				break;
			case 'q':
				myvars.q=atoi(optarg);
				break;
			case 't':
				myvars.stampfile=optarg;
				break;
			case 'r':
				myvars.rotation=atoi(optarg);
/*
				int r=atoi(optarg);
				if(r==90)
					myvars.rotation=PP_ROTATION_90;
				if(r==270)
					myvars.rotation=PP_ROTATION_270;
				if(r==180)
					myvars.rotation=PP_ROTATION_180;
*/
				break;
		}
	}
}


static char *buildfilename(char *root,char *suffix,char *fileext)
{
	/* Build a filename like <imagename>-<channel>.<extension> */
	char *extension;

	char *filename=(char *)malloc(strlen(root)+strlen(suffix)+4);

	root=strdup(root);
	extension = root + strlen (root) - 1;
	while (extension >= root)
	{
		if (*extension == '.') break;
		extension--;
	}
	if (extension >= root)
	{
		*(extension++) = '\0';
		sprintf(filename,"%s-%s.%s", root, suffix, fileext);
	}
	else
		sprintf(filename,"%s-%s", root, suffix);
	free(root);

	return(filename);
}


int main(int argc,char **argv)
{
	try
	{
		myvars.s=myvars.l=0;
		myvars.q=85;
		myvars.tl=myvars.tr=myvars.bl=myvars.br=false;
		myvars.stampfile=NULL;
		myvars.rotation=PP_ROTATION_NONE;

		ParseOptions(argc,argv);

		if(argc>optind && myvars.stampfile)
		{
			for(int i=optind;i<argc;++i)
			{
				try
				{
					int s,l;
					ImageSource *is=ISLoadImage(argv[i]);
					if((s=myvars.s))
					{
						if(is->width>is->height)
							is=ISScaleImageBySize(is,l=(is->width*myvars.s)/is->height,myvars.s,IS_SCALING_AUTOMATIC);
						else
							is=ISScaleImageBySize(is,myvars.s,l=(is->height*myvars.s)/is->width,IS_SCALING_AUTOMATIC);
					}
					else if((l=myvars.l))
					{
						if(is->width>is->height)
							is=ISScaleImageBySize(is,myvars.l,s=(is->height*myvars.l)/is->width,IS_SCALING_AUTOMATIC);
						else
							is=ISScaleImageBySize(is,s=(is->width*myvars.l)/is->height,myvars.l,IS_SCALING_AUTOMATIC);
					}
					else
					{
						if(is->width>is->height)
						{
							s=is->height;
							l=is->width;
						}
						else
						{
							s=is->width;
							l=is->height;
						}
					}
					if(myvars.rotation)
						is=new ImageSource_Rotate(is,myvars.rotation);
					if(STRIP_ALPHA(is->type)==IS_TYPE_GREY)
						is=new ImageSource_Promote(is,IS_TYPE_RGB);

					ImageSource_Montage *mon=new ImageSource_Montage(IS_TYPE_RGB,int(is->xres));

					cerr << "File: " << argv[i] << endl;
					ImageSource *is2=ISLoadImage(myvars.stampfile);
					is2=ISScaleImageBySize(is2,s,(s*is2->height)/is2->width,IS_SCALING_AUTOMATIC);
					if(STRIP_ALPHA(is2->type)==IS_TYPE_GREY)
						is2=new ImageSource_Promote(is2,IS_TYPE_RGB);
					mon->Add(is2,is->width-is2->width,is->height-is2->height);
					mon->Add(is,0,0);
					
					char *outfile=buildfilename(argv[i],"st","jpg");
					JPEGSaver ts(outfile,mon,myvars.q);
			        ProgressText p;
			        ts.SetProgress(&p);
			        ts.Save();
			        delete mon;
			        free(outfile);
				}
				catch(const char *err)
				{
					cerr << "Skipping file: " << err << endl;
				}
			}
		}
	}
	catch(const char *err)
	{
		cerr << "Error: " << err << endl;
	}
	return(0);
}
