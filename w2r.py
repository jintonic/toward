#!/usr/bin/env python

usage='''
Convert binary output file from DAQ to ROOT files:
python3 w2r.py
'''

# main window
from tkinter import Tk, Label, Listbox, Button, RIGHT
window = Tk(); window.wm_title('WaveDump binary -> ROOT');

# listbox
Label(window, text="Select run:").pack()
lst = Listbox(window, height=10, selectbackground='orchid')
lst.focus(); lst.pack()

from os import walk, system
for folder, subdirs, files in walk('.'):
    if '.git' in subdirs: subdirs.remove('.git')
    if 'share' in subdirs: subdirs.remove('share')
    if 'WaveDumpConfig.txt' in files:
        lst.insert("end",folder)
        if lst.size()%2: lst.itemconfig("end", bg='azure')

lst.selection_set(lst.size()-1)
lst.see(lst.size()-1)

# functions and associated key bindings
def quit_gui(event=None):
    window.quit()    # stop the main loop
    window.destroy() # necessary on Windows to prevent fatal error

def convert_file(event=None):
    idx,=lst.curselection()
    run='"'+lst.get(idx)+'"'; ch=0
    response=system("root -b -q w2r.C'("+run+","+str(ch)+")'")

# button
but = Button(window, text='Convert', command=convert_file)
but.pack(side=RIGHT);

but.bind('<Return>', convert_file)
lst.bind('q', quit_gui)

# give focus to the GUI window in Mac
# https://stackoverflow.com/questions/17774859
from os import system
from platform import system as platform
if platform() == 'Darwin':  # How Mac OS X is identified by Python
    system('''/usr/bin/osascript -e 'tell app "Finder" to set frontmost of process "Python" to true' ''')

# If you put window.destroy() here, it will cause an error if the window is
# closed with the window manager.
window.mainloop()
