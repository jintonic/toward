#!/usr/bin/env python3
usage='''
Convert CAEN DAQ binary output file to ROOT format:
python3 b2r.py
'''
from tkinter import *
root=Tk(); root.resizable(0,0)
root.wm_title('Convert CAEN DAQ binary output to ROOT format (press q to quit)')
def quit_gui(event=None): root.quit(); root.destroy()
root.bind('q', quit_gui)

Label(root, text="Select run folder:").grid(column=0, row=0, sticky='nw')
rlist=Listbox(root, height=8,
# https://anzeljg.github.io/rin2/book2/2405/docs/tkinter/listbox.html
        selectbackground='orchid', selectforeground='white',
# https://stackoverflow.com/a/48992537/1801749
        exportselection=False)
rlist.grid(column=0, row=1, sticky='ew'); rlist.focus()

folders=[] # obtain a list of folders containing DAQ cfg files
from os import walk, system, path, listdir
for folder, subdirs, files in walk('.'):
    if '.git' in subdirs: subdirs.remove('.git')
    if 'share' in subdirs: subdirs.remove('share')
    if 'WaveDumpConfig.txt' in files: folders.append(folder)
    if 'settings.xml' in files: folders.append(folder)
folders.sort()
for folder in folders:
    rlist.insert("end",folder)
    if rlist.size()%2: rlist.itemconfig("end", bg='azure', fg='black')
rlist.selection_set(rlist.size()-1) # select the last run
rlist.see(rlist.size()-1) # scroll to the last run

Label(root, text="Select binary file:").grid(column=1, row=0, sticky='nw')
clist=Listbox(root,height=8,exportselection=False,
        selectbackground='orchid',selectforeground='white')
clist.grid(column=1, row=1, sticky='ew')

Label(root, text="Existing ROOT files:").grid(column=2, row=0, sticky='nw')
flist=Listbox(root, height=8)
flist.grid(column=2, row=1, sticky='ew')

from subprocess import Popen
def call_show_py(event=None):
    if rlist.size()==0 or flist.size()==0: return
    run=rlist.get(rlist.curselection()[0]).replace('\\','/')
    for folder, subdirs, files in walk(run):
        if "RAW" in subdirs: run=run+"/RAW"
    Popen(['python3', 'show.py', run])
show=Button(root, text='Show', command=call_show_py)
show.grid(column=2, row=2, sticky='se')
show.bind('<Return>', call_show_py)
root.bind('s', call_show_py)

def call_analyze_C(event=None):
    if rlist.size()==0 or flist.size()==0: return
    run=rlist.get(rlist.curselection()[0]).replace('\\','/')
    for folder, subdirs, files in walk(run):
        if "RAW" in subdirs: run=run+"/RAW"
    Popen(['root', '-l', 'analyze.C("'+run+'")'])
ana=Button(root, text='Analyze', command=call_analyze_C)
ana.grid(column=2, row=2, sticky="sw")
ana.bind('<Return>', call_analyze_C)
root.bind('a', call_analyze_C)

Label(root, text="DAQ Configurations:").grid(column=0, row=2, sticky='sw')
text=Text(root, width=80, height=25)
text.grid(column=0, row=3, columnspan=3)

thr,polarity,nbase,ssize,bits=10,1,100,2,14
def parse_wavedump_cfg(run=''):
    global thr, polarity, nbase, ssize, bits
    with open(run+'/WaveDumpConfig.txt','r') as cfg:
        for line in cfg:
            if line=='': continue
            if line[0]=='#': continue
            part=line.split()
            if len(part)==0: continue
            if part[0]=='RECORD_LENGTH': nbase=int(int(part[1])*0.1)
            if part[0]=='Threshold': thr=part[1]
            if part[0]=='PULSE_POLARITY':
                polarity=1 if part[1].lower()=='positive' else -1
            if part[0]=='Digitizer':
                if part[1]>'750': bits=10
                elif part[1]>='740': bits=12
                elif part[1]=='720': bits=12
                else:
                    if part[1]=='721' or part[1]=='731': ssize=1
            text.insert(INSERT,line)

import xml.etree.ElementTree as ET
def parse_compass_cfg(run=''):
    tree = ET.parse(run+'/settings.xml'); cfg = tree.getroot()
    daq = cfg.find('board').find('modelName').text
    parameters = cfg.find('board').find('parameters')
    global thr, polarity, nbase, ssize, bits
    thr = parameters[8][1][0].text
    polarity = 1 if parameters[4][1][0].text == 'POLARITY_POSITIVE' else -1
    nbase = int(float(cfg.find('board').find('sampleTime').text)*0.1)
    bits = cfg.find('board').find('adcBitCount').text
    text.insert(INSERT,'digitizer: {}\nthreshold: {}\npolarity: {}'.format(
        daq,thr,polarity))

def list_files_in(run=''):
    clist.delete(0,'end'); flist.delete(0,'end')
    for folders, subdirs, files in walk(run):
        for file in files:
            if file[-4:]==".dat": clist.insert("end",file) # WaveDump output
            if file[-4:]==".bin": clist.insert("end",file) # CoMPASS output
            if file[-4:]=="root": flist.insert("end",file)
        if clist.size()%2: clist.itemconfig("end", bg='azure', fg='black')
    clist.selection_set(0)
    show['state']='normal' if flist.size()>0 else 'disabled'
    ana['state']='normal' if flist.size()>0 else 'disabled'

def run_selected(event=None):
    text.delete(1.0,'end')

    run=rlist.get(rlist.curselection()[0])
    for folder, subdirs, files in walk(run):
        if 'WaveDumpConfig.txt' in files: parse_wavedump_cfg(run)
        if 'settings.xml' in files: parse_compass_cfg(run)

    list_files_in(run)

rlist.bind("<<ListboxSelect>>", run_selected)
run_selected()

def convert_file(event=None):
    run=rlist.get(rlist.curselection()[0]).replace("\\","/")
    file=clist.get(clist.curselection()[0])
    ch=file[8:9] if file[-3:]=='bin' else file[4:5]
    script='c2r.C' if file[-3:]=='bin' else 'w2r.C'
    if file[-3:]=="bin": run=run+"/RAW"
    argument='{}("{}","{}",{},{},{},{},{},{})'.format(
            script,run,file,ch,thr,polarity,nbase,ssize,bits)
    Popen(['root', '-b', '-q', argument]).wait()
    list_files_in(run)

convert=Button(root, text='Convert', command=convert_file)
convert.grid(column=1,row=2)
convert.bind('<Return>', convert_file)
root.bind('c', convert_file)

# give focus to the GUI window in Mac
# https://stackoverflow.com/questions/17774859
from platform import system as platform
if platform() == 'Darwin':  # How Mac OS X is identified by Python
    system('''/usr/bin/osascript -e 'tell app "Finder" to set frontmost of process "Python" to true' ''')

# If you put root.destroy() here, it will cause an error if the window is
# closed with the window manager.
root.mainloop()
