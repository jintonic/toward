#!/usr/bin/env python
usage='''
Convert WaveDump binary output file from DAQ to ROOT files:
python3 w2r.py
'''
from tkinter import *
root=Tk(); root.wm_title('Convert WaveDump binary output to ROOT files')
def quit_gui(event=None): root.quit(); root.destroy()
root.bind('q', quit_gui)

Label(root, text="Select run folder:").grid(column=0, row=0, sticky='nw')
rlist=Listbox(root, height=8,
# https://anzeljg.github.io/rin2/book2/2405/docs/tkinter/listbox.html
        selectbackground='orchid', selectforeground='white',
# https://stackoverflow.com/a/48992537/1801749
        exportselection=False)
rlist.grid(column=0, row=1); rlist.focus()

from os import walk, system, path
for folder, subdirs, files in walk('.'):
    if '.git' in subdirs: subdirs.remove('.git')
    if 'share' in subdirs: subdirs.remove('share')
    if 'WaveDumpConfig.txt' in files:
        rlist.insert("end",folder)
        if rlist.size()%2: rlist.itemconfig("end", bg='azure')
rlist.selection_set(rlist.size()-1) # select the last run
rlist.see(rlist.size()-1) # scroll to the last run

Label(root, text="Select channel file:").grid(column=1, row=0, sticky='nw')
clist=Listbox(root,height=8,exportselection=False,
        selectbackground='orchid',selectforeground='white')
clist.grid(column=1,row=1)

Label(root, text="Existing ROOT files:").grid(column=2, row=0, sticky='nw')
flist=Listbox(root, height=8)
flist.grid(column=2,row=1)

def call_show_py(event=None):
    if rlist.size()==0 or flist.size()==0: return
    system("python show.py "+rlist.get(rlist.curselection()[0])+"&")
show=Button(root, text='Show', command=call_show_py)
show.grid(column=2,row=2,sticky='se')
show.bind('<Return>', call_show_py)

def call_analyze_C(event=None):
    if rlist.size()==0 or flist.size()==0: return
    system("root analyze.C'(\""+rlist.get(rlist.curselection()[0])+"\")'")
ana=Button(root, text='Analyze', command=call_analyze_C)
ana.grid(column=2,row=2,sticky='sw')
ana.bind('<Return>', call_analyze_C)

Label(root, text="WaveDumpConfig.txt:").grid(column=0, row=2, sticky='sw')
text=Text(root, width=70, height=25)
text.grid(column=0, row=3, columnspan=3)

def run_selected(event=None):
    clist.delete(0,'end')
    flist['state']='normal'; flist.delete(0,'end')
    text['state']='normal'; text.delete(1.0,'end')

    # parse configuraiton file
    global thr, polarity, nbase, ssize, bits
    thr,polarity,nbase,ssize,bits=10,1,100,2,14
    run=rlist.get(rlist.curselection()[0])
    with open(run+'/WaveDumpConfig.txt','r') as cfg:
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
                elif part[1]=='720': bits=12
                else:
                    if part[1]=='721' or part[1]=='731': byte=1
            text.insert(INSERT,line)
    text['state']='disabled'

    # list binary and root files
    for ch in range(8):
        if path.exists(run+'/wave'+str(ch)+'.root'):
            flist.insert("end",'wave'+str(ch)+'.root')
        if path.exists(run+'/wave'+str(ch)+'.dat'):
            clist.insert("end",'wave'+str(ch)+'.dat')
            if clist.size()%2: clist.itemconfig("end", bg='azure')
    clist.selection_set(0)
    show['state']='normal' if flist.size()>0 else 'disabled'
    ana['state']='normal' if flist.size()>0 else 'disabled'
    flist['state']='disabled'
rlist.bind("<<ListboxSelect>>", run_selected)
run_selected()

def convert_file(event=None):
    run=rlist.get(rlist.curselection()[0]); ch=clist.curselection()[0]
    cmd="""root -b -q w2r.C'("{}",{},{},{},{},{},{})'""".format(
            run,ch,thr,polarity,nbase,ssize,bits)
    system(cmd)
    flist['state']='normal'; flist.delete(0,'end')
    for ch in range(8):
        if path.exists(run+'/wave'+str(ch)+'.root'):
            flist.insert("end",'wave'+str(ch)+'.root')
    show['state']='normal' if flist.size()>0 else 'disabled'
    ana['state']='normal' if flist.size()>0 else 'disabled'
    flist['state']='disabled'
convert=Button(root, text='Convert', command=convert_file)
convert.grid(column=1,row=2,sticky='se')
convert.bind('<Return>', convert_file)

# give focus to the GUI window in Mac
# https://stackoverflow.com/questions/17774859
from platform import system as platform
if platform() == 'Darwin':  # How Mac OS X is identified by Python
    system('''/usr/bin/osascript -e 'tell app "Finder" to set frontmost of process "Python" to true' ''')

# If you put root.destroy() here, it will cause an error if the window is
# closed with the window manager.
root.mainloop()
