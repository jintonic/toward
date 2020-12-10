#!/usr/bin/env python

usage='''
Show waveforms in run/number/wave[0-7].root:
python3 show.py <number>

where <number> should be a non-negative integer, e.g. 123
wave[0-7].root will then be searched in folder run/123/

If no <number> is specified, all sub-directories in run/ 
will be checked if they are named as non-negative integers.
wave[0-7].root will then be searched in run/<biggest integer>
'''

hotkeys='''
   --- List of key bindings ---
0-7:        toggle channel 0 to 7
<Space>:    next event
b:          previous event
q:          quit
'''

# get the largest run in run/*/
runs=[]
from os import scandir, path
with scandir('run') as dirs:
    for folder in dirs:
        if folder.name.isdigit(): runs.append(folder.name)
runs.sort(); run=runs[-1]

# get run number
from sys import argv
if len(argv)>1: # if run number is specified
    if argv[1] in runs: run=argv[1]
    else: print("no run/"+argv[1]+"/, quit"); exit()

# check root files
folder="run/"+run+"/"
print("check ROOT files in "+folder+":")
import uproot4
t=[0]*8; n=[0]*8; nfiles=0
for ch in range(8):
    file=folder+"wave"+str(ch)+".root"
    if path.exists(file):
        t[ch]=uproot4.open(file)['t'].arrays()
        n[ch]=len(t[ch])
        print(file+" contains "+str(n[ch])+" events")
        if n[ch]>0: nfiles+=1
if nfiles<1: print("no root file in "+folder+", quit"); exit()
    
    
# title bar
for ch in range(8):
    if n[ch]>0:
        title="event 0/"+str(n[ch])+" in run "+run+" (press h for help)"
        break
from tkinter import Tk, Entry, TOP, BOTTOM, BOTH
window = Tk(); window.wm_title(title)

# canvas
from matplotlib.figure import Figure
fig=Figure(); ax=fig.add_subplot()
evt=0; # draw the first event
channel=[0]*8
for ch in range(8):
    if n[ch]>0:
        line,=ax.plot(t[ch][b's'][evt], label="channel "+str(ch))
        channel[ch]=line
ax.legend()

from matplotlib.backends.backend_tkagg import FigureCanvasTkAgg
canvas = FigureCanvasTkAgg(fig, master=window); canvas.draw()
canvas.get_tk_widget().pack(side=TOP, fill=BOTH, expand=1)

# ZOOM
import matplotlib.backends.backend_tkagg as tkagg
tkagg.NavigationToolbar2Tk(canvas, window)
# to modify the toolbar above: 
# from matplotlib.backends.backend_tkagg import NavigationToolbar2Tk
# class selfbar(NavigationToolbar2Tk):
    # def __init__(,,,):



# command line
cmdline=Entry(window)
cmdline.insert(0, "<Tab>: switch in between the canvas above and this cmd line")
cmdline.pack(side=BOTTOM, fill=BOTH)

def show_previous_event():
    min=999999999
    for ch in range(8):
        if channel[ch]==0:continue
        if channel[ch].get_visible() and min>n[ch]: min=n[ch]
    global evt; evt-=1
    if evt==-1: evt=min-1
    title="event "+str(evt)+"/"+str(min)+" in run "+run+" (press h for help)"
    window.wm_title(title)
    ax.cla() #clear plots
    for ch in range(8):
        if channel[ch]==0:continue
        if channel[ch].get_visible(): 
            #channel[ch].set_ydata(t[ch][b's'][evt])
            line,=ax.plot(t[ch][b's'][evt], label="channel "+str(ch))
            channel[ch]=line
    ax.legend()
    canvas.draw()

def show_next_event():
    min=999999999
    for ch in range(8):
        if channel[ch]==0:continue
        if channel[ch].get_visible() and min>n[ch]: min=n[ch]
    global evt; evt+=1;
    if evt==min: evt=0
    title="event "+str(evt)+"/"+str(min)+" in run "+run+" (press h for help)"
    window.wm_title(title)
    ax.cla() #clear plots
    for ch in range(8):
        if channel[ch]==0:continue
        if channel[ch].get_visible(): 
            #channel[ch].set_ydata(t[ch][b's'][evt])
            line,=ax.plot(t[ch][b's'][evt], label="channel "+str(ch))
            channel[ch]=line
    ax.legend()
    canvas.draw()

def jump_event(evt):
    min=999999999
    for ch in range(8):
        if channel[ch]==0:continue
        if channel[ch].get_visible() and min>n[ch]: min=n[ch]
    title="event "+str(evt)+"/"+str(min)+" in run "+run+" (press h for help)"
    window.wm_title(title)
    ax.cla() #clear plots
    for ch in range(8):
        if channel[ch]==0:continue
        if channel[ch].get_visible(): 
            #channel[ch].set_ydata(t[ch][b's'][evt])
            line,=ax.plot(t[ch][b's'][evt], label="channel "+str(ch))
            channel[ch]=line
    ax.legend()
    canvas.draw()

# Mouse over to display coordinates
def motion_notify_event(event):
    if(event.xdata==None or event.ydata==None):
        title2 = title + "     (None,None)"
        window.wm_title(title2)
    else:
        title2 = title + "     ("+str(int(event.xdata))+","+str(round(event.ydata,2))+")"
        window.wm_title(title2)
    
def toggle_ch(event):
    ch=int(event.key)
    if channel[ch]==0: return
    if channel[ch].get_visible(): channel[ch].set_visible(False)
    else: channel[ch].set_visible(True)
    ax.legend(); canvas.draw()

def handle_key_press(event):
    if event.key=="q":
        window.quit()    # stop the main loop
        window.destroy() # necessary on Windows to prevent fatal error
    elif event.key.isdigit(): toggle_ch(event)
    elif event.key=="h": print(hotkeys)
    elif event.key==" ": show_next_event()
    elif event.key=="b": show_previous_event()
    else:
        # handle default matplotlib key bindings
        from matplotlib.backend_bases import key_press_handler
        key_press_handler(event, canvas)

canvas.mpl_connect("key_press_event", handle_key_press)
fig.canvas.mpl_connect("motion_notify_event", motion_notify_event)

# https://stackoverflow.com/questions/47475783
def run_cmd(event):
    print("cmd: "+cmdline.get())
    if(str.isdigit(cmdline.get())):
        jump_event(int(cmdline.get()))
        print("jump to event "+cmdline.get())
cmdline.bind('<Return>', run_cmd)

# give focus to the GUI window in Mac
# https://stackoverflow.com/questions/17774859
from os import system
from platform import system as platform
if platform() == 'Darwin':  # How Mac OS X is identified by Python
    system('''/usr/bin/osascript -e 'tell app "Finder" to set frontmost of process "Python" to true' ''')

# If you put window.destroy() here, it will cause an error if the window is
# closed with the window manager.
window.mainloop()
