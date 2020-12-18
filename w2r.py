#!/usr/bin/env python

usage='''
Convert binary output file from DAQ to ROOT files:
python3 w2r.py
'''

# main window
from tkinter import *
root=Tk(); root.wm_title('Convert WaveDump binary output to ROOT format')
def quit_gui(event=None):
    root.quit()    # stop the main loop
    root.destroy() # necessary on Windows to prevent fatal error
root.bind('q', quit_gui)

# run
Label(root, text="Select run:").grid(column=0,row=0, sticky='nw')
lst=Listbox(root, height=10, selectbackground='orchid')
lst.focus(); lst.grid(column=0,row=1)

from os import walk, system
for folder, subdirs, files in walk('.'):
    if '.git' in subdirs: subdirs.remove('.git')
    if 'share' in subdirs: subdirs.remove('share')
    if 'WaveDumpConfig.txt' in files:
        lst.insert("end",folder)
        if lst.size()%2: lst.itemconfig("end", bg='azure')
lst.selection_set(lst.size()-1) # select the last run
lst.see(lst.size()-1) # scroll to the last run

# channel
Label(root, text="Select channel:").grid(column=1, row=0, sticky='ne')
fcha=Frame(root); fcha.grid(column=1, row=1)
channel=StringVar()
for ch in range(0,8):
    b=Radiobutton(fcha, text=str(ch), variable=channel,value=str(ch))
    b.grid(column=ch,row=1)

# button
def convert_file(event=None):
    idx,=lst.curselection()
    run='"'+lst.get(idx)+'"'; ch=0
    response=system("root -b -q w2r.C'("+run+","+str(ch)+")'")
but = Button(root, text='Convert', command=convert_file)
but.grid(column=1,row=1,sticky='se');
but.bind('<Return>', convert_file)

# give focus to the GUI window in Mac
# https://stackoverflow.com/questions/17774859
from os import system
from platform import system as platform
if platform() == 'Darwin':  # How Mac OS X is identified by Python
    system('''/usr/bin/osascript -e 'tell app "Finder" to set frontmost of process "Python" to true' ''')

# If you put root.destroy() here, it will cause an error if the window is
# closed with the window manager.
root.mainloop()
