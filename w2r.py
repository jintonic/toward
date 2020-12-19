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
Label(root, text="Select run folder:").grid(column=0, row=0, sticky='nw')
# https://stackoverflow.com/a/48992537/1801749
rlist=Listbox(root, height=8, selectbackground='orchid', exportselection=False)
rlist.grid(column=0, row=1)

from os import walk, system, path
for folder, subdirs, files in walk('.'):
    if '.git' in subdirs: subdirs.remove('.git')
    if 'share' in subdirs: subdirs.remove('share')
    if 'WaveDumpConfig.txt' in files:
        rlist.insert("end",folder)
        if rlist.size()%2: rlist.itemconfig("end", bg='azure')
rlist.selection_set(rlist.size()-1) # select the last run
rlist.see(rlist.size()-1) # scroll to the last run

# channel
Label(root, text="Select channel file:").grid(column=1, row=0, sticky='nw')
clist=Listbox(root, height=8, selectbackground='orchid', exportselection=False)
clist.grid(column=1,row=1)

# ROOT files
Label(root, text="Existing ROOT files:").grid(column=2, row=0, sticky='nw')
flist=Listbox(root, height=8)
flist.grid(column=2,row=1)

def show_file(event=None):
    if rlist.size()==0 or flist.size()==0: return
    response=system("python show.py "+rlist.get(rlist.curselection())+"&")
show=Button(root, text='Show', command=show_file)
show.grid(column=2,row=2,sticky='se');

Label(root, text="WaveDumpConfig.txt:").grid(column=0, row=2, sticky='sw')
text=Text(root, width=70, height=20);
text.grid(column=0, row=3, columnspan=3)

def run_selected(event=None):
    clist.delete(0,'end'); flist['state']='normal'; flist.delete(0,'end')
    run=rlist.get(rlist.curselection())

    text['state']='normal'; text.delete('1.0','end')
    f=rlist.get(rlist.curselection()[0])+'/WaveDumpConfig.txt'
    global thr, polarity, nbase, ssize, bits
    thr,polarity,nbase,ssize,bits=10,1,100,2,14
    with open(f,'r') as cfg:
        for line in cfg:
            if line=='': continue
            if line[0]=='#': continue
            part=line.split()
            if len(part)==0: continue
            if part[0]=='RECORD_LENGTH': nbase=int(int(part[1])*0.1)
            if part[0]=='TRIGGER_THRESHOLD': thr=part[1]
            if part[0]=='PULSE_POLARITY':
                if part[1].lower()=='negative': polarity=-1
            if part[0]=='Digitizer':
                if part[1]>'750': bits=10
                elif part[1]>='740': bits=12
                elif part[1]=='720': bints=12
                else:
                    if part[1]=='721' or part[1]=='731': byte=1
            text.insert(INSERT,line)
    text['state']='disabled'

    for ch in range(8):
        if path.exists(run+'/wave'+str(ch)+'.root'):
            flist.insert("end",'wave'+str(ch)+'.root')
            if flist.size()%2: flist.itemconfig("end", bg='azure')
        if path.exists(run+'/wave'+str(ch)+'.dat'):
            clist.insert("end",'wave'+str(ch)+'.dat')
            if clist.size()%2: clist.itemconfig("end", bg='azure')
    clist.selection_set(0)
    if flist.size()>0: show['state']='normal'
    else: show['state']='disabled'
    flist['state']='disabled'
run_selected()
rlist.bind("<<ListboxSelect>>", run_selected)

# button
def convert_file(event=None):
    run=rlist.get(rlist.curselection()[0])
    ch=clist.curselection()[0]
    cmd="""root -b -q w2r.C'("{}",{},{},{},{},{},{})'"""
    cmd.format(run,ch,thr,polarity,nbase,ssize,bits)
    system(cmd)
    run_selected()
convert=Button(root, text='Convert', command=convert_file)
convert.grid(column=1,row=2,sticky='se');
convert.bind('<Return>', convert_file)

# give focus to the GUI window in Mac
# https://stackoverflow.com/questions/17774859
from os import system
from platform import system as platform
if platform() == 'Darwin':  # How Mac OS X is identified by Python
    system('''/usr/bin/osascript -e 'tell app "Finder" to set frontmost of process "Python" to true' ''')

# If you put root.destroy() here, it will cause an error if the window is
# closed with the window manager.
root.mainloop()
