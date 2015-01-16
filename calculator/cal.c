/**
 a simple calculator
**/
#include<gtk/gtk.h>
#include <stdlib.h>
#include <python2.7/Python.h>

gchar ch[1000000]= {0};
gchar ans[1000000];
gint digit=0;
FILE *fp;

GtkWidget *entry;

//signal handler
/* destroy */
void signal_handler_destroy(GtkObject *object,gpointer data){
    gtk_main_quit();
}
/* clicked */
void signal_handler_clicked(GtkButton *button,gpointer data){
    gtk_entry_set_max_length(GTK_ENTRY(entry),100);
    gtk_entry_append_text(GTK_ENTRY(entry),(gchar *)data);

    char c = ((gchar *)data)[0];
    if(c<58&&c>41) ch[digit++] = c;
}
/* clicked */
void signal_handler_clicked_clear(GtkButton *button,gpointer data){
    gtk_editable_set_editable(GTK_EDITABLE(entry),TRUE);
    gtk_entry_set_text(GTK_ENTRY(entry),"");
    digit=0;
}
/* clicked */
void signal_handler_clicked_leave(GtkButton *button,gpointer data){
    gtk_main_quit();
}
/* call python to calculate the answer */
void call_python_cal(){
    Py_Initialize();
    PyRun_SimpleString("import sys");
    PyRun_SimpleString("sys.path.append('./')");
    PyObject * pModule = NULL;
    pModule = PyImport_ImportModule("calpy");
    PyEval_CallObject(pModule, NULL);
    Py_Finalize();
}

void signal_handler_clicked_equation(GtkButton *button,gpointer data)
{


    gint i = 0;
    while(i<digit)  putchar(ch[i++]);
    putchar('\n');
    if(ch[0]=='*'||ch[0]=='/'||ch[0]=='.'||ch[0]=='?')
        gtk_entry_set_text(GTK_ENTRY(entry),"Eorr input!");
    else
    {
        fp = fopen("ab.txt", "w");
        i=0;
        while(ch[i]!='+'&&ch[i]!='-'&&ch[i]!='*'&&ch[i]!='/'){
            fprintf(fp,"%c",ch[i++]);
        }
        fprintf(fp,"%c",'\n');
        fprintf(fp,"%c",ch[i++]);
        fprintf(fp,"%c",'\n');
        while(i<digit)  fprintf(fp,"%c",ch[i++]);
        fprintf(fp,"%c",'\n');
        fclose(fp);

        call_python_cal();

        fp = fopen("Ans.txt", "r");
        fscanf(fp,"%s",ans);
        printf("= %s\n\n",ans);
        gtk_entry_set_text(GTK_ENTRY(entry),ans);
        fclose(fp);
    }
}


//main
void gtk_visual_design() {
    GtkWidget *window;
    GtkWidget *table;
    GtkWidget *button;
    GtkWidget *frame;


    /* make some widgets */
    window=gtk_window_new(GTK_WINDOW_TOPLEVEL);
    gtk_window_set_title(GTK_WINDOW(window),"A Simple Calculator");
    gtk_window_set_default_size(GTK_WINDOW(window),320,200);
    table=gtk_table_new(TRUE,5,5);
    entry=gtk_entry_new();
    frame=gtk_frame_new("Tips:   support big number   (million digits)");
    gtk_frame_set_shadow_type(GTK_FRAME(frame),GTK_SHADOW_IN);

    gtk_table_attach_defaults(GTK_TABLE(table),frame,0,5,0,1);
    gtk_container_add(GTK_CONTAINER(frame),entry);
    button=gtk_button_new_with_label("0");
    gtk_table_attach_defaults(GTK_TABLE(table),button,0,2,4,5);
    gtk_signal_connect(GTK_OBJECT(button),"clicked",GTK_SIGNAL_FUNC(signal_handler_clicked),"0");
    button=gtk_button_new_with_label("1");
    gtk_table_attach_defaults(GTK_TABLE(table),button,0,1,1,2);
    gtk_signal_connect(GTK_OBJECT(button),"clicked",GTK_SIGNAL_FUNC(signal_handler_clicked),"1");
    button=gtk_button_new_with_label("2");
    gtk_table_attach_defaults(GTK_TABLE(table),button,1,2,1,2);
    gtk_signal_connect(GTK_OBJECT(button),"clicked",GTK_SIGNAL_FUNC(signal_handler_clicked),"2");
    button=gtk_button_new_with_label("3");
    gtk_table_attach_defaults(GTK_TABLE(table),button,2,3,1,2);
    gtk_signal_connect(GTK_OBJECT(button),"clicked",GTK_SIGNAL_FUNC(signal_handler_clicked),"3");
    button=gtk_button_new_with_label("4");
    gtk_table_attach_defaults(GTK_TABLE(table),button,0,1,2,3);
    gtk_signal_connect(GTK_OBJECT(button),"clicked",GTK_SIGNAL_FUNC(signal_handler_clicked),"4");
    button=gtk_button_new_with_label("5");
    gtk_table_attach_defaults(GTK_TABLE(table),button,1,2,2,3);
    gtk_signal_connect(GTK_OBJECT(button),"clicked",GTK_SIGNAL_FUNC(signal_handler_clicked),"5");
    button=gtk_button_new_with_label("6");
    gtk_table_attach_defaults(GTK_TABLE(table),button,2,3,2,3);
    gtk_signal_connect(GTK_OBJECT(button),"clicked",GTK_SIGNAL_FUNC(signal_handler_clicked),"6");
    button=gtk_button_new_with_label("7");
    gtk_table_attach_defaults(GTK_TABLE(table),button,0,1,3,4);
    gtk_signal_connect(GTK_OBJECT(button),"clicked",GTK_SIGNAL_FUNC(signal_handler_clicked),"7");
    button=gtk_button_new_with_label("8");
    gtk_table_attach_defaults(GTK_TABLE(table),button,1,2,3,4);
    gtk_signal_connect(GTK_OBJECT(button),"clicked",GTK_SIGNAL_FUNC(signal_handler_clicked),"8");
    button=gtk_button_new_with_label("9");
    gtk_table_attach_defaults(GTK_TABLE(table),button,2,3,3,4);
    gtk_signal_connect(GTK_OBJECT(button),"clicked",GTK_SIGNAL_FUNC(signal_handler_clicked),"9");

    button=gtk_button_new_with_label("+");
    gtk_table_attach_defaults(GTK_TABLE(table),button,3,4,1,2);
    gtk_signal_connect(GTK_OBJECT(button),"clicked",GTK_SIGNAL_FUNC(signal_handler_clicked),"+");
    button=gtk_button_new_with_label("-");
    gtk_table_attach_defaults(GTK_TABLE(table),button,3,4,2,3);
    gtk_signal_connect(GTK_OBJECT(button),"clicked",GTK_SIGNAL_FUNC(signal_handler_clicked),"-");
    button=gtk_button_new_with_label("*");
    gtk_table_attach_defaults(GTK_TABLE(table),button,4,5,1,2);
    gtk_signal_connect(GTK_OBJECT(button),"clicked",GTK_SIGNAL_FUNC(signal_handler_clicked),"*");
    button=gtk_button_new_with_label("/");
    gtk_table_attach_defaults(GTK_TABLE(table),button,4,5,2,3);
    gtk_signal_connect(GTK_OBJECT(button),"clicked",GTK_SIGNAL_FUNC(signal_handler_clicked),"/");
    button=gtk_button_new_with_label("=");
    gtk_table_attach_defaults(GTK_TABLE(table),button,3,5,4,5);
//gtk_signal_connect(GTK_OBJECT(button),"clicked",GTK_SIGNAL_FUNC(signal_handler_clicked),"=");
    gtk_signal_connect(GTK_OBJECT(button),"clicked",GTK_SIGNAL_FUNC(signal_handler_clicked_equation),"=");

    button=gtk_button_new_with_label("C");
    gtk_table_attach_defaults(GTK_TABLE(table),button,3,5,3,4);
    gtk_signal_connect(GTK_OBJECT(button),"clicked",GTK_SIGNAL_FUNC(signal_handler_clicked_clear),NULL);
    button=gtk_button_new_with_label(".");
    gtk_table_attach_defaults(GTK_TABLE(table),button,2,3,4,5);
    gtk_signal_connect(GTK_OBJECT(button),"clicked",GTK_SIGNAL_FUNC(signal_handler_clicked),".");

    gtk_container_add(GTK_CONTAINER(window),table);

    gtk_widget_show_all(window);

    gtk_signal_connect(GTK_OBJECT(window),"destroy",GTK_SIGNAL_FUNC(signal_handler_destroy),NULL);
}

int main(gint argc,gchar *argv[])
{
    gtk_init(&argc,&argv);
    gtk_visual_design();
    gtk_main();
    g_print("\nNow leave the app!\n");
    return 0;
}
