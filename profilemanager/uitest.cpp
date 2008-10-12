#include <iostream>

#include "profilemanager.h"

#include <gtk/gtk.h>
#include "profileselector.h"
#include "intentselector.h"
#include "patheditor.h"
#include "imageselector.h"

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "gettext.h"

using namespace std;


class Test
{
	public:
	Test() : pm(&f,"[Colour Management]")
	{}
	~Test() {}
	void Go()
	{
//		pm.ClearPaths();
		GtkWidget *window=gtk_window_new(GTK_WINDOW_TOPLEVEL);
		gtk_container_set_border_width(GTK_CONTAINER(window),8);
		gtk_window_set_title (GTK_WINDOW (window), "Profile Manager");
		gtk_signal_connect (GTK_OBJECT (window), "delete_event",
			(GtkSignalFunc) gtk_main_quit, NULL);
		gtk_window_set_default_size (GTK_WINDOW (window), 400,300);

		GtkWidget *vbox = gtk_vbox_new (FALSE, 0);
		gtk_container_add (GTK_CONTAINER (window), vbox);
		gtk_widget_show (vbox);
		
		ps = profileselector_new(&pm,IS_TYPE_RGB);
		g_signal_connect(ps,"changed",G_CALLBACK(profile_changed),this);
		gtk_box_pack_start(GTK_BOX(vbox),ps,FALSE,FALSE,8);
		gtk_widget_show(ps);

		is = intentselector_new(&pm);
		g_signal_connect(is,"changed",G_CALLBACK(intent_changed),this);
		gtk_box_pack_start(GTK_BOX(vbox),is,FALSE,FALSE,8);
		gtk_widget_show(is);

		pe = patheditor_new(&pm);
		g_signal_connect(pe,"changed",G_CALLBACK(paths_changed),this);
		gtk_box_pack_start(GTK_BOX(vbox),pe,TRUE,TRUE,8);
		gtk_widget_show(pe);

		gtk_widget_show(window);

		intentselector_setintent(INTENTSELECTOR(is),LCMSWRAPPER_INTENT_SATURATION);

		gtk_main();
	}
	private:
	ConfigFile f;
	ProfileManager pm;
	GtkWidget *ps;
	GtkWidget *is;
	GtkWidget *pe;

	static void	profile_changed(GtkWidget *widget,gpointer user_data)
	{
		cerr << "Received changed signal from ProfileSelector" << endl;
		Test *demo=(Test *)user_data;
		ProfileSelector *c=PROFILESELECTOR(demo->ps);
		const char *val=profileselector_get_filename(c);
		if(val)
			cerr << "Selected: " << val << endl;
		else
			cerr << "No profile selected... " << endl;
	}


	static void	intent_changed(GtkWidget *widget,gpointer user_data)
	{
		cerr << "Received changed signal from IntentSelector" << endl;
		Test *demo=(Test *)user_data;
		IntentSelector *is=INTENTSELECTOR(demo->is);

		int intent=intentselector_getintent(is);
		cerr << "Intent " << intent << ": " << demo->pm.GetIntentName(LCMSWrapper_Intent(intent)) << endl;
	}


	static void	paths_changed(GtkWidget *widget,gpointer user_data)
	{
		cerr << "Received changed signal from PathEditor" << endl;
		Test *demo=(Test *)user_data;
		patheditor_get_paths(PATHEDITOR(demo->pe),&demo->pm);
		cerr << "Updated path list" << endl;
		profileselector_refresh(PROFILESELECTOR(demo->ps));
		cerr << "Refreshed ProfileManager" << endl;
	}
};


int main(int argc, char **argv)
{
	gtk_init(&argc,&argv);

	gtk_set_locale();
	bindtextdomain(PACKAGE, LOCALEDIR);
	bind_textdomain_codeset(PACKAGE, "UTF-8");
	textdomain(PACKAGE);

	Test demo;
	
	demo.Go();
}

